/*******************************************************
 * file name:   mem.h
 * description: mem manager 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *	1. Add struct MemoryArea by kari.zhang @ 2015-11-30
 *******************************************************/

#ifndef __MEM__H__
#define __MEM__H__

struct MemoryArea; 
typedef struct MemoryArea MemoryArea; 

/*
 * Create a specified capability memory area
 */
MemoryArea* createMemoryArea(int cap);

/*
 * Destroy the specified memory area
 */
void destroyMemoryArea(MemoryArea* area);

/*
 * Alloc memory
 * Parameters:
 *		area:	which area to allocate
 *		size:	size will allocate
 * Return:
 *		if OK, base address of allocated
 *		if Error, NULL
 * Notice:
 *		memory allocate by sysAlloc must be free by sysFree,
 *		Otherwise, memory leak will occur
 */
extern void* sysAlloc(MemoryArea* area, int size);

/*
 * Free memory
 * Parameters:
 *		area:
 *		base:	which to free
 * Return:
 *		if OK, base address of allocated
 *		if Error, NULL
 * Notice:
 *		memory allocate by sysAlloc must be free by sysFree,
 *		Otherwise, memory leak will occur
 */

extern void sysFree(MemoryArea* area, void *base);

#endif
