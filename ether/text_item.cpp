/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

TextItem::TextItem(const std::string & txt, int size, int color)
:
	Item(),
	_text(txt),
	_size(size),
	_color(color)
{
}

TextItem::~TextItem()
{
}

SDL_Texture * TextItem::prepare(SDL_Renderer * renderer)
{
	TTF_Font * font = TTF_OpenFont("/usr/share/s9xlauncher/roboto.ttf", _size);
	SDL_Surface * surface = TTF_RenderText_Solid(font, _text.c_str(), make_color(_color));
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
	return texture;
}

void TextItem::setText(const std::string & text)
{
	_text = text;
	invalidate();
}

void TextItem::setColor(int color)
{
	_color = color;
	invalidate();
}

void TextItem::setSize(int size)
{
	_size = size;
	invalidate();
}
