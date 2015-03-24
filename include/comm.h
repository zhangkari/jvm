#ifndef __COMM__H__
#define __COMM__H__

#include <stdio.h>

#define PUBLIC
#define PRIVATE static

#define E "ERROR:"
#define W "WARNING:"
#define I "INFO:"
#define D "DEBUG:"

#define Log(format, ...) fprintf(stdout, format, ##__VA_ARGS__)
#define LogD(format, ...) fprintf(stdout, D#format" %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define LogE(format, ...) fprintf(stdout, E#format" %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)

#define VALIDATE_NOT_NULL(X) \
    do { \
        if (NULL == X) { \
            LogE(#X" = NULL in %s", __FUNCTION__);\
            return -1; \
        } \
    } while (0)
	
#define VALIDATE_NOT_NULL2(X, Y)    \
    do {                            \
        VALIDATE_NOT_NULL(X);       \
        VALIDATE_NOT_NULL(Y);       \
    } while (0)
	
#define VALIDATE_NOT_NULL3(X, Y, Z)             \
    do {                                        \
        VALIDATE_NOT_NULL2(X, Y);               \
        VALIDATE_NOT_NULL(X);                   \
    } while (0)
    
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#endif
