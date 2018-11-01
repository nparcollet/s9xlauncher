#include "gui.h"


UiLayout::UiLayout()
:
	UiObject()
{
}

UiLayout::~UiLayout()
{
	for (int i = 0; i < _childs.size(); i ++) {
		delete _childs[i];
	}
}

void UiLayout::clear()
{
	for (int i = 0; i < _childs.size(); i ++) {
		delete _childs[i];
	}
	_childs.clear();
	refresh();
}

void UiLayout::set(int index, UiObject * obj)
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

void UiLayout::resize(int size)
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

UiObject * UiLayout::get(int index)
{
	return _childs[index];
}

const UiObject * UiLayout::get(int index) const
{
	return _childs[index];
}

int UiLayout::count() const
{
	return _childs.size();
}

void UiLayout::render(SDL_Renderer * renderer)
{
	for (int i = 0; i < _childs.size(); i ++) {
		if (_childs[i]) {
			_childs[i]->render(renderer);
		}
	}
}
