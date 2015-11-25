/*******************************
 * file name:   jvm.c
 * description: program entry 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * 1. Modified by Kari.Zhang @ 2015-01-19
 *      rearchitect & redesign
 * ****************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "class.h"
#include "instruction.h"
#include "jvm.h"
#include "mem.h"

/*
 * Push stack frame into stack
 */
bool pushStack(JavaStack *stack, StackFrame *frame) {
	if (NULL == stack || NULL == frame) {
		return FALSE;
	}
	if (stack->top + 1 >= stack->size) {
		fprintf(stderr, "Stack is overflow\n");
		return FALSE;
	}

	stack->base[++stack->top] = frame;
	return TRUE;
}

StackFrame* popStack(JavaStack *stack) {
	if (NULL == stack || stack->top < 1) {
		fprintf(stderr, "Stack is downflow\n");
		return NULL;
	}

	return stack->base[--stack->top];
}

bool isEmpty(JavaStack *stack) {
	assert(NULL != stack);
	return stack->top < 1;
}


void initMem(InitArgs *args) {

}

void initGC(InitArgs *args) {

}

void initVM(InitArgs *args, VM *vm) {

}

bool startVM(VM *vm) {

}

bool exitVM(VM *vm) {

}

/**
 * Find rt.jar in CLASSPATH
 * Parameters:
 *		[OUT] path
 * Return:
 *		 0 OK
 *		-1 ERROR
 */
static int findRtJar (char **path) {
	if (NULL == path) {
		return -1;
	}

#define MAX_PATH 256
#define CLASSPATH "CLASSPATH"

	char rtpath[MAX_PATH];
	char *clspath = getenv(CLASSPATH);
	char *start = clspath;
	char *cursor = clspath;

	// skip if start with :
	while (*cursor == ':') {
		++start;
		++cursor;
	}

	while (*cursor != '\0') {
		if (*cursor == ':') {
			memset (rtpath, 0, MAX_PATH);
			strncpy (rtpath, start, cursor - start);
			if ( *(cursor - 1) == '/') {
				strcat(rtpath, "rt.jar");
			} else {
				strcat(rtpath, "/rt.jar");
			}

			FILE *fp = fopen (rtpath, "r");
			if (NULL != fp) {
				*path = (char *)sysAlloc(strlen(rtpath) + 1);
				strcpy(*path, rtpath);
				return 0;
			}

			start = cursor + 1;

		} // if (*cursor == NULL)

		++cursor;

	} // while	

	return -1;
}

void setDefaultInitArgs(InitArgs *args)
{
    assert (NULL != args);

	char* rtpath = NULL;
	if (findRtJar(&rtpath) < 0) {
		printf ("Falal error:rt.jar not found\n");
		exit (1);
	}
	args->bootpath = rtpath;

#define DEFAULT_STACK_SIZE (64 * KB)
    args->java_stack = DEFAULT_STACK_SIZE;

#define MIN_HEAP_SIZE (4 * MB)
    args->min_heap = MIN_HEAP_SIZE;

#define MAX_HEAP_SIZE (16 * MB)
    args->max_heap = MAX_HEAP_SIZE;

    args->vfprintf = vfprintf;
    args->vfscanf = vfscanf;
    args->exit = exit;
    args->abort = abort;
}

int readSysConfig(char *path, Property *props) {
	return 0;
}

int setInitArgs(Property *props, int nprop, InitArgs *args) {
	return 0;
}

int parseCmdLine(int argc, char **argv, Property **props) {
	if (argc == 1) {
		printf("Usage\n");
		printf("  jvm CLASS\n");
		exit(0);
	}

	return 0;
}

/**
 * Find static void main (String []args)
 * Return:
 *		ERROR:	-1
 *		OK:		index in method area
 */
static int findEntryMain(const ClassEntry *clsEntry) {
	if (NULL == clsEntry) {
		return -1;
	}

#define MAIN_METHOD "([Ljava/lang/String;)V"

	int retCode = -1;
	int i;
	for (i = 0; i < clsEntry->methods_count; ++i) {
		MethodEntry *method = clsEntry->methods + i;
		if (strcmp(method->name, "main") == 0 &&
				strcmp(method->type, MAIN_METHOD) == 0 &&
				(method->acc_flags & ACC_PUBLIC) &&
				(method->acc_flags & ACC_STATIC)) {
			retCode = i;
			break;
		}
	}
	
	return retCode;
}

int main(int argc, char *argv[]) {
	InitArgs initArgs;	
	setDefaultInitArgs(&initArgs);

	Property *props = NULL;
	int len = parseCmdLine(argc, argv, &props);
	setInitArgs(props, len, &initArgs);
	
    char path[256];
    strcpy(path, argv[1]);
    strcat(path, ".class");
	Class *mainClass = loadClassFromFile(path, argv[1]);
    if (NULL == mainClass) {
        fprintf(stderr, "Failed loadClass from file\n");
        return -1;
    }

    ClassEntry *clsEntry = CLASS_CE(mainClass);
	int mainIdx = findEntryMain(clsEntry);
	if (mainIdx < 0) {
		printf("Error: Not found main() in %s, please define as:\n", path); 
		printf("  public static void main(String[] args)\n");
		return -1;
	}

	MethodEntry *mainMethod = clsEntry->methods + mainIdx;

}
