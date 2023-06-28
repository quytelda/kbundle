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
