//
//
//
//
//



#ifndef mutex_H
#define mutex_H

#include "bareos.h"

struct MutexWaiter
{
	struct MutexWaiter *next;
	struct MutexWaiter *prev;
	//list of threads waiting on the mutex
	
	void *waiting_thread; //thread waiting on mutex
};



struct Mutex
{
	struct MutexWaiter *next;
	struct MutexWaiter *prev;
	//list of threads waiting on the mutex

	void *owner;
	//owner of the mutex

	uint32_t counter; //times owned counter (recursion counter)
};

static inline uint32_t MutexInit(struct Mutex *mutex)
{
	if(mutex->next != 0 && mutex->prev != 0)
	{
		return 1;
	}

	mutex->owner = 0;
	mutex->counter = 0;
	mutex->next = (void *)mutex;
	mutex->prev = (void *)mutex;

	return 0;
}

static inline uint32_t MutexDeinit(struct Mutex *mutex)
{
	if(mutex->owner != 0)
	{
		return 1;
	}

	mutex->next = 0;
	mutex->prev = 0;

	return 0;
}

uint32_t MutexLock(struct Mutex *mutex);

uint32_t MutexUnlock(struct Mutex *mutex);













#endif
