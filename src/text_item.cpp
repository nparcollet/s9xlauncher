#include "sui.h"

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

SDL_Surface * UiTextItem::prepare()
{
	TTF_Font * font = TTF_OpenFont("/usr/share/s9xlauncher/roboto.ttf", _size);
	SDL_Surface * surface = TTF_RenderText_Solid(font, _text.c_str(), make_color(_color));
	TTF_CloseFont(font);
	return surface;
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
