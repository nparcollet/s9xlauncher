/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#ifndef ETHER_H
#define ETHER_H

// C++ Includes
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>

// Standard C Includes
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

// System C Includes
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/inotify.h>
#include <sys/stat.h>

// SDL Includes
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

// Helper macros that serve various purposes.
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

// Logger macros
#define LOG_DEBUG(fmt, ...)		Logger::Debug(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)	Logger::Warning(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)		Logger::Error(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)		Logger::Fatal(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

// Class forward declaration
class Logger;
class Mutex;
class Locker;
class Condition;
class Variant;
class Service;
class Dispatcher;
class Event;
class Application;
class Storage;
class Cache;
class Object;
class Item;
class Layout;
class LineLayout;
class TextItem;
class ImageItem;

// Simple Key Mapping
#define KEY_OK		101
#define KEY_CANCEL	102
#define KEY_LEFT	103
#define KEY_RIGHT	104
#define KEY_UP		105
#define KEY_DOWN	106

// Static class providing logging facilities
class Logger {
public:
	static void Debug(const char * file, const char * func, int line, const char * format, ...);
	static void Warning(const char * file, const char * func, int line, const char * format, ...);
	static void Error(const char * file, const char * func, int line, const char * format, ...);
	static void Fatal(const char * file, const char * func, int line, const char * format, ...);
};

// Object used to control access to resources in multithreaded environment
class Mutex {
	friend class Condition;
public:
	Mutex();
	~Mutex();
	void lock();
	void unlock();
private:
	pthread_mutex_t _mutex;
};

// Auto lock mechanism for mutex objects. Instanciate at function start and it is active
class Locker {
public:
	Locker(Mutex & mutex);
	~Locker();
private:
	Mutex & _mutex;
};

// Inter thread condition monitoring and triggering
class Condition {
public:
	Condition();
	~Condition();
	void wait(Mutex & mutex);
	void signal();
	void broadcast();
private:
	pthread_cond_t _cond;
};

// A json comptatible variant implementation
class Variant {
public:
	typedef enum { TypeNull, TypeBoolean, TypeNumber, TypeString, TypeArray, TypeObject } Type;
    Variant();
    Variant(bool value);
	Variant(int value);
	Variant(float value);
	Variant(double value);
	Variant(const std::string & value);
	Variant(const char * value);
	Variant(const std::list<Variant> & value);
	Variant(const std::map<std::string, Variant> & value);
	~Variant();
	Variant & operator=(const Variant & other);
	Variant(const Variant & other);
	int type() const;
	double asNumber() const;
	bool asBoolean() const;
	std::string	asString() const;
	std::list<Variant> asArray() const;
	std::map<std::string, Variant> asObject() const;
	void set(bool value);
	void set(int value);
	void set(double value);
	void set(const std::string value);
	void set(const std::list<Variant> & value);
	void set(const std::map<std::string, Variant> & value);
	void clear();
private:
    void * _ptr;
    int _type;
};

// Service running on a separate thread that post can events to the dispatcher
class Service {
public:
	Service(const std::string & name);
	virtual ~Service();
	bool start();
	void stop();
	bool running();
	std::string name() const;
protected:
	virtual bool setup() = 0;
	virtual void teardown() = 0;
	virtual void run() = 0;
	void notify(const std::string & type, const std::vector<Variant> & args);
private:
	static void * ThreadFunc(void * ctx);
	std::string _name;
	bool _running;
	Mutex _mutex;
	pthread_t _thread;
};

// An event object managed by the application dispatcher
class Event {
public:
	Event(const std::string & sender, const std::string & type, const std::vector<Variant> & args);
	~Event();
	std::string sender() const;
	std::string type() const;
	std::string string(int index) const;
	double number(int index) const;
private:
	std::string _sender;
	std::string _type;
	std::vector<Variant> _args;
};

// Object in charge of receiving and dispatching events on the main thread
class Dispatcher {
public:
	static Dispatcher & instance();
	bool attach(Application * app);
	void post(const Event & event);
	bool process();
protected:
	Dispatcher();
	~Dispatcher();
private:
	Application * _app;
	std::list<Event> _queue;
	Mutex _mutex;
};

// Main application interface, to be subclassed
class Application {
public:
	Application(int argc, char ** argv);
	virtual ~Application();
	int run(int width, int height);
	void dispatch(const Event & event);
	void quit();
	virtual Object * build() = 0;
protected:
	virtual void onKeyPressed(int key);
	virtual void onStorageAdded(const std::string & device, const std::string & mountPoint);
	virtual void onStorageRemoved(const std::string & device);
	virtual void onCacheLoad(const std::string & path);
	virtual void onCacheError(const std::string & path);
	virtual void onCustomEvent(const Event & event);
private:
	void input();
private:
	SDL_Window * _window;
	SDL_Renderer * _renderer;
	std::list<SDL_Joystick *> _joysticks;
	Object * _root;
	bool _quit;
	Mutex _mutex;
	Cache * _cache;
};

// Service that manages a cache of images that can be loaded asynchroneously
class Cache : public Service
{
public:
	static Cache & instance();
	SDL_Surface * image(const std::string & path);
protected:
	Cache();
	~Cache();
protected:
	bool setup();
	void teardown();
	void run();
private:
	Mutex _mutex;
	Condition _cond;
	std::map<std::string, SDL_Surface*> _data;
	std::list<std::string> _queue;
	bool _quit;
};

// Service that monitors when USB devices are mounted / unmounted
class Storage : public Service {
public:
	static Storage & instance();
protected:
	Storage();
	~Storage();
protected:
	bool setup();
	void teardown();
	void run();
	void populate();
private:
	std::string _path;
	int _fd;
	int _wd;
};

// Base class of every UI element
class Object
{
public:
	Object();
	virtual ~Object();
	virtual void render(SDL_Renderer * renderer) = 0;
	void moveTo(int x, int y);
	void moveBy(int dx, int dy);
	int x() const;
	int y() const;
	int absx() const;
	int absy() const;
	void attach(Layout * layout);
	void detach();
	Layout * layout();
	const Layout * layout() const;
private:
	Layout * _layout;
	int _x;
	int _y;
};

// Base class of visible UI elements
class Item : public Object {
public:
	Item();
	virtual ~Item();
	void invalidate();
	void render(SDL_Renderer * renderer);
	void update();
	SDL_Color make_color(int rgba);
	int width() const;
	int height() const;
	void hide();
	void show();
	void setVisible(bool visible);
	void resize(int width, int height);
protected:
	virtual SDL_Texture * prepare(SDL_Renderer * renderer) = 0;
private:
	SDL_Texture * _texture;
	int _width;
	int _height;
	bool _visible;
};

// Base class of organizationnal UI elements
class Layout : public Object {
public:
	Layout();
	virtual ~Layout();
	Object * get(int index);
	const Object * get(int index) const;
	void set(int index, Object * obj);
	int count() const;
	void update();
	void render(SDL_Renderer * renderer);
protected:
	void clear();
	void resize(int count);
	virtual void refresh() = 0;
private:
	std::vector<Object *> _childs;
};


// A simple UI text, with a size and color
class TextItem : public Item {
public:
	TextItem(const std::string & txt, int size, int color);
	virtual ~TextItem();
	void setText(const std::string & text);
	void setColor(int color);
	void setSize(int size);
protected:
	SDL_Texture * prepare(SDL_Renderer * renderer);
private:
	std::string _text;
	int _size;
	int _color;
};

// A simple UI image that use the global image cache
class ImageItem : public Item
{
public:
	ImageItem(int width, int height, const std::string & path);
	virtual ~ImageItem();
	std::string path() const;
	void setPath(const std::string & path);
protected:
	SDL_Texture * prepare(SDL_Renderer * renderer);
private:
	std::string _path;
};

// A UI layout for organizing other element on a line
class LineLayout : public Layout {
public:
	LineLayout(int count, int length, int angle);
	virtual ~LineLayout();
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

#endif // ETHER_H
