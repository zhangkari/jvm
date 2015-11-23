/*************************************
 *file name:	instruction.h 
 *description:	define instruction set
 *
 ************************************/

#ifndef __INSTRUCTION__H__
#define __INSTRUCTION__H__

#include "jvm.h"

typedef bool (*HandlerFunc)(void *);

typedef struct {
	U1 opcode;		      // opcode 
	char *name;		      // name of opcode
	union {
		U1 u1;
		U2 u2;
		U4 u4;
		U8 u8;
	} operand;		      // operand
	U1 tag;			      // indicate operand's lenght (0, 1, 2, 4 or 8)
	HandlerFunc handler;  // handler of opcode
} Instruction;

/**
 * Validate whether the opcode is valid
 * return: 
 *		TURE  valid
 *		FALSE invalid
 */
bool validate_opcode(int opcode);

/**
 * Retrieve the opcode's name
 * Return:
 *		opcode's name if valid
 *		"Invalid opcode" if invalid
 * Notice:
		do not free me
 */
const char* stropcode(int opcode);

/**
  * Get a instruction in the buffer
  * Param:
		buff:		the byte code stream
		codelen:	code's length
 * Return:
		A pointer of the instruction if valid opcode
		NULL if invalid opcode
 * Notice:
		do not free me
 */
const Instruction* getCachedInstruction(U1 *code, int codelen, int offset);

/**
 * Log the instruction info
 */
void logInstruction(const Instruction *inst);

#endif
