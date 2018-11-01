#include "gui.h"

UiMenu::UiMenu()
{
}

UiMenu::~UiMenu()
{
	if (_root) {
		delete _root;
	}
}

void UiMenu::setRoot(UiObject * root)
{
	_root = root;
}

void UiMenu::show()
{
	UiApplication::instance().show(this);
}

void UiMenu::hide()
{
	UiApplication::instance().show(nullptr);
}

void UiMenu::render(SDL_Renderer * renderer)
{
	int w, h;
	SDL_RenderGetLogicalSize(renderer, &w, &h);
	if (_root) {
		_root->moveTo(w/2, h/2);
		_root->render(renderer);
	}
}
