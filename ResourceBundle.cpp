#include "ResourceBundle.hpp"

ResourceBundle::ResourceBundle(const QString &path)
{
    this->root = new QDir(path);
    this->manifest = new ResourceBundleManifest(*this->root);
}

ResourceBundle::~ResourceBundle()
{
    delete root;
    delete manifest;
}

bool ResourceBundle::init()
{
    if (!root->exists()) {
        return false;
    }

    if (!manifest->init()) {
        return false;
    }

    if (!this->scanFiles()) {
        return false;
    }

    return true;
}

bool ResourceBundle::scanFiles()
{
    if (!root->exists()) {
        return false;
    }

    for (QString dirName : resourceDirNames) {
        QDir dir(root->absoluteFilePath(dirName));
        if (!dir.exists()) {
            continue;
        }

        this->resourceFiles += dir.entryInfoList(QDir::Files);
    }

    return true;
}

bool ResourceBundle::updateManifest()
{
    if (resourceFiles.size() <= 0) {
        return false;
    }

    if (!manifest->isInitialized()) {
        return false;
    }
    QSet<FileEntry> manifestEntries = manifest->fileEntryList();

    QSet<FileEntry> fileEntries;
    for (QFileInfo fileInfo : resourceFiles) {
        QString absolutePath = fileInfo.filePath();
        QString full_path  = root->relativeFilePath(absolutePath);
        QString media_type = fileInfo.dir().dirName();
        QString md5sum;

        QFile file(absolutePath);
        if (!md5(file, &md5sum)) {
            return false;
        }

        FileEntry entry = { full_path, media_type, md5sum };
        fileEntries.insert(entry);
    }

    // Remove manifest entries which have no matching file.
    QSet<FileEntry> missingFiles = manifestEntries.subtract(fileEntries);
    for (FileEntry entry : missingFiles) {
        manifest->removeFileEntry(entry.full_path);
        std::cout << "Removed manifest entry: "
                  << qPrintable(entry.full_path)
                  << std::endl;
    }

    // Add or update manifest entries for each existing file.
    for (FileEntry entry : fileEntries) {
        manifest->addFileEntry(entry);
    }

    return manifest->save();
}

bool ResourceBundle::addTag(const QString &path, const QString &tagName)
{
    return manifest->addTag(path, tagName)
        && manifest->save();
}

bool ResourceBundle::removeTag(const QString &path, const QString &tagName)
{
    return manifest->removeTag(path, tagName)
        && manifest->save();
}
