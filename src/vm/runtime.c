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
#include <strings.h>
#include <sys/time.h>
#include "class.h"
#include "engine.h"
#include "instpool.h"
#include "jnikit.h"
#include "jvm.h"
#include "runtime.h"
#include "utility.h"

// static Pools
static SlotBufferPool *sSlotBufferPool = NULL;
static StackFramePool *sStackFramePool = NULL;

// TODO
// gc need hold sRefHandlePool
RefHandlePool  *sRefHandlePool  = NULL;

/** static function declare **/
static int findEntryMain(const ClassEntry *clsEntry);
/** declare end **/

void initVM(InitArgs *args, VM *vm) {
    assert(NULL != args || NULL != vm);
    vm->initArgs = args;

    ExecEnv *env = (ExecEnv *)calloc(1, sizeof(ExecEnv));
    assert (NULL != env);

    env->initConf = args;
    env->heapArea = createMemoryArea(args->max_heap);
    assert(NULL != env->heapArea);

    env->stackArea = createMemoryArea(args->java_stack);
    assert(NULL != env->stackArea);

    env->javaStack = (JavaStack *)calloc(1, sizeof(JavaStack));
    assert(NULL != env->javaStack);
    env->javaStack->frames = (StackFrame **)calloc(STACK_MAX_DEPTH, sizeof(StackFrame *));
    assert(NULL != env->javaStack->frames);

#ifdef LOG_TIME_COST
    uint64_t t1 = current_ms();
#endif
    // load classes from rt.jar
    int loadCnt = loadClassFromJar(args->bootpath, &env->rtClsArea);
    if (loadCnt < 1) {
        printf("Error: Failed load run time class.\n");
        exit(1);
    }
    env->rtClsCnt = loadCnt;

#ifdef LOG_TIME_COST
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

    args->in = stdin;
    args->out = stdout;
    args->fprintf = fprintf;
    args->fscanf = fscanf;
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

/*
 * Create a specified capability SlotBufferPool
 */
int createSlotBufferPool(int cap) 
{
	if (sSlotBufferPool != NULL) {
#ifdef LOG_DETAIL
		printf("SlotBuffer Pool already exist\n");
#endif
		return 0;
	}

	assert (cap >= 1 && cap <= STACK_MAX_DEPTH);
	
	SlotBufferPool* pool = (SlotBufferPool *)calloc(1, sizeof(*pool));
	if (NULL == pool) {
		return -1;
	}

	pool->capacity = cap;
	pool->slotbufs = (SlotBuffer *)calloc(cap, sizeof(SlotBuffer));
	if (NULL == pool->slotbufs) {
		return -1;
	}

	sSlotBufferPool = pool;

	return 0;
}

/*
 * Destroy SlotBufferPool
 */
void destroySlotBufferPool() 
{
	SlotBufferPool *pool = sSlotBufferPool;
	if (NULL != pool) {
		if (NULL != pool->slotbufs) {
			free (pool->slotbufs);
			pool->slotbufs = NULL;
		}
		sSlotBufferPool = NULL;
	}
}

/*
 * Obtain a SlotBuffer.
 * Notice: call recycleSlotBuffer to release !
 */
SlotBuffer* obtainSlotBuffer()
{
	assert(NULL != sSlotBufferPool);

	SlotBufferPool* pool = sSlotBufferPool;
	SlotBuffer *slot = NULL;
	int i;
	for (i = 0; i < pool->capacity; ++i) {
		slot = pool->slotbufs + i;
		if (!slot->use) {
			slot->use = 1;
			return slot;
		}
	}

	printf("Failed obtain SlotBuffer in pool.\n");
	printf("Please check if stack overflowed.\n");

	return NULL;
}

/*
 * Obtain an SlotBuffer that match the specified capacity
 * It takes the same effect with 
 *  obtainSlotBuffer + ensureCapSlotBufferCap
 */
SlotBuffer* obtainCapSlotBuffer(int cap)
{
	assert (cap > 0);

	assert (0 && "Not implemented yet!");
}

/*
 * Recyle SlotBuffer for reuse.
 */
void recycleSlotBuffer(SlotBuffer* slotbuf)
{
	assert (NULL != slotbuf);

	slotbuf->use = 0;
	slotbuf->validCnt = 0;
}

/*
 * Ensure SlotBuffer capability
 */
int ensureSlotBufferCap(SlotBuffer* buffer, int count)
{
    // count maybe zero
	assert (NULL != buffer && count >= 0);

	if (buffer->capacity - buffer->validCnt >= count) {
		return 0;
	}

	Slot *slots = NULL;
	slots = (Slot *)realloc(buffer->slots, count * sizeof(Slot));
	if (NULL == slots) {
        printf("Failed realloc mem.\n");
        assert(0 && "Failed realloc mem.");
		return -1;
	}
    free (buffer->slots);
	buffer->slots = slots;
	buffer->capacity = count;

	return 0;
}

/*
 * Create a specified capability StackFramePool
 */
int createStackFramePool(int cap) 
{
	if (sStackFramePool != NULL) {
#ifdef LOG_DETAIL
		printf("StackFrame Pool already exist\n");
#endif
		return 0;
	}

	assert (cap >= 1 && cap <= STACK_MAX_DEPTH);
	
	StackFramePool* pool = (StackFramePool *)calloc(1, sizeof(*pool));
	if (NULL == pool) {
		return -1;
	}

	pool->capacity = cap;
	pool->frames = (StackFrame *)calloc(cap, sizeof(StackFrame));
	if (NULL == pool->frames) {
		return -1;
	}

	sStackFramePool = pool;

	return 0;
}

/*
 * Destroy StackFramePool
 */
void destroyStackFramePool() 
{
	StackFramePool *pool = sStackFramePool;
	if (NULL != pool) {
		if (NULL != pool->frames) {
			free (pool->frames);
			pool->frames = NULL;
		}
		sStackFramePool = NULL;
	}
}

/*
 * Obtain a StackFrame.
 * Notice: call recycleStackFrame to release !
 */
StackFrame* obtainStackFrame()
{
	assert(NULL != sStackFramePool);

	StackFramePool* pool = sStackFramePool;
	StackFrame *frame = NULL;
	int i;
	for (i = 0; i < pool->capacity; ++i) {
		frame = pool->frames + i;
		if (!frame->use) {
			memset(frame, 0, sizeof(StackFrame));
			frame->use = 1;
			return frame;
		}
	}

	printf("Failed obtain StackFrame in pool.\n");
	printf("Please check if stack overflowed.\n");

	return NULL;
}

/*
 * Recyle StackFrame for reuse.
 */
void recycleStackFrame(StackFrame* frame)
{
	assert (NULL != frame);

	frame->use = 0;
}

/*
 * Create a specified capacity RefHandlePool
 */
int createRefHandlePool(int cap) {
	if (sRefHandlePool != NULL) {
#ifdef LOG_DETAIL
		printf("RefHandle Pool already exist\n");
#endif
		return 0;
	}

	assert (cap >= 1 && cap <= STACK_MAX_DEPTH);
	
	RefHandlePool* pool = (RefHandlePool *)calloc(1, sizeof(*pool));
	if (NULL == pool) {
		return -1;
	}

	pool->capacity = cap;
	pool->handles = (RefHandle *)calloc(cap, sizeof(RefHandle));
	if (NULL == pool->handles) {
		return -1;
	}

	sRefHandlePool = pool;

	return 0;
}

/*
 * Destroy RefHandlePool
 */
void destroyRefHandlePool() {
	RefHandlePool *pool = sRefHandlePool;
	if (NULL != pool) {
		if (NULL != pool->handles) {
			free (pool->handles);
			pool->handles = NULL;
		}
		sRefHandlePool = NULL;
	}
}

/*
 * Obtain a RefHandle
 * Notice: call recycleRefHandle to release !
 */
RefHandle* obtainRefHandle() {
	assert(NULL != sRefHandlePool);

	RefHandlePool* pool = sRefHandlePool;
	RefHandle *handle = NULL;
	int i;
	for (i = 0; i < pool->capacity; ++i) {
		handle = pool->handles + i;
		if (!handle->use) {
			handle->use = 1;
			return handle;
		}
	}

	printf("Failed obtain RefHandle in pool.\n");
	printf("Please check if stack overflowed.\n");

	return NULL;
}

/*
 * Recyle RefHandle for reuse.
 */
void recycleRefHandle(RefHandle* handle) {
	assert (NULL != handle);
	handle->use = 0;
}

/*
 * Push stack frame into java stack
 */
bool pushJavaStack(JavaStack *stack, StackFrame *frame) {
	if (NULL == stack || NULL == frame) {
		return FALSE;
	}
	if (stack->top + 1 >= STACK_MAX_DEPTH) {
		fprintf(stderr, "Stack is overflow\n");
		return FALSE;
	}

	stack->frames[stack->top++] = frame;
	return TRUE;
}

StackFrame* popJavaStack(JavaStack *stack) {
	if (NULL == stack || stack->top < 1) {
		return NULL;
	}

	return stack->frames[--stack->top];
}

/*
 * Peek java stack
 * Return stack top element (not pop out)
 */
StackFrame* peekJavaStack(JavaStack *stack) {
	if (NULL == stack || stack->top < 1) {
		return NULL;
	}

	return stack->frames[stack->top - 1];
}

bool isJavaStackEmpty(JavaStack *stack) {
	assert(NULL != stack);
	return stack->top < 1;
}

/*
 * Push operand into operand stack
 */
bool pushOperandStack(OperandStack *stack, const Slot *slot) {

	assert(NULL != stack && NULL != slot);
	assert(NULL != stack->slots);
	assert(stack->capacity > 0);
	assert(stack->validCnt < stack->capacity);

	Slot *current = stack->slots + stack->validCnt;
	++stack->validCnt;
	current->tag = slot->tag;
	current->value = slot->value;

	return TRUE;
}

/*
 * Pop operand from operand stack
 */
Slot* popOperandStack(OperandStack *stack) {
	assert(NULL != stack);
	assert(stack->validCnt > 0);
	assert(stack->validCnt <= stack->capacity);

	--stack->validCnt;
	return stack->slots + stack->validCnt;
}

/*
 *
 */
Slot* peekOperandStack(OperandStack *stack) {
    assert (NULL != stack);
    assert (stack->validCnt >= 0);
    assert (stack->validCnt <= stack->capacity);

    return stack->slots + (stack->validCnt -1);
}

/*
 * Initialize Slot with ConstPoolEntry
 */
void initSlot(Slot *slot, ConstPool *pool, ConstPoolEntry *entry) {

    assert(NULL != slot && NULL != pool && NULL != entry);

#if 0
    printf("++++ initSlot() ++++\n");

    printf("Reference types are not implemented!\n");
#endif

    U2 index;
    U2 name_idx;
    U2 cls_idx;
    U2 type_idx;
    U2 nametype_idx;

    slot->tag = entry->tag;
	switch (entry->tag) {
		case CONST_Utf8:
			slot->value = (uintptr_t)entry->info.utf8_info.bytes;
			break;

		case CONST_Integer:
			slot->value = (uintptr_t)entry->info.integer_info.bytes;
			break;

		case CONST_Float:
			slot->value = (uintptr_t)entry->info.float_info.bytes;
			break;

		case CONST_Long:
			slot->value = (uintptr_t)&entry->info.long_info;
			break;

		case CONST_Double:
			slot->value = (uintptr_t)&entry->info.double_info;
			break;

		case CONST_Class:
			name_idx = entry->info.class_info.name_index;
		    slot->value = (uintptr_t)pool->entries[name_idx].info.utf8_info.bytes;
			break;

		case CONST_String:
			index = entry->info.string_info.string_index;
			slot->value = (uintptr_t)pool->entries[index].info.utf8_info.bytes;
			break;

		case CONST_Fieldref:
			cls_idx = entry->info.fieldref_info.class_index;
			nametype_idx = entry->info.fieldref_info.name_type_index;
			name_idx = pool->entries[nametype_idx].info.nametype_info.name_index;
			index = pool->entries[cls_idx].info.class_info.name_index;
			type_idx = pool->entries[nametype_idx].info.nametype_info.type_index;

#if 0
			printf("Field\t#%d.#%d;  // %s.%s:%s;\n",
					cls_idx, 
					nametype_idx,
					pool->entries[index].info.utf8_info.bytes,
					pool->entries[name_idx].info.utf8_info.bytes,
					pool->entries[type_idx].info.utf8_info.bytes);
#endif
			
			slot->value = (uintptr_t)(pool->entries[index].info.utf8_info.bytes);

			break;

		case CONST_Methodref:
			cls_idx = entry->info.methodref_info.class_index,
					index = pool->entries[cls_idx].info.class_info.name_index;
			nametype_idx = entry->info.methodref_info.name_type_index,
						 name_idx = pool->entries[nametype_idx].info.nametype_info.name_index;
			type_idx = pool->entries[nametype_idx].info.nametype_info.type_index;

#if 0
			printf("Method\t#%d.#%d; // %s.%s:%s\n",
					cls_idx, 
					nametype_idx, 
					pool->entries[index].info.utf8_info.bytes,
					pool->entries[name_idx].info.utf8_info.bytes,
					pool->entries[type_idx].info.utf8_info.bytes);
#endif
			break;

		case CONST_IfMethodref:
			printf("InterfaceMethodref_info not implemented\n");
			break;

		case CONST_NameAndType:
			name_idx = entry->info.nametype_info.name_index;
			type_idx = entry->info.nametype_info.type_index;
#if 0
			printf("NameAndType #%d:%d;// \"%s\":%s\n",
					name_idx,
					type_idx,
					pool->entries[name_idx].info.utf8_info.bytes,
					pool->entries[type_idx].info.utf8_info.bytes);
#endif
			break;

		case CONST_MethodHandle:
			printf("MethodHandle not implemented.\n");
			break;

		case CONST_MethodType:
        	type_idx = entry->info.methodtype_info.type_index;
			slot->value = (uintptr_t)pool->entries[type_idx].info.utf8_info.bytes;
			break;

		case CONST_InvokeDynamic:
			printf("InvokeDynamic not implemented.\n");
			break;
	}

#if 0
    printf("---- initSlot() ----\n");
#endif

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

/*
 * Create an instance with the type specified by cls
 */
extern Object* newInstance(MemoryArea* mem, const Class* cls) {
    assert (NULL != mem && NULL != cls);

    int size = sizeof(Class) + sizeof(ClassEntry); 
    Object *obj = sysAlloc(mem, size); 
    if (NULL == obj) {
        assert (0 && "sysAlloc() return NULL"); 
    }
    bzero(obj, size);

    const int CNT = CLASS_CE(cls)->fields_count;
    ClassEntry *ce = CLASS_CE(cls);
    int fcnt = 0;
    int i;
    for (i = 0; i < CNT; i++) {
        if (ACC_STATIC & ce->fields[i].acc_flags) {
            continue;
        }
        fcnt++;
    }

    if (fcnt > 0) {
        CLASS_CE(obj)->fields_count = fcnt;
        FieldEntry* fields = sysAlloc(mem, fcnt * sizeof(FieldEntry));
        if (NULL == obj) {
            assert (0 && "sysAlloc() return NULL"); 
        }

        CLASS_CE(obj)->fields = fields;
        fcnt = 0;
        for (i = 0; i < CNT; i++) {
            if (ACC_STATIC & ce->fields[i].acc_flags) {
                continue;
            }

            memcpy(fields + fcnt, ce->fields + i, sizeof(FieldEntry));
            fcnt++;
        }
    }

    return obj;
}

/*
 * Free the specified instance
 */
extern void freeInstance(MemoryArea* mem, Object* obj) {
    sysFree(mem, CLASS_CE(obj)->fields);
    sysFree(mem, obj);
}

/**
 * Map java method name to native method name
 * Parameters: *      method:     the name of the method
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

void Java_java_lang_Object_registerNatives(ExecEnv *env, Class *cls) {
#ifdef LOG_DETAIL 
    printf("\t*java.lang.Object.registerNatives()\n");
#endif
}

void Java_java_lang_System_registerNatives(ExecEnv *env, Class *cls) {
#ifdef LOG_DETAIL
    printf("\t*java.lang.System.registerNatives()\n");
#endif
}

long Java_java_lang_System_currentTimeMillis(ExecEnv *env, Class *cls) {
#ifdef LOG_DETAIL
    printf("\tjava_lang_System_currentTimeMillis\n");
#endif
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
  * PrintStream.println(String)
  */
void Java_java_io_PrintStream_println(ExecEnv *env, 
        jobject *thiz, 
        jobject *param) {

#ifdef LOG_DETAIL
    printf("\tjava_io_PrintStream_println\n");
#endif

    Slot* slot = (Slot *)param;
    if (slot->tag == CONST_String) {
        char* str = (char *)(((Slot*)param)->value);
        env->initConf->fprintf(env->initConf->out, "%s\n", str);
    } else if (slot->tag == CONST_Integer) {
        int value = (int)(((Slot*)param)->value);
        env->initConf->fprintf(env->initConf->out, "%d\n", value);
    }

}
