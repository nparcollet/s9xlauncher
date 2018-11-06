/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Layout::Layout()
:
	Object()
{
}

Layout::~Layout()
{
	for (int i = 0; i < _childs.size(); i ++) {
		delete _childs[i];
	}
}

void Layout::clear()
{
	for (int i = 0; i < _childs.size(); i ++) {
		delete _childs[i];
	}
	_childs.clear();
	refresh();
}

void Layout::set(int index, Object * obj)
{
	if (_childs[index] != obj) {
		if (_childs[index]) {
			_childs[index]->detach();
			delete _childs[index];
		}
		_childs[index] = obj;
		obj->attach(this);
		refresh();
	}
}

void Layout::resize(int size)
{
	int oldSize = _childs.size();
	if (size != oldSize) {
		if (size < oldSize) {
			for (int i = size; i < oldSize; i ++) {
				delete _childs[i];
			}
		}
		_childs.resize(size);
		for (int i = oldSize; i < size; i ++) {
			_childs[i] = nullptr;
		}
		refresh();
	}
}

Object * Layout::get(int index)
{
	return _childs[index];
}

const Object * Layout::get(int index) const
{
	return _childs[index];
}

int Layout::count() const
{
	return _childs.size();
}

void Layout::render(SDL_Renderer * renderer)
{
	for (int i = 0; i < _childs.size(); i ++) {
		if (_childs[i]) {
			_childs[i]->render(renderer);
		}
	}
}
