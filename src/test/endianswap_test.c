/***********************************************
 * file name:   endianswap_test.c
 * description: test functions in endianswap.h
 * author:      kari.zhang
 * 
 * modifications:
 *  1. create by kari.zhang @ 2016-01-13
 **********************************************/
#include <CUnit/CUnit.h>
#include <comm.h>
#include <endianswap.h>

void test_READ_U2()
{
	U2 u2val = 0xaabb;
	char *data = (char *)&u2val;
	U2 val;
	READ_U2(val, data);

	CU_ASSERT_TRUE(0xbbaa == val);
}

void test_READ_U4()
{
	U4 u4val = 0xaabbccdd; 
	char *data = (char *)&u4val;
	U4 val;
	READ_U4(val, data);

	CU_ASSERT_TRUE(0xddccbbaa == val);
}

void test_READ_INT32()
{
	int u4val = 0xaabbccdd;
	char *data = (char *)&u4val;
	int val;
	READ_INT32(val, data);

	CU_ASSERT_TRUE(0xddccbbaa == val);
}