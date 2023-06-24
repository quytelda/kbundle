#ifndef __RESOURCE_BUNDLE_HPP
#define __RESOURCE_BUNDLE_HPP

#include <iostream>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <quazipnewinfo.h>
#include <JlCompress.h>

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
    bool build(const QString &path);

private:
    QString bundlePath(const QString &path);

    bool zipAddFile(const QString &path);

    QDir *root = nullptr;
    ResourceBundleManifest *manifest = nullptr;
    QFileInfoList resourceFiles;
    QuaZip *archive = nullptr;
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
