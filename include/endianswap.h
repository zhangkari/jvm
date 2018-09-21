/*******************************************************
 * file name:   endianswap.h
 * description: swap endian
 * date:        2015-12-20
 * author:      kari.zhang
 *
 * modifications:
 *	1. Rename to endianswap.h by kari.zhang @ 2016-01-13
 *
 *  2. Remove swap_16() and swap_32(), 
 *     Because both *.class and networt protocol use Big-Endian,
 *     So, use ntohs() ntohl() instead of swap_16() or swap_32() @ 2018-09-21
 *
 *******************************************************/

#ifndef __ENDIANSWAP__H__
#define __ENDIANSWAP__H__

#include <arpa/inet.h>
#include <comm.h>

#define READ_U1(u1, data) do {	    									\
        u1 = *(data);							        			    \
        (data) ++;                                                      \
} while (0);

#define READ_U2(u2, data) do {	    									\
        u2 = *((U2*)(data));                                            \
        u2 = ntohs(u2);                                              \
        (data) += 2;                                                    \
} while (0);

#define READ_U4(u4, data) do {	    									\
        u4 = *((U4*)(data));                                            \
        u4 = ntohl(u4);                                              \
        (data) += 4;                                                    \
} while (0);

#define READ_INT32(x,y) READ_U4(x,y)

#endif
