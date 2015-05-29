/****************************
 * file name:   class.c
 *
 * *************************/
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "class.h"
#include "instruction.h"

#define MAGIC_NUMBER 0XCAFEBABE

Class* defineClass(char *classname, char *data, int offset, int len, Object *class_loader) {

	return NULL;
}

void linkClass(Class *class) {

}

Class* initClass(Class *class) {
	return class;
}

Class* findSystemClass(char *classname) {
	return NULL;
}

FieldEntry* findField(Class *class, char *name, char *type) {
	return NULL;
}

MethodEntry* findMethod(Class *class, char *name, char *type) {
	return NULL;
}

MethodEntry* lookupVirtualMethod(Class *class, char *name, char *type) {
	return NULL;
}
