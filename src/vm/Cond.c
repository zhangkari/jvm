
/****************************
 * file name:   Cond.c
 * author:      Karic.Zhang
 * date:        2019-08-20
 * **************************/

#include <errno.h>
#include <stdlib.h>
#include "Cond.h"

/**
 * Create a Cond
 */
Cond* cond_create() {
    Cond* cond = calloc(1, sizeof(Cond));
    int status = pthread_cond_init(&cond->cond, &cond->attr);
    if (status != 0) {
        if (status == EINVAL) {
            printf("The value specified by attr is invalid.");
        } else if (status == ENOMEM) {
            printf("allocate memory for cond failed.");
        } else if (status == EAGAIN) {
            printf("lack resources to create cond");
        }
        free(cond);
        return NULL;
    }
    return cond;
}

/**
 * Locks mutex, If the mutex is already locked, 
 * the calling thread will block until the mutex becomes available
 * Return TRUE: success, FALSE:failed
 */
bool cond_wait(Cond* cond, Mutex* mutex) {
    return pthread_cond_wait(&cond->cond, &mutex->mutex) == 0;
}

/**
 * Try to lock mutex and will not block to wait for the mutex
 * Return TRUE: success, FALSE: failed
 */
bool cond_signal(Cond* cond) {
    return pthread_cond_signal(&cond->cond);
}

/**
 * Free lock
 */ 
void cond_free(Cond* cond) {
    if(pthread_cond_destroy(&cond->cond) != 0) {
        printf("Failed destroy cond");
    }
    free(cond);
}