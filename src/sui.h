#ifndef SIMPLE_UI_H
#define SIMPLE_UI_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#define EV_INPUT	1
#define EV_ADDED	2
#define EV_REMOVED	3

#define KEY_OK		101
#define KEY_CANCEL	102
#define KEY_LEFT	103
#define KEY_RIGHT	104
#define KEY_UP		105
#define KEY_DOWN	106


class Mutex;
class Monitor;

class UiObject;
class UiItem;
class UiLayout;
class UiTextItem;
class UiMenu;
class UiApplication;

class Mutex
{
public:
	Mutex();
	~Mutex();
	void lock();
	void unlock();
private:
	pthread_mutex_t _mutex;
};

class Monitor
{
public:
	Monitor(const std::string & path = "/media");
	bool start();
	void stop();
	void run();
	void scan();
private:
	Mutex _mutex;
	pthread_t _monitor;
	std::string _path;
	int _fd;
	int _wd;
};

class Event
{
public:
	Event(int type) : _type(type) {}
	virtual ~Event() {}
	int type() const { return _type; }
private:
	int _type;
};

class StorageAddedEvent : public Event
{
public:
	StorageAddedEvent(const std::string & device, const std::string & mountpoint)
	: Event(EV_ADDED), _device(device), _mountpoint(mountpoint) {}
	~StorageAddedEvent() {}
	std::string device() const { return _device; }
	std::string mountpoint() const { return _mountpoint; }
private:
	std::string _device;
	std::string _mountpoint;
};

class StorageRemovedEvent : public Event
{
public:
	StorageRemovedEvent(const std::string & device)
	: Event(EV_REMOVED), _device(device) {}
	~StorageRemovedEvent() {}
	std::string device() const { return _device; }
private:
	std::string _device;
};

class InputEvent : public Event
{
public:
	InputEvent(int key)
	: Event(EV_INPUT), _key(key) {}
	~InputEvent() {}
	int key() const { return _key; }
private:
	int _key;
};

class UiObject
{
public:
	UiObject();
	virtual ~UiObject();
	virtual void render(SDL_Renderer * renderer) = 0;
	void moveTo(int x, int y);
	void moveBy(int dx, int dy);
	int x() const;
	int y() const;
	int absx() const;
	int absy() const;
	void attach(UiLayout * layout);
	void detach();
	UiLayout * layout();
	const UiLayout * layout() const;
private:
	UiLayout * _layout;
	int _x;
	int _y;
};

class UiItem : public UiObject
{
public:
	UiItem();
	virtual ~UiItem();
	void invalidate();
	void render(SDL_Renderer * renderer);
	void update();
	SDL_Color make_color(int rgba);
	int width() const;
	int height() const;
	void hide();
	void show();
	void setVisible(bool visible);
protected:
	virtual SDL_Texture * prepare() = 0;
private:
	SDL_Texture * _texture;
	int _width;
	int _height;
	bool _visible;
};

class UiTextItem : public UiItem
{
public:
	UiTextItem(const std::string & txt, int size, int color);
	virtual ~UiTextItem();
	void setText(const std::string & text);
	void setColor(int color);
	void setSize(int size);
protected:
	SDL_Texture * prepare();
private:
	std::string _text;
	int _size;
	int _color;
};

class UiImageItem : public UiItem
{
public:
	UiImageItem(int maxWidth, int maxHeight, const std::string & path);
	virtual ~UiImageItem();
	void setPath(const std::string & path);
protected:
	SDL_Texture * prepare();
private:
	std::string _path;
	int _maxWidth;
	int _maxHeight;
};

class UiLayout : public UiObject
{
public:
	UiLayout();
	virtual ~UiLayout();
	UiObject * get(int index);
	const UiObject * get(int index) const;
	void set(int index, UiObject * obj);
	int count() const;
	void update();
	void render(SDL_Renderer * renderer);
protected:
	void clear();
	void resize(int count);
	virtual void refresh() = 0;
private:
	std::vector<UiObject *> _childs;
};

class UiLineLayout : public UiLayout
{
public:
	UiLineLayout(int count, int length, int angle);
	virtual ~UiLineLayout();
	void setLength(int length);
	void setAngle(int angle);
	int length() const;
	int angle() const;
protected:
	virtual void refresh();
private:
	int _length;
	int _angle;
};

class UiMenu
{
public:
	UiMenu();
	virtual ~UiMenu();
	void render(SDL_Renderer * renderer);
	virtual void event(Event * event) = 0;
	void show();
	void hide();
protected:
	void setRoot(UiObject * root);
private:
	UiObject * _root;
	int _width;
	int _height;
};

class UiApplication
{
public:
	static UiApplication & instance();
	bool start(int width, int height);
	void stop();
	void show(UiMenu * menu);
	void quit();
	void run();
	void post(Event * event);
	SDL_Renderer * renderer();
	const SDL_Renderer * renderer() const;
protected:
	UiApplication();
	~UiApplication();
private:
	void input();
	void render();
private:
	SDL_Window * _window;
	SDL_Renderer * _renderer;
	std::list<SDL_Joystick *> _joysticks;
	UiMenu * _menu;
	bool _quit;
	std::list<Event*> _events;
	Mutex _mutex;
	Monitor _monitor;
};

#endif // SIMPLE_UI_H
