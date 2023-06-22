#include "ResourceBundleManifest.hpp"

ResourceBundleManifest::ResourceBundleManifest(const QDir &root)
    : bundleRoot(root)
{
    QString manifestPath = root.absoluteFilePath(MANIFEST_PATH);
    manifestFile = new QFile(manifestPath);
}

ResourceBundleManifest::ResourceBundleManifest(const QString &manifestPath)
{
    manifestFile = new QFile(manifestPath);
}

ResourceBundleManifest::~ResourceBundleManifest()
{
    delete manifestFile;
}

bool ResourceBundleManifest::load()
{
    if (!manifestFile->open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!doc.setContent(manifestFile, false)) {
        manifestFile->close();
        return false;
    }
    manifestFile->close();

    return true;
}

bool ResourceBundleManifest::save()
{
    if (!bundleRoot.exists("META-INF") &&
        !bundleRoot.mkdir("META-INF")) {
        return false;
    }

    if (!manifestFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(manifestFile);
    doc.save(out, 1);
    manifestFile->close();

    return true;
}

bool ResourceBundleManifest::create()
{
    if (!doc.documentElement().isNull()) {
        return false;
    }

    QDomElement root = doc.createElement("manifest:manifest");
    root.setAttribute("xmlns:manifest", MANIFEST_XMLNS);
    root.setAttribute("manifest:version", "1.2");
    doc.appendChild(root);

    // A file entry for the bundle's root directory is always included.
    QDomElement dirEntry = doc.createElement(TAG_FILE_ENTRY);
    dirEntry.setAttribute(ATTR_MEDIA_TYPE, "application/x-krita-resourcebundle");
    dirEntry.setAttribute(ATTR_FULL_PATH , "/");
    root.appendChild(dirEntry);

    return true;
}

bool ResourceBundleManifest::init()
{
    bool ok;

    if (manifestFile->exists()) {
        ok = this->load();
    } else {
        ok = this->create();
    }

    return ok;
}

bool ResourceBundleManifest::isInitialized()
{
    return !doc.documentElement().isNull();
}

bool ResourceBundleManifest::findFileEntry(const QString &full_path, FileEntry *entry)
{
    QDomElement e = this->findEntry(full_path);
    return parseFileEntry(e, entry);
}

QSet<FileEntry> ResourceBundleManifest::fileEntryList()
{
    QSet<FileEntry> entries;

    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        return entries;
    }

    QDomElement e;
    FOREACH_CHILD_ELEMENT(root, e) {
        FileEntry entry;
        if (!parseFileEntry(e, &entry)) {
            continue;
        }

        if (entry.full_path != "/") {
            entries.insert(entry);
        }
    }

    return entries;
}

bool ResourceBundleManifest::addFileEntry(const FileEntry &entry)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        return false;
    }

    // Check whether this file already has an existing entry.
    // If not, create a new one.
    QDomElement e = this->findEntry(entry.full_path);
    if (e.isNull()) {
        e = doc.createElement(TAG_FILE_ENTRY);
        root.appendChild(e);
    }

    e.setAttribute(ATTR_MEDIA_TYPE, entry.media_type);
    e.setAttribute(ATTR_FULL_PATH , entry.full_path);
    e.setAttribute(ATTR_MD5SUM    , entry.md5sum);

    return true;
}

bool ResourceBundleManifest::removeFileEntry(const QString &path)
{
    QDomElement e = this->findEntry(path);
    if (e.isNull()) {
        return false;
    }

    return !doc.documentElement().removeChild(e).isNull();
}

bool ResourceBundleManifest::parseFileEntry(const QDomElement &elem, FileEntry *entry)
{
    if (!entry || elem.tagName() != TAG_FILE_ENTRY) {
        std::cerr << "Failed to parse file entry on line "
                  << elem.lineNumber()
                  << std::endl;
        return false;
    }

    entry->media_type = elem.attribute(ATTR_MEDIA_TYPE);
    entry->full_path  = elem.attribute(ATTR_FULL_PATH);
    entry->md5sum     = elem.attribute(ATTR_MD5SUM);

    return true;
}

QDomElement ResourceBundleManifest::findEntry(const QString &path)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        return QDomElement();
    }

    QDomElement e;
    QString full_path;
    FOREACH_CHILD_ELEMENT(root, e) {
        if (e.tagName() != TAG_FILE_ENTRY) {
            continue;
        }

        full_path = e.attribute(ATTR_FULL_PATH);
        if (full_path == path) {
            return e;
        }
    }

    // No matching entry was found.
    return QDomElement();
}

bool ResourceBundleManifest::addTag(const QString &path, const QString &tagName)
{
    QDomElement e = findEntry(path);
    if (e.isNull()) {
        return false;
    }

    QDomElement tagList = e.firstChildElement(TAG_TAGS);
    if (tagList.isNull()) {
        tagList = doc.createElement(TAG_TAGS);
        e.appendChild(tagList);
    }

    QDomElement tag = doc.createElement(TAG_TAG);
    tagList.appendChild(tag);
    QDomText text = doc.createTextNode(tagName);
    tag.appendChild(text);

    return true;
}

bool ResourceBundleManifest::removeTag(const QString &path, const QString &tagName)
{
    QDomElement e = findEntry(path);
    if (e.isNull()) {
        return false;
    }

    QDomElement tagList = e.firstChildElement(TAG_TAGS);
    if (tagList.isNull()) {
        return false;
    }

    QDomElement t;
    FOREACH_CHILD_ELEMENT(tagList, t) {
        if (t.tagName() == TAG_TAG && t.text() == tagName) {
            return !tagList.removeChild(t).isNull();
        }
    }

    return false;
}
