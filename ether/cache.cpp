/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"


Cache::Cache()
:
	Service("cache")
{
}

Cache::~Cache()
{
}

Cache & Cache::instance()
{
	static Cache self;
	return self;
}

bool Cache::setup()
{
	Locker locker(_mutex);
	_quit = false;
	return true;
}

void Cache::teardown()
{
	Locker locker(_mutex);
	_queue.clear();
	_quit = true;
	_cond.signal();
}

void Cache::run()
{
	Locker locker(_mutex);
	do  {

		while (_queue.empty() && !_quit) {
			_cond.wait(_mutex);
		}

		if (!_quit) {
			std::string path = _queue.front();
			_queue.pop_front();
			_mutex.unlock();
			SDL_Surface * img = IMG_Load(path.c_str());
			_mutex.lock();
			if (img) {
				_data[path] = img;
			} else {
				LOG_ERROR("Cache(), failed to load image at %s: %s", path.c_str(), SDL_GetError());
				_data.erase(path);
			}
			notify(img ? "load" : "error", std::vector<Variant>({path}));
		}

	} while (!_quit);
}

SDL_Surface * Cache::image(const std::string & path)
{
	Locker locker(_mutex);
	std::map<std::string, SDL_Surface*>::iterator ite = _data.find(path);
	if (ite == _data.end()) {
		_data[path] = nullptr;
		_queue.push_front(path);
		_cond.signal();
		return nullptr;
	}
	return ite->second;
}
