#ifndef __RESOURCE_BUNDLE_HPP
#define __RESOURCE_BUNDLE_HPP

#include <iostream>
#include <QtCore>
#include <QtXml>

#include "ResourceBundleManifest.hpp"

class ResourceBundle
{
public:
    ResourceBundle(const QString &path);
    ~ResourceBundle();

private:
    QDir *root;
    ResourceBundleManifest *manifest;
};

#endif /* __RESOURCE_BUNDLE_HPP */
