/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#ifndef S9X_H
#define S9X_H

#include "ether.h"

class S9XRom;
class S9XLauncher;

class S9XRom {
public:
	S9XRom(const std::string & device, const std::string & basedir, const std::string & filename);
	~S9XRom();
	bool run();
	std::string cover() const;
	std::string name() const;
	std::string path() const;
	std::string device() const;
	std::string filename() const;
	std::string basedir() const;
protected:
	bool mkconf();
private:
	std::string _device;
	std::string _basedir;
	std::string _filename;
	std::string _path;
	std::string _cover;
	std::string _name;
};

class S9XLauncher : public Application {
public:
	S9XLauncher(int argc, char ** argv);
	virtual ~S9XLauncher();
	virtual Object * build();
protected:
	void onKeyPressed(int key);
	void onStorageAdded(const std::string & device, const std::string & mountPoint);
	void onStorageRemoved(const std::string & device);
	void onCacheLoad(const std::string & path);
	void onCacheError(const std::string & path);
private:
	void refresh();
	LineLayout * _vline;
	LineLayout * _hline;
	TextItem * _title;
	TextItem * _left;
	TextItem * _right;
	ImageItem * _image;
	std::vector<S9XRom> _roms;
	int _current;
};

#endif // S9X_H
