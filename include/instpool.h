/*******************************************
 *file name:	instpool.h 
 *description:	define instruction pool 
 *author:		kari.zhang
 *
 *modifications:
 *	1. Create by kari.zhang @ 2015-12-29
 ******************************************/

#ifndef __INSTPOOL__H__
#define __INSTPOOL__H__

#include <instruction.h>

/**
 * Create instruction pool
 * Return:
 *      TRUE or FALSE
 */
bool createInstPool();

/*
 * Destroy instruction pool
 */
void destroyInstPool();

/*
 * Clone an instruction
 * Return:
 *      Error:  NULL
 *      OK:     Valid pointer
 */
const Instruction* cloneInstruction(const Instruction* inst);

/*
 * Free an instruction that constructed by cloneInstruction
 */
void freeInstruction(const Instruction* inst);

#endif
