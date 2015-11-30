/*******************************************************
 * file name:   mem.c
 * description: mem manager 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *	1. Add struct MemoryArea by kari.zhang @ 2015-11-30
 ******************************************************/

#include <assert.h>
#include <stdlib.h>
#include "comm.h"
#include "mem.h"
#include "utility.h"

struct MemoryArea {
	void *base;			// base address
	void *current;		// address used currently
	U4  used;			// memory area has used
	U4  capability;		// area capability
	void **free;		// space that user free
	U4  freeCnt;		// count of free
};

/*
 * Create a specified capability memory area
 */
MemoryArea* createMemoryArea(int cap)
{
	if (cap <= 0) {
		return NULL;
	}

	MemoryArea *area = (MemoryArea *)calloc(1, sizeof(MemoryArea));
	if (NULL != area) {
		area->base = calloc(1, cap);
		if (NULL == area->base) {
			free (area);
			area = NULL;
		}
		else {
			area->used = 0;
			area->capability = cap;
			area->current = area->base;
#define MAX_FREE_CNT 64
			area->free = (void **)calloc(MAX_FREE_CNT, sizeof(void *));
			assert(NULL != area->free);
			area->freeCnt = 0;
		}
	}

	return area;
}

/*
 * Destroy the specified memory area
 */
void destroyMemoryArea(MemoryArea* area)
{
	if (NULL != area) {
		if (NULL != area->base) {
			free (area->base);
			area->base = NULL;
			area->current = NULL;
			area->used = 0;
			area->capability = 0;
			*area->free = NULL;
			area->freeCnt = 0;
		}

		free (area);
	}
}

/**
 * Find a memory block by first match algorithum in the free list
 * Parameters:
 *		area:		MemoryArea
 *		size:		the buffer size will allocate
 * Return:
 *		if OK,		return start addres
 *		if Error,	return NULL
 */
static void* findFirstMatch(MemoryArea* area, int size) {
	if (NULL == area || area->freeCnt <= 0 || size <= 0) {
		return NULL;
	}

	int i;
	for (i = 0; i < area->freeCnt; ++i) {
		void *base = *(area->free + i);
		int *plen = (int *)((char *)base - 4);
		int len = *plen;
		if (len >= size + 4) {
			// reassign the free node value
			void *new_base = base + 4 + size;
			int *new_len = (int *)((char *)base + size);
			*new_len = len - size;
			*(area->free +i) = new_base;

			return base;
		}
	}

	return NULL;
}

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
void* sysAlloc(MemoryArea* area, int size)
{
	if (NULL == area || size <= 0) {
		LogD("Invalid parameters");
		return NULL;
	}

	void *retAddr = NULL;
	retAddr =  findFirstMatch(area, size);
	if (NULL != retAddr) {
		return retAddr;
	}

	// 4 bytes used to indicate the length of the buffer will allocate
	// Ii will be used in sysFree()
	if (area->used + size + 4 > area->capability) {
		LogD("memory area is overflowed");
		return NULL;
	}

	int *curr = (int *)(area->current);
	*curr = size;
	area->current += 4;
	retAddr = area->current;
	area->current += size;
	area->used += 4;
	area->used += size;

	return retAddr;
}

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

void sysFree(MemoryArea* area, void *base)
{
	if (NULL == area || NULL == base) {
		LogD("area is NULL or base is NULL");
		return;
	}

	area->free[area->freeCnt] = base;
	++area->freeCnt;

	if (area->freeCnt >= MAX_FREE_CNT) {
		LogD("Max Free Cnt! gc start work! "); 		
	}
}
