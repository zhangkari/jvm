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
#include "memtest.h"

static int suite_success_clean() {
    return 0;
}

static int suite_success_init() {
    return 0;
}

CU_TestInfo memSuite[] = {
    {"test_createMemoryArea:", test_createMemoryArea},
    {"test_destroyMemoryArea:", test_destroyMemoryArea},
    {"test_sysAlloc:", test_sysAlloc},
    {"test_sysFree:", test_sysFree},
    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] = {
    {"mem:", suite_success_init, suite_success_clean, NULL, NULL, memSuite},
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
