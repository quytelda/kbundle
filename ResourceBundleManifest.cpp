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

#include "ResourceBundleManifest.hpp"

ResourceBundleManifest::ResourceBundleManifest(const QString &manifestPath)
    : manifestFile(manifestPath)
{
}

bool ResourceBundleManifest::exists() const
{
    return manifestFile.exists();
}

bool ResourceBundleManifest::load()
{
    if (!manifestFile.open(QIODevice::ReadOnly)) {
        std::cerr << "Failed to open manifest file: "
                  << qPrintable(manifestFile.fileName())
                  << std::endl;
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(&manifestFile, false)) {
        std::cerr << "Failed to parse manifest XML." << std::endl;
        manifestFile.close();
        return false;
    }
    manifestFile.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != TAG_MANIFEST ||
        root.attribute(ATTR_VERSION) != "1.2") {
        std::cerr << "Invalid manifest or unsupported version." << std::endl;
        return false;
    }

    QDomElement e;
    FOREACH_CHILD_ELEMENT(root, e) {
        FileEntry entry;
        if (!fileEntryFromXML(e, &entry)) {
            return false;
        }

        // Skip entry for bundle root that is always present.
        if (entry.path == "/") {
            continue;
        }

        entries.insert(entry.path, entry);
    }

    return true;
}

bool ResourceBundleManifest::save()
{
    // If the "META-INF" directory doesn't exists, create it.
    // The manifest file is created when written.
    QDir dir = QFileInfo(manifestFile).dir();
    if (!dir.exists() && !dir.mkdir(".")) {
        std::cerr << "Failed to create directory: "
                  << qPrintable(dir.canonicalPath())
                  << std::endl;
        return false;
    }

    if (!manifestFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr << "Failed to open manifest file for writing."
                  << std::endl;
        return false;
    }

    QDomDocument doc = toXML();
    QTextStream out(&manifestFile);
    doc.save(out, 1);
    manifestFile.close();

    return true;
}

void ResourceBundleManifest::clear()
{
    entries.clear();
}

bool ResourceBundleManifest::addEntry(FileEntry &entry)
{
    // Overwrite existing entries, but keep the tags
    if (entries.contains(entry.path)) {
        entry.tags += entries[entry.path].tags;
    }

    entries.insert(entry.path, entry);
    return true;
}

bool ResourceBundleManifest::removeEntry(const QString &path)
{
    int removed = entries.remove(path);
    return (removed == 1);
}

QDomDocument ResourceBundleManifest::toXML() const
{
    QDomDocument doc;

    QDomElement root = doc.createElement(TAG_MANIFEST);
    root.setAttribute("xmlns:manifest", MANIFEST_XMLNS);
    root.setAttribute(ATTR_VERSION, "1.2");
    doc.appendChild(root);

    // A file entry for the bundle's root directory is always included.
    QDomElement dirEntry = doc.createElement(TAG_FILE_ENTRY);
    dirEntry.setAttribute(ATTR_MEDIA_TYPE, "application/x-krita-resourcebundle");
    dirEntry.setAttribute(ATTR_FULL_PATH , "/");
    root.appendChild(dirEntry);

    for (FileEntry entry : entries) {
        QDomElement entryElem = fileEntryToXML(doc, entry);
        root.appendChild(entryElem);
    }

    return doc;
}

QSet<QString> ResourceBundleManifest::resourceList() const
{
    return QSet<QString>(entries.keyBegin(), entries.keyEnd());
}

bool ResourceBundleManifest::addTag(const QString &path, const QString &tag)
{
    if (!entries.contains(path)) {
        return false;
    }

    FileEntry &entry = entries[path];

    if (entry.tags.contains(tag)) {
        return false;
    }
    entry.tags.append(tag);

    return true;
}

bool ResourceBundleManifest::removeTag(const QString &path, const QString &tag)
{
    if (!entries.contains(path)) {
        return false;
    }

    return entries[path].tags.removeOne(tag);
}


bool ResourceBundleManifest::tagListFromXML(const QDomElement &elem, QStringList *tagList) const
{
    QDomElement tagsElem = elem.firstChildElement(TAG_TAGS);
    if (tagsElem.isNull()) {
        return true;
    }

    QDomElement e;
    FOREACH_CHILD_ELEMENT(tagsElem, e) {
        if (e.tagName() != TAG_TAG) {
            return false;
        }

        QString tag = e.text();
        if (tag.isNull()) {
            return false;
        }

        tagList->append(tag);
    }

    return true;
}

bool ResourceBundleManifest::fileEntryFromXML(const QDomElement &elem, FileEntry *entry) const
{
    if (!entry || elem.tagName() != TAG_FILE_ENTRY) {
        std::cerr << "Failed to parse file entry on line "
                  << elem.lineNumber()
                  << std::endl;
        return false;
    }

    entry->mediaType = elem.attribute(ATTR_MEDIA_TYPE);
    entry->path      = elem.attribute(ATTR_FULL_PATH);
    entry->md5sum    = elem.attribute(ATTR_MD5SUM);

    QStringList tagList;
    if (!tagListFromXML(elem, &tagList)) {
        return false;
    }
    entry->tags = tagList;

    return true;
}

QDomElement ResourceBundleManifest::tagListToXML(QDomDocument &doc, const QStringList &tagList) const
{
    QDomElement tagsElem = doc.createElement(TAG_TAGS);

    for (QString tag : tagList) {
        QDomElement tagElem = doc.createElement(TAG_TAG);
        QDomText    tagText = doc.createTextNode(tag);
        tagElem.appendChild(tagText);
        tagsElem.appendChild(tagElem);
    }

    return tagsElem;
}

QDomElement ResourceBundleManifest::fileEntryToXML(QDomDocument &doc, const FileEntry &entry) const
{
    QDomElement entryElem = doc.createElement(TAG_FILE_ENTRY);

    entryElem.setAttribute(ATTR_MEDIA_TYPE, entry.mediaType);
    entryElem.setAttribute(ATTR_FULL_PATH , entry.path);
    entryElem.setAttribute(ATTR_MD5SUM    , entry.md5sum);

    if (!entry.tags.isEmpty()) {
        QDomElement tagsElem = tagListToXML(doc, entry.tags);
        entryElem.appendChild(tagsElem);
    }

    return entryElem;
}
