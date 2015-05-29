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

#define READ_U1(u1, data) do {										\
	u1 = *((U1*)data++);											\
} while (0);

#define READ_U2(u2, data) do {										\
	U1 v1 = *((U1*)(data++));										\
	U1 v2 = *((U1*)(data++));										\
	u2 = v2 << 8 + v1;												\
} while (0);

#define READ_U4(u4, data) do {										\
	U1 v1 = *((U1*)(data++));										\
	U1 v2 = *((U1*)(data++));										\
	U1 v3 = *((U1*)(data++));										\
	U1 v4 = *((U1*)(data++));										\
	u4 = v4 << 24 + v3 << 16 + v2 << 8 + v1;						\
} while (0);

Class* defineClass(char *classname, char *data, int offset, int len, Object *class_loader) {
	if (NULL == classname || NULL == data || offset >= len) {
		return NULL;
	}
	U4 left = len - offset;
	unsigned char *base = (unsigned char *)data + offset;

	U4 magic;
	READ_U4(magic, base);
	if (magic != MAGIC_NUMBER) {
		fprintf(stderr, "Unrecognized format");
		return NULL;
	}
	left -= 4;

	U2 minor_version;
	U2 major_version;
	READ_U2(minor_version, base);
	READ_U2(major_version, base);
	printf("minor version:%d\n", minor_version);
	printf("major version:%d\n", major_version);
	left -= 4;


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

Class* loadClassFromFile(char *path, char *classname)
{
	FILE *fp = fopen(path, "rb");
	if (NULL == fp) {
		fprintf(stderr, "Failed open file:%s\n", path);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	U4 size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	printf("file:%s len:%d\n", path, size);

	fclose(fp);
	return NULL;
}

Class* loadClassFromJar(char *path, char *classname)
{
	return NULL;
}
