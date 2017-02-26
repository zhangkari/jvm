/**************************************
 * file name:   runtime.h
 * description: define jvm runtime lib
 * date:        2016-02-29
 * author:      kari.zhang
 *
 * modifications:
 * 1. Modified by kari.zhang@2016-5-30
 * ***********************************/

#ifndef __RUNTIME__H__
#define __RUNTIME__H__

#include "class.h"
#include "comm.h"
#include "mem.h"

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
	// ExecEnv not include me any more
	// GC mark me as the root node in the future
	Class *mainClass;           // class has static void main(String[])
	char *bootpath;             // java.lang.*
	U4 java_stack;              // java stack size that user specified
	U4 min_heap;                // min heap size, generation area size 
	U4 max_heap;                // max heap size, total starage size
	Property *cmdline_props;    // system properties
	int props_count;            // properties count

	// System.out
	int (*vfprintf)(FILE *stream, const char *fmt, va_list va);

	// System.in
	int (*vfscanf)(FILE *stream, const char *fmt, va_list va);

	void (*exit)(int status);   // System.exit()
	void (*abort)(void);        // System.abort()
} InitArgs;

typedef struct ReferenceHandle {
	U1	use;			// 0 free, 1 used
	Class  *cls_ptr;
	Object *obj_ptr;
} RefHandle;

// Slot store type and value
typedef struct Slot {
    TypeTag     tag;
    uintptr_t   value;
} Slot;

/*
 * SlotBuffer
 */
typedef struct SlotBuffer {
    Slot *slots;	// slot list
    U4    validCnt;	// valid slot count
	U4	  capacity;	// capacity of slot list
	U1	  use;		// 1 means in use, 0 means free
} SlotBuffer;

typedef SlotBuffer LocalVarTable;
typedef SlotBuffer OperandStack;

/**
 * SlotBufferPool
 */
typedef struct SlotBufferPool {
	U4			capacity;
	SlotBuffer *slotbufs;
} SlotBufferPool;

typedef struct StackFrame {
	U1			  use;			// used in StackFramePool for recycling
	LocalVarTable *localTbl;
	OperandStack  *opdStack;
	ConstPool	  *constPool;	// for dynamic linking 
	int32		  pc_reg;		// pc register, -1 means invalid
} StackFrame;

typedef struct StackFramePool {
	U4			capacity;
	StackFrame *frames;
} StackFramePool;

typedef struct RefHandlePool {
	U4		   capacity;
	RefHandle *handles;
} RefHandlePool;

/**
 * Java stack
 */
typedef struct JavaStack {
	int top;
	StackFrame **frames;
} JavaStack;

/*
 * Java virtual machine executing environment in runtime
 */
typedef struct ExecEnv {
    MemoryArea *heapArea;   // java heap memory area
    MemoryArea *stackArea;  // java stack memory area
    JavaStack  *javaStack;  // JavaStack to store stack frames
    U2 userClsCnt;	    // user class count
    Class **userClsArea;    // user class list exclude entry main()
    U2 rtClsCnt;	    // runtime class count
    Class **rtClsArea;	    // runtime class address list
    MethodEntry *mainMethod;// user class main()
    void* dl_handle;	    // dynamic link library handle 
} ExecEnv;

typedef struct VM {
    InitArgs *initArgs;	// initialization arguments
    ExecEnv *execEnv;	// executing environment
} VM;

/**
 * Set the initial arguments to java virtual machine
 */
extern void setDefaultInitArgs(InitArgs *args);
extern int parseCmdLine(int argc, char **argv, Property **props);
extern int readSysConfig(char *path, Property *props);
extern int setInitArgs(Property *props, int nprop, InitArgs *args);

/**
 * findClass & linkClass
 */
extern Class* findClass(const char *clsname, const ExecEnv *env);
extern bool linkClass(Class *cls, const ExecEnv *env);
extern bool initializeClass(Class *cls, ExecEnv *env);

/**
 * Manage java virtual machine
 */
extern void initVM(InitArgs *args, VM *vm);
extern void startVM(VM *vm);
extern void destroyVM(VM *vm);

/*
 * Create a specified capability SlotBufferPool
 */
extern int createSlotBufferPool(int cap);

/*
 * Destroy SlotBufferPool
 */
extern void destroySlotBufferPool();

/*
 * Obtain a SlotBuffer.
 * BE CAREFUL: call recycleSlotBuffer to release
 */
extern SlotBuffer* obtainSlotBuffer();

/*
 * Obtain an SlotBuffer that match the specified capacity
 * It takes the same effect with 
 *  obtainSlotBuffer + ensureCapSlotBufferCap
 */
extern SlotBuffer* obtainCapSlotBuffer(int cap);

/*
 * Recyle SlotBuffer for reuse.
 */
extern void recycleSlotBuffer(SlotBuffer* slotbuf);

/*
 * Ensure SlotBuffer capability
 */
extern int ensureSlotBufferCap(SlotBuffer* buffer, int count);

/*
 * Create a specified capability StackFramePool
 */
extern int createStackFramePool(int cap); 

/*
 * Destroy StackFramePool
 */
extern void destroyStackFramePool(); 

/*
 * Obtain a StackFrame.
 * Notice: call recycleStackFrame to release !
 */
extern StackFrame* obtainStackFrame();

/*
 * Recyle StackFrame for reuse.
 */
extern void recycleStackFrame(StackFrame* frame);

/*
 * Create a specified capacity RefHandlePool
 */
extern int createRefHandlePool(int cap);

/*
 * Destroy RefHandlePool
 */
extern void destroyRefHandlePool();

/*
 * Obtain a RefHandle
 * Notice: call recycleRefHandle to release !
 */
extern RefHandle* obtainRefHandle();

/*
 * Recyle RefHandle for reuse.
 */
extern void recycleRefHandle(RefHandle* handle);

/*
 * Push stack frame into java stack
 */
extern bool pushJavaStack(JavaStack *stack, StackFrame *frame);

/*
 * Pop stack frame from java stack
 */
extern StackFrame* popJavaStack(JavaStack *stack);

/*
 * Peek java stack
 * Return stack top element (not pop out)
 */
extern StackFrame* peekJavaStack(JavaStack *stack);

/*
 * Push operand into operand stack
 */
extern bool pushOperandStack(OperandStack *stack, const Slot *slot);

/*
 * Pop operand from operand stack
 */
extern Slot* popOperandStack(OperandStack *stack);

/*
 * Peek operand stack
 * Return stack top element (not pop out)
 */
extern Slot* peekOperandStack(OperandStack *stack);

/*
 * Check whether java stack is empry
 */
extern bool isJavaStackEmpty(JavaStack *stack);

/*
 * Initialize Slot with ConstPoolEntry
 */
extern void initSlot(Slot *slot, ConstPool *pool, ConstPoolEntry *entry);

/**
 * Native method
 */
typedef int (*NativeFuncPtr)(ExecEnv *env, Object* thiz, ...);
extern NativeFuncPtr retrieveNativeMethod(const MethodEntry* method);

/**
 * Map java method name to native method name
 * Parameters:
 *      method:     the name of the method
 *      clsname:    the class name of the method 
 *      signature:  the signature of the method
 */
extern char* mapMethodName(const char* method, const char* clsname, const char* signature);

/*
 * Create an instance with the type specified by cls
 */
extern Object* newInstance(MemoryArea* mem, const Class* cls);

/*
 * Free the specified instance
 */
extern void freeInstance(MemoryArea* mem, Object* obj);

#endif
