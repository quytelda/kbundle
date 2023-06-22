#ifndef __FILE_ENTRY_HPP
#define __FILE_ENTRY_HPP

#include <QtCore>

struct FileEntry {
    QString full_path;
    QString media_type;
    QString md5sum;
};

inline bool operator==(const FileEntry &lhs, const FileEntry &rhs)
{
    return (lhs.full_path == rhs.full_path);
}

inline uint qHash(const FileEntry &key, uint seed)
{
    return qHash(key.full_path, seed);
}

#endif /* __FILE_ENTRY_HPP */
