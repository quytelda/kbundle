/* Copyright 2023 Quytelda Kahja
 *
 * This file is part of kbundle.
 *
 * kbundle is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kbundle is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kbundle. If not, see <https://www.gnu.org/licenses/>.
 */

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

    bool scanFiles();
    bool loadManifest();
    bool saveManifest();
    bool updateManifest();
    bool addTag   (const QString &rpath, const QString &tagName);
    bool removeTag(const QString &rpath, const QString &tagName);
    bool build(const QString &path);

private:
    QString filePath    (const QString &rpath);
    QString resourcePath(const QString &path);

    bool zipAddFile(const QString &path);

    QDir *root = nullptr;
    ResourceBundleManifest *manifest = nullptr;
    QFileInfoList resourceFiles;
    QuaZip *archive = nullptr;
};

inline bool md5(QFile &file, QString *result)
{
    bool ok = false;
    QCryptographicHash hash(QCryptographicHash::Md5);

    if (!file.open(QIODevice::ReadOnly)) {
        goto exit;
    }

    if(!hash.addData(&file)) {
        goto exit;
    }
    *result = QString(hash.result().toHex());
    ok = true;

exit:
    file.close();
    return ok;
}

#endif /* __RESOURCE_BUNDLE_HPP */
