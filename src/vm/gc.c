/*******************************************************
 * file name:   gc.c
 * description: garbage collector
 * date:        2017-02-23
 * author:      kari.zhang
 *
 * modifications:
 *	1. Create by kari.zhang @ 2017-02-23
 *******************************************************/

#include <assert.h>
#include <stdlib.h>
#include "gc.h"
#include "runtime.h"

// TODO
extern RefHandlePool *sRefHandlePool;

#define MAX_GENERATIONS  3
struct gc_context {
    MemoryArea *mem;        
}; 

/*
 * Create a gc instance
 */
gc_context* gcCreate(MemoryArea *mem) {
    assert (NULL != mem);
    gc_context *gc = (gc_context *)calloc(1, sizeof(gc_context));
    if (NULL != gc) {
	gc->mem = mem;
    }

    return gc;
}

/*
 * Destroy the gc instance
 */
void gcDestroy(gc_context* gc) {
    if (NULL != gc) {
	free (gc);
	gc = NULL;
    }
}

/*
 * gc start to work
 */
extern void gcWork(gc_context* gc) {

}
