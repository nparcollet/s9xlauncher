/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

ImageItem::ImageItem(int width, int height, const std::string & path)
:
	Item(),
	_path(path)
{
	resize(width, height);
}

ImageItem::~ImageItem()
{
}

std::string ImageItem::path() const
{
	return _path;
}

void ImageItem::setPath(const std::string & path)
{
	_path = path;
	invalidate();
}

SDL_Texture * ImageItem::prepare(SDL_Renderer * renderer)
{
	if (!_path.empty()) {
		SDL_Surface * surface = Cache::instance().image(_path);
		if (surface) {
			return SDL_CreateTextureFromSurface(renderer, surface);
		}
	}
	return nullptr;
}
