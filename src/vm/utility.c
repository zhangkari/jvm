/*******************************************************
 * file name:   utility.c
 * description: common utiltiy
 * date:        2016-05-22
 * author:      kari.zhang
 *
 * modifications:
 *	1. Created by kari.zhang @ 2016-5-22
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "utility.h"

uint64_t current_ms() {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        printf("Failed get current_ms().\n");
        exit(1);
    }

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
