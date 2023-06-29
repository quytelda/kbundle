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

#define TAG_MANIFEST    "manifest:manifest"
#define TAG_FILE_ENTRY  "manifest:file-entry"
#define TAG_TAGS        "manifest:tags"
#define TAG_TAG         "manifest:tag"
#define ATTR_VERSION    "manifest:version"
#define ATTR_MEDIA_TYPE "manifest:media-type"
#define ATTR_FULL_PATH  "manifest:full-path"
#define ATTR_MD5SUM     "manifest:md5sum"

class ResourceBundleManifest
{
public:
    ResourceBundleManifest(const QString &manifestPath);

    bool exists();
    bool load();
    bool save();
    void clear();
    bool addEntry(const FileEntry &entry);
    bool removeEntry(const QString &path);
    bool addTag   (const QString &path, const QString &tag);
    bool removeTag(const QString &path, const QString &tag);
    QDomDocument toXML();
    QSet<QString> resourceList();

private:
    bool tagListFromXML(const QDomElement &elem, QStringList *tagList);
    bool fileEntryFromXML(const QDomElement &elem, FileEntry *entry);
    QDomElement tagListToXML(QDomDocument &doc, const QStringList &tagList);
    QDomElement fileEntryToXML(QDomDocument &doc, const FileEntry &entry);

    QFile manifestFile;
    QMap<QString, FileEntry> entries;
};

#endif /* __RESOURCE_BUNDLE_MANIFEST_HPP */
