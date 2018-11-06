/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "s9x.h"

S9XRom::S9XRom(const std::string & device, const std::string & basedir, const std::string & filename)
:
	_device(device),
	_basedir(basedir),
	_filename(filename)
{
	struct stat st;

	_name  = _filename.substr(0, _filename.length() - 4); // .smc
	_cover = "";
	_path  = _basedir + "/" + _filename;

	int p = _path.find('\'');
	while (p != std::string::npos) {
		_path = _path.replace(p, 1, "'\\''");
		p = _path.find('\'', p + 4);
	}

	if (stat((_basedir + "/" + _name + ".jpg").c_str(), &st) == 0) {
		_cover = _basedir + "/" + _name + ".jpg";
	} else if (stat((_basedir + "/" + _name + ".jpeg").c_str(), &st) == 0) {
		_cover = _basedir + "/" + _name + ".jpeg";
	} else if (stat((_basedir + "/" + _name + ".png").c_str(), &st) == 0) {
		_cover = _basedir + "/" + _name + ".png";
	}
}

S9XRom::~S9XRom()
{
}

std::string S9XRom::cover() const
{
	return _cover;
}

std::string S9XRom::name() const
{
	return _name;
}

std::string S9XRom::path() const
{
	return _path;
}

std::string S9XRom::device() const
{
	return _device;
}

std::string S9XRom::filename() const
{
	return _filename;
}

std::string S9XRom::basedir() const
{
	return _basedir;
}

bool S9XRom::run()
{
	if (_path.empty()) {
		LOG_ERROR("S9XRom(), Unable to start, no path");
	} else if (!mkconf()) {
		LOG_ERROR("S9XRom(), Failed to created configuration");
	} else {
		LOG_DEBUG("S9XRom(), Starting ROM %s", _name.c_str());
		std::string cmd = "/usr/bin/snes9x -nomouse -nostdconf -conf /tmp/snes9x.conf '" + _path + "'";
		system(cmd.c_str());
		return true;
	}
	return false;
}

bool S9XRom::mkconf()
{
	std::ofstream handle;
	handle.open("/tmp/snes9x.conf");
	if (!handle.is_open()) {
		LOG_ERROR("S9XRom(), Failed to open snes9x configuration file");
	} else {
		handle << "[Display]" << std::endl;
		handle << "DisplayFrameRate = TRUE" << std::endl;
		handle << "[Settings]" << std::endl;
		//handle << "FrameSkip = 1" << std::endl;
		handle << "[Unix]" << std::endl;
		handle << "BaseDir=" << _basedir << "/snes9x" << std::endl;
		handle << "[Unix/SDL2 Controls]" << std::endl;
		handle << "J00:B10 = ExitEmu" << std::endl;
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
		handle << "J01:Axis0 = Joypad2 Axis Left/Right T=50%" << std::endl;
		handle << "J01:Axis1 = Joypad2 Axis Up/Down T=50%" << std::endl;
		handle << "J01:B1 = Joypad2 A" << std::endl;
		handle << "J01:B0 = Joypad2 B" << std::endl;
		handle << "J01:B2 = Joypad2 X" << std::endl;
		handle << "J01:B3 = Joypad2 Y" << std::endl;
		handle << "J01:B4 = Joypad2 L" << std::endl;
		handle << "J01:B5 = Joypad2 R" << std::endl;
		handle << "J01:B9 = Joypad2 Select" << std::endl;
		handle << "J01:B8 = Joypad2 Start" << std::endl;
		handle.close();
		return true;
	}
	return false;
}
