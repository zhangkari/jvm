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

#define KB 1024
#define MB (KB*KB)

#define MIN_HEAP 4*KB
#define MIN_STACK 1*KB

#define DEFAULT_MIN_HEAP 16*MB
#define DEFAULT_MAX_HEAP 128*MB
#define DEFAULT_STACK 246*KB

/* size of emergency area - big enough to create
   a StackOverFlow exception */
#define STACK_RED_ZONE_SIZE 1*KB

#define JAVA_COMPAT_VERSION "0.0.1" 

#define _T(x,y) x##y 

typedef struct JavaStack JavaStack;

typedef struct Property {
	char *key;
	char *value;
} Property;

typedef struct InitArgs {
	char *classpath;
	char *bootpath;
	U4 java_stack;
	U4 min_heap;
	U4 max_heap;
	Property *cmdline_props;
	int props_count;
	void *main_stack_base;
	int (*vfprintf)(FILE *stream, const char *fmt, va_list va);
	void (*exit)(int status);
	void (*abort)(void);
} InitArgs;

typedef struct StackFrame {

} StackFrame;

typedef struct ExecEnv {
	JavaStack *stack;
} ExecEnv;

typedef struct VM {
	InitArgs *intArgs;
} VM;

extern void setDefaultInitArgs(InitArgs *args);
extern int parseCmdLine(int argc, char **argv, Property *props);
extern int readSysConfig(char *path, Property *props);
extern int setInitArgs(Property *props, int nprop, InitArgs *args);

extern void initVM(VM *vm);
extern void initMem(InitArgs *args);
extern void initGC(InitArgs *args);
extern bool startVM(VM *vm);
extern bool exitVM(VM *vm);

#endif
