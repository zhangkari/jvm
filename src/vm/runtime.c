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
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "class.h"
#include "engine.h"
#include "instpool.h"
#include "jvm.h"
#include "runtime.h"
#include "utility.h"

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

#ifdef LOG_DETAIL
    uint64_t t1 = current_ms();
#endif
    // load classes from rt.jar
    env->rtClsCnt = loadClassFromJar(args->bootpath, &env->rtClsArea);
    if (env->rtClsCnt < 1) {
        printf("Error: Failed load run time class.\n");
        exit(1);
    }
#ifdef LOG_DETAIL
    uint64_t t2 = current_ms();
    printf("load %d classes cost %lu ms\n", env->rtClsCnt, t2 - t1);
#endif

    /*
     * temporary solution
     */
    // TODO
#define USER_CLS_MAX_CNT 4*1024
    env->userClsArea = (Class **) calloc(USER_CLS_MAX_CNT, sizeof(Class*));
    assert (NULL != env->userClsArea);

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

    if (createRefHandlePool(STACK_MAX_DEPTH) < 0) {
        printf("Failed create RefHandle pool.\n");
        exit (-1);
    }
}

/**
 * retrieve native method from the specified path
 */
NativeFuncPtr retrieveNativeMethod(const MethodEntry *method)
{
    assert (NULL != method);
    assert (ACC_NATIVE & method->acc_flags);

    void* handle = NULL; 
    char* path = NULL;
    ClassEntry *cls = CLASS_CE(method->class);
    char *clsname = cls->name;
    if (!strncmp(clsname, "java", 4)) {
        path = NULL;
    } else {
        path = (char *)calloc(1, strlen(clsname) + 4);
        assert (NULL != path);

        strcpy(path, clsname);
        strcat(path, ".so");
    }

    handle = dlopen(path, RTLD_LAZY);
    if (NULL == handle) {
        printf("Fatal error:failed load %s, %s\n", path, dlerror());
        exit(-1);
    }

    char *nativeName = mapMethodName(method->name, clsname, method->type);
    void *funcPtr = dlsym(handle, nativeName);

    if (NULL != path) {
        free (path);
    }

    if (NULL != nativeName) {
        free (nativeName);
    }

    return (NativeFuncPtr)funcPtr;
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
    executeMethod(vm->execEnv, mainMethod);
}

void destroyVM(VM *vm) {
    destroySlotBufferPool();
    destroyStackFramePool();
    destroyInstPool();
    destroyRefHandlePool();

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
#ifdef DEBUG
#define RT_JAR "./rt.jar"
    rtpath = strdup(RT_JAR);
#else
    if (findRtJar(&rtpath) < 0) {
        printf ("Falal error: rt.jar not found\n");
        exit (1);
    }
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

// Locate class path by classname
static bool locateClassPath(const char *classname, char path[], int size) {
    if (NULL == classname || NULL == path || strlen(classname) + 6 >= size) {
        return FALSE;
    }

    memset(path, 0, size);
    strcpy(path, classname);
    strcat(path, ".class");

    return TRUE;
}

Class* findClass(const char *clsname, const ExecEnv *env) {
    if (NULL == clsname || NULL == env) {
        return NULL;
    }

    Class *cls = findClassImpl(clsname, env->rtClsArea, env->rtClsCnt);
    if (NULL == cls) {
        cls = findClassImpl(clsname, env->userClsArea, env->userClsCnt);
    }
    if (NULL == cls) {
        char path[MAX_PATH];
        bool status = locateClassPath(clsname, path, MAX_PATH);
        assert (status);
        cls = loadClassFromFile(path, clsname);
        env->userClsArea[env->userClsCnt] = cls;
        ExecEnv *ev = (ExecEnv *)env;
        ev->userClsCnt += 1;
        assert (env->userClsCnt < USER_CLS_MAX_CNT);
    }
    return cls;
}

bool linkClass(Class *cls, const ExecEnv *env) {
    return linkClassImpl(cls, env);
}

bool initializeClass(Class *cls, ExecEnv *env) {

    if (NULL == cls || NULL == env) {
        return FALSE;
    }

    ClassEntry *ce = CLASS_CE(cls);

    assert(ce->state >= CLASS_RESOLVED);

    if (ce->state == CLASS_INITED) {
        return TRUE;
    }

    if (ce->state == CLASS_INITING) {
        printf("class is initializing. Not support mutli-thread !\n");
        exit (1);
    }

    if (NULL != ce->super) {
        if (!initializeClass(ce->super, env)) {
            return FALSE;
        }
    }

#define METHOD_NAME "<clinit>"	
#define METHOD_TYPE "()V"
    MethodEntry *method = findMethod(cls, METHOD_NAME, METHOD_TYPE);
    if (NULL != method) {
        executeMethod(env, method);
    }

    ce->state = CLASS_INITED;

    return TRUE;
}

/**
 * Replace char s with char t in src
 *
 * Return:
 *  replaced string if OK
 *  NULL if failed
 *
 * Notice:
 *  Remember to call free()
 */
static char* strrpl(const char *src, char s, char t) {
    const char *dst = NULL;
    if (NULL != src) {
        dst = strdup(src);
        char *tmp = (char *)dst;
        while (*tmp++) {
            if (*tmp == s) {
                *tmp = t;
            }
        }
    }

    return (char *)dst;
}

/**
 * Map java method name to native method name
 * Parameters:
 *      method:     the name of the method
 *      clsname:    the class name of the method 
 *      signature:  the signature of the method
 */
extern char* mapMethodName(const char* method, const char* clsname, const char* signature) {

    assert (NULL != method);
    assert (NULL != clsname);
    assert (NULL != signature);

    char *infix = strrpl(clsname, '/', '_');
    assert (NULL != infix);

#define PREFIX "Java_"
    int len = strlen(PREFIX) + strlen(method) + strlen(clsname);
    char *dest = (char *)calloc(1, len + 1);
    sprintf(dest, "%s%s_%s", PREFIX, infix, method);

    if (NULL != infix) {
        free (infix);
    }

    return dest;
}

void  Java_java_lang_Object_registerNatives(ExecEnv *env, Class *cls) {
    printf("\t*java.lang.Object.registerNatives()\n");
}

void Java_java_lang_System_registerNatives(ExecEnv *env, Class *cls) {
    printf("\t*java.lang.System.registerNatives()\n");
}

long Java_java_lang_System_currentTimeMillis(ExecEnv *env, Class *cls) {
    printf("\tjava_lang_System_currentTimeMillis\n");
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
