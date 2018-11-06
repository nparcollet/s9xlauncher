/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Mutex::Mutex()
{
	if (pthread_mutex_init(&_mutex, nullptr) == -1) {
		LOG_FATAL("Mutex(), Error initializing the mutex: %s (%d)", strerror(errno), errno);
	}
}

Mutex::~Mutex()
{
	if (pthread_mutex_destroy(&_mutex) == -1) {
		LOG_FATAL("Mutex(), Error destroying the mutex: %s (%d)", strerror(errno), errno);
	}
}

void Mutex::lock()
{
	if (pthread_mutex_lock(&_mutex) == -1) {
		LOG_FATAL("Mutex(), Error locking the mutex: %s (%d)", strerror(errno), errno);
	}
}

void Mutex::unlock()
{
	if (pthread_mutex_unlock(&_mutex) == -1) {
		LOG_FATAL("Mutex(), Error unlocking the mutex: %s (%d)", strerror(errno), errno);
	}
}
