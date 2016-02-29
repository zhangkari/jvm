/**************************************
 * file name:   runtime.h
 * description: define jvm runtime lib
 * date:        2016-02-29
 * author:      kari.zhang
 *
 * modifications:
 *
 * ***********************************/

#ifndef __RUNTIME__H__
#define __RUNTIME__H__

#include "jvm.h"

typedef int (*NativeFuncPtr)(ExecEnv *env, const char* name, ...);

extern void* getNativeMethod(const char* method);

#endif
