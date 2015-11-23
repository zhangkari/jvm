/*******************************
 * file name:   jvm.h
 * description: define jvm 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __JVM__H__
#define __JVM__H__

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

#define KB 1024
#define MB (KB*KB)

#define MIN_HEAP 4*KB
#define MIN_STACK 1*KB

#define DEFAULT_MIN_HEAP 16*MB
#define DEFAULT_MAX_HEAP 128*MB
#define DEFAULT_STACK 246*KB

/*
 * size of emergency area - big enough to create
 * a StackOverFlow exception 
 */
#define STACK_RED_ZONE_SIZE 1*KB

#define JAVA_COMPAT_VERSION "0.0.1" 

/*
 * Store system property
 */
typedef struct Property {
	char *key;
	char *value;
} Property;

/**
 * Initialization arguments
 */
typedef struct InitArgs {
	char *classpath;            // class has static void main(String[])
	char *bootpath;             // java.lang.*
	U4 java_stack;              // java stack size that user specified
	U4 min_heap;                // min heap size
	U4 max_heap;                // max heap size
	Property *cmdline_props;    // system properties
	int props_count;            // properties count
	void *main_stack_base;      // void static main (String[]) stack
	int (*vfprintf)(FILE *stream, const char *fmt, va_list va); // System.out
	int (*vfscanf)(FILE *stream, const char *fmt, va_list va); // System.in
	void (*exit)(int status);   // System.exit()
	void (*abort)(void);        // System.abort()
} InitArgs;

/*
 * Stack frame
 */
typedef struct StackFrame {

} StackFrame;

/**
 * Java stack
 */
typedef struct JavaStack {
	int top;
	int size;
	StackFrame **base;
} JavaStack;

/*
 * Local variable table
 */
typedef struct LocalTable {

} LocalTable;

typedef struct ExecEnv {
    U1 *cur_heap;
    U1 *cur_stack;
    JavaStack *stack;
} ExecEnv;

typedef struct VM {
    U1 *heap_base;
    U4 heap_size;
    U1 *stack_base;
    U4 stack_size;
	InitArgs *initArgs;
    ExecEnv *execEnv;
} VM;

extern void setDefaultInitArgs(InitArgs *args);
extern int parseCmdLine(int argc, char **argv, Property *props);
extern int readSysConfig(char *path, Property *props);
extern int setInitArgs(Property *props, int nprop, InitArgs *args);

extern void initMem(InitArgs *args);
extern void initGC(InitArgs *args);
extern void initVM(InitArgs *args, VM *vm);
extern bool startVM(VM *vm);
extern bool exitVM(VM *vm);

#endif
