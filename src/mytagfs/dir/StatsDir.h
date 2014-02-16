/*
 * StatsDir.h
 *
 *  Created on: Feb 16, 2014
 *      Author: mdraven
 */

#ifndef STATSDIR_H_
#define STATSDIR_H_

#include "Dir.h"
#include "tagsistant/Tagsistant.h"

namespace mytagfsdir {

class StatsDir : public Dir {
	tagsistant::Tagsistant& tagsistant;
public:
	StatsDir(tagsistant::Tagsistant& tagsistant);

	virtual int getattr(const char *path, struct stat *stbuf) override;
	virtual int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi) override;
	virtual int readlink(const char *path, char *buf, size_t bufsiz) override;
	virtual int open(const char *path, struct fuse_file_info *fi) override;
	virtual int read(const char *path, char *buf, size_t size, off_t offset,
				struct fuse_file_info *fi) override;

	virtual const std::string& getDirName() const;

	virtual ~StatsDir();
};

} /* namespace mytagfsdir */

#endif /* STATSDIR_H_ */
