#include "sui.h"


UiItem::UiItem()
:
	UiObject(),
	_texture(nullptr)
{
}

UiItem::~UiItem()
{
	if (_texture) {
		SDL_DestroyTexture(_texture);
	}
}

void UiItem::invalidate()
{
	if (_texture) {
		SDL_DestroyTexture(_texture);
	}
	_width		= -1;
	_height		= -1;
	_texture	= nullptr;
}

void UiItem::render(SDL_Renderer * renderer)
{
	SDL_Rect dst;
	dst.x = absx() - width() / 2;
	dst.y = absy() - height() / 2;
	dst.w = width();
	dst.h = height();
	if (!_texture) {
		SDL_Surface * surface = prepare();
		_texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		SDL_QueryTexture(_texture, nullptr, nullptr, &_width, &_height);
	}
	SDL_RenderCopy(renderer, _texture, nullptr, &dst);
}

SDL_Color UiItem::make_color(int rgba)
{
	SDL_Color color;
	color.r = (rgba >> 24) & 0x000000FF;
	color.g = (rgba >> 16) & 0x000000FF;
	color.b = (rgba >>  8) & 0x000000FF;
	color.a = (rgba >>  0) & 0x000000FF;
	return color;
}

int UiItem::width() const
{
	return _width;
}

int UiItem::height() const
{
	return _height;
}
