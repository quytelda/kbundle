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
    return (lhs.full_path  == rhs.full_path)
        && (lhs.media_type == rhs.media_type)
        && (lhs.md5sum     == rhs.md5sum);
}

inline uint qHash(const FileEntry &key, uint seed)
{
    return qHash(key.full_path , seed)
        ^  qHash(key.media_type, seed)
        ^  qHash(key.md5sum    , seed);
}

#endif /* __FILE_ENTRY_HPP */
