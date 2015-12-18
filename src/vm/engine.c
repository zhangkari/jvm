/****************************************************
 * file name:   engine.c
 * description:	java virtual machine executing engine
 * author:		Kari.Zhang
 * modifications:
 *		1. Created by Kari.zhang @ 2015-11-25
 *
 *		2. Update executeMethod() 
 *			 by kari.zhang @ 2015-12-18
 * **************************************************/

#include <assert.h>
#include <stdlib.h>
#include "engine.h"
#include "jvm.h"

void executeMethod(VM *vm, MethodEntry *method)
{
	assert(NULL != vm && NULL != method);

	printf("execute %s start:\n", method->name);

	StackFrame *frame = obtainStackFrame();
	assert (NULL != frame);

	OperandStack *oprdStack = obtainSlotBuffer();
	assert (NULL != oprdStack);
	if (ensureSlotBufferCap(oprdStack, method->max_stack) < 0) {
		assert (0 && "Failed ensure operand statck capacity");
	}

	LocalVarTable *localTbl = obtainSlotBuffer();
	assert (NULL != localTbl);
	if (ensureSlotBufferCap(localTbl, method->max_stack) < 0) {
		assert (0 && "Failed ensure local variable table capacity");
	}

	frame->localTbl  = localTbl;
	frame->opdStack  = oprdStack;
	frame->constPool = CLASS_CE(method->class)->constPool;

	// Resolve compile error
#if 0
	if (!pushStack(vm->execEnv->javaStack, frame)) {
		printf ("Failed push stack frame to java stack.\n");
		exit (1);
	}
#endif

	printf("execute %s finish.\n", method->name);
}
