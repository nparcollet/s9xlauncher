#include "sui.h"

UiImageItem::UiImageItem(int maxWidth, int maxHeight, const std::string & path)
:
	UiItem(),
	_path(path),
	_maxWidth(maxWidth),
	_maxHeight(maxHeight)
{
}

UiImageItem::~UiImageItem()
{
}

void UiImageItem::setPath(const std::string & path)
{
	_path = path;
	invalidate();
}

SDL_Texture * UiImageItem::prepare()
{
	static std::map<std::string,SDL_Surface *> cache;
	SDL_Renderer * renderer = UiApplication::instance().renderer();
	if (cache.find(_path) == cache.end()) {
		SDL_Surface * surface = SDL_CreateRGBSurface(0, _maxWidth, _maxHeight, 32, 0, 0, 0, 0);
		SDL_Surface * img = IMG_Load(_path.c_str());
		if (img) {
			SDL_BlitScaled(img, nullptr, surface, nullptr);
			SDL_FreeSurface(img);
		} else {
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 64, 64, 64));
		}
		cache[_path] = surface;
	}
	return SDL_CreateTextureFromSurface(renderer, cache[_path]);
}
