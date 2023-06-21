#ifndef __RESOURCE_BUNDLE_MANIFEST_HPP
#define __RESOURCE_BUNDLE_MANIFEST_HPP

#include <iostream>
#include <QtXml>

#include "FileEntry.hpp"

#define FOREACH_CHILD_ELEMENT(parent, e)          \
    for ((e) = (parent).firstChildElement();      \
         !(e).isNull();                           \
         (e) = (e).nextSiblingElement())

#define MANIFEST_PATH "META-INF/manifest.xml"
#define MANIFEST_XMLNS "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0"

#define TAG_FILE_ENTRY  "file-entry"
#define ATTR_MEDIA_TYPE "media-type"
#define ATTR_FULL_PATH  "full-path"
#define ATTR_MD5SUM     "md5sum"

class ResourceBundleManifest
{
public:
    ResourceBundleManifest(const QDir &root);
    ResourceBundleManifest(const QString &manifestPath);
    ~ResourceBundleManifest();

    bool exists();
    bool load();
    bool save();
    bool create();
    bool findFileEntry(const QString &full_path, FileEntry *entry);
    bool addFileEntry(const FileEntry &entry);
    QSet<FileEntry> fileEntryList();

private:
    bool parseFileEntry(const QDomElement &elem, FileEntry *entry);

    QFile *manifestFile;
    QDomDocument doc;
};

#endif /* __RESOURCE_BUNDLE_MANIFEST_HPP */
