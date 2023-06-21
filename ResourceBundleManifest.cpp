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

bool ResourceBundleManifest::exists()
{
    return manifestFile->exists();
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
    return false;
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
                  << e.lineNumber()
                  << std::endl;
        return false;
    }

    entry->media_type = elem.attributeNS(MANIFEST_XMLNS, ATTR_MEDIA_TYPE);
    entry->full_path  = elem.attributeNS(MANIFEST_XMLNS, ATTR_FULL_PATH);
    entry->md5sum     = elem.attributeNS(MANIFEST_XMLNS, ATTR_MD5SUM);

    return true;
}
