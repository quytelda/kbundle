#ifndef __RESOURCE_BUNDLE_MANIFEST_HPP
#define __RESOURCE_BUNDLE_MANIFEST_HPP

#include <iostream>

#include <QtCore>
#include <QtXml>

#define MANIFEST_PATH "META-INF/manifest.xml"
#define MANIFEST_XMLNS "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0"

#define TAG_FILE_ENTRY  "file-entry"
#define ATTR_MEDIA_TYPE "media-type"
#define ATTR_FULL_PATH  "full-path"
#define ATTR_MD5SUM     "md5sum"

struct FileEntry {
    QString full_path;
    QString media_type;
    QString md5sum;
};

bool parseFileEntry(const QDomElement &elem, FileEntry *entry)
{
    if (elem.tagName() != TAG_FILE_ENTRY) {
        return false;
    }

    entry->media_type = elem.attributeNS(MANIFEST_XMLNS, ATTR_MEDIA_TYPE);
    entry->full_path  = elem.attributeNS(MANIFEST_XMLNS, ATTR_FULL_PATH);
    entry->md5sum     = elem.attributeNS(MANIFEST_XMLNS, ATTR_MD5SUM);

    return true;
}

class ResourceBundleManifest
{
public:
    ResourceBundleManifest(const QDir &root);
    ResourceBundleManifest(const QString &manifestPath);
    ~ResourceBundleManifest();

    bool exists();
    bool load();
    bool create();
    bool findFileEntry(const QString &full_path, FileEntry *entry);
    bool addFileEntry(const FileEntry &entry);
    bool save();

private:
    QFile *manifestFile;
    QDomDocument doc;
};

#endif /* __RESOURCE_BUNDLE_MANIFEST_HPP */
