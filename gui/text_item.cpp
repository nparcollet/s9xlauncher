#include "gui.h"

UiTextItem::UiTextItem(const std::string & txt, int size, int color)
:
	UiItem(),
	_text(txt),
	_size(size),
	_color(color)
{
}

UiTextItem::~UiTextItem()
{
}

SDL_Texture * UiTextItem::prepare()
{
	SDL_Renderer * renderer = UiApplication::instance().renderer();
	TTF_Font * font = TTF_OpenFont("/usr/share/s9xlauncher/roboto.ttf", _size);
	SDL_Surface * surface = TTF_RenderText_Solid(font, _text.c_str(), make_color(_color));
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
	return texture;
}

void UiTextItem::setText(const std::string & text)
{
	_text = text;
	invalidate();
}

void UiTextItem::setColor(int color)
{
	_color = color;
	invalidate();
}

void UiTextItem::setSize(int size)
{
	_size = size;
	invalidate();
}
