#include "ResourceBundle.hpp"

bool md5(QFile &file, QString *result)
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
