/*************************
 * file:    Thread.c
 * desc:    define Thread
 * author:  Kari.Zhang
 * date:    2017-12-14
 ************************/

#include <assert.h>
#include <stdlib.h>

#ifdef linux
#include <pthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "Thread.h"

typedef struct Thread {
#ifdef linux
    pthread_t pid;
    pthread_attr_t *attr;
#endif

#ifdef WIN32
    HANDLE hThread;
    // thread attr
#endif

    ThreadRoutine func;
    void* param;

} Thread;

Thread* createThread(ThreadRoutine func, void* param)
{
    assert(func && "func must not be NULL !");
    Thread* thread = (Thread *)calloc(1, sizeof(Thread));
    do {
        if (thread == NULL) {
            break;
        }
        thread->func = func;
        thread->param = param;
    } while (0);

    return thread;
}

bool startThread(Thread* thread) {
    assert(thread && "thread must not be NULL !");
#ifdef linux
    int result = pthread_create(
            &thread->pid, 
            thread->attr, 
            thread->func,
            thread->param);

        if (result) {
            free(thread);
            thread = NULL;
            return FALSE;
        }
        return TRUE; 
#endif

    return FALSE;
}

void destroyThread(Thread* thread)
{
    assert(thread && "thread must not be NULL !");
}

bool joinThread(pthread_t pid, void** retval)
{
    int status = pthread_join(pid, retval);
    return status == 0 ? TRUE : FALSE;
}

bool isThreadValid(const Thread* thread)
{
    if (thread != NULL) {
        #ifdef linux
            return thread->pid != 0;
        #endif

        #ifdef WIN32
            return thread->hThread != NULL;
        #endif
    }

    return FALSE;
}

#ifdef linux
pthread_t getThreadId(const Thread* thread) 
{
    assert(thread && "thread must not be NULL !");
    return thread->pid;
}
#endif

#ifdef WIN32
HANDLE getThreadHandle(const Thread* thread)
{
    assert(thread && "thread must not be NULL !");
    return thread->hThread;
}
#endif
