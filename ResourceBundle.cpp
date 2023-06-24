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
    delete archive;
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

bool ResourceBundle::build(const QString &path)
{
    if (archive) {
        return false;
    }

    archive = new QuaZip(path);
    if (!archive->open(QuaZip::mdCreate)) {
        std::cerr << "Failed to create ZIP archive: "
                  << qPrintable(path)
                  << std::endl;
        return false;
    }

    bool ok = false;

    if (!zipAddFile("mimetype")) {
        goto exit;
    }

    for (QFileInfo resourceInfo : resourceFiles) {
        QString resourcePath = bundlePath(resourceInfo.filePath());
        if (!zipAddFile(resourcePath)) {
            goto exit;
        }
    }

    if (!zipAddFile("preview.png")) {
        goto exit;
    }

    if (!zipAddFile("META-INF/manifest.xml")) {
        goto exit;
    }

    if (!zipAddFile("meta.xml")) {
        goto exit;
    }

    // Success
    ok = true;

exit:
    archive->close();
    return ok;
}

QString ResourceBundle::bundlePath(const QString &path)
{
    return root->relativeFilePath(path);
}

bool ResourceBundle::zipAddFile(const QString &path)
{
    if (!archive || archive->getMode() == QuaZip::mdNotOpen) {
        return false;
    }

    QString fullPath = root->filePath(path);
    if (!QFileInfo::exists(fullPath)) {
        return false;
    }

    return JlCompress::compressFile(archive, fullPath, path);
}
