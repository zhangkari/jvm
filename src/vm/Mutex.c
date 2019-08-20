
/****************************
 * file name:   Mutex.c
 * author:      Karic.Zhang
 * date:        2019-08-20
 * **************************/

#include <errno.h>
#include <stdlib.h>
#include "Mutex.h"

Mutex* mutex_create() {
    Mutex* mutex = calloc(1, sizeof(Mutex));
    int status = pthread_mutex_init(&mutex->mutex, NULL);
    if (status != 0) {
        if (status == EINVAL) {
            printf("The value specified by attr is invalid.");
        } else if (status == ENOMEM) {
            printf("allocate memory for mutex failed.");
        }
        free(mutex);
        return NULL;
    }
    return mutex;
}

bool mutex_lock(Mutex* mutex) {
    int status = pthread_mutex_lock(&mutex->mutex);
    if (status == 0) {
        return TRUE;
    } else if (status == EINVAL) {
        printf("mutex is invalid.");
        return FALSE;
    } else if (status == EDEADLK) {
        printf("Deadlock would occur.");
        return FALSE;
    }
    return FALSE;
}

bool mutex_tryLock(Mutex* lock) {
    int status = pthread_mutex_trylock(&lock->mutex);
    if (status == 0) {
        return TRUE;
    } else if (status == EBUSY) {
        return FALSE;
    } else if (status == EINVAL) {
        printf("mutex is invalid.");
        return FALSE;
    }
    return FALSE;
}

void mutex_free(Mutex* lock) {
    if (pthread_mutex_destroy(&lock->mutex) != 0) {
        printf("destory mutex failed.");
    }
    free (lock);
}
