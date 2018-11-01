#ifndef CORE_H
#define CORE_H

#include <pthread.h>

class Mutex;
class Condition;

class Mutex
{
public:
	Mutex();
	~Mutex();
	void lock();
	void unlock();
private:
	pthread_mutex_t _mutex;
};

class Condition
{
public:
	Condition(Mutex & mutex);
	~Condition();
	void wait();
	void signal();
	void broadcast();
private:
	pthread_cond_t _cond;
	Mutex & _mutex;
};

// Event Loop
// Service
// Task

#endif // CORE_H
