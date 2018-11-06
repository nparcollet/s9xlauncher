/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

void * Service::ThreadFunc(void * ctx)
{
	((Service*)ctx)->run();
	return (nullptr);
}

Service::Service(const std::string & name)
:
	_name(name),
	_running(false)
{
}

Service::~Service()
{
	stop();
}

bool Service::start()
{
	Locker locker(_mutex);
	if (_running) {
		LOG_ERROR("Service(), Already running");
	} else if (!setup()) {
		LOG_ERROR("Service(), Failed to setup");
	} else if(pthread_create(&_thread, nullptr, Service::ThreadFunc, this)) {
		LOG_ERROR("Service(), Failed to create thread: %s (%d)", strerror(errno), errno);
	} else {
		LOG_DEBUG("Service(), Started %s", _name.c_str());
		_running = true;
		return true;
	}
	return false;
}

void Service::stop()
{
	Locker locker(_mutex);
	if (_running) {
		teardown();
		if (pthread_join(_thread, nullptr) == -1) {
			LOG_FATAL("Service(), Failed to join thread: %s (%d)", strerror(errno), errno);
		} else {
			LOG_DEBUG("Service(), Stopped %s", _name.c_str());
			_running = false;
		}
	}
}

bool Service::running()
{
	Locker locker(_mutex);
	return _running;
}

std::string Service::name() const
{
	return _name;
}

void Service::notify(const std::string & type, const std::vector<Variant> & args)
{
	Dispatcher::instance().post(Event(name(), type, args));
}
