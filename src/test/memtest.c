/**************************************
 * file name:   memtest.c
 * description: test functions in mem.h
 * author:      kari.zhang
 * 
 * modifications:
 *  1. create by kari.zhang @ 2016-01-12
 **************************************/

#include <CUnit/CUnit.h>
#include <comm.h>
#include <mem.h>

// define MemArea. same with MemoryArea
typedef struct MemArea {
	void *base;			// base address
	void *current;		// address used currently
	U4  used;			// memory area has used
	U4  capability;		// area capability
	void **free;		// space that user free
	U4  freeCnt;		// count of free
} MemArea;

#define CAP 4 * 1024 * 1024
void test_createMemoryArea()
{
    MemoryArea *mem = createMemoryArea(CAP);
    MemArea *area = (MemArea *)mem;

    CU_ASSERT_PTR_NOT_NULL(area);
    CU_ASSERT_PTR_NOT_NULL(area->base);
    CU_ASSERT_PTR_NOT_NULL(area->current);
    CU_ASSERT_TRUE(0 == area->used);
    CU_ASSERT_TRUE(CAP == area->capability);
    CU_ASSERT_TRUE(NULL != area->free);
    CU_ASSERT_TRUE(0 == area->freeCnt);
}

void test_destroyMemoryArea()
{
    MemoryArea *mem = createMemoryArea(CAP);
    CU_ASSERT_PTR_NOT_NULL(mem);
    destroyMemoryArea(mem);

    //TODO
    MemArea *area = (MemArea *)mem;
    CU_ASSERT_PTR_NULL(area->base);
    CU_ASSERT_PTR_NULL(area->current);
}

void test_sysAlloc()
{
    MemoryArea *mem = createMemoryArea(CAP);
    CU_ASSERT_PTR_NOT_NULL(mem);
    void *p = NULL;
    p = sysAlloc(mem, 0);
    CU_ASSERT_PTR_NULL(p);

    p = sysAlloc(mem, CAP);
    CU_ASSERT_PTR_NULL(p);

    p = sysAlloc(mem, 4 * 1024);
    CU_ASSERT_PTR_NOT_NULL(p);
    sysFree(mem, p);
}

void test_sysFree()
{
    //TODO
}
