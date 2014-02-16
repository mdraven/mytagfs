/*
 * Dir.h
 *
 *  Created on: Feb 16, 2014
 *      Author: mdraven
 */

#ifndef DIR_H_
#define DIR_H_

#include <string>

#define FUSE_USE_VERSION 30
#include <fuse.h>

namespace mytagfsdir {

class Dir {
public:
	virtual int getattr(const char *path, struct stat *stbuf) = 0;
	virtual int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi) = 0;
	virtual int readlink(const char *path, char *buf, size_t bufsiz) = 0;

	virtual const std::string& getDirName() const = 0;

	virtual ~Dir() {};
};

} /* namespace mytagfsdir */

#endif /* DIR_H_ */
