/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Item::Item()
:
	Object(),
	_texture(nullptr),
	_visible(true),
	_width(-1),
	_height(-1)
{
}

Item::~Item()
{
	if (_texture) {
		SDL_DestroyTexture(_texture);
	}
}

void Item::hide()
{
	setVisible(false);
}
void Item::show()
{
	setVisible(true);
}

void Item::setVisible(bool visible)
{
	_visible = visible;
}

void Item::invalidate()
{
	if (_texture) {
		SDL_DestroyTexture(_texture);
	}
	_texture	= nullptr;
}

void Item::render(SDL_Renderer * renderer)
{
	if (_visible) {
		if (!_texture) {
			_texture = prepare(renderer);
		}
		if (_texture) {
			SDL_Rect dst;
			SDL_QueryTexture(_texture, nullptr, nullptr, &dst.w, &dst.h);
			dst.w = width()  == -1 ? dst.w : width();
			dst.h = height() == -1 ? dst.h : height();
			dst.x = absx() - dst.w/ 2;
			dst.y = absy() - dst.h / 2;
			SDL_RenderCopy(renderer, _texture, nullptr, &dst);
		}
	}
}

SDL_Color Item::make_color(int rgba)
{
	SDL_Color color;
	color.r = (rgba >> 24) & 0x000000FF;
	color.g = (rgba >> 16) & 0x000000FF;
	color.b = (rgba >>  8) & 0x000000FF;
	color.a = (rgba >>  0) & 0x000000FF;
	return color;
}

void Item::resize(int width, int height)
{
	_width = width;
	_height = height;
}

int Item::width() const
{
	return _width;
}

int Item::height() const
{
	return _height;
}
