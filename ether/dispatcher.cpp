/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Dispatcher & Dispatcher::instance()
{
	static Dispatcher self;
	return self;
}

Dispatcher::Dispatcher()
:
	_app(nullptr)
{
}

Dispatcher::~Dispatcher()
{
}

bool Dispatcher::attach(Application * app)
{
	if (_app) {
		LOG_ERROR("Dispatcher(), Application already attacehd");
	} else {
		_app = app;
		return true;
	}
	return false;
}

void Dispatcher::post(const Event & event)
{
	_mutex.lock();
	_queue.push_back(event);
	_mutex.unlock();
}

bool Dispatcher::process()
{
	Locker locker(_mutex);
	if (!_queue.empty()) {
		Event event = _queue.front();
		_queue.pop_front();
		_mutex.unlock();
		_app->dispatch(event);
		_mutex.lock();
	}
	return _queue.empty() ? false : true;
}
