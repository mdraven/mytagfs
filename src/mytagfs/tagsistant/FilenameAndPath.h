#ifndef FILENAMEANDPATH_H
#define FILENAMEANDPATH_H

#include "Tagsistant.h"

#include <string>

namespace tagsistant {

    class FilenameAndPath {
        friend void Tagsistant::cacheObjectsFromDB();

        std::string filename;
        int inode;
        const std::string& tagsistant_directory;

        FilenameAndPath(const std::string& filename, int inode,
                        const std::string& tagsistant_directory);
    public:
        const std::string& getFilename() const {
            return filename;
        }

        int getInode() const {
            return inode;
        }

        size_t getPathLen() const;

        void getPath(char* buffer, size_t buffer_sz) const;

        virtual ~FilenameAndPath();
    };

}
#endif // FILENAMEANDPATH_H
