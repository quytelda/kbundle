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

inline const char *showFileEntry(const FileEntry &entry)
{
    return qPrintable(entry.full_path  + ":" +
                      entry.media_type + ":" +
                      entry.md5sum);
}

#endif /* __FILE_ENTRY_HPP */
