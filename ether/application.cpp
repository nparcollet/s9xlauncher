/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Application::Application(int argc, char ** argv)
:
	_window(nullptr),
	_renderer(nullptr),
	_root(nullptr)
{
}

Application::~Application()
{
}

void Application::quit()
{
	_quit = true;
}

void Application::suspend()
{
	_suspend = true;
}

void Application::resume()
{
	_suspend = false;
	_repeat.clear();
}

int Application::run(int width, int height)
{
	if (!Dispatcher::instance().attach(this)) {
		LOG_ERROR("Application(), failed to attach dispatcher");
	} else if (!Cache::instance().start()) {
		LOG_ERROR("Application(), failed to start the CACHE service");
	} else if (!Storage::instance().start()) {
		LOG_ERROR("Application(), failed to start the STORAGE service");
	} else if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		LOG_ERROR("Application(), to initialize SDL: %s", SDL_GetError());
	} else if (TTF_Init() == -1) {
		LOG_ERROR("Application(), to initialize TTF: %s", SDL_GetError());
	} else if (SDL_JoystickEventState(SDL_ENABLE) == -1) {
		LOG_ERROR("Application(), to enable joystick event state: %s", SDL_GetError());
	} else if (!(_window = SDL_CreateWindow("s9x_launcher", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL))) {
		LOG_ERROR("Application(), to create window: %s", SDL_GetError());
	} else if (!(_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED))) {
		LOG_ERROR("Application(), to create renderer: %s", SDL_GetError());
	} else if (SDL_RenderSetLogicalSize(_renderer, width, height) == -1) {
		LOG_ERROR("Application(), to set logical size: %s", SDL_GetError());
	} else if (!(_root = build())) {
		LOG_ERROR("Application(), to create root object");
	} else {
		LOG_DEBUG("Application(), Starting application event loop");
		_quit = false;
		_suspend = false;
		while(!_quit) {

			// TODO: Better than that, ultimatly, Dispatcher is the only thing we loop on, and we,
			// as the application, only handle events. That would mean having SDL on a separate
			// thread thou, not sure if this is viable.

			// Keep track of time when starting the loop
			unsigned int start = SDL_GetTicks();

			// Process SDL Inputs
			input();

			while (Dispatcher::instance().process()) {
			}

			// Process with SDL Rendering
			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
			SDL_RenderClear(_renderer);
			_root->moveTo(width / 2, height / 2);
			_root->render(_renderer);
			SDL_RenderPresent(_renderer);

			// Wait before refresh to have 25FPS
			unsigned int end = SDL_GetTicks();
			if (end - start < 40) {
				SDL_Delay(end - start);
			}
		}

		LOG_DEBUG("Application(), Stopping application event loop");
		std::list<SDL_Joystick*>::iterator jite = _joysticks.begin();
		while (jite != _joysticks.end()) {
			SDL_JoystickClose(*jite);
			++ jite;
		}
		_joysticks.clear();
		if (_renderer) {
			SDL_DestroyRenderer(_renderer);
			_renderer = nullptr;
		}
		if (_window) {
			SDL_DestroyWindow(_window);
			_window = nullptr;
		}
		SDL_JoystickEventState(SDL_DISABLE);
		TTF_Quit();
		SDL_Quit();
		Cache::instance().stop();
		Storage::instance().stop();
		return 0;
	}
	return -1;
}

void Application::dispatch(const Event & event)
{
	if (event.sender() == "app") {
		if (event.type() == "key") {
			onKeyPressed(event.number(0));
		} else {
			onCustomEvent(event);
		}
	} else if (event.sender() == "cache") {
		if (event.type() == "load") {
			onCacheLoad(event.string(0));
		} else if (event.type() == "error") {
			onCacheError(event.string(0));
		} else {
			onCustomEvent(event);
		}
	} else if (event.sender() == "storage") {
		if (event.type() == "added") {
			onStorageAdded(event.string(0), event.string(1));
		} else if (event.type() == "removed") {
			onStorageRemoved(event.string(0));
		} else {
			onCustomEvent(event);
		}
	} else {
		onCustomEvent(event);
	}
}

void Application::onKeyPressed(int key)
{
	LOG_WARNING("Application(), No handler for %s(%d)", __FUNCTION__, key);
}

void Application::onStorageAdded(const std::string & device, const std::string & mountPoint)
{
	LOG_WARNING("Application(), No handler for %s(%s, %s)", __FUNCTION__, device.c_str(), mountPoint.c_str());
}

void Application::onStorageRemoved(const std::string & device)
{
	LOG_WARNING("Application(), No handler for %s(%s)", __FUNCTION__, device.c_str());
}

void Application::onCacheLoad(const std::string & path)
{
	LOG_WARNING("Application(), No handler for %s(%s)", __FUNCTION__, path.c_str());
}

void Application::onCacheError(const std::string & path)
{
	LOG_WARNING("Application(), No handler for %s(%s)", __FUNCTION__, path.c_str());
}

void Application::onCustomEvent(const Event & event)
{
	LOG_WARNING("Application(), No handler for %s(%s:%s)", __FUNCTION__, event.sender().c_str(), event.type().c_str());
}

void Application::input()
{
	static int rate  = 40;  // 25/s
	static int delay = 660; // Before first repeat
	unsigned int now = SDL_GetTicks();
	SDL_Event e;

	while (!_suspend && SDL_PollEvent(&e)) {

		// SDL Requested application to stop
		if (e.type == SDL_QUIT) {
			quit();
		}

		// Keyboard event
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_UP) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_UP})));
			} else if (e.key.keysym.sym == SDLK_DOWN) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_DOWN})));
			} else if (e.key.keysym.sym == SDLK_LEFT) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_LEFT})));
			} else if (e.key.keysym.sym == SDLK_RIGHT) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_RIGHT})));
			} else if (e.key.keysym.sym == SDLK_RETURN) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_OK})));
			} else if (e.key.keysym.sym == SDLK_ESCAPE) {
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_CANCEL})));
			}
		}

		// Joystick button down event
		else if (e.type == SDL_JOYBUTTONDOWN) {
			if (e.jbutton.button == 0) {
				_repeat[KEY_OK] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_OK})));
			} else if (e.jbutton.button == 1) {
				_repeat[KEY_CANCEL] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_CANCEL})));
			}
		}

		// Joystick button up event
		else if (e.type == SDL_JOYBUTTONUP) {
			if (e.jbutton.button == 0) {
				_repeat.erase(KEY_OK);
			} else if (e.jbutton.button == 1) {
				_repeat.erase(KEY_CANCEL);
			}
		}

		// Joystick hat event
		else if (e.type == SDL_JOYHATMOTION) {
			if (e.jhat.value & SDL_HAT_UP) {
				_repeat[KEY_UP] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_UP})));
			} else {
				_repeat.erase(KEY_UP);
			}
			if (e.jhat.value & SDL_HAT_DOWN) {
				_repeat[KEY_DOWN] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_DOWN})));
			} else {
				_repeat.erase(KEY_DOWN);
			}
			if (e.jhat.value & SDL_HAT_LEFT) {
				_repeat[KEY_LEFT] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_LEFT})));
			} else {
				_repeat.erase(KEY_LEFT);
			}
			if (e.jhat.value & SDL_HAT_RIGHT) {
				_repeat[KEY_RIGHT] = now + delay;
				Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({KEY_RIGHT})));
			} else {
				_repeat.erase(KEY_RIGHT);
			}
		}

		// Joystick added event
		else if (e.type == SDL_JOYDEVICEADDED) {
			int did = e.jdevice.which;
			SDL_Joystick * joystick =  SDL_JoystickOpen(did);
			if (!joystick) {
				LOG_ERROR("Application(), to connect joystick %d", did);
			} else {
				LOG_DEBUG("Application(), Connected joystick %d", did);
				_joysticks.push_back(joystick);
			}
		}

		// Joystick removed event
		else if (e.type == SDL_JOYDEVICEREMOVED) {
			int did = e.jdevice.which;
			std::list<SDL_Joystick*>::iterator ite = _joysticks.begin();
			while (ite != _joysticks.end()) {
				if (SDL_JoystickInstanceID(*ite)) {
					LOG_DEBUG("Application(), Disconnected joystick %d", did);
					SDL_JoystickClose(*ite);
					_joysticks.erase(ite);
					break;
				}
				++ ite;
			}
		}
	}

	// Simulate repeat event for joysticks
	std::map<int, int>::iterator ite = _repeat.begin();
	while (ite != _repeat.end()) {
		if (ite->second != -1 && now > ite->second) {
			_repeat[ite->first] = now + rate;
			Dispatcher::instance().post(Event("app", "key", std::vector<Variant>({ite->first})));
		}
		++ ite;
	}
}
