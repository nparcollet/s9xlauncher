/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

LineLayout::LineLayout(int count, int length, int angle)
:
	Layout(),
	_length(length),
	_angle(angle)
{
	resize(count);
}

LineLayout::~LineLayout()
{
}

void LineLayout::setLength(int length)
{
	_length = length;
	refresh();
}

void LineLayout::setAngle(int angle)
{
	_angle = angle;
	refresh();
}

int LineLayout::length() const
{
	return _length;
}

int LineLayout::angle() const
{
	return _angle;
}

void LineLayout::refresh()
{
	if (count() < 1) {
		return;
	} else if (count() < 2) {
		get(0)->moveTo(0, 0);
	} else {
		float a = _angle * 3.14 / 180.0;
		float s = _length / (count() - 1);
		for (int i = 0; i < count(); i ++) {
			Object * obj = get(i);
			if (obj) {
				obj->moveTo(cos(a) * (i*s -_length / 2.0), sin(a) * (i*s- _length / 2.0));
			}
	  	}
	}
}
