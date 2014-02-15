
#include "FilenameAndPath.h"

#include <cstring>
#include <algorithm>

namespace tagsistant {

    static const char archive[] = "/archive/";
    static const char underscore[] = "___";


    static size_t number_decimals(unsigned int num) {
        size_t count = 0;

        if(num == 0)
            ++count;

        while(num != 0) {
            num /= 10;
            ++count;
        }

        return count;
    }

    FilenameAndPath::FilenameAndPath(const std::string& filename, int inode,
                                     const std::string& tagsistant_directory) :
        filename(filename),
        inode(inode),
        tagsistant_directory(tagsistant_directory) {
        if(tagsistant_directory.back() == '/')
            throw std::runtime_error("Path to the directory ends with /");
    }

    void FilenameAndPath::getPath(char* buffer, size_t buffer_sz) const {
        size_t s = std::min(buffer_sz - 1, tagsistant_directory.size());
        memcpy(buffer, tagsistant_directory.c_str(), s + 1);
        buffer += s;
        buffer_sz -= s;

        s = std::min(buffer_sz - 1, sizeof(archive) - 1);
        memcpy(buffer, archive, s + 1);
        buffer += s;
        buffer_sz -= s;

        size_t num_dec = number_decimals(inode);
        s = std::min(buffer_sz - 1, num_dec);
        snprintf(buffer, s+1, "%d", inode);
        buffer += s;
        buffer_sz -= s;

        s = std::min(buffer_sz - 1, sizeof(underscore) - 1);
        memcpy(buffer, underscore, s + 1);
        buffer += s;
        buffer_sz -= s;

        s = std::min(buffer_sz - 1, filename.size());
        memcpy(buffer, filename.c_str(), s + 1);
        buffer += s;
        buffer_sz -= s;
    }

    FilenameAndPath::~FilenameAndPath()
    {}

    size_t FilenameAndPath::getPathLen() const {
        size_t filename_with_inode = number_decimals(inode) + sizeof(underscore)-1 + filename.size();
        return tagsistant_directory.size() + sizeof(archive)-1 + filename_with_inode;
    }

}
