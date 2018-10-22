#include "sui.h"

UiMenu::UiMenu()
{
}

UiMenu::~UiMenu()
{
	if (_root) {
		delete _root;
	}
}

void UiMenu::render(SDL_Renderer * renderer)
{
	int w, h;
	SDL_RenderGetLogicalSize(renderer, &w, &h);
	if (!_root) {
		_root = build();
	}
	_root->moveTo(w/2, h/2);
	_root->render(renderer);
}
