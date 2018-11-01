#include "gui.h"

UiLineLayout::UiLineLayout(int count, int length, int angle)
:
	UiLayout(),
	_length(length),
	_angle(angle)
{
	resize(count);
}

UiLineLayout::~UiLineLayout()
{
}

void UiLineLayout::setLength(int length)
{
	_length = length;
	refresh();
}

void UiLineLayout::setAngle(int angle)
{
	_angle = angle;
	refresh();
}

int UiLineLayout::length() const
{
	return _length;
}

int UiLineLayout::angle() const
{
	return _angle;
}

void UiLineLayout::refresh()
{
	if (count() < 1) {
		return;
	} else if (count() < 2) {
		get(0)->moveTo(0, 0);
	} else {
		float a = _angle * 3.14 / 180.0;
		float s = _length / (count() - 1);
		for (int i = 0; i < count(); i ++) {
			UiObject * obj = get(i);
			if (obj) {
				obj->moveTo(cos(a) * (i*s -_length / 2.0), sin(a) * (i*s- _length / 2.0));
			}
	  	}
	}
}
