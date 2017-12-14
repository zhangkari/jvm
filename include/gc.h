/*******************************************************
 * file name:   gc.h
 * description: garbage collector
 * date:        2017-02-23
 * author:      kari.zhang
 *
 * modifications:
 *	1. Create by kari.zhang @ 2017-02-23
 *******************************************************/

#ifndef __GC__H__
#define __GC__H__

#include "mem.h"

struct gc_context; 
typedef struct gc_context gc_context; 

/*
 * Create a gc instance
 */
gc_context* gcCreate(MemoryArea *mem);

/*
 * Destroy the gc instance
 */
void gcDestroy(gc_context* gc);

/*
 * gc start to work
 */

extern void gcWork(gc_context* gc);

extern void* gcRoutine(void *param);

#endif
