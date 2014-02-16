#include "Tagsistant.h"

#include "FilenameAndPath.h"
#include "Tags.h"

#include <algorithm>
#include <soci/sqlite3/soci-sqlite3.h>
#include <boost/lexical_cast.hpp>

namespace tagsistant {

    Tagsistant::Tagsistant(const std::string& tagsistant_directory) :
        tagsistant_directory(tagsistant_directory),
        tags(new Tags){
        if(tagsistant_directory.back() == '/')
            throw std::runtime_error("Path to the directory ends with /");

        soci::session sql(soci::sqlite3, tagsistant_directory + '/' + "tags.sql");


        cacheTagsFromDB(sql);
        cacheObjectsFromDB(sql);
        cacheTaggingFromDB(sql);
    }

    std::vector<std::string> Tagsistant::getTagnames() const {
        return tags->getNames();
    }

    std::vector<FilenameAndPath> Tagsistant::getPathsToFiles(const std::vector<std::string>& tagnames) const {
        std::vector<FilenameAndPath> ret;
        std::vector<int> inodes = getInodeIntersectionFromTagnames(tagnames);

        for(int v : inodes) {
            auto it = std::find_if(objects.begin(), objects.end(), [v](const FilenameAndPath& o) {
                return v == o.getInode();
            });
            if(it == objects.end())
                throw std::runtime_error("Object not found");

            ret.push_back(*it);
        }

        return ret;
    }

    boost::optional<FilenameAndPath> Tagsistant::isFileContainsInTags(const std::vector<std::string>& tagnames,
                                                                      const std::string& filename) const {
        for(auto const& o : objects) {
            if(o.getFilename() != filename)
                continue;
            for(auto const& t : tagnames) {
                int tag_id = tags->getId(t);
                auto it = std::find(tagging.at(tag_id).begin(), tagging.at(tag_id).end(), o.getInode());
                if(it == tagging.at(tag_id).end())
                    break;
                return o;
            }
        }

        return boost::optional<FilenameAndPath>();
    }

    void Tagsistant::cacheObjectsFromDB(soci::session& sql) {
        soci::rowset<soci::row> row = (sql.prepare << "select inode, objectname from objects");

        objects.clear();

        for(auto const& v : row) {
            int inode = v.get<int>(0);
            const std::string& object_name = v.get<std::string>(1);
            objects.push_back(FilenameAndPath(object_name, inode, tagsistant_directory));
        }
    }

    std::vector<int> Tagsistant::getInodeIntersectionFromTagnames(const std::vector<std::string>& tagnames) const {
        std::vector<int> ret;

        if(tagnames.empty())
            return ret;

        int tag_id = tags->getId(tagnames[0]);

        ret = tagging.at(tag_id);
        std::sort(ret.begin(), ret.end());

        for(size_t i = 1; i < tagnames.size(); ++i) {
            int tag_id = tags->getId(tagnames[i]);

            if(tagging.find(tag_id) == tagging.end())
                continue;

            std::vector<int> new_tag_ids = tagging.at(tag_id);
            std::sort(new_tag_ids.begin(), new_tag_ids.end());

            std::vector<int> acc_tag_ids = std::move(ret);

            std::set_intersection(new_tag_ids.begin(), new_tag_ids.end(),
                                  acc_tag_ids.begin(), acc_tag_ids.end(),
                                  std::back_inserter(ret));
        }

        return ret;
    }

    void Tagsistant::cacheTagsFromDB(soci::session& sql) {
        soci::rowset<soci::row> row = (sql.prepare << "select tag_id, tagname from tags");

        tags->clear();
        for(auto const& v : row)
            tags->add(v.get<std::string>(1),v.get<int>(0));
    }

    void Tagsistant::cacheTaggingFromDB(soci::session& sql) {
        soci::rowset<soci::row> inodes = (sql.prepare << "select inode, tag_id from tagging");

        tagging.clear();

        for(auto const& v : inodes)
            tagging[v.get<int>(1)].push_back(v.get<int>(0));
    }

    bool Tagsistant::checkTagnames(
    		const std::vector<std::string>& maybe_tagnames) const {
        for(auto const& v : maybe_tagnames)
            if(std::find(tags->getNames().begin(), tags->getNames().end(),
            		v) == tags->getNames().end())
                return false;
        return true;
    }

    const std::string& Tagsistant::getTagsistantDirectory() const {
    	return tagsistant_directory;
    }

    Tagsistant::~Tagsistant()
    {}
}

