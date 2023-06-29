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

#ifndef __FILE_ENTRY_HPP
#define __FILE_ENTRY_HPP

#include <QtCore>

struct FileEntry {
    QString path;
    QString mediaType;
    QString md5sum;
    QStringList tags;
};

inline bool operator==(const FileEntry &lhs, const FileEntry &rhs)
{
    return (lhs.path == rhs.path);
}

inline uint qHash(const FileEntry &key, uint seed)
{
    return qHash(key.path, seed);
}

inline const char *showFileEntry(const FileEntry &entry)
{
    return qPrintable(entry.path      + ":" +
                      entry.mediaType + ":" +
                      entry.md5sum    + ":" +
                      entry.tags.join(','));
}

#endif /* __FILE_ENTRY_HPP */
