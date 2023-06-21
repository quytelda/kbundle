#include "ResourceBundleManifest.hpp"

ResourceBundleManifest::ResourceBundleManifest(const QDir &root)
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

    if (!doc.setContent(manifestFile, true)) {
        manifestFile->close();
        return false;
    }
    manifestFile->close();

    return true;
}

bool ResourceBundleManifest::save() {
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

    QDomElement root = doc.createElementNS(MANIFEST_XMLNS, "manifest:manifest");
    root.setAttributeNS(MANIFEST_XMLNS, "manifest:version", "1.2");
    doc.appendChild(root);

    // A file entry for the bundle's root directory is always included.
    //
    // XXX: This should be implemented using namespaces formally.
    // However the QtXML namespace-enabled functions insert redundant
    // 'xmlns' attributes into every tag in the document.
    QDomElement dirEntry = doc.createElement("manifest:" TAG_FILE_ENTRY);
    dirEntry.setAttribute("manifest:" ATTR_MEDIA_TYPE, "application/x-krita-resourcebundle");
    dirEntry.setAttribute("manifest:" ATTR_FULL_PATH , "/");
    root.appendChild(dirEntry);

    return true;
}

bool ResourceBundleManifest::findFileEntry(const QString &full_path, FileEntry *entry)
{
    QDomElement root = doc.documentElement();
    if(root.isNull()) {
        return false;
    }

    QDomElement e;
    FOREACH_CHILD_ELEMENT(root, e) {
        if (!parseFileEntry(e, entry)) {
            continue;
        }

        if (entry->full_path == full_path) {
            return true;
        }
    }

    return false;
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
        entries.insert(entry);
    }

    return entries;
}

bool ResourceBundleManifest::addFileEntry(const FileEntry &entry)
{
    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        return false;
    }

    QDomElement elem = doc.createElementNS(MANIFEST_XMLNS, TAG_FILE_ENTRY);
    elem.setAttributeNS(MANIFEST_XMLNS, ATTR_MEDIA_TYPE, entry.media_type);
    elem.setAttributeNS(MANIFEST_XMLNS, ATTR_FULL_PATH , entry.full_path);
    elem.setAttributeNS(MANIFEST_XMLNS, ATTR_MD5SUM    , entry.md5sum);

    // TODO: Use QDomNode::insertAfter(newChild, refChild) to keep
    // matching media-types together.
    root.appendChild(elem);

    return true;
}

bool ResourceBundleManifest::parseFileEntry(const QDomElement &elem, FileEntry *entry)
{
    if (!entry || elem.tagName() != TAG_FILE_ENTRY) {
        std::cerr << "Failed to parse file entry on line "
                  << elem.lineNumber()
                  << std::endl;
        return false;
    }

    entry->media_type = elem.attributeNS(MANIFEST_XMLNS, ATTR_MEDIA_TYPE);
    entry->full_path  = elem.attributeNS(MANIFEST_XMLNS, ATTR_FULL_PATH);
    entry->md5sum     = elem.attributeNS(MANIFEST_XMLNS, ATTR_MD5SUM);

    return true;
}
