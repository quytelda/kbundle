#ifndef __RESOURCE_BUNDLE_HPP
#define __RESOURCE_BUNDLE_HPP

#include <iostream>
#include <QtCore>
#include <QtXml>

#include "ResourceBundleManifest.hpp"

const QStringList resourceDirNames = {
    "brushes",
    "paintoppresets",
    "patterns",
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

#endif /* __RESOURCE_BUNDLE_HPP */
