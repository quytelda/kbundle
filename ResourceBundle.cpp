#include "ResourceBundle.hpp"

ResourceBundle::ResourceBundle(const QString &path)
{
    this->root = new QDir(path);

    QString manifestPath = root->filePath(MANIFEST_PATH);
    this->manifest = new ResourceBundleManifest(manifestPath);
}

ResourceBundle::~ResourceBundle()
{
    delete root;
    delete manifest;
    delete archive;
}

bool ResourceBundle::loadManifest()
{
    if (!manifest->exists()) {
        return false;
    }

    return manifest->load();
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

    manifest->clear();

    for (QFileInfo info : resourceFiles) {
        QString filePath  = info.filePath();
        QString mediaType = info.dir().dirName();
        QString path      = resourcePath(filePath);
        QString md5sum;

        QFile file(filePath);
        if (!md5(file, &md5sum)) {
            return false;
        }

        FileEntry entry = {
            .path      = path,
            .mediaType = mediaType,
            .md5sum    = md5sum,
            .tags      = QStringList(),
        };
        if (!manifest->addEntry(entry)) {
            return false;
        }
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

    for (QFileInfo info : resourceFiles) {
        QString rpath = resourcePath(info.filePath());
        if (!zipAddFile(rpath)) {
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

QString ResourceBundle::filePath(const QString &rpath)
{
    return root->filePath(rpath);
}

QString ResourceBundle::resourcePath(const QString &path)
{
    return root->relativeFilePath(path);
}

bool ResourceBundle::zipAddFile(const QString &path)
{
    if (!archive || archive->getMode() == QuaZip::mdNotOpen) {
        return false;
    }

    QString fullPath = filePath(path);
    if (!QFileInfo::exists(fullPath)) {
        return false;
    }

    return JlCompress::compressFile(archive, fullPath, path);
}
