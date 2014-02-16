/*
 * TagsDir.h
 *
 *  Created on: Feb 16, 2014
 *      Author: mdraven
 */

#ifndef TAGSDIR_H_
#define TAGSDIR_H_

#include "Dir.h"
#include "tagsistant/Tagsistant.h"

#include <string>

namespace mytagfsdir {

class TagsDir : public Dir {
	tagsistant::Tagsistant& tagsistant;
public:
	TagsDir(tagsistant::Tagsistant& tagsistant);

	virtual int getattr(const char *path, struct stat *stbuf) override;
	virtual int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi) override;
	virtual int readlink(const char *path, char *buf, size_t bufsiz) override;

	virtual const std::string& getDirName() const;

	virtual ~TagsDir();
};

} /* namespace mytagfsdir */

#endif /* TAGSDIR_H_ */
