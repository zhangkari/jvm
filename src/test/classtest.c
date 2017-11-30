/**************************************
 * file name:   classtest.c
 * description: test functions in class.h
 * author:      kari.zhang
 * 
 * modifications:
 *  1. create by kari.zhang @ 2016-05-22
 **************************************/

#include <stdio.h>
#include <CUnit/CUnit.h>
#include "class.h"
#include "runtime.h"

#define RT_PATH "rt.jar"
#define CLASS_PATH "Hello.class"
#define CLASS_NAME "Hello"

/* 
 Object 
 System 
 ClassLoader 
*/
#define MIN_JRE_CNT 3

static void testJavaClassLoader(const VM *vm) {
	Class *cls = findClass("java/lang/ClassLoader", vm->execEnv);
	CU_ASSERT_PTR_NOT_NULL(cls);
	ClassEntry *entry = CLASS_CE(cls);
	CU_ASSERT_PTR_NOT_NULL(entry);

#if 0
	logConstPool(entry->constPool);
#endif

	char* ALL_SET[] = {
			// method name
			"defineClass",							

			// method type 1
			"([BII)Ljava/lang/Class;",	

			// method type 2
			"(Ljava/lang/String;[BII)Ljava/lang/Class;",

			// method type 3
			"(Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;",

			// method type 4
			"(Ljava/lang/String;Ljava/nio/ByteBuffer;Ljava/security/ProtectionDomain;)Ljava/lang/Class;"
			
	};

	int i = 0;
	int size = sizeof(ALL_SET) / sizeof(ALL_SET[0]);
    for (i = 1 ; i < size; ++i) {
        MethodEntry *method = findMethod(cls, ALL_SET[0], ALL_SET[i]);
        CU_ASSERT_PTR_NOT_NULL(method);

#ifdef LOG_DETAIL
        logMethodEntry(method);
#endif
    }
}

static void testJavaClass(const VM *vm) {
	const char* CLASS_SET[] = {
		"java/lang/Object",
		"java/lang/System",
		"java/lang/Class",
	};

	int i;
	int size = sizeof(CLASS_SET)/sizeof(CLASS_SET[0]);
    for (i = 0; i < size; i++) {
        Class *cls = findClass(CLASS_SET[i], vm->execEnv);
        CU_ASSERT_PTR_NOT_NULL(cls);
#ifdef LOG_DETAIL
        logClassEntry(CLASS_CE(cls));
#endif
    }
}

void test_class() {
	
	InitArgs initArgs;
	setDefaultInitArgs(&initArgs);

	VM vm;
	memset(&vm, 0, sizeof(vm));
	initVM(&initArgs, &vm);

	Class ** jreCls = NULL;
	int jreCnt = loadClassFromJar(RT_PATH, &jreCls);
	CU_ASSERT_TRUE(jreCnt > MIN_JRE_CNT);
	CU_ASSERT_PTR_NOT_NULL(jreCls);
#ifdef LOG_DETAIL
	printf("jre class count:%d\n", jreCnt);
#endif

//	testJavaClassLoader(&vm);
	testJavaClass(&vm);

	Class *mainClass = loadClassFromFile(CLASS_PATH, CLASS_NAME);	
	CU_ASSERT_PTR_NOT_NULL(mainClass);
#ifdef LOG_DETAIL
	logClassEntry(CLASS_CE(mainClass));
#endif

}
