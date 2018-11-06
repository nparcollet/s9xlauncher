/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "s9x.h"

S9XCommand::S9XCommand(const std::string & path)
:
	_path(path)
{
}

S9XCommand::~S9XCommand()
{
}

bool S9XCommand::start()
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


std::string S9XCommand::basedir() const
{
	int p = _path.rfind("/");
	if (p != std::string::npos) {
		return _path.substr(0, p);
	} else {
		return "/";
	}
}

bool S9XCommand::mkconf()
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
