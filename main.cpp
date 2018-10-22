/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "sui.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

static std::string rom = "";

class S9XLauncher : public UiMenu
{
public:

	S9XLauncher(const std::string & path) : UiMenu(), _path(path)
	{
	}

	// Refresh List of roms
	bool refresh()
	{
		_all.clear();
		DIR * handle;
		if ((handle = opendir(_path.c_str())) == nullptr) {
			SDL_Log("Unable to open roms directory");
		} else {
			struct dirent *ent;
			while ((ent = readdir(handle)) != nullptr) {
				char * p = strrchr(ent->d_name, '.');
				if (p && strcasecmp(p, ".smc") == 0) {
					_all.push_back(ent->d_name);
				}
			}
			closedir (handle);
		}
		_cur = _all.size() > 0 ? 0 : -1;
		_title->setText(_cur == -1 ? "No rom found, press any key to rescan" : _all[_cur]);
	}

	// Build Menu element then refresh
	UiObject * build()
	{
		_vline = new UiLineLayout(2, 300, 90);
		_title = new UiTextItem("", 48, 0xAAAAAAFF);
		_hline = new UiLineLayout(3, 900, 0);
		_hline->set(0, new UiTextItem("<", 64, 0x336699FF));
		_hline->set(1, _title);
		_hline->set(2, new UiTextItem(">", 64, 0x336699FF));
		_vline->set(0, new UiTextItem("SNES9X Launcher", 64, 0xFFFFFFFF));
		_vline->set(1, _hline);
		refresh();
		return _vline;
	}

	// Handle events
	void event(int eid)
	{
		if (_cur == -1 || _all.size() == 0) {
			refresh();
		} else if (eid == EV_CANCEL) {
			rom.clear();
			UiApplication::instance().quit();
		} else if (eid == EV_OK) {
			rom = _all[_cur];
			UiApplication::instance().quit();
		} else  if (eid == EV_LEFT) {
			_cur = (_all.size() + _cur - 1) % _all.size();
			_title->setText(_all[_cur]);
		} else if (eid == EV_RIGHT) {
			_cur = (_all.size() + _cur + 1) % _all.size();
			_title->setText(_all[_cur]);
		} else {
			SDL_Log("Unhandled event %d", eid);
		}
	}

private:

	// Priv Data
	UiLineLayout * _vline;
	UiLineLayout * _hline;
	UiTextItem * _title;
	std::vector<std::string> _all;
	int _cur;
	std::string _path;

};

int main(int argc, char ** argv)
{
	if (!UiApplication::instance().start(1280, 720)) {
		SDL_Log("Failed to setup launcher");
	} else {
		std::string path = "/media/sda1";
		S9XLauncher * launcher = new S9XLauncher(path);
		UiApplication::instance().show(launcher);
		UiApplication::instance().run();
		if (!rom.empty()) {

			std::string cmd = "/usr/bin/snes9x -nomouse " + path + "/" + rom;

			SDL_Log("Running game %s", rom.c_str());
			UiApplication::instance().stop();
			system(cmd.c_str());
			//std::string fpath = path + "/" + rom;
			//execl("/usr/bin/snes9x", "snes9x", "-nomouse", fpath.c_str(), nullptr);
			//execl("/usr/bin/strace", "strace", "-o", "/root/strace.log", "/usr/bin/snes9x", "-nomouse", fpath.c_str(), nullptr);
		}
		return 0;
	}
	return 1;
}
