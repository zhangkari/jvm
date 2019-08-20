/*************************
 * file:    Mutex.h
 * desc:    define Lock
 * author:  Kari.Zhang
 * date:    2019-08-20
 ************************/

#ifndef __MUTEX__H__
#define __MUTEX__H__

#include <pthread.h>
#include "comm.h"

typedef struct Mutex {
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
} Mutex;

/**
 * Create a lock
 */
Mutex* mutex_create();

/**
 * Locks mutex, If the mutex is already locked, 
 * the calling thread will block until the mutex becomes available
 * Return TRUE: success, FALSE:failed
 */
bool mutex_lock(Mutex* mutex);

/**
 * Try to lock mutex and will not block to wait for the mutex
 * Return TRUE: success, FALSE: failed
 */
bool mutex_tryLock(Mutex* lock);

/**
 * Free lock
 */ 
void mutex_free(Mutex* mutex);

#endif
