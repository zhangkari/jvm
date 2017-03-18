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

void executeMethod(ExecEnv *env, const MethodEntry *method)
{
    assert(NULL != env && NULL != method);

    if (ACC_NATIVE & method->acc_flags) {

        printf("\t<natvie %s:%s>\n", 
                method->name, method->type);

        NativeFuncPtr funcPtr = retrieveNativeMethod(method);
        if (funcPtr != NULL) {
            printf("\t*exec %s\n", method->name);
            funcPtr(env, method->class);

        } else {
            ClassEntry *cls = CLASS_CE(method->class);
            printf("\t*Failed retrieve native method:%s.%s:%s\n", cls->name, method->name, method->type);

        }

        if (NULL != env->dl_handle) {
            dlclose(env->dl_handle);
        }

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

    StackFrame *lastFrame = popJavaStack(env->javaStack);
    assert (lastFrame != NULL);
    // TODO
    // lastFrame->retAddr;

#ifdef DEBUG
    printf("[-- execute %s.%s finish.]\n", clsname, method->name);
#endif

}


/*
 *  Execute constructor, 
 *  the parameters will popped from OperandStack
 *  and placed in LocalTable, these will be done before enter INST_FUNCs
 */
void executeMethod_spec(ExecEnv *env, const MethodEntry *method)
{
    assert(NULL != env && NULL != method);

    assert (!strcmp(method->name, "<init>"));
    if (strcmp(method->type, "()V")) {
	assert (0 && "just default constructor support!");
    }

#ifdef DEBUG
    char* clsname = CLASS_CE(method->class)->name;
    printf("[++ execute %s.%s start:]\n", clsname, method->name);
#endif

    StackFrame *frame = obtainStackFrame();
    assert (NULL != frame);

    OperandStack *oprdStack = obtainSlotBuffer();
    assert (NULL != oprdStack);
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

    Slot* thiz = popOperandStack(peekJavaStack(env->javaStack)->opdStack);
    // add slots[0] = thiz
    memcpy(frame->localTbl->slots, thiz, sizeof(Slot));
    ++frame->localTbl->validCnt;

    //free (thiz);
    // add slots[1]
    //


    if (!pushJavaStack(env->javaStack, frame)) {
        printf ("Failed push stack frame to java stack.\n");
        exit (1);
    }

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

    StackFrame *lastFrame = popJavaStack(env->javaStack);
    assert (lastFrame != NULL);
    // TODO
    // lastFrame->retAddr;

#ifdef DEBUG
    printf("[-- execute %s.%s finish.]\n", clsname, method->name);
#endif

}
