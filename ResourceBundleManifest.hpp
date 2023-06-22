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

#define TAG_FILE_ENTRY  "manifest:file-entry"
#define ATTR_MEDIA_TYPE "manifest:media-type"
#define ATTR_FULL_PATH  "manifest:full-path"
#define ATTR_MD5SUM     "manifest:md5sum"

class ResourceBundleManifest
{
public:
    ResourceBundleManifest(const QDir &root);
    ResourceBundleManifest(const QString &manifestPath);
    ~ResourceBundleManifest();

    bool load();
    bool save();
    bool create();
    bool init();
    bool isInitialized();
    bool findFileEntry(const QString &full_path, FileEntry *entry);
    bool addFileEntry(const FileEntry &entry);
    bool removeFileEntry(const QString &path);
    QSet<FileEntry> fileEntryList();

private:
    QDomElement findEntry(const QString &path);
    bool parseFileEntry(const QDomElement &elem, FileEntry *entry);

    const QDir bundleRoot;
    QFile *manifestFile;
    QDomDocument doc;
};

#endif /* __RESOURCE_BUNDLE_MANIFEST_HPP */
