/****************************************************
 * file name:   engine.c
 * description:	java virtual machine executing engine
 * author:		Kari.Zhang
 * modifications:
 *		1. Created by Kari.zhang @ 2015-11-25
 *
 *		2. Update executeMethod() 
 *			 by kari.zhang @ 2015-12-18
 * **************************************************/

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"
#include "jvm.h"
#include "runtime.h"

static int sFrameIdx = 0;
static int sOperandIdx = 0;

static void executeNative(ExecEnv *env, const MethodEntry *method) {
#ifdef DEBUG
    printf("\t<natvie %s:%s>\n", 
            method->name, method->type);
#endif

    NativeFuncPtr funcPtr = retrieveNativeMethod(method);
    if (funcPtr != NULL) {

#ifdef DEBUG
        printf("\t*exec %s\n", method->name);
#endif

        StackFrame *frame = popJavaStack(env->javaStack);
        OperandStack *stack = frame->opdStack;

        // TODO
        // util now just support one argument
        // const int cnt = stack->validCnt;

        Slot *slot = stack->slots;
        assert(slot);
        assert(slot->tag == ReferenceType);
        RefHandle *ref = (RefHandle *)slot->value;
        assert(ref->cls_ptr);
        assert(ref->obj_ptr);

        Slot *param = stack->slots + 1;
        assert(param);

        // TODO
        // util now just support one argument
        funcPtr(env, ref->obj_ptr, param);

    } else {
        ClassEntry *cls = CLASS_CE(method->class);
        printf("\t*Failed retrieve native method:%s.%s:%s\n", cls->name, method->name, method->type);

    }

    if (NULL != env->dl_handle) {
        dlclose(env->dl_handle);
    }

}

void executeMethod(ExecEnv *env, const MethodEntry *method)
{
    assert(NULL != env && NULL != method);

    if (ACC_NATIVE & method->acc_flags) {
        executeNative(env, method);
        return;
    }

#ifdef DEBUG
    char* clsname = CLASS_CE(method->class)->name;
    printf("[++ execute %s.%s start:]\n", clsname, method->name);
#endif

    StackFrame *frame = obtainStackFrame();
    assert (NULL != frame);

    OperandStack *oprdStack = obtainSlotBuffer();
    assert (NULL != oprdStack);

#ifdef DEBUG
    frame->id = sFrameIdx++;
    oprdStack->id = sOperandIdx++;
#endif

    if (ensureSlotBufferCap(oprdStack, method->max_stack) < 0) {
        printf("Failed ensure operand statck capacity");
        exit(-1);
    }

    LocalVarTable *localTbl = obtainSlotBuffer();
    assert (NULL != localTbl);
    if (ensureSlotBufferCap(localTbl, method->max_locals) < 0) {
        printf("Failed ensure local variable table capacity");
        exit(-1);
    }

    frame->localTbl  = localTbl;
    frame->opdStack  = oprdStack;
    frame->constPool = CLASS_CE(method->class)->constPool;

    if (!pushJavaStack(env->javaStack, frame)) {
        printf ("Failed push stack frame to java stack.\n");
        exit (1);
    }

#ifdef DEBUG
    printf("[++ push stack frame:%d]\n", frame->id);
#endif

    // extract & parse instructions from the byte code
    extractInstructions((MethodEntry *)method);

    InstExecEnv instEnv;
    const Instruction *inst = NULL;
    int i;


#if 0
    do {

        i = frame->pc_reg;
        inst = method->instTbl[i]; 

        //
        // FIXME !!!
        // please take care of these instructions such as goto, goto_w
        frame->pc_reg++;
        if (strncmp(stropcode(getInstOpcode(inst)), "goto", 4) == 0) {
            printf("take care of instruction named 'goto' \n");
            exit(1);
        }

        memset(&instEnv, 0, sizeof(instEnv));
        instEnv.inst = (Instruction *)inst;
        instEnv.env = env;

        inst->handler(&instEnv);

    } while (frame->pc_reg < method->instCnt);
#endif

    for (i = 0;  i < method->instCnt; i++) {
        memset(&instEnv, 0, sizeof(instEnv));
        inst = method->instTbl[i];
        instEnv.inst = (Instruction *)inst;
        instEnv.env  = env;

        inst->handler(&instEnv);
    }

#ifdef DEBUG
    printf("[-- execute %s.%s finish.]\n", clsname, method->name);
#endif

}

/*  
 * pop operands from statck to local table
 */
static void popOpdStackToLocalTbl(OperandStack* stack, 
        LocalVarTable *tbl, int count) {

    assert (stack != NULL);
    assert (tbl != NULL);
    assert (count >= 0);
    assert (stack->validCnt >= count);
    assert (tbl->capacity >= count);

    Slot* s;
    int i;
    for (i = count - 1; i >= 0; i--) {
       s = popOperandStack(stack); 
       memcpy(tbl->slots + i, s, sizeof(*s));
    }
    tbl->validCnt = count;
}

/*
 *  Execute constructor, 
 *  the parameters will popped from OperandStack
 *  and placed in LocalTable, these will be done before enter INST_FUNCs
 */
void executeMethod_spec(ExecEnv *env, const MethodEntry *method)
{
    assert(NULL != env && NULL != method);

    if (ACC_NATIVE & method->acc_flags) {
        executeNative(env, method);
        return;
    }

    // do not just support constructor only!
    // do not forget to refacor me !
#if 0
    assert (!strcmp(method->name, "<init>"));
#endif

#ifdef DEBUG
    char* clsname = CLASS_CE(method->class)->name;
    printf("[++ execute %s.%s spec start:]\n", clsname, method->name);
#endif

    StackFrame *frame = obtainStackFrame();
    assert (NULL != frame);

    OperandStack *oprdStack = obtainSlotBuffer();
    assert (NULL != oprdStack);

#ifdef DEBUG
    frame->id = sFrameIdx++;
    oprdStack->id = sOperandIdx++;
#endif

    if (ensureSlotBufferCap(oprdStack, method->max_stack) < 0) {
        printf("Failed ensure operand statck capacity");
        exit(-1);
    }

    LocalVarTable *localTbl = obtainSlotBuffer();
    assert (NULL != localTbl);
    if (ensureSlotBufferCap(localTbl, method->max_locals) < 0) {
        printf("Failed ensure local variable table capacity");
        exit(-1);
    }

    frame->localTbl  = localTbl;
    frame->opdStack  = oprdStack;
    frame->constPool = CLASS_CE(method->class)->constPool;

    /* pop operands from statck to local table */
    StackFrame *top = peekJavaStack(env->javaStack);
    OperandStack *stack = top->opdStack;
    // usually, max_localal equals args_count except main(String[])
    popOpdStackToLocalTbl(stack, frame->localTbl, method->max_locals);
    /** pop finish **/

    if (!pushJavaStack(env->javaStack, frame)) {
        printf ("Failed push stack frame to java stack.\n");
        exit (1);
    }

#ifdef DEBUG
    printf("[++ push stack frame:%d]\n", frame->id);
#endif

    // extract & parse instructions from the byte code
    extractInstructions((MethodEntry *)method);

    InstExecEnv instEnv;
    const Instruction *inst = NULL;
    int i;

    for (i = 0;  i < method->instCnt; i++) {
        memset(&instEnv, 0, sizeof(instEnv));
        inst = method->instTbl[i];
        instEnv.inst = (Instruction *)inst;
        instEnv.env  = env;

        inst->handler(&instEnv);
    }

#ifdef DEBUG
    printf("[-- execute %s.%s spec finish.]\n", clsname, method->name);
#endif

}
