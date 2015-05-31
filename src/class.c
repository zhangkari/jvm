/****************************
 * file name:   class.c
 *
 ***************************/
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "class.h"
#include "instruction.h"

#define MAGIC_NUMBER 0XCAFEBABE

#define READ_U1(u1, data) do {										\
	u1 = *((U1*)data);											    \
    data++;                                                         \
} while (0);

#define READ_U2(u2, data) do {										\
	U1 v1 = *((U1*)data);							    			\
    data++;                                                         \
    u2 = v1;                                                        \
    u2<<=8;                                                         \
	U1 v2 = *((U1*)data);							    			\
    data++;                                                         \
	u2 += v2;												        \
} while (0);

#define READ_U4(u4, data) do {										\
	U1 v1 = *((U1*)data);							    			\
    data++;                                                         \
    u4 = v1;                                                        \
    u4<<=8;                                                         \
	U1 v2 = *((U1*)data);							    			\
    data++;                                                         \
    u4 += v2;                                                       \
    u4<<=8;                                                         \
	U1 v3 = *((U1*)data);							    			\
    u4 += v3;                                                       \
    u4<<=8;                                                         \
    data++;                                                         \
	U1 v4 = *((U1*)data);							    			\
    u4 += v4;                                                       \
    data++;                                                         \
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
		fprintf(stderr, "Unrecognized class format\n");
		return NULL;
	}
	left -= 4;

	U2 minor_version;
	U2 major_version;
	READ_U2(minor_version, base);
	READ_U2(major_version, base);
	left -= 4;
	printf("minor version:%d\n", minor_version);
	printf("major version:%d\n", major_version);

    U2 cp_count;
    READ_U2(cp_count, base);
    left -= 2;

    printf("constant pool count = %d\n", cp_count);

    U1 tag;
    U2 clsidx;
    U2 nametypeidx;
    U2 stridx;
    U4 bytes;
    U4 u4high;
    U4 u4low;
    U2 nameidx;
    U2 typeidx;

    U2 length;
    char buff[256];
    int i;
    for (i = 1; i < cp_count; ++i) {
        memset(buff, 0, 256);
        READ_U1(tag, base);
        left -= 1;
        switch (tag) {
            case CONST_Utf8:
                READ_U2(length, base);
                left -= 2;
                strncpy(buff, base, length);
                left -= length;
                base += length;
                printf("utf8:%s\n", buff);

                break;

            case CONST_Integer:
                READ_U4(bytes, base);
                left -= 4;
                break;

            case CONST_Float:
                READ_U4(bytes, base);
                left -= 4;
                break;

            case CONST_Long:
                READ_U4(u4high, base);
                READ_U4(u4low, base);
                left -= 8;
                break;

            case CONST_Double:
                READ_U4(u4high, base);
                READ_U4(u4low, base);
                left -= 8;
                break;

            case CONST_Class:
                READ_U2(nameidx, base);
                left -= 2;
                break;

            case CONST_String:
                READ_U2(stridx, base);
                left -= 2;
                break;

            case CONST_Fieldref:
                READ_U2(clsidx, base);
                READ_U2(nametypeidx, base);
                left -= 4;
                break;

            case CONST_Methodref:
                READ_U2(clsidx, base);
                READ_U2(nametypeidx, base);
                left -= 4;
                break;

            case CONST_IfMethodref:
                READ_U2(clsidx, base);
                READ_U2(nametypeidx, base);
                left -= 4;
                break;

            case CONST_NameAndType:
                READ_U2(nameidx, base);
                READ_U2(typeidx, base);
                left -= 4;
                break;

            case CONST_MethodHandle:
                break;

            case CONST_MethodType:
                break;

            case CONST_InvokeDynamic:
                break;
        }
    }

    U2 acc_flags;
    READ_U2(acc_flags, base);
    left -= 2;

    U2 this_class;
    READ_U2(this_class, base);
    left -= 2;

    U2 super_class;
    READ_U2(super_class, base);
    left -= 2;

    U2 ifcount;
    READ_U2(ifcount, base);
    left -= 2;

    // interfaces
    base += 2 * ifcount;
    left -= 2 * ifcount;

    U2 field_count;
    READ_U2(field_count, base);

    printf("left = %d\n", left);

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

Class* loadClassFromFile(char *path, char *classname) {
    if (NULL == path || NULL == classname) {
        fprintf(stderr, "path = NULL || NULL = classname\n");
        return NULL;
    }
    char fname[64];
    if (strlen(path) > 57 /*64-6*/) {
        fprintf(stderr, "Error:class name is too long\n");
        return NULL;
    }
    strcpy(fname, path);
    strcat(fname, ".class");
	FILE *fp = fopen(fname, "rb");
	if (NULL == fp) {
		fprintf(stderr, "Error:failed load class\n");
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	U4 size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

    U1 *buff = (U1 *)sysAlloc(size);
    if (NULL == buff) {
        fprintf(stderr, "Failed malloc mem for class file\n");
        fclose(fp);
        return NULL;
    }
    if (size != fread(buff, 1, size, fp)) {
        fprintf(stderr, "Failed read class file\n");
        sysFree(buff);
        fclose(fp);
        return NULL;
    }
    Class *class = defineClass(classname, buff, 0, size, NULL);
    sysFree(buff);
	fclose(fp);
	return class;
}

Class* loadClassFromJar(char *path, char *classname) {
	return NULL;
}
