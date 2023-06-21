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
};

class ResourceBundle
{
public:
    ResourceBundle(const QString &path);
    ~ResourceBundle();

    bool init();
    bool scanFiles();

private:
    QDir *root;
    ResourceBundleManifest *manifest;
    QFileInfoList resourceFiles;
};

#endif /* __RESOURCE_BUNDLE_HPP */
