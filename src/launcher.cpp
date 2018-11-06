/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "s9x.h"

S9XLauncher::S9XLauncher(int argc, char ** argv)
:
	Application(argc, argv),
	_current(-1)
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
	} else if (_current != -1 && _roms.size() > 0) {
		if (key == KEY_LEFT) {
			_current = (_current + _roms.size() - 1) % _roms.size();
		} else if (key == KEY_RIGHT) {
			_current = (_current + 1) % _roms.size();
		} else if (key == KEY_OK) {
			suspend();
			_roms[_current].run();
			resume();
		} else {
			LOG_WARNING("S9XLauncher(), Unmappaed key event %d", key);
		}
		refresh();
	}
}

void S9XLauncher::onStorageRemoved(const std::string & device)
{
	std::vector<S9XRom>::iterator ite = _roms.begin();
	while (ite != _roms.end()) {
		if (ite->device() == device) {
			ite = _roms.erase(ite);
		} else {
			++ ite;
		}
	}
	LOG_DEBUG("S9XLauncher(), Disconnected %s, %d roms remain", device.c_str(), _roms.size());
	_current = _roms.size() > 0 ? 0 : -1;
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
				_roms.push_back(rom);
				LOG_DEBUG("S9XLauncher(), Added rom %s", rom.name().c_str());
			}
		}
		closedir (handle);
	}
	LOG_DEBUG("S9XLauncher(), Connected %s, %d roms available", device.c_str(), _roms.size());
	_current = _roms.size() > 0 ? 0 : -1;
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
	if (_current == -1 || _roms.size() == 0) {
		_title->setText("Please connect a disk with ROM files");
		_image->setPath("");
		_left->hide();
		_right->hide();
	} else {
		_title->setText(_roms[_current].name());
		_image->setPath(_roms[_current].cover());
		_left->show();
		_right->show();
	}
}
