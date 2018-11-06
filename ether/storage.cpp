/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

#define BUF_LEN (8 * (sizeof(struct inotify_event) + NAME_MAX + 1))

Storage & Storage::instance()
{
	static Storage self;
	return self;
}

Storage::Storage()
:
	Service("storage"),
	_path("/media"),
	_fd(-1)
{
}

Storage::~Storage()
{
}

bool Storage::setup()
{
	int fd, wd;
	if ((fd = inotify_init()) == -1) {
		LOG_ERROR("Storage(), Failed to initialize inotify: %s (%d)", strerror(errno), errno);
	} else if ((wd = inotify_add_watch(fd, _path.c_str(), IN_CREATE|IN_DELETE|IN_ISDIR)) == -1) {
		LOG_ERROR("Storage(), Failed to watch directory %s: %s (%d)", _path.c_str(), strerror(errno), errno);
		close(_fd);
	} else {
		_fd = fd;
		_wd = wd;
		return true;
	}
	return false;
}

void Storage::teardown()
{
	if (_fd != -1) {
		inotify_rm_watch(_fd, _wd);
		close(_fd);
		_wd = -1;
		_fd = -1;
	}
}

void Storage::run()
{
	populate();
	for (;;) {
		char buf[BUF_LEN] __attribute__ ((aligned(8)));
		ssize_t r = read(_fd, buf, BUF_LEN);
		if (r == 0) {
			LOG_ERROR("Storage(), Nothing to read: %s (%d)", strerror(errno), errno);
		} else if (r == -1) {
			LOG_ERROR("Storage(), Read error: %s (%d)", strerror(errno), errno);
			break;
		} else {
			for (char * p = buf; p < buf + r;) {
				struct inotify_event * e = (struct inotify_event *) p;
				if (e->mask & IN_DELETE) {
					notify("removed", std::vector<Variant>({e->name}));
				} else if (e->mask & IN_CREATE) {
					notify("added", std::vector<Variant>({e->name, _path + "/" + e->name}));
				}
				p += sizeof(struct inotify_event) + e->len;
			}
		}
	}
}

void Storage::populate()
{
	DIR * handle;
	if ((handle = opendir(_path.c_str())) == nullptr) {
		LOG_WARNING("Monitor(), Unable to open %s: %s", _path.c_str(), strerror(errno));
	} else {
		LOG_DEBUG("Monitor(), Notifying initial devices");
		struct dirent *e;
		struct stat st;
		while ((e = readdir(handle)) != nullptr) {
			std::string mountPoint = _path + "/" + e->d_name;
			if (e->d_name[0] != '.' && stat(mountPoint.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
				notify("added", std::vector<Variant>({e->d_name, mountPoint}));
			}
		}
		closedir (handle);
	}
}
