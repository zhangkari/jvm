/*******************************
 * file name:   mem.h
 * description: mem manager 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __MEM__H__
#define __MEM__H__

void* sysAlloc(int size);
void sysFree(void *base);

void* gcAlloc(VM *vm, int size);
void gcFree(VM *vm, void *base);

void gcMark(VM *vm, Class *class);

#endif
