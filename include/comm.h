/*******************************
 * file name:   comm.h
 * description: define base type  
 * date:        2015-11-25
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __COMM__H__
#define __COMM__H__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define TRUE  1
#define FALSE 0

#define bool char

typedef uint8_t  U1;
typedef uint16_t U2;
typedef uint32_t U4;
typedef uint64_t U8;

typedef int32_t int32;

#define KB 1024
#define MB (KB*KB)

#endif
