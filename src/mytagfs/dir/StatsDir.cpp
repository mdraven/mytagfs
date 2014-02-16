/*
 * StatsDir.cpp
 *
 *  Created on: Feb 16, 2014
 *      Author: mdraven
 */

#include "StatsDir.h"

#include <cstring>

namespace mytagfsdir {

const static char repository_path_str[] = "repository path: ";

static char* copy_strings(char *buf, size_t& size, off_t& offset,
		const char* str, size_t str_sz) {
	if(offset >= 0 && offset < str_sz) {
		size_t p;
		if(offset + size < str_sz)
			p = size;
		else
			p = str_sz - offset;

		memcpy(buf, str + offset, p);
		buf += p;
		size -= p;

		if(size != 0)
			offset = 0;
	}
	else if(offset > 0)
		offset -= str_sz;

	return buf;
}

StatsDir::StatsDir(tagsistant::Tagsistant& tagsistant) :
	tagsistant(tagsistant) {}

int StatsDir::getattr(const char* path, struct stat* stbuf) {
	if(strcmp(path, "/stats") == 0) {
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFDIR | 0555;
		stbuf->st_nlink = 2;

		return 0;
	}
	else if(strcmp(path, "/stats/configuration") == 0) {
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;

		stbuf->st_size = sizeof(repository_path_str)-1 + tagsistant.getTagsistantDirectory().size();

		return 0;
	}

	return -ENOENT;
}

int StatsDir::readdir(const char* path, void* buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info* fi) {
	if(strcmp(path, "/stats") == 0) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		filler(buf, "configuration", NULL, 0);

		return 0;
	}

	return -ENOENT;
}

int StatsDir::readlink(const char* path, char* buf, size_t bufsiz) {
	return -ENOENT;
}

const std::string& StatsDir::getDirName() const {
	static std::string dir_name("stats");
	return dir_name;
}

int StatsDir::read(const char* path, char* buf, size_t size, off_t offset,
		struct fuse_file_info* fi) {
	if(strcmp(path, "/stats/configuration") == 0) {

		size_t sz = size;
		buf = copy_strings(buf, sz, offset, repository_path_str, sizeof(repository_path_str)-1);
		buf = copy_strings(buf, sz, offset, tagsistant.getTagsistantDirectory().c_str(),
				tagsistant.getTagsistantDirectory().size());

		size_t was_read = size - sz;

		return was_read;
	}

	return -ENOENT;
}

int StatsDir::open(const char* path, struct fuse_file_info* fi) {
	if(strcmp(path, "/stats/configuration") == 0)
		return 0;
	return -ENOENT;
}

StatsDir::~StatsDir() {}

} /* namespace mytagfsdir */
