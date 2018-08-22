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

#include "Thread.h"

typedef struct ThreadLock {
    void* ptr;
    void* condition;
} TLock;

typedef struct ThreadCtrlBlock {
    U8 tid;      // thread id     
    TLock *lock; // thread lock
} TCB;

typedef struct Thread {
    TCB* tcb;
    ThreadRoutine func;
    void* param;
    pthread_t pid;
    pthread_attr_t *attr;
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
        return thread->pid != 0;
    }
    return FALSE;
}

U8 getThreadId(const Thread* thread) 
{
    assert(thread && "thread must not be NULL !");
    return thread->pid;
}
