/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#ifndef S9X_H
#define S9X_H

#include "ether.h"

class S9XCommand;
class S9XLauncher;

class S9XCommand {
public:
	S9XCommand(const std::string & path = "/media");
	~S9XCommand();
	bool start();
protected:
	std::string basedir() const;
	bool mkconf();
private:
	std::string _path;
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
	typedef struct {
		std::string device;
		std::string path;
		std::string cover;
		std::string name;
	} Rom;
	LineLayout * _vline;
	LineLayout * _hline;
	TextItem * _title;
	TextItem * _left;
	TextItem * _right;
	ImageItem * _image;
	std::list<Rom> _all;
	std::list<Rom>::iterator _cur;
};

#endif // S9X_H
