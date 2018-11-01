#include "sui.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void * MonitorFunc(void * ctx)
{
	((Monitor*)ctx)->run();
	return nullptr;
}

Monitor::Monitor(const std::string & path)
:
	_path(path),
	_fd(-1),
	_wd(-1)
{
}

void Monitor::scan()
{
	DIR * handle;
	if ((handle = opendir(_path.c_str())) == nullptr) {
		SDL_Log("Monitor(), unable to open %s: %s", _path.c_str(), strerror(errno));
	} else {
		struct dirent *e;
		struct stat st;
		while ((e = readdir(handle)) != nullptr) {
			std::string name       = e->d_name;
			std::string mountpoint = _path + "/" + name;
			if (name[0] != '.' && stat(mountpoint.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
				UiApplication::instance().post(new StorageAddedEvent(name, mountpoint));
			}
		}
		closedir (handle);
	}
}
void Monitor::run()
{
	_mutex.lock();
	scan();
	while (_fd != -1) {
		_mutex.unlock();
		char buf[BUF_LEN] __attribute__ ((aligned(8)));
		ssize_t r = read(_fd, buf, BUF_LEN);
		if (r == 0) {
			SDL_Log("Monitor(), Nothing to read: %s", strerror(errno));
		} else if (r == -1) {
			SDL_Log("Monitor(), Read error: %s", strerror(errno));
			break;
		} else {
			for (char * p = buf; p < buf + r;) {
				struct inotify_event * e = (struct inotify_event *) p;
				if (e->mask & IN_DELETE) {
					UiApplication::instance().post(new StorageRemovedEvent(e->name));
				} else if (e->mask & IN_CREATE) {
					std::string name       = e->name;
					std::string mountpoint = _path + "/" + name;
					UiApplication::instance().post(new StorageAddedEvent(name, mountpoint));
				}
				p += sizeof(struct inotify_event) + e->len;
			}
		}
		_mutex.lock();
	}
	_mutex.unlock();
}

bool Monitor::start()
{
	_mutex.lock();
	if (_fd != -1) {
		SDL_Log("Monitor(), Monitor already running");
	} else if ((_fd = inotify_init()) == -1) {
		SDL_Log("Monitor(), Failed to initialize inotify: %s", strerror(errno));
	} else if ((_wd = inotify_add_watch(_fd, _path.c_str(), IN_CREATE|IN_DELETE|IN_ISDIR)) == -1) {
		SDL_Log("Monitor(), Failed to watch directory %s: %s", _path.c_str(), strerror(errno));
	} else if(pthread_create(&_monitor, NULL, MonitorFunc, this)) {
		SDL_Log("Monitor(), Failed to start monitor");
	} else {
		SDL_Log("Monitor(), Started");
		_mutex.unlock();
		return true;
	}
	_mutex.unlock();
	return false;
}

void Monitor::stop()
{
	_mutex.lock();
	if (_fd == -1) {
		SDL_Log("Monitor(), Monitor not running");
		_mutex.unlock();
	} else {
		SDL_Log("Monitor(), Stopping thread");
		inotify_rm_watch(_fd, _wd);
		close(_fd);
		_fd = -1;
		_wd = -1;
		_mutex.unlock();
		if(pthread_join(_monitor, NULL)) {
			SDL_Log("Monitor(), Error joining thread");
		} else {
			SDL_Log("Monitor(), Stopped");
		}
	}
}
