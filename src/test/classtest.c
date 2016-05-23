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

#define RT_PATH "rt.jar"
#define CLASS_PATH "Hello.class"
#define CLASS_NAME "Hello"
#define MIN_JRE_CNT 8

extern int gRtClsCnt;
extern Class **gRtClsArea;

static void testJavaClassLoader() {
	Class *cls = findSystemClass("java/lang/ClassLoader");
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

#if 0
		logMethodEntry(method);
#endif
	}
}

void test_jre_class() {

	Class ** jreCls = NULL;
	int jreCnt = loadClassFromJar(RT_PATH, &jreCls);
	CU_ASSERT_TRUE(jreCnt > MIN_JRE_CNT);
	CU_ASSERT_PTR_NOT_NULL(jreCls);
	printf("jre class count:%d\n", jreCnt);
	gRtClsCnt = jreCnt;
	gRtClsArea = jreCls;

	testJavaClassLoader();

	Class *mainClass = loadClassFromFile(CLASS_PATH, CLASS_NAME);	
	CU_ASSERT_PTR_NOT_NULL(mainClass);

}
