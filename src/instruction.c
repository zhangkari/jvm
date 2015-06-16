/*************************************
 *file name:	instruction.c 
 *description:	define instruction set
 *
 ************************************/

#include <instruction.h>


#define nop			0x00
#define aconst_null 0x01
#define iconst_m1	0x02
#define iconst_0	0x03
#define iconst_1	0x04
#define iconst_2	0x05
#define iconst_3	0x06
#define iconst_4	0x07
#define iconst_5	0x08


#define _T(X) #X
#define INIT_INST(X) X, _T(X), 0, 0, NULL

static Instruction sInstructionTable[] = {
	INIT_INST(nop),
	INIT_INST(aconst_null),
	INIT_INST(iconst_m1),
	INIT_INST(iconst_0),
	INIT_INST(iconst_1),
	INIT_INST(iconst_2),
	INIT_INST(iconst_3),
	INIT_INST(iconst_4),
	INIT_INST(iconst_5),
};

static void* sFunctionTable[] = {
	NULL,
	NULL,
};

bool validate_opcode(int opcode) {
	return FALSE;
}

const char* stropcode(int opcode) {
	return NULL;
}


