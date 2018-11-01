#include <core.h>


Condition::Condition(Mutex & mutex)
:
	_mutex(mutex)
{
	pthread_cond_init(&_cond, nullptr);
}


Condition::~Condition()
{
	pthread_cond_destroy(&_cond);
}

void Condition::wait()
{
}

void Condition::signal()
{
}

void Condition::broadcast()
{
}
