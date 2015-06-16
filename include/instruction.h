/*************************************
 *file name:	instruction.h 
 *description:	define instruction set
 *
 ************************************/

#ifndef __INSTRUCTION__H__
#define __INSTRUCTION__H__

#include "jvm.h"

typedef struct {
	U1 opcode;			// opcode 
	char *name;			// name of opcode
	union {
		U2 u2;
		U4 u4;
		U8 u8;
	} operand;			// operand
	U1 tag;				// indicate the lenght of operand(0, 2, 4 or 8)
	void *handler;		// handler of opcode
} Instruction;


bool validate_opcode(int opcode);
const char* stropcode(int opcode);
void logInstruction(Instruction *inst);

#endif
