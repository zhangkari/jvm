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

#define PATH "Hello.class"
#define CLASS_NAME "Hello"

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

	bool status = linkClass(mainClass, vm.execEnv);
    assert (status);

    status = resolveClass(mainClass);
    assert (status);

    status = initializeClass(mainClass, vm.execEnv);
    assert (status);
    
    Class *cls = findClass("java/lang/Object", vm.execEnv);
    assert (NULL != cls);

	startVM(&vm);
	destroyVM(&vm);
}
