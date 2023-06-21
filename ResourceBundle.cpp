#include "ResourceBundle.hpp"

ResourceBundle::ResourceBundle(const QString &path)
{
    this->root = new QDir(path);
    this->manifest = new ResourceBundleManifest(*this->root);
}

ResourceBundle::~ResourceBundle()
{
    delete root;
    delete manifest;
}

bool ResourceBundle::init()
{
    if (!root->exists()) {
        return false;
    }

    if (!manifest->init()) {
        return false;
    }

    if (!this->scanFiles()) {
        return false;
    }

    return true;
}

bool ResourceBundle::scanFiles()
{
    if (!root->exists()) {
        return false;
    }

    for (QString dirName : resourceDirNames) {
        QDir dir(root->absoluteFilePath(dirName));
        if (!dir.exists()) {
            continue;
        }

        this->resourceFiles += dir.entryInfoList(QDir::Files);
    }

    return true;
}
