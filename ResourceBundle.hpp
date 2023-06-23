#ifndef __RESOURCE_BUNDLE_HPP
#define __RESOURCE_BUNDLE_HPP

#include <iostream>
#include <QtCore>
#include <QtXml>

#include "ResourceBundleManifest.hpp"

const QStringList resourceDirNames = {
    "brushes",
    "gamutmasks",
    "gradients",
    "paintoppresets",
    "palettes",
    "patterns",
    "seexpr_scripts",
    "workspaces",
};

class ResourceBundle
{
public:
    ResourceBundle(const QString &path);
    ~ResourceBundle();

    bool init();
    bool scanFiles();
    bool updateManifest();
    bool addTag(const QString &path, const QString &tagName);
    bool removeTag(const QString &path, const QString &tagName);

private:
    QDir *root;
    ResourceBundleManifest *manifest;
    QFileInfoList resourceFiles;
};

inline bool md5(QFile &file, QString *result)
{
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    if(!hash.addData(&file)) {
        return false;
    }

    *result = QString(hash.result().toHex());
    return true;
}

#endif /* __RESOURCE_BUNDLE_HPP */
