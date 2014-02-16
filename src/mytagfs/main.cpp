
#include "tagsistant/Tagsistant.h"
#include "dir/TagsDir.h"

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <boost/program_options.hpp>

std::vector<mytagfsdir::Dir*> global_dirs;

int mytagfs_getattr(const char *path, struct stat *stbuf) {
	memset(stbuf, 0, sizeof(struct stat));

	if(strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0555;
		stbuf->st_nlink = 2;

		return 0;
	}

	for(auto dir : global_dirs)
		if(dir->getattr(path, stbuf) == 0)
			return 0;

	return -ENOENT;
}

int mytagfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	if(strcmp(path, "/") == 0) {
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);

		for(const auto dir : global_dirs)
			filler(buf, dir->getDirName().c_str(), NULL, 0);

		return 0;
	}

	for(auto dir : global_dirs)
		if(dir->readdir(path, buf, filler, offset, fi) == 0)
			return 0;

	return -ENOENT;
}

int mytagfs_readlink(const char *path, char *buf, size_t bufsiz) {
	for(auto dir : global_dirs)
		if(dir->readlink(path, buf, bufsiz) == 0)
			return 0;

	return -ENOENT;
}

int main(int argc, const char* argv[]) {
	if(argc == 0)
		return -1;

	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()("help,h", "produce help message")
	    			("repository", po::value<std::string>(), "specify the location of repository");

	po::variables_map vm;
	po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
	po::store(parsed, vm);
	po::notify(vm);

	if(vm.count("help")) {
		std::cout << "usage: " << argv[0] << " --repository <path> <mount point>" << std::endl;

		std::cout << desc << std::endl;

		return 0;
	}

	std::unique_ptr<tagsistant::Tagsistant> tagsistant;

	if(vm.count("repository"))
		tagsistant.reset(new tagsistant::Tagsistant(vm["repository"].as<std::string>()));
	else {
		std::cout << "Repository was not set" << std::endl;
		return -1;
	}

	std::vector<std::string> unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);

	std::vector<const char*> fuse_argv;
	fuse_argv.push_back(argv[0]);

	for(const auto& v : unrecognized)
		fuse_argv.push_back(v.c_str());

	mytagfsdir::TagsDir tags_dir(*tagsistant);
	global_dirs.push_back(&tags_dir);

	fuse_operations ops = {0};
	ops.getattr = mytagfs_getattr;
	ops.readdir = mytagfs_readdir;
	ops.readlink = mytagfs_readlink;

	return fuse_main(fuse_argv.size(), const_cast<char**>(fuse_argv.data()), &ops, NULL);
}

