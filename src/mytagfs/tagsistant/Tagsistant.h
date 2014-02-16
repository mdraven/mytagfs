#ifndef TAGSISTANT_H
#define TAGSISTANT_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <boost/optional.hpp>
#include <soci/soci.h>

namespace tagsistant {

    class FilenameAndPath;
    class Tags;

    class Tagsistant {

        friend class FilenameAndPath;

        std::string tagsistant_directory;

        std::vector<FilenameAndPath> objects;
        std::map<int, std::vector<int>> tagging;
        std::unique_ptr<Tags> tags;

        std::vector<int> getInodeIntersectionFromTagnames(const std::vector<std::string>& tagnames) const;

        void cacheTagsFromDB();
        void cacheObjectsFromDB();
        void cacheTaggingFromDB();

    public:
        Tagsistant(const std::string& tagsistant_directory);

        std::vector<std::string> getTagnames() const;
        std::vector<std::string> getTagnames(const std::vector<std::string>& maybe_tagnames) const;
        bool checkTagnames(const std::vector<std::string>& maybe_tagnames) const;
        std::vector<FilenameAndPath> getPathsToFiles(const std::vector<std::string>& tagnames) const;
        boost::optional<FilenameAndPath> isFileContainsInTags(const std::vector<std::string>& tagnames, const std::string& filename) const;

        void rehashCache();

        virtual ~Tagsistant();
    };

}

#endif // TAGSISTANT_H
