/*************************
 * file:    Lock.h
 * desc:    define Lock
 * author:  Kari.Zhang
 * date:    2019-08-20
 ************************/

#ifndef __COND__H__
#define __COND__H__

#include <pthread.h>
#include "comm.h"
#include "Mutex.h"

typedef struct Cond {
    pthread_cond_t cond;
    pthread_condattr_t attr;
} Cond;

/**
 * Create a Cond
 */
Cond* cond_create();

/**
 * Locks mutex, If the mutex is already locked, 
 * the calling thread will block until the mutex becomes available
 * Return TRUE: success, FALSE:failed
 */
bool cond_wait(Cond* cond, Mutex* mutex);

/**
 * Try to lock mutex and will not block to wait for the mutex
 * Return TRUE: success, FALSE: failed
 */
bool cond_signal(Cond* cond);

/**
 * Free lock
 */ 
void cond_free(Cond* cond);

#endif
