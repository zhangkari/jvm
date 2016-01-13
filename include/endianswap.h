/*******************************************************
 * file name:   endianswap.h
 * description: swap endian
 * date:        2015-12-20
 * author:      kari.zhang
 *
 * modifications:
 *	1. Rename to endianswap.h by kari.zhang @ 2016-01-13
 *******************************************************/

#ifndef __ENDIANSWAP__H__
#define __ENDIANSWAP__H__

#include <byteswap.h>
#include <comm.h>
#include <machineconf.h>

#define READ_U1(u1, data) do {										\
	u1 = *(data);							        			    \
	(data) ++;                                                      \
} while (0);

#ifdef SMALL_ENDIAN
 #define READ_U2(u2, data) do {										\
    u2 = *((U2*)(data));                                            \
    u2 = bswap_16(u2);                                              \
    (data) += 2;                                                    \
 } while (0);
#else 
 #define READ_U2(u2, data) do {										\
    u2 = *((U2*)(data));                                            \
    (data) += 2;                                                    \
 } while (0);
#endif

#ifdef SMALL_ENDIAN
 #define READ_U4(u4, data) do {										\
    u4 = *((U4*)(data));                                            \
    u4 = bswap_32(u4);                                              \
	(data) += 4;                                                    \
 } while (0);
#else 
 #define READ_U4(u4, data) do {										\
    u4 = *((U4*)(data));                                            \
	(data) += 4;                                                    \
 } while (0);
#endif

#ifdef SMALL_ENDIAN
 #define READ_INT32(u4, data) do {				    				\
    u4 = *((int32*)(data));                                         \
    u4 = bswap_32(u4);                                              \
	(data) += 4;                                                    \
 } while (0);
#else 
 #define READ_INT32(u4, data) do {				    				\
    u4 = *((int32*)(data));                                         \
	(data) += 4;                                                    \
 } while (0);
#endif

#endif
