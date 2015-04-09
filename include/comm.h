#ifndef __COMM__H__
#define __COMM__H__

#include <stdint.h>
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
    
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

#endif
