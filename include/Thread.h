/*************************
 * file:    Thread.h
 * desc:    define Thread
 * author:  Kari.Zhang
 * date:    2017-12-14
 ************************/

#ifndef __THREAD__H__
#define __THREAD__H__

#include <pthread.h>
#include "comm.h"

typedef struct Thread Thread;
typedef void* (*ThreadRoutine)(void*);

/*
 * Create thread with ThreadRoutine & params
 */
Thread* createThread(ThreadRoutine func, void *param);

/**
 * Start the specified thread
 */
bool startThread(Thread* thread);

/**
 * destroy the specified thread
 */
void destroyThread(Thread* thread);

/**
 * Check if the thread is alive
 */
bool isThreadAlive(const Thread* thread);

/**
 * The current thread wait the specified thread to wait
 * Params:
 *  retVal: the return code of pid
 */
bool joinThread(U8 pid, void ** retval);

/**
 * get the thread id
 */
U8 getThreadId(const Thread* thread);

/**
 * get the current thread id
 **/
U8 getCurrentThreadId();

#endif
