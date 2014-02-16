/*
 * TagsDir.cpp
 *
 *  Created on: Feb 16, 2014
 *      Author: mdraven
 */

#include "TagsDir.h"
#include "tagsistant/FilenameAndPath.h"
#include "config.h"

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#include <cstring>

namespace mytagfsdir {

static void parse_path(const char* path, std::vector<std::string>& before_at,
		boost::optional<std::vector<std::string>>& after_at) {
	if(strncmp(path, "/tags/", 6) != 0)
		throw std::runtime_error("Path doesn't begin with '/tags/'");

	std::vector<std::string> chunks;
	const char* path_without_begin = path + 6; // /tags/
	boost::split(chunks, path_without_begin, boost::is_any_of("/"));

	before_at.clear();
	after_at.reset();

	size_t i = 0;
	for(; i < chunks.size(); ++i)
		if(chunks[i] == MYTAGFS_FILES_TAG) {
			after_at.reset(std::vector<std::string>());
			break;
		}
		else
			before_at.push_back(std::move(chunks[i]));

	++i;

	for(; i < chunks.size(); ++i)
		after_at->push_back(std::move(chunks[i]));
}

TagsDir::TagsDir(tagsistant::Tagsistant& tagsistant) :
	tagsistant(tagsistant) {}

int TagsDir::getattr(const char* path, struct stat* stbuf) {
	if(strcmp(path, "/tags") == 0) {
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFDIR | 0555;
		stbuf->st_nlink = 2;

		return 0;
	}
	else if(strncmp(path, "/tags/", 6) == 0) {
		std::vector<std::string> before_at;
		boost::optional<std::vector<std::string>> after_at;

		parse_path(path, before_at, after_at);

		if(!tagsistant.checkTagnames(before_at))
			return -ENOENT;

		if(!after_at || after_at->size() == 0) {
			memset(stbuf, 0, sizeof(struct stat));
			stbuf->st_mode = S_IFDIR | 0555;
			stbuf->st_nlink = 2;
		}
		else {
			boost::optional<tagsistant::FilenameAndPath> file_and_path = tagsistant.isFileContainsInTags(before_at,
					after_at.get()[0]);
			if(!file_and_path)
				return -ENOENT;

			memset(stbuf, 0, sizeof(struct stat));
			stbuf->st_mode = S_IFLNK | 0444;
			stbuf->st_nlink = 1;

			stbuf->st_size = file_and_path->getPathLen(); // without terminator
		}

		return 0;
	}

	return -ENOENT;
}

int TagsDir::readdir(const char* path, void* buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info* fi) {
	if(strcmp(path, "/tags") == 0) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);

		std::vector<std::string> tagnames = tagsistant.getTagnames();

		for(size_t i = 0; i < tagnames.size(); ++i)
			filler(buf, tagnames[i].c_str(), NULL, 0);

		return 0;
	}
	else if(strncmp(path, "/tags/", 6) == 0) {

		std::vector<std::string> tagnames = tagsistant.getTagnames();
		if(tagnames.size() == 0) {
			filler(buf, ".", NULL, 0);
			filler(buf, "..", NULL, 0);

			return 0;
		}

		std::vector<std::string> before_at;
		boost::optional<std::vector<std::string>> after_at;

		parse_path(path, before_at, after_at);

		std::vector<int> in_path;

		for(auto const& v : before_at) {
			auto it = std::find_if(tagnames.begin(), tagnames.end(),
					[&v](const std::string& t) {
				return v == t;
			});
			if(it == tagnames.end())
				return -ENOENT;

			in_path.push_back(std::distance(tagnames.begin(), it));
		}

		if(!after_at) {
			filler(buf, ".", NULL, 0);
			filler(buf, "..", NULL, 0);

			filler(buf, MYTAGFS_FILES_TAG, NULL, 0);

			for(size_t i = 0; i < tagnames.size(); ++i)
				if(std::find(in_path.begin(), in_path.end(), i) == in_path.end())
					filler(buf, tagnames[i].c_str(), NULL, 0);
		}
		else if(after_at->size() == 0) {
			std::vector<tagsistant::FilenameAndPath> filenames_and_paths = tagsistant.getPathsToFiles(before_at);
			if(filenames_and_paths.size() == 0)
				return -ENOENT;

			for(auto const& v : filenames_and_paths)
				filler(buf, v.getFilename().c_str(), NULL, 0);
		}

		return 0;
	}

	return -ENOENT;
}

int TagsDir::readlink(const char* path, char* buf, size_t bufsiz) {
	if(strncmp(path, "/tags/", 6) == 0) {
		std::vector<std::string> before_at;
		boost::optional<std::vector<std::string>> after_at;

		parse_path(path, before_at, after_at);

		if(!tagsistant.checkTagnames(before_at))
			return -ENOENT;

		if(!after_at || after_at->size() == 0) {
			return -ENOENT;
		}
		else {
			boost::optional<tagsistant::FilenameAndPath> file_and_path = tagsistant.isFileContainsInTags(before_at,
					after_at.get()[0]);
			if(!file_and_path)
				return -ENOENT;

			file_and_path->getPath(buf, bufsiz);

			return 0;
		}
	}

	return -ENOENT;
}

const std::string& TagsDir::getDirName() const {
	static std::string dir_name("tags");
	return dir_name;
}

TagsDir::~TagsDir() {}

} /* namespace mytagfsdir */
