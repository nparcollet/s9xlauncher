/* Copyright 2018 PARCOLLET Nicolas, see COPYING file */
#include "ether.h"

Locker::Locker(Mutex & mutex)
:
	_mutex(mutex)
{
	_mutex.lock();
}

Locker::~Locker()
{
	_mutex.unlock();
}
