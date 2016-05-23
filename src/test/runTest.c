/****************************************
 * file name:   runTest.c
 * author:      kari.zhang
 *
 * modifications:
 *  1. created by kari.zhang @ 2016-01-12
 ****************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "classtest.h"
#include "endianswap_test.h"
#include "memtest.h"

static int suite_success_clean() {
    return 0;
}

static int suite_success_init() {
    return 0;
}

// mem test suite
CU_TestInfo memSuite[] = {
    {"test_createMemoryArea:", test_createMemoryArea},
    {"test_destroyMemoryArea:", test_destroyMemoryArea},
    {"test_sysAlloc:", test_sysAlloc},
    {"test_sysFree:", test_sysFree},
    CU_TEST_INFO_NULL
};

 // endian swap test suite
CU_TestInfo endianSuite[] = {
    {"test_READ_U2:", test_READ_U2},
    {"test_READ_U4:", test_READ_U4},
    {"test_READ_INT32:", test_READ_INT32},
    CU_TEST_INFO_NULL
};

// class test suite
CU_TestInfo classSuite[] = {
	{"test_jre_class", test_jre_class},
	CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] = {
    // mem test suite
    {"mem:", suite_success_init, suite_success_clean, NULL, NULL, memSuite},

    // endian swap test suite
    {"endian:", suite_success_init, suite_success_clean, NULL, NULL, endianSuite},
    
	// class test suite
	{"class:", suite_success_init, suite_success_clean, NULL, NULL, classSuite},

    CU_SUITE_INFO_NULL
};

static void addTests() {
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if (CUE_SUCCESS != CU_register_suites(suites)) {
        exit(EXIT_FAILURE);
    }
}

int runTest()
{
    if (CU_initialize_registry()) {
        fprintf(stderr, "init test registry failed.\n");
        exit (EXIT_FAILURE);
    }

    addTests();
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
