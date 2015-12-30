/*******************************************
 *file name:	instpool.c 
 *description:	implement instruction pool 
 *author:		kari.zhang
 *
 *modifications:
 *	1. Create by kari.zhang @ 2015-12-29
 ******************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <instpool.h>

typedef struct InstPool {
    Instruction* base;
    U4 capacity;
    U4 validCnt;
	U4 freeIndex;
} InstPool;

InstPool* sPool = NULL;

/**
 * Create instruction pool
 * Return:
 *      TRUE or FALSE
 */
bool createInstPool()
{
#define INSTRUCTION_POOL_SIZE 64 * 1024
    sPool = (InstPool *)calloc(1, sizeof(InstPool));
    assert (NULL != sPool);
    
    sPool->capacity = INSTRUCTION_POOL_SIZE;
    sPool->base = (Instruction *)calloc(sPool->capacity, sizeof(Instruction));
    assert (NULL != sPool->base);
    
    return TRUE;
}

/*
 * Destroy instruction pool
 */
void destroyInstPool()
{
    if (NULL != sPool) {
        if (NULL != sPool->base) {
            free (sPool->base);
            sPool->base = NULL;
        }

        free (sPool);
        sPool = NULL;
    }
}

/*
 * Clone an instruction
 * Return:
 *      Error:  NULL
 *      OK:     Valid pointer
 */
const Instruction* cloneInstruction(const Instruction* inst)
{
	assert (NULL != inst);
	assert (sPool->validCnt < sPool->capacity);

	Instruction *dest = NULL;
	dest = sPool->base + sPool->freeIndex;

	assert (0 == dest->reserve);

	memcpy(dest, inst, sizeof(Instruction)); 
	dest->reserve = 1;
	++sPool->validCnt;

	int i = sPool->freeIndex + 1;
	while (i < sPool->capacity) {
		if (0 == (sPool->base + i)->reserve) {
			sPool->freeIndex = i;
			return dest;
		}
		++i;
	}

	i = sPool->freeIndex - 1;
	while (i >= 0) {
		if (0 == (sPool->base + i)->reserve) {
			sPool->freeIndex = i;
			return dest;
		}
		i--;
	}

	assert (0 && "No free instruction left");
}

/*
 * Free an instruction that constructed by cloneInstruction
 */
void freeInstruction(const Instruction* inst)
{
    assert (NULL != inst);

    Instruction *ins = (Instruction *)inst;
    ins->reserve = 0;
    sPool->validCnt--;
	sPool->freeIndex = (inst - sPool->base);
}
