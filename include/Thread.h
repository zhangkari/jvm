/*************************
 * file:    Thread.h
 * desc:    define Thread
 * author:  Kari.Zhang
 * date:    2017-12-14
 ************************/

#ifndef __THREAD__H__
#define __THREAD__H__

#ifdef linux
#include <pthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "comm.h"

typedef void* (*ThreadRoutine)(void*);
typedef struct Thread Thread;

Thread* createThread(ThreadRoutine func, void *param);
bool startThread(Thread* thread);
void destroyThread(Thread* thread);
bool isThreadValid(const Thread* thread);
bool joinThread(pthread_t pid, void ** retval);

#ifdef linux
pthread_t getThreadId(const Thread* thread);
#endif

#ifdef WIN32
HANDLE getThreadHandle(const Thread* thread);
#endif

#endif
