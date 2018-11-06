/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Condition::Condition()
{
	if (pthread_cond_init(&_cond, nullptr) == -1) {
		LOG_FATAL("Condition(), failed to create condition: %s (%d)", strerror(errno), errno);
	}
}

Condition::~Condition()
{
	if (pthread_cond_destroy(&_cond) == -1) {
		LOG_FATAL("Condition(), failed to destroy condition: %s (%d)", strerror(errno), errno);
	}
}

void Condition::wait(Mutex & mutex)
{
	if (pthread_cond_wait(&_cond, &mutex._mutex) == -1) {
		LOG_FATAL("Condition(), failed to wait on condition: %s (%d)", strerror(errno), errno);
	}
}

void Condition::signal()
{
	if (pthread_cond_signal(&_cond) == -1) {
		LOG_FATAL("Condition(), failed to signal condition: %s (%d)", strerror(errno), errno);
	}
}

void Condition::broadcast()
{
	if (pthread_cond_broadcast(&_cond) == -1) {
		LOG_FATAL("Condition(), failed to broadcast condition: %s (%d)", strerror(errno), errno);
	}
}
