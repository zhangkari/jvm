/*******************************
 * file name:   mem.c
 * description: mem manager 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __MEM__H__
#define __MEM__H__

#include <stdlib.h>
#include "class.h"
#include "jvm.h"
#include "mem.h"

void* sysAlloc(int size) {
    return malloc(size);
}

void sysFree(void *base) {
    free (base);
}

void* gcAlloc(VM *vm, int size) {

}

void gcFree(VM *vm, void *base) {

}

void gcMark(VM *vm, Class *class) {

}

#endif
