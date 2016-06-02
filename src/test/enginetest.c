/**************************************
 * file name:   enginetest.c
 * description: test instruction engine
 * author:      kari.zhang
 * 
 * modifications:
 *  1. create by kari.zhang @ 2016-05-30
 **************************************/

#include <assert.h>
#include <stdio.h>
#include <CUnit/CUnit.h>
#include "class.h"
#include "engine.h"
#include "runtime.h"

#define PATH "Output.class"
#define CLASS_NAME "Output"

void test_engine()
{
	InitArgs initArgs;
	setDefaultInitArgs(&initArgs);

	VM vm;
	memset(&vm, 0, sizeof(vm));
	initVM(&initArgs, &vm);

	Class *mainClass = loadClassFromFile(PATH, CLASS_NAME);
	assert(NULL != mainClass);
	initArgs.mainClass = mainClass;

	logClassEntry(CLASS_CE(mainClass));

	linkClass(mainClass, vm.execEnv);
	startVM(&vm);
	destroyVM(&vm);
}
