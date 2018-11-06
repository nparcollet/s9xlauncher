/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Object::Object()
:
	_x(0),
	_y(0),
	_layout(nullptr)
{
}

Object::~Object()
{
}

void Object::attach(Layout * layout)
{
	_layout = layout;
}

void Object::detach()
{
	_layout = nullptr;
}

Layout * Object::layout()
{
	return _layout;
}

const Layout * Object::layout() const
{
	return _layout;
}

void Object::moveTo(int x, int y)
{
	_x = x;
	_y = y;
}

void Object::moveBy(int dx, int dy)
{
	_x += dx;
	_y += dy;
}

int Object::x() const
{
	return _x;
}

int Object::y() const
{
	return _y;
}

int Object::absx() const
{
	return _layout ? _layout->absx() + _x : _x;
}

int Object::absy() const
{
	 return _layout ? _layout->absy() + _y : _y;
}
