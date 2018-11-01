#include "gui.h"

UiObject::UiObject()
:
	_x(0),
	_y(0),
	_layout(nullptr)
{
}

UiObject::~UiObject()
{
}

void UiObject::attach(UiLayout * layout)
{
	_layout = layout;
}

void UiObject::detach()
{
	_layout = nullptr;
}

UiLayout * UiObject::layout()
{
	return _layout;
}

const UiLayout * UiObject::layout() const
{
	return _layout;
}

void UiObject::moveTo(int x, int y)
{
	_x = x;
	_y = y;
}

void UiObject::moveBy(int dx, int dy)
{
	_x += dx;
	_y += dy;
}

int UiObject::x() const
{
	return _x;
}

int UiObject::y() const
{
	return _y;
}

int UiObject::absx() const
{
	return _layout ? _layout->absx() + _x : _x;
}

int UiObject::absy() const
{
	 return _layout ? _layout->absy() + _y : _y;
}
