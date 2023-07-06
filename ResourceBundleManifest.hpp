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

    bool exists() const;
    bool load();
    bool save();
    void insertEntry(const FileEntry &entry);
    bool removeEntry(const QString &path);
    bool hasEntry   (const QString &path) const;
    bool addTag   (const QString &path, const QString &tag);
    bool removeTag(const QString &path, const QString &tag);
    QDomDocument toXML() const;
    QSet<QString> resourceList() const;

private:
    bool tagListFromXML  (const QDomElement &elem, QStringList *tagList) const;
    bool fileEntryFromXML(const QDomElement &elem, FileEntry   *entry  ) const;
    QDomElement tagListToXML  (QDomDocument &doc, const QStringList &tagList) const;
    QDomElement fileEntryToXML(QDomDocument &doc, const FileEntry   &entry  ) const;

    QFile manifestFile;
    QMap<QString, FileEntry> entries;
};

#endif /* __RESOURCE_BUNDLE_MANIFEST_HPP */
