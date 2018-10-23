/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "sui.h"


static std::string SelectedRom = "";

class S9XLauncher : public UiMenu
{
public:

	S9XLauncher() : UiMenu()
	{
		_cur = _all.end();
		_vline = new UiLineLayout(2, 300, 90);
		_hline = new UiLineLayout(3, 900, 0);
		_hline->set(0, _left = new UiTextItem("<", 48, 0x336699FF));
		_hline->set(1, _title = new UiTextItem("", 32, 0xAAAAAAFF));
		_hline->set(2, _right = new UiTextItem(">", 48, 0x336699FF));
		_vline->set(0, new UiTextItem("SNES9X Launcher", 64, 0xFFFFFFFF));
		_vline->set(1, _hline);
		setRoot(_vline);
	}

	virtual ~S9XLauncher()
	{
	}

	void event(Event * e)
	{
		if (e->type() == EV_INPUT) {
			int key = ((InputEvent*)e)->key();
			if (_cur == _all.end()) {
				refresh();
			} else if (key == KEY_CANCEL) {
				SelectedRom.clear();
				UiApplication::instance().quit();
			} else if (key == KEY_OK) {
				SelectedRom = _cur->path;
				UiApplication::instance().quit();
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
		} else {
			_title->setText(_cur->name);
			_left->show();
			_right->show();
		}
	}

private:

	typedef struct {
		std::string device;
		std::string path;
		std::string name;
	} Rom;

	// Priv Data
	UiLineLayout * _vline;
	UiLineLayout * _hline;
	UiTextItem * _title;
	UiTextItem * _left;
	UiTextItem * _right;

	std::list<Rom> _all;
	std::list<Rom>::iterator _cur;
};

int main(int argc, char ** argv)
{
	if (!UiApplication::instance().start(1280, 720)) {
		SDL_Log("Failed to setup launcher");
	} else {
		S9XLauncher * launcher = new S9XLauncher();
		UiApplication::instance().show(launcher);
		UiApplication::instance().run();
		if (!SelectedRom.empty()) {
			SDL_Log("Starting ROM at: %s", SelectedRom.c_str());
			std::string cmd = "/usr/bin/snes9x -nomouse '" + SelectedRom + "'";
			UiApplication::instance().stop();
			system(cmd.c_str());
		}
		return 0;
	}
	return 1;
}
