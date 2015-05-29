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
#include "class.h"
#include "instruction.h"
#include "jvm.h"

struct JavaStack {
	int top;
	int size;
	StackFrame **base;
};

bool pushStack(JavaStack *stack, StackFrame *frame) {
	if (NULL == stack || NULL == frame) {
		return FALSE;
	}
	if (stack->top + 1 >= stack->size) {
		fprintf(stderr, "Stack is overflow");
		return FALSE;
	}

	stack->base[++stack->top] = frame;
	return TRUE;
}

StackFrame* popStack(JavaStack *stack) {
	if (NULL == stack || stack->top < 1) {
		fprintf(stderr, "Stack is bottomflow");
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

void initVM(VM *vm) {

}

bool startVM(VM *vm) {

}

bool exitVM(VM *vm) {

}

int main(int argc, char *argv[]) {

}
