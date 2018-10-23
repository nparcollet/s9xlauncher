#include "sui.h"

UiApplication & UiApplication::instance()
{
	static UiApplication self;
	return self;
}

bool UiApplication::start(int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
	} else if (TTF_Init() == -1) {
		SDL_Log("Failed to initialize TTF: %s", SDL_GetError());
	} else if (SDL_JoystickEventState(SDL_ENABLE) == -1) {
		SDL_Log("Failed to enable joystick event state: %s", SDL_GetError());
	} else if (!(_window = SDL_CreateWindow("s9x_launcher", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL))) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
	} else if (!(_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED))) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
	} else if (SDL_RenderSetLogicalSize(_renderer, width, height) == -1) {
		SDL_Log("Failed to set logical size: %s", SDL_GetError());
	} else {
		SDL_Log("Application setup complete");
		return true;
	}
	return false;
}

void UiApplication::stop()
{
	std::list<SDL_Joystick*>::iterator jite = _joysticks.begin();
	while (jite != _joysticks.end()) {
		SDL_JoystickClose(*jite);
		++ jite;
	}
	_joysticks.clear();
	if (_menu) {
		delete _menu;
		_menu = nullptr;
	}
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
}


void UiApplication::quit()
{
	_quit = true;
}

void UiApplication::run()
{
	_quit = false;
	while(!_quit) {
		unsigned int start = SDL_GetTicks();
		if (_menu) {
			input();
			render();
		}
		unsigned int end = SDL_GetTicks();
		if (end - start < 20) {
			SDL_Delay(end - start);
		}
	}
}

void UiApplication::show(UiMenu * menu)
{
	if (_menu) {
		delete _menu;
	}
	_menu = menu;
}

UiApplication::UiApplication()
:
	_window(nullptr),
	_renderer(nullptr),
	_menu(nullptr)
{
}

UiApplication::~UiApplication()
{
	stop();
}

void UiApplication::input()
{
	// Joystick repeat:
	// - we have a map of all 'keys' currently pressed
	// - for each key we store the next time this should be retriggered
	// - on down, set to now + a delay
	// - a new block of code go throu to repeat entries
	// - when now > expected trigger, we retrigger and update the next repeat
	// - on event released, remove entry from the map.
	// Button Up: set time as -1
	// In addition to event, check entry != -1 and retrigger after a delay
	static int rate  = 40;  // 25/s
	static int delay = 660; // Before first repeat
	static std::map<int, int> lastsent;

	unsigned int now = SDL_GetTicks();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_QUIT) {
			quit();
		}

		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_UP) {
				_menu->event(EV_UP);
			} else if (event.key.keysym.sym == SDLK_DOWN) {
				_menu->event(EV_DOWN);
			} else if (event.key.keysym.sym == SDLK_LEFT) {
				_menu->event(EV_LEFT);
			} else if (event.key.keysym.sym == SDLK_RIGHT) {
				_menu->event(EV_RIGHT);
			} else if (event.key.keysym.sym == SDLK_RETURN) {
				_menu->event(EV_OK);
			} else if (event.key.keysym.sym == SDLK_ESCAPE) {
				_menu->event(EV_CANCEL);
			}
		}

		else if (event.type == SDL_JOYBUTTONDOWN) {
			if (event.jbutton.button == 0) {
				lastsent[EV_OK] = now + delay;
				_menu->event(EV_OK);
			} else if (event.jbutton.button == 1) {
				lastsent[EV_CANCEL] = now + delay;
				_menu->event(EV_CANCEL);
			}
		}

		else if (event.type == SDL_JOYBUTTONUP) {
			if (event.jbutton.button == 0) {
				lastsent.erase(EV_OK);
			} else if (event.jbutton.button == 1) {
				lastsent.erase(EV_CANCEL);
			}
		}

		else if (event.type == SDL_JOYHATMOTION) {
			if (event.jhat.value & SDL_HAT_UP) {
				lastsent[EV_UP] = now + delay;
				_menu->event(EV_UP);
			} else {
				lastsent.erase(EV_UP);
			}
			if (event.jhat.value & SDL_HAT_DOWN) {
				lastsent[EV_DOWN] = now + delay;
				_menu->event(EV_DOWN);
			} else {
				lastsent.erase(EV_DOWN);
			}
			if (event.jhat.value & SDL_HAT_LEFT) {
				lastsent[EV_LEFT] = now + delay;
				_menu->event(EV_LEFT);
			} else {
				lastsent.erase(EV_LEFT);
			}
			if (event.jhat.value & SDL_HAT_RIGHT) {
				lastsent[EV_RIGHT] = now + delay;
				_menu->event(EV_RIGHT);
			} else {
				lastsent.erase(EV_RIGHT);
			}
		}

		else if (event.type == SDL_JOYDEVICEADDED) {
			int did = event.jdevice.which;
			SDL_Joystick * joystick =  SDL_JoystickOpen(did);
			if (!joystick) {
				SDL_Log("Failed to connect joystick %d", did);
			} else {
				SDL_Log("Connected joystick %d", did);
				_joysticks.push_back(joystick);
			}
		}

		else if (event.type == SDL_JOYDEVICEREMOVED) {
			int did = event.jdevice.which;
			std::list<SDL_Joystick*>::iterator ite = _joysticks.begin();
			while (ite != _joysticks.end()) {
				if (SDL_JoystickInstanceID(*ite)) {
					SDL_Log("Disconnected joystick %d", did);
					_joysticks.erase(ite);
					break;
				}
				++ ite;
			}
		}
	}

	// Simulate repeat event for joysticks
	std::map<int, int>::iterator ite = lastsent.begin();
	while (ite != lastsent.end()) {
		if (ite->second != -1 && now > ite->second) {
			lastsent[ite->first] = now + rate;
			_menu->event(ite->first);
		}
		++ ite;
	}
}

void UiApplication::render()
{
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
	if (_menu) {
		_menu->render(_renderer);
	}
	SDL_RenderPresent(_renderer);
}
