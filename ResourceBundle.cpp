/* Copyright 2023 Quytelda Kahja
 *
 * This file is part of kbundle.
 *
 * kbundle is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kbundle is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kbundle. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ResourceBundle.hpp"

ResourceBundle::ResourceBundle(const QString &path)
    : root(path)
{
    QString manifestPath = externalPath(MANIFEST_PATH);
    this->manifest = new ResourceBundleManifest(manifestPath);
}

ResourceBundle::~ResourceBundle()
{
    delete manifest;
    delete archive;
}

bool ResourceBundle::load()
{
    if (manifest->exists() && !manifest->load()) {
        std::cerr << "Failed to load manifest file."
                  << std::endl;
        return false;
    }

    if (!scanFiles()) {
        std::cerr << "Failed to scan bundle directory."
                  << std::endl;
        return false;
    }

    return true;
}

bool ResourceBundle::scanFiles()
{
    if (!root.exists()) {
        std::cerr << "Bundle directory does not exist: "
                  << qPrintable(root.path())
                  << std::endl;
        return false;
    }

    resourceFiles.clear();

    for (QString dirName : resourceDirNames) {
        QDir dir(externalPath(dirName));
        if (!dir.exists()) {
            continue;
        }

        for (QString name : dir.entryList(QDir::Files)) {
            resourceFiles.append(dir.filePath(name));
        }
    }

    return true;
}

bool ResourceBundle::pruneManifest()
{
    // Find resources that exist in the manifest but not on disk.
    QSet<QString> diskFiles;
    for (const QString &path : resourceFiles) {
        diskFiles.insert(internalPath(path));
    }
    QSet<QString> missingFiles = manifest->resourceList().subtract(diskFiles);

    for (QString rpath : missingFiles) {
        if (!manifest->removeEntry(rpath)) {
            return false;
        }
    }

    return true;
}

bool ResourceBundle::updateManifest()
{
    if (resourceFiles.isEmpty()) {
        return false;
    }

    pruneManifest();

    for (QString path : resourceFiles) {
        QFile file(path);
        QString md5sum;
        if (!md5(file, &md5sum)) {
            return false;
        }

        FileEntry entry = {
            .path      = internalPath(path),
            .mediaType = QFileInfo(path).dir().dirName(),
            .md5sum    = md5sum,
            .tags      = QStringList(),
        };
        manifest->insertEntry(entry);
    }

    return manifest->save();
}

bool ResourceBundle::addTag(const QString &path, const QString &tag)
{
    QString rpath = internalPath(path);
    return manifest->addTag(rpath, tag)
        && manifest->save();
}

bool ResourceBundle::removeTag(const QString &path, const QString &tag)
{
    QString rpath = internalPath(path);
    return manifest->removeTag(rpath, tag)
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

    for (QString path : resourceFiles) {
        QString rpath = internalPath(path);
        if (!zipAddFile(rpath)) {
            goto exit;
        }
    }

    if (!zipAddFile("preview.png")) {
        goto exit;
    }

    if (!zipAddFile(MANIFEST_PATH)) {
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

QString ResourceBundle::externalPath(const QString &rpath) const
{
    return root.filePath(rpath);
}

QString ResourceBundle::internalPath(const QString &path) const
{
    return root.relativeFilePath(QFileInfo(path).absoluteFilePath());
}

bool ResourceBundle::zipAddFile(const QString &rpath)
{
    if (!archive || archive->getMode() == QuaZip::mdNotOpen) {
        return false;
    }

    QString path = externalPath(rpath);
    if (!QFileInfo::exists(path)) {
        return false;
    }

    return JlCompress::compressFile(archive, path, rpath);
}
