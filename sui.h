#ifndef SIMPLE_UI_H
#define SIMPLE_UI_H

#include <string>
#include <list>
#include <map>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include <dirent.h>
#include <string.h>
#include <malloc.h>

#define EV_OK		101
#define EV_CANCEL	102
#define EV_LEFT		103
#define EV_RIGHT	104
#define EV_UP		105
#define EV_DOWN		106

class UiObject;
class UiItem;
class UiLayout;
class UiTextItem;
class UiMenu;
class UiApplication;

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
protected:
	virtual SDL_Surface * prepare() = 0;
private:
	SDL_Texture * _texture;
	int _width;
	int _height;
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
	SDL_Surface * prepare();
private:
	std::string _text;
	int _size;
	int _color;
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
	virtual UiObject * build() = 0;
	virtual void event(int eid) = 0;
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
};

#endif // SIMPLE_UI_H
