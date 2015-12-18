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
#include "engine.h"

void executeMethod(VM *vm, MethodEntry *method)
{
	assert(NULL != vm && NULL != method);

	printf("execute %s start:\n", method->name);


	printf("execute %s finish.\n", method->name);
}
