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
