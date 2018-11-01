/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "sui.h"


class S9XLauncher
{
public:

	S9XLauncher(const std::string & path)
	:
		_path(path)
	{
	}

	bool start()
	{
		if (_path.empty()) {
			SDL_Log("Unable to start, no path specified");
		} else if (!mkconf()) {
			SDL_Log("Unable to start, failed to created configuration");
		} else {
			SDL_Log("Starting ROM at %s (%s)", _path.c_str(), basedir().c_str());
			std::string cmd = "/usr/bin/snes9x -nomouse -nostdconf -conf /tmp/snes9x.conf '" + _path + "'";
			system(cmd.c_str());
			SDL_PumpEvents();
			SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
			return true;
		}
		return false;
	}

protected:

	std::string basedir()
	{
		int p = _path.rfind("/");
		if (p != std::string::npos) {
			return _path.substr(0, p);
		} else {
			return "/";
		}
	}

	bool mkconf()
	{
		std::ofstream handle;
		handle.open("/tmp/snes9x.conf");
		if (!handle.is_open()) {
			SDL_Log("Failed to open snes9x configuration file");
		} else {
			handle << "[Display]" << std::endl;
			handle << "DisplayFrameRate = TRUE" << std::endl;
			handle << "[Settings]" << std::endl;
			handle << "FrameSkip = 1" << std::endl;
			handle << "[Unix]" << std::endl;
			handle << "BaseDir=" << basedir() << "/snes9x" << std::endl;
			handle << "[Unix/SDL2 Controls]" << std::endl;
			handle << "J00:Axis0 = Joypad1 Axis Left/Right T=50%" << std::endl;
			handle << "J00:Axis1 = Joypad1 Axis Up/Down T=50%" << std::endl;
			handle << "J00:B1 = Joypad1 A" << std::endl;
			handle << "J00:B0 = Joypad1 B" << std::endl;
			handle << "J00:B2 = Joypad1 X" << std::endl;
			handle << "J00:B3 = Joypad1 Y" << std::endl;
			handle << "J00:B4 = Joypad1 L" << std::endl;
			handle << "J00:B5 = Joypad1 R" << std::endl;
			handle << "J00:B9 = Joypad1 Select" << std::endl;
			handle << "J00:B8 = Joypad1 Start" << std::endl;
			handle << "J00:B10 = ExitEmu" << std::endl;
			handle.close();
			return true;
		}
		return false;
	}

private:
	std::string _path;
};

class S9XHome : public UiMenu
{
public:

	S9XHome()
	:
		UiMenu(),
		_triggered(time(nullptr))
	{
		_cur = _all.end();
		_vline = new UiLineLayout(3, 450, 90);
		_hline = new UiLineLayout(3, 900, 0);
		_hline->set(0, _left = new UiTextItem("<", 48, 0x336699FF));
		_hline->set(1, _title = new UiTextItem("", 32, 0xAAAAAAFF));
		_hline->set(2, _right = new UiTextItem(">", 48, 0x336699FF));
		_vline->set(0, new UiTextItem("SNES9X Launcher", 64, 0xFFFFFFFF));
		_vline->set(1, _image = new UiImageItem(450, 300, ""));
		_vline->set(2, _hline);
		setRoot(_vline);
	}

	virtual ~S9XHome()
	{
	}

	void event(Event * e)
	{
		if (e->type() == EV_INPUT) {
			int key = ((InputEvent*)e)->key();
			if (_cur == _all.end()) {
				refresh();
			} else if (key == KEY_CANCEL) {
				UiApplication::instance().quit();
			} else if (key == KEY_OK) {
				if (time(nullptr) - _triggered > 5) {
					S9XLauncher launcher(_cur->path);
					launcher.start();
					_triggered = time(nullptr);
				}
			} else  if (key == KEY_LEFT) {
				-- _cur;
				if (_cur == _all.end()) {
					-- _cur;
				}
				refresh();
			} else if (key == KEY_RIGHT) {
				++ _cur;
				if (_cur == _all.end()) {
					_cur = _all.begin();
				}
				refresh();
			}
		} else if (e->type() == EV_ADDED) {
			StorageAddedEvent * sae = (StorageAddedEvent*)e;
			on_connected(sae->device(), sae->mountpoint());
		} else if (e->type() == EV_REMOVED) {
			StorageRemovedEvent * sre = (StorageRemovedEvent*)e;
			on_disconnected(sre->device());
		}
	}

protected:

	void on_disconnected(const std::string & device)
	{
		std::list<Rom>::iterator ite = _all.begin();
		while (ite != _all.end()) {
			if (ite->device == device) {
				ite = _all.erase(ite);
			} else {
				++ ite;
			}
		}
		SDL_Log("Disconnected %s, %d roms remain", device.c_str(), _all.size());
		_cur = _all.size() > 0 ? _all.begin() : _all.end();
		refresh();
	}

	void on_connected(const std::string & device, const std::string & mountpoint)
	{
		DIR * handle;
		sleep(1); // Because we need to listen to a mount event on subdir, not directory created ...
		if ((handle = opendir(mountpoint.c_str())) == nullptr) {
			SDL_Log("Unable to open roms directory");
		} else {
			struct dirent *ent;
			while ((ent = readdir(handle)) != nullptr) {
				char * p = strrchr(ent->d_name, '.');
				if (p && strcasecmp(p, ".smc") == 0) {
					Rom rom;
					rom.device = device;
					rom.path   = mountpoint + "/" + ent->d_name;
					rom.name   = std::string(ent->d_name).substr(0, strlen(ent->d_name) - 4);
					rom.cover  = mountpoint + "/" +  rom.name + ".jpg";
					_all.push_back(rom);
					SDL_Log("Added rom %s from %s", rom.name.c_str(), rom.device.c_str());
				}
			}
			closedir (handle);
		}
		_cur = _all.size() > 0 ? _all.begin() : _all.end();
		SDL_Log("Connected %s, %d roms available", device.c_str(), _all.size());
		refresh();
	}

	void refresh()
	{
		if (_cur == _all.end()) {
			_title->setText("Please connect a disk with ROM files");
			_left->hide();
			_right->hide();
			_image->setPath("");
		} else {
			_title->setText(_cur->name);
			_left->show();
			_right->show();
			_image->setPath(_cur->cover);
		}
	}

private:

	typedef struct {
		std::string device;
		std::string path;
		std::string cover;
		std::string name;
	} Rom;

	// Priv Data
	UiLineLayout * _vline;
	UiLineLayout * _hline;
	UiTextItem * _title;
	UiTextItem * _left;
	UiTextItem * _right;
	UiImageItem * _image;

	std::list<Rom> _all;
	std::list<Rom>::iterator _cur;
	time_t _triggered;
};

int main(int argc, char ** argv)
{
	UiApplication & app = UiApplication::instance();
	if (!app.start(1280, 720)) {
		SDL_Log("Failed to setup launcher");
	} else {
		S9XHome * home = new S9XHome();
		home->show();
		app.run();
		return 0;
	}
	return 1;
}
