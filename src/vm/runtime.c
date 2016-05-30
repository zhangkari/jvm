/**************************************
 * file name:   runtime.c
 * description: define jvm runtime lib
 * date:        2016-02-29
 * author:      kari.zhang
 *
 * modifications:
 *
 * ***********************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "class.h"
#include "engine.h"
#include "instpool.h"
#include "jvm.h"
#include "runtime.h"
#include "utility.h"

/******************************************/
/* run time classes preload from rt.jar */
// gRtClsCnt  = VM->execEnv->rtClsCnt
// gRtClsArea = VM->execEnv->rtClsArea
extern int gRtClsCnt;
extern Class **gRtClsArea;
/******************************************/

/** static function declare **/
static int findEntryMain(const ClassEntry *clsEntry);
/** declare end **/

void initVM(InitArgs *args, VM *vm) {
	assert(NULL != args || NULL != vm);
	vm->initArgs = args;

    ExecEnv *env = (ExecEnv *)calloc(1, sizeof(ExecEnv));
    assert (NULL != env);

	env->heapArea = createMemoryArea(args->max_heap);
	assert(NULL != env->heapArea);

	env->stackArea = createMemoryArea(args->java_stack);
	assert(NULL != env->stackArea);

    env->javaStack = (JavaStack *)calloc(1, sizeof(JavaStack));
    assert(NULL != env->javaStack);
    env->javaStack->frames = (StackFrame **)calloc(STACK_MAX_DEPTH, sizeof(StackFrame *));
    assert(NULL != env->javaStack->frames);

#ifdef DEBUG
	uint64_t t1 = current_ms();
#endif
    // load classes from rt.jar
    env->rtClsCnt = loadClassFromJar(args->bootpath, &env->rtClsArea);
    if (env->rtClsCnt < 1) {
        printf("Error: Failed load run time class.\n");
        exit(1);
    }
#ifdef DEBUG
	uint64_t t2 = current_ms();
	printf("load %d classes cost %lu ms\n", env->rtClsCnt, t2 - t1);
#endif

	/*
	 * the two variables are assigned values here & defined in class.c . 
	 * These smell bad! Refactor them in some days;
	 */
    gRtClsCnt = env->rtClsCnt;
    gRtClsArea = env->rtClsArea;

    vm->execEnv = env;

	if (createSlotBufferPool(STACK_MAX_DEPTH) < 0) {
		printf("Failed create slot buffer pool.\n");
		exit (-1);
	}

	if (createStackFramePool(STACK_MAX_DEPTH) < 0) {
		printf("Failed create StackFrame pool.\n");
		exit (-1);
	}

    if (!createInstPool()) {
        printf("Failed create instruction pool.\n");
        exit (-1);
    }
}


void* getNativeMethod(const char* method)
{
	return NULL;
}

void startVM(VM *vm) {
	Class *mainClass = vm->initArgs->mainClass;
    ClassEntry *clsEntry = CLASS_CE(mainClass);
	int mainIdx = findEntryMain(clsEntry);
	if (mainIdx < 0) {
		printf("Error: Not found main() in %s, please define as:\n", 
				clsEntry->name); 
		printf("  public static void main(String[] args)\n");
		return;
	}

	MethodEntry *mainMethod = clsEntry->methods + mainIdx;
	// Used as a root node by gc to mark garbage in the future
	vm->execEnv->mainMethod = mainMethod;
	executeMethod(vm, mainMethod);
}

void destroyVM(VM *vm) {
	destroySlotBufferPool();
    destroyStackFramePool();
    destroyInstPool();

	assert (NULL != vm);
}

/* 
 * This function smell bad !!!
 *
 *     //FIXME
 *
 * Check if rt.jar in the path 
 * Parameters:
 *      path:   [IN, OUT] such as /usr/lib or /usr/lib/
 *              after return, path will be altered, 
 *              file postfix will be added, /user/lib/rt.jar
 * Return:
 *      1  OK   
 *      0  ERROR
 */
static int existRtJar(char *path) {
    if (NULL == path) {
        return FALSE;
    }

    if ( *(path + strlen(path) - 1) == '/') {
        strcat(path, "rt.jar");
    } else {
        strcat(path, "/rt.jar");
    }

    FILE *fp = fopen (path, "r");
    if (NULL != fp) {
        fclose (fp);
        return TRUE;
    }

    return FALSE;
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
    if (NULL == clspath) {
        printf ("Fatal error: CLASSPATH not set\n");
        return -1;
    }

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
            if (existRtJar(rtpath)) {
            	*path = (char *)calloc(1, strlen(rtpath) + 1);
				strcpy(*path, rtpath);
                return 0;
            }

			start = cursor + 1;

		} // if (*cursor == ':')

		++cursor;

	} // while	

    // No ":" separator
    if (start == clspath) {
        memset (rtpath, 0, MAX_PATH);
        strncpy (rtpath, start, cursor - start);
        if (existRtJar(rtpath)) {
            *path = (char *)calloc(1, strlen(rtpath) + 1);
            strcpy(*path, rtpath);
            return 0;
        }
    }

	return -1;
}


void setDefaultInitArgs(InitArgs *args)
{
    assert (NULL != args);

	char* rtpath = NULL;

/*
 * do not search rt.jar in CLASSPATH
 * We just support jdk1.5 util now
 * We enable it in some day
 */
#if 0
	if (findRtJar(&rtpath) < 0) {
		printf ("Falal error: rt.jar not found\n");
		exit (1);
	}
#else
    #define RT_JAR "./rt.jar"
    rtpath = strdup(RT_JAR);
#endif

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

/*
 * Set properties to InitArgs
 */
int setInitArgs(Property *props, int nprop, InitArgs *args) {
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

static void usage() {
	printf("Usage: jvm [-options] class [args...]\n");
	printf("Options include:\n");
	printf("	 -v			print version\n");
	printf("	 -h			print this\n");
}

/*
 * Parse command line user input
 */
int parseCmdLine(int argc, char **argv, Property **props) {
	if (argc == 1) {
		usage();
		exit(0);
	} 
	else if (argc == 2) {
		if (strcmp(argv[1], "-v") == 0) {
			printf("jvm %s compiled on %s copyright@kari.zhang\n", JAVA_VERSION, __DATE__);
			exit(0);
		}
		else if (strcmp(argv[1], "-h") == 0) {
			usage();
			exit(0);
		}
		else if (argv[1][0] == '-') {
			printf ("Invalid options\n");
			usage();
			exit(0);
		}
	}

	return 0;
}

