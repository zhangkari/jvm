/****************************************************
 * file name:   engine.h
 * description:	java virtual machine executing engine
 * author:		Kari.Zhang
 * modifications:
 *		1. Created by Kari.zhang @ 2015-11-25
 *
 * **************************************************/

#ifndef __ENGINE__H__
#define __ENGINE__H__

#include "class.h"
#include "runtime.h"

extern void* engineRoutine(void *env);
extern void executeMethod(ExecEnv *env, MethodEntry *method);
extern void executeMethod_spec(ExecEnv *env, MethodEntry *method);

#endif
