/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "s9x.h"

S9XLauncher::S9XLauncher(int argc, char ** argv)
:
	Application(argc, argv),
	_cur(_all.end())
{
}

S9XLauncher::~S9XLauncher()
{
}

Object * S9XLauncher::build()
{
	_vline = new LineLayout(3, 450, 90);
	_hline = new LineLayout(3, 900, 0);
	_hline->set(0, _left = new TextItem("<", 48, 0x336699FF));
	_hline->set(1, _title = new TextItem("", 32, 0xAAAAAAFF));
	_hline->set(2, _right = new TextItem(">", 48, 0x336699FF));
	_vline->set(0, new TextItem("SNES9X Launcher", 64, 0xFFFFFFFF));
	_vline->set(1, _image = new ImageItem(450, 300, ""));
	_vline->set(2, _hline);
	return _vline;
}

void S9XLauncher::onKeyPressed(int key)
{
	if (key == KEY_CANCEL) {
		LOG_DEBUG("S9XLauncher(), Stopping Application");
		quit();
	} else if (_cur != _all.end()) {
		if (key == KEY_LEFT) {
			_cur = -- _cur == _all.end() ? -- _cur : _cur;
		} else if (key == KEY_RIGHT) {
			_cur = ++ _cur == _all.end() ? _all.begin() : _cur;
		} else if (key == KEY_OK) {
			_cur->run();
		} else {
			LOG_WARNING("S9XLauncher(), Unmappaed key event %d", key);
		}
		refresh();
	}
}

void S9XLauncher::onStorageRemoved(const std::string & device)
{
	std::list<S9XRom>::iterator ite = _all.begin();
	while (ite != _all.end()) {
		if (ite->device() == device) {
			ite = _all.erase(ite);
		} else {
			++ ite;
		}
	}
	LOG_DEBUG("S9XLauncher(), Disconnected %s, %d roms remain", device.c_str(), _all.size());
	_cur = _all.size() > 0 ? _all.begin() : _all.end();
	refresh();
}

void S9XLauncher::onStorageAdded(const std::string & device, const std::string & mountPoint)
{
	DIR * handle;
	// TODO: Sleep needed because we need to listen to a mount event on subdir, not dir created
	sleep(1);
	if ((handle = opendir(mountPoint.c_str())) == nullptr) {
		LOG_ERROR("S9XLauncher(), Unable to open roms directory");
	} else {
		struct dirent *ent;
		while ((ent = readdir(handle)) != nullptr) {
			char * p = strrchr(ent->d_name, '.');
			if (p && strcasecmp(p, ".smc") == 0) {
				S9XRom rom(device, mountPoint, ent->d_name);
				_all.push_back(rom);
				LOG_DEBUG("S9XLauncher(), Added rom %s", rom.name().c_str());
			}
		}
		closedir (handle);
	}
	_cur = _all.size() > 0 ? _all.begin() : _all.end();
	LOG_DEBUG("S9XLauncher(), Connected %s, %d roms available", device.c_str(), _all.size());
	refresh();
}

void S9XLauncher::onCacheLoad(const std::string & path)
{
	LOG_DEBUG("S9XLauncher(), Cached image at %s", path.c_str());
	if (_image && _image->path() == path) {
		refresh();
	}
}

void S9XLauncher::onCacheError(const std::string & path)
{
	LOG_WARNING("S9XLauncher(), Failed to cache image at %s", path.c_str());
	if (_image && _image->path() == path) {
		_image->setPath("");
		refresh();
	}
}

void S9XLauncher::refresh()
{
	if (_cur == _all.end()) {
		_title->setText("Please connect a disk with ROM files");
		_left->hide();
		_right->hide();
		_image->setPath("");
	} else {
		_title->setText(_cur->name());
		_left->show();
		_right->show();
		_image->setPath(_cur->cover());
	}
}
