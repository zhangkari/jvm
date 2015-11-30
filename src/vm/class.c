/********************************************************
 * file name:   class.c
 * author:		kari.zhang
 *
 * Modifications:
 *	1. Created by kari.zhang
 *  2. Redesign defineClass() by kari.zhang @ 2015-11-30
 *******************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "class.h"
#include "instruction.h"
#include "libjar.h"

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

#define READ_U8(u8, data) do {										\
	U1 v1 = *((U1*)data);							    			\
	data++;                                                         \
	u8 = v1;                                                        \
	u8<<=8;                                                         \
	\
	U1 v2 = *((U1*)data);							    			\
	data++;                                                         \
	u8 += v2;                                                       \
	u8<<=8;                                                         \
	\
	U1 v3 = *((U1*)data);							    			\
	u8 += v3;                                                       \
	u8<<=8;                                                         \
	data++;                                                         \
	\
	U1 v4 = *((U1*)data);							    			\
	u8 += v4;                                                       \
	u8<<=8;															\
	data++;                                                         \
	\
	U1 v5 = *((U1*)data);											\
	u8 += v5;														\
	u8<<=8;															\
	data++;															\
	\
	U1 v6 = *((U1*)data);											\
	u8 += v6;														\
	u8<<=8;															\
	data++;															\
	\
	U1 v7 = *((U1*)data);											\
	u8 += v7;														\
	u8<<=8;															\
	data++;															\
	\
	U1 v8 = *((U1*)data);											\
	u8 += v8;														\
	data++;															\
} while (0);


/*
 * Read Constant Value Pool
 */
static U1* readConstPool(ClassEntry* class, U2 cp_count, U1* base) {

	assert(NULL != class && cp_count > 0 && NULL != base);

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

	ConstPool *constPool = newConstPool(cp_count);
	assert(NULL != constPool);
	int i;
	int j;
	for (i = 1; i < cp_count; ++i) {
		READ_U1(tag, base);
		constPool->entries[i].tag = tag;
		switch (tag) {
			case CONST_Utf8:
				READ_U2(length, base);
				constPool->entries[i].info.utf8_info.bytes = calloc(1, length + 1);
				assert(NULL != constPool->entries[i].info.utf8_info.bytes);
				memcpy(constPool->entries[i].info.utf8_info.bytes, base, length);
				constPool->entries[i].info.utf8_info.length = length;
				base += length;
				break;

			case CONST_Integer:
				READ_U4(bytes, base);
				constPool->entries[i].info.integer_info.bytes = bytes;
				break;

			case CONST_Float:
				READ_U4(bytes, base);
				constPool->entries[i].info.float_info.bytes = bytes;
				break;

			case CONST_Long:
				READ_U4(u4high, base);
				READ_U4(u4low, base);
				constPool->entries[i].info.long_info.high_bytes = u4high;
				constPool->entries[i].info.long_info.low_bytes = u4low;
				++i;
				break;

			case CONST_Double:
				READ_U4(u4high, base);
				READ_U4(u4low, base);
				constPool->entries[i].info.double_info.high_bytes = u4high;
				constPool->entries[i].info.double_info.low_bytes = u4low;
				++i;
				break;

			case CONST_Class:
				READ_U2(nameidx, base);
				constPool->entries[i].info.class_info.name_index = nameidx;
				break;

			case CONST_String:
				READ_U2(stridx, base);
				constPool->entries[i].info.string_info.string_index = stridx;
				break;

			case CONST_Fieldref:
				READ_U2(clsidx, base);
				READ_U2(nametypeidx, base);
				constPool->entries[i].info.fieldref_info.class_index = clsidx;
				constPool->entries[i].info.fieldref_info.name_type_index = nametypeidx;
				break;

			case CONST_Methodref:
				READ_U2(clsidx, base);
				READ_U2(nametypeidx, base);
				constPool->entries[i].info.methodref_info.class_index = clsidx;
				constPool->entries[i].info.methodref_info.name_type_index = nametypeidx;
				break;

			case CONST_IfMethodref:
				READ_U2(clsidx, base);
				READ_U2(nametypeidx, base);
				constPool->entries[i].info.ifmethodref_info.class_index = clsidx;
				constPool->entries[i].info.ifmethodref_info.name_type_index = nametypeidx;
				break;

			case CONST_NameAndType:
				READ_U2(nameidx, base);
				READ_U2(typeidx, base);
				constPool->entries[i].info.nametype_info.name_index = nameidx;
				constPool->entries[i].info.nametype_info.type_index = typeidx;
				break;

			case CONST_MethodHandle:
				printf("MethodHandle\n");
				break;

			case CONST_MethodType:
				printf("MethodType\n");
				break;

			case CONST_InvokeDynamic:
				printf("InvokeDynamic\n");
				break;

			default:
				printf("Unknown ConstantPoolEntry\n");
		}
	}

	class->constPool = constPool;

	return base;
}

static U1* readClassField(ClassEntry *class, U2 field_count, U1* base) {

	assert (NULL != class && base != NULL && field_count >= 0);

	char *name = NULL;
	U2 acc_flags;
	U2 attr_count;
	U2 clsidx;
	U2 nameidx;
	U2 typeidx;

	ConstPool *constPool = class->constPool;
	int i, j;
	for (i = 0; i < field_count; ++i) {
		READ_U2(acc_flags, base);
		class->fields[i].acc_flags = acc_flags;

		READ_U2(nameidx, base);
		name = constPool->entries[nameidx].info.utf8_info.bytes;
		class->fields[i].name = calloc(1, strlen(name) + 1);
		strcpy(class->fields[i].name, name);

		READ_U2(typeidx, base);
		name = constPool->entries[typeidx].info.utf8_info.bytes;
		class->fields[i].type = calloc(1, strlen(name) + 1);
		strcpy(class->fields[i].type, name);

		U2 const_value_idx;
		U4 attr_length;
		READ_U2(attr_count, base);
		for (j = 0; j < attr_count; ++j) {
			READ_U2(nameidx, base);
			READ_U4(attr_length, base);
			name = constPool->entries[nameidx].info.utf8_info.bytes;

			if (!strcmp(name, "ConstantValue")) {
				READ_U2(const_value_idx, base);	
				class->fields[i].constant = constPool->entries[const_value_idx].tag;
			} else if (!strcmp(name, "Signature")) {
				U2 signature_idx;
				READ_U2(signature_idx, base);
				//printf("signature:%s\n", constPool->entries[signature_idx].info.utf8_info.bytes);
			}
		} // for (j = 0;
	} // for (i = 0;

	return base;
}


#define DEPRECATED_ATTR_LEN_VALUE 0


/**
 * Read class methods' contributes
 */
static U1* readMethodAttrib(ClassEntry *class, MethodEntry* method, U2 method_attr_count, U1* base) {

	assert(NULL != class && method != NULL && method_attr_count >= 0 && base != NULL);

	U2 nameidx;
	U2 attr_name_idx;
	U4 attr_length;
	char *name = NULL;

	ConstPool *constPool = class->constPool;
	int j;

	for (j = 0; j < method_attr_count; ++j) {
		READ_U2(attr_name_idx, base);
		READ_U4(attr_length, base);
		name = constPool->entries[attr_name_idx].info.utf8_info.bytes;

		if (!strcmp(name, "Code")) {
			U2 max_slot;
			READ_U2(max_slot, base);
			method->stack_frame.slotCnt = max_slot;
			Slot *slots = (Slot *)calloc(max_slot, sizeof (Slot));
			assert (NULL != slots);
			method->stack_frame.slotTbl = slots;

			READ_U2(max_slot, base);
			method->local_tbl.slotCnt = max_slot;
			slots = (Slot *)calloc(max_slot, sizeof(Slot));
			assert (NULL != slots);
			method->local_tbl.slotTbl = slots;

			U4 code_length;
			READ_U4(code_length, base);
			method->code_length = code_length;

			method->code = calloc(1, code_length);
			assert(NULL != method->code);
			memcpy(method->code, base, code_length);
			base += code_length;

			U2 excep_tbl_len;
			READ_U2(excep_tbl_len, base);
			method->excep_tbl.length = excep_tbl_len;
			if (excep_tbl_len > 0) {
				method->excep_tbl.entries = calloc(excep_tbl_len, sizeof(ExceptionEntry));
				assert(NULL != method->excep_tbl.entries);
				int k;
				U2 start_pc;
				U2 end_pc;
				U2 handler_pc;
				U2 catch_type;
				for (k = 0; k < excep_tbl_len; ++k) {
					READ_U2(start_pc, base);
					method->excep_tbl.entries[k].start_pc = start_pc;	

					READ_U2(end_pc, base);
					method->excep_tbl.entries[k].end_pc = end_pc;

					READ_U2(handler_pc, base);
					method->excep_tbl.entries[k].handler_pc = handler_pc;

					READ_U2(catch_type, base);
					method->excep_tbl.entries[k].catch_type = catch_type;
				}
			}

			U2 code_attr_count;
			READ_U2(code_attr_count, base);

			int m;
			for (m = 0; m < code_attr_count; ++m) {
				READ_U2(nameidx, base);
				READ_U4(attr_length, base);
				name = constPool->entries[nameidx].info.utf8_info.bytes;

				if (!strcmp(name, "LineNumberTable")) {
					U2 line_tbl_len;
					READ_U2(line_tbl_len, base);
					int k;
					for (k = 0; k < line_tbl_len; ++k) {
						U2 start_pc;
						READ_U2(start_pc, base);
						U2 line_no;
						READ_U2(line_no, base);
					}
				} 
				else if(!strcmp(name, "Exceptions")){
					U2 excep_num;
					READ_U2(excep_num, base);
					int k;
					U2 excep_idx;
					for (k = 0; k < excep_num; ++k) {
						READ_U2(excep_idx, base);
					}
				}
				else if(!strcmp(name, "RuntimeVisibleAnnotations")) {
					base += attr_length;
				}

#if 0
				else if(!strcmp(name, "StackMapTable")) {
					printf ("StackMapTable\n");
					U2 entry_num;
					READ_U2(entry_num, base);
					int k;
					for (k = 0; k < entry_num; ++k) {
						U1 type;
						READ_U1(type, base);
						printf("type:%d\n", type);
						if (type >= 0 && type <= 63) {
							printf("same_frame\n");
						} else if (type >= 64 && type <= 127) {
							printf("same_locals_1_stack_item_frame\n");
						} else if (247 == type){
							printf("same_locals_1_stack_item_frame_extended\n");
							U2 offset_delta;
							READ_U2(offset_delta, base);

						} else if (type >= 248 && type <= 250) {
							printf("chop_frame\n");
							U2 offset_delta;
							READ_U2(offset_delta, base);
						} else if (251 == type) {
							printf("same_frame_extended\n");
							U2 offset_delta;
							READ_U2(offset_delta, base);
						} else if (type >= 252 && type <= 254) {
							printf("append_frame\n");
							U2 offset_delta;
							READ_U2(offset_delta, base);
						} else if (type == 255) {
							printf("full_frame\n");
						}
					}
				}
#endif
				else {
					printf("Unknown Code attr:%s\n", name);
					base += attr_length;
				}
			}	// end for (Code attr's attr count)
		} // Code end
		else if (!strcmp(name, "Deprecated")) {
			if (attr_length != DEPRECATED_ATTR_LEN_VALUE) {
				fprintf(stderr, "Deprecated attribute is Invalid\n");
			}

		} else if(!strcmp(name, "Exceptions")){
			U2 excep_num;
			READ_U2(excep_num, base);
			int k;
			U2 excep_idx;
			for (k = 0; k < excep_num; ++k) {
				READ_U2(excep_idx, base);
			}
		} else {
			base += attr_length;
			printf("Unknown method attr:%s\n", name);
		}
	}	// for (method attr count)

	return base;
}

/**
 * Read class methods
 */
static U1* readClassMethod(ClassEntry* class, U2 methods_count, U1* base) {

	assert (NULL != class && base != NULL && methods_count >= 0);

	char *name = NULL;
	U2 acc_flags;
	U2 attr_count;
	U2 clsidx;
	U2 nameidx;
	U2 typeidx;

	ConstPool* constPool = class->constPool;
	int i;
	for (i = 0; i < methods_count; ++i) {
		class->methods[i].class = CE_CLASS(class);

		READ_U2(acc_flags, base);
		class->methods[i].acc_flags = acc_flags;

		READ_U2(nameidx, base);
		name = constPool->entries[nameidx].info.utf8_info.bytes;
//		if (NULL != name) {
			class->methods[i].name = calloc(1, strlen(name) + 1);
			strcpy(class->methods[i].name, name);
//		}

		READ_U2(typeidx, base);
		name = constPool->entries[typeidx].info.utf8_info.bytes;
		class->methods[i].type = calloc(1, strlen(name) + 1);
		strcpy(class->methods[i].type, name);

		U2 attr_name_idx;
		U4 attr_length;
		U2 method_attr_count;

		READ_U2(method_attr_count, base);
		base = readMethodAttrib(class, class->methods + i, method_attr_count, base);
	}

	return base;
}


/*
 * Read class attributes
 */
static U1* readClassAttrib(ClassEntry* class, U2 attr_count, U1* base) {

	assert(NULL != class && attr_count >= 0 && base != NULL);
	
	U2 attr_name_idx;
	U2 attr_length;
	ConstPool* constPool = class->constPool;
	int i, j;
	for (i = 0; i < attr_count; ++i) {
		READ_U2(attr_name_idx, base);
		READ_U4(attr_length, base);
		char *name = constPool->entries[attr_name_idx].info.utf8_info.bytes;

		if (strcmp(name, "SourceFile") == 0) {
			U2 fname_idx;
			READ_U2(fname_idx, base);
			char* str = constPool->entries[fname_idx].info.utf8_info.bytes;
			class->source_file = calloc(1, strlen(str) + 1);
			strcpy(class->source_file, str);
		} 
		else if (strcmp(name, "InnerClasses") == 0) {
			U2 classes_num;
			READ_U2(classes_num, base);
			for (j = 0; j < classes_num; ++j) {
				U2 inner_cls_idx;
				READ_U2(inner_cls_idx, base);

				U2 outer_cls_idx;
				READ_U2(outer_cls_idx, base);

				U2 inner_name_idx;
				READ_U2(inner_name_idx, base);

				U2 inner_cls_acc_flags;
				READ_U2(inner_cls_acc_flags, base);
			}
		}
		else if (strcmp(name, "Signature") == 0) {
			U2 signature_idx;
			READ_U2(signature_idx, base);
		}
		else if (strcmp(name, "EnclosingMethod") == 0) {
			U2 cls_idx, method_idx;
			READ_U2(cls_idx, base);
			READ_U2(method_idx, base);
		}
		else {
			printf ("Unknown attribute:%s\n", name);
		}
	}

	return base;
}


/*
 * Load a class from in memory 
 * Parameters:
 *		classname:		name of the class
 *		data:			data of the class
 *		len:			length of the data
 * Return:
 *		if Error,	return NULL
 *		if OK,		return an valid class
 */
Class* defineClass(const char *classname, const char *data, int len) {

	if (NULL == classname || NULL == data || len <= 0) {
		return NULL;
	}

	/* change to unsigned char*
	 * for READ_U2, READ_U4, READ_U8
	 */
	unsigned char *base = (unsigned char *)data;

	U4 magic;
	READ_U4(magic, base);
	if (magic != MAGIC_NUMBER) {
		fprintf(stderr, "Unrecognized class format\n");
		return NULL;
	}

    Class* cls = allocClass();
    assert(NULL != cls);

	ClassEntry *class = CLASS_CE(cls);
	assert(NULL != class);
	class->state = CLASS_BAD;

	U2 minor_version;
	U2 major_version;
	READ_U2(minor_version, base);
	READ_U2(major_version, base);
	class->reserve[0] = minor_version;
	class->reserve[1] = major_version;

	// read constant value pool
	U2 cp_count;
	READ_U2(cp_count, base);
	base = readConstPool(class, cp_count, base);

	// read class access falgs
	U2 acc_flags;
	READ_U2(acc_flags, base);
	class->acc_flags = acc_flags;

	// read class name
	ConstPool *constPool = class->constPool;
	char *name = NULL;
	U2 this_class;
	READ_U2(this_class, base);
	name = constPool->entries[constPool->entries[this_class].info.class_info.name_index].info.utf8_info.bytes;
	class->name = calloc(1, strlen(name) + 1);
	strcpy(class->name, name);

	// read super class
	U2 super_class;
	READ_U2(super_class, base);
	if (0 == super_class) {
		printf ("super class index = 0\n");
	}
	else if(super_class > 0 && super_class < constPool->length) {
		name = constPool->entries[constPool->entries[super_class].info.class_info.name_index].info.utf8_info.bytes;
		class->super_name = calloc(1, strlen(name) + 1);
		strcpy(class->super_name, name);
	}
	else {
		printf("Invalid super class index\n");
	}

	// read interfaces
	U2 ifcount;
	READ_U2(ifcount, base);
	class->interfaces_count = ifcount;
	class->interfaces_index = calloc(ifcount, sizeof(U2));
	int i;
	U2 ifidx;
	for (i = 0; i < ifcount; ++i) {
		READ_U2(ifidx, base);
		class->interfaces_index[i] = ifidx;
		name = constPool->entries[constPool->entries[ifidx].info.class_info.name_index].info.utf8_info.bytes;	
	}

	// read class fields
	U2 field_count;
	READ_U2(field_count, base);
    class->fields_count = field_count;
	class->fields = (FieldEntry *)calloc(field_count, sizeof(FieldEntry));
	assert(NULL != class->fields);
	base = readClassField(class, field_count, base);

	// read class methods
	U2 methods_count;
	READ_U2(methods_count, base);
	class->methods_count = methods_count;
	class->methods = calloc(methods_count, sizeof(MethodEntry));
	assert(NULL != class->methods);
	base = readClassMethod(class, methods_count, base);

	// read class attributes
	U2 attr_count;
	READ_U2(attr_count, base);
	base = readClassAttrib(class, attr_count, base);

	
	/** check memory buffer overflow */
	if ((char *)base > data + len) {
		fprintf(stderr, "buffer overflow in defineClass\n");
		return NULL;
	}

	class->state = CLASS_LOADED;

	return cls;
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
	FILE *fp = fopen(path, "rb");
	if (NULL == fp) {
		fprintf(stderr, "Error:failed load class\n");
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	U4 size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	U1 *buff = (U1 *)calloc(1, size);
	if (NULL == buff) {
		fprintf(stderr, "Failed malloc mem for class file\n");
		fclose(fp);
		return NULL;
	}
	if (size != fread(buff, 1, size, fp)) {
		fprintf(stderr, "Failed read class file\n");
		free(buff);
		fclose(fp);
		return NULL;
	}
	Class *class = defineClass(classname, buff, size);
	free(buff);
	fclose(fp);
	return class;
}

/*
 * Unpack jar arguments
 */
typedef struct UnpackJarArg {
	Class **classes;
	U4 clsCnt;
} UnpackJarArg;

/*
 * Unpack jar start
 */
static void on_start (int total, void* param) {

	printf("total %d files\n", total);

	if (NULL != param) {
		UnpackJarArg* arg = (UnpackJarArg*) param;
		arg->clsCnt = total;
		arg->classes = (Class**)calloc(total, sizeof(Class *));
		if (NULL == arg->classes) {
			printf("Failed alloc mem for loading class.\n");
			exit (1);
		}
	}
}

/*
 * Unpack jar progress
 */
static void on_progress (int index, 
		const char* name,
		const char* mem,
		int size,
		void* param) {

	if (NULL == name || mem == NULL) {
		printf ("name or mem is NULL\n");
		return;
	}

    printf ("%-4d name:%s\n", index, name);
	if (NULL != param && size > 0) {
		UnpackJarArg* arg = (UnpackJarArg*) param;
		Class** cls = arg->classes;
		cls[index] = defineClass(name, mem, size);
	}
}

/*
 * Unpack error
 */
static int on_error(int errcode, int index, void* param) {
    printf("unzip jar failed at %d, error code:%d\n", index, errcode);
	exit(1);
	return 1;
}

/*
 * Unpack finish
 */
static void on_finish(void* param) {

}


/*
 * Load all the class from the specified jar
 * Parameters:
 *			path:		input jar
 *			classes:	[OUT] classes output
 * Return:
 *			count of classes 
 *			it means error when return value <= 0
 */
U4 loadClassFromJar(char *path, Class ***classes) {
	if (NULL == path || NULL == classes) {
		return -1;
	}
	
	UnpackJarArg arg;
	memset(&arg, 0, sizeof(arg) );

	executeUnpackJar(path, on_start, on_progress, on_error, on_finish, &arg);
	*classes = arg.classes;

	return arg.clsCnt;
}

/*
 * Create a constant pool by the specified length 
 */
ConstPool* newConstPool(int length) {
	if (length <= 0) {
		return NULL;
	}

	ConstPool *pool = (ConstPool *)calloc(1, sizeof (ConstPool));
	if (NULL == pool) {
		fprintf(stderr, "Failed alloc mem for ConstPool\n");
		return NULL;
	}

	pool->length = length;
	pool->entries = (ConstPoolEntry *)calloc(length, sizeof(ConstPoolEntry));
	if (NULL == pool->entries) {
		free (pool);
		return NULL;
	}

	return pool;
}

/*
 * Log information of .class file
 */
void logClassEntry(ClassEntry *clsEntry)
{
    if (NULL == clsEntry) {
        return;
    }

    printf("Compiled from \"%s\"\n", clsEntry->source_file);

    if (clsEntry->acc_flags & ACC_PUBLIC) {
        printf("public ");
    } else if (clsEntry->acc_flags & ACC_PROTECTED) {
        printf("protected ");
    } else if (clsEntry->acc_flags & ACC_PRIVATE) {
        printf("private ");
    }

    if (clsEntry->acc_flags & ACC_FINAL) {
        printf("final ");
    }

    if (clsEntry->acc_flags & ACC_STATIC) {
        printf("static ");
    }

     
    if (clsEntry->acc_flags & ACC_ABSTRACT && 
           !(clsEntry->acc_flags & ACC_INTERFACE)) {
        printf("abstract ");
    }

    /** ignore me **/
    /*
    if (clsEntry->acc_flags & ACC_SUPER) {
        printf("super ");
    }
    */

    if (clsEntry->acc_flags & ACC_INTERFACE) {
        printf("interface ");
    } else {
        printf("class ");
    }

    printf("%s", clsEntry->name);
    if (NULL != clsEntry->super_name) {
        printf (" extends %s", clsEntry->super_name);
    }
    
    int i;
    for (i = 0; i < clsEntry->interfaces_count; ++i) {
        if (0 == i) {
            printf(" implemented");
        }
		int ifidx = clsEntry->interfaces_index[i];
		char *name = clsEntry->constPool->entries[clsEntry->constPool->entries[ifidx].info.class_info.name_index].info.utf8_info.bytes;	
        printf(" %s", name);
    }
    printf("\n");

    printf("  SourceFile: \"%s\"\n", clsEntry->source_file);
    printf("  minor version: %d\n", (int)clsEntry->reserve[0]);
    printf("  major version: %d\n", (int)clsEntry->reserve[1]);
    printf("  Constant pool count:%d\n", clsEntry->constPool->length);

    int cls_idx;
    int nametype_idx;
    int name_idx;
    int type_idx;
    int index;

	U4 u4high;
	U4 u4low;
	U8 u8;

    for (i = 1; i < clsEntry->constPool->length; ++i) {
        printf("const #%d = ", i);
        switch (clsEntry->constPool->entries[i].tag) {
            case CONST_Utf8:
                printf("Asciz\t%s\n", clsEntry->constPool->entries[i].info.utf8_info.bytes);
                break;

            case CONST_Integer:
				printf("int\t%d\n", clsEntry->constPool->entries[i].info.integer_info.bytes);
                break;

            case CONST_Float:
				printf("float\t%d\n", clsEntry->constPool->entries[i].info.float_info.bytes);
                break;

            case CONST_Long:
				u4high = clsEntry->constPool->entries[i].info.long_info.high_bytes;
				u4low = clsEntry->constPool->entries[i].info.long_info.low_bytes;
				u8 = u4high;
				u8<<=32;
				u8 += u4low;
				++i;
				//printf("long\t%lld\n", u8);
                break;

            case CONST_Double:
				u4high = clsEntry->constPool->entries[i].info.double_info.high_bytes;
				u4low = clsEntry->constPool->entries[i].info.double_info.low_bytes;
				u8 = u4high<<32 + u4low;
				++i;
				//printf("double\t%lld\n", u8);
                break;

            case CONST_Class:
                name_idx = clsEntry->constPool->entries[i].info.class_info.name_index;
                printf("class\t#%d; //%s\n",
                        name_idx,
                        clsEntry->constPool->entries[name_idx].info.utf8_info.bytes);
                break;

            case CONST_String:
                index = clsEntry->constPool->entries[i].info.string_info.string_index;
                printf("String\t#%d; //%s\n",
                        index,
                        clsEntry->constPool->entries[index].info.utf8_info.bytes);
                break;

            case CONST_Fieldref:
				cls_idx = clsEntry->constPool->entries[i].info.fieldref_info.class_index;
				nametype_idx = clsEntry->constPool->entries[i].info.fieldref_info.name_type_index;
				name_idx = clsEntry->constPool->entries[nametype_idx].info.nametype_info.name_index;
                index = clsEntry->constPool->entries[cls_idx].info.class_info.name_index;
				type_idx = clsEntry->constPool->entries[nametype_idx].info.nametype_info.type_index;

				printf("Field\t#%d.#%d;  // %s.%s:%s;\n",
						cls_idx, 
						nametype_idx,
                        clsEntry->constPool->entries[index].info.utf8_info.bytes,
						clsEntry->constPool->entries[name_idx].info.utf8_info.bytes,
						clsEntry->constPool->entries[type_idx].info.utf8_info.bytes);

                break;

            case CONST_Methodref:
                cls_idx = clsEntry->constPool->entries[i].info.methodref_info.class_index,
                index = clsEntry->constPool->entries[cls_idx].info.class_info.name_index;
                nametype_idx = clsEntry->constPool->entries[i].info.methodref_info.name_type_index,
                name_idx = clsEntry->constPool->entries[nametype_idx].info.nametype_info.name_index;
                type_idx = clsEntry->constPool->entries[nametype_idx].info.nametype_info.type_index;
                printf("Method\t#%d.#%d; // %s.%s:%s\n",
                        cls_idx, 
                        nametype_idx, 
                        clsEntry->constPool->entries[index].info.utf8_info.bytes,
                        clsEntry->constPool->entries[name_idx].info.utf8_info.bytes,
                        clsEntry->constPool->entries[type_idx].info.utf8_info.bytes);
                break;

            case CONST_IfMethodref:
				printf("InterfaceMethodref_info\n");
                break;

            case CONST_NameAndType:
                name_idx = clsEntry->constPool->entries[i].info.nametype_info.name_index;
                type_idx = clsEntry->constPool->entries[i].info.nametype_info.type_index;
                printf("NameAndType #%d:%d;// \"%s\":%s\n",
                        name_idx,
                        type_idx,
                        clsEntry->constPool->entries[name_idx].info.utf8_info.bytes,
                        clsEntry->constPool->entries[type_idx].info.utf8_info.bytes);
                break;

            case CONST_MethodHandle:
                break;

            case CONST_MethodType:
                break;

            case CONST_InvokeDynamic:
                break;
        }
    }

    for (i = 0; i < clsEntry->fields_count; ++i) {
        if (clsEntry->fields[i].acc_flags & ACC_PUBLIC) {
            printf("public ");
        } else if (clsEntry->fields[i].acc_flags & ACC_PROTECTED) {
            printf("protected ");
        } else if (clsEntry->fields[i].acc_flags & ACC_PRIVATE) {
            printf("private ");
        }


        char *type = clsEntry->fields[i].type;
        if (!strcmp(type, "I")) {
            printf("int ");
        } else if (!strcmp(type, "J")) {
            printf("long ");
        } else if (!strcmp(type, "D")) {
            printf("double ");
        }
        printf("%s;\n", clsEntry->fields[i].name);
    }

    for (i = 0; i < clsEntry->methods_count; ++i) {
        if (clsEntry->methods[i].acc_flags & ACC_PUBLIC) {
            printf("public ");
        }
        else if (clsEntry->methods[i].acc_flags & ACC_PROTECTED) {
            printf("protected ");
        }
        else if (clsEntry->methods[i].acc_flags & ACC_PRIVATE) {
            printf("private ");
        }

		if (clsEntry->methods[i].acc_flags & ACC_ABSTRACT) {
			printf("abstract ");
		}

        printf("%s ", clsEntry->methods[i].type);

        char *name = clsEntry->methods[i].name;
        if (!strcmp(name, "<init>")) {
            printf("%s;\n", clsEntry->name);
        } else {
            printf("%s;\n", clsEntry->methods[i].name);
        }

        printf("  Code:\n");
        printf("   Stack=%d, Locals=%d, Args_size=%d\n",
                    clsEntry->methods[i].stack_frame.slotCnt,
                    clsEntry->methods[i].local_tbl.slotCnt,
                    clsEntry->methods[i].args_count);

        int j;
        for (j = 0; j < clsEntry->methods[i].code_length; ++j) {
            U1 *code = (U1 *)clsEntry->methods[i].code + j;
			const Instruction* inst = getCachedInstruction(code, clsEntry->methods[i].code_length - j, j);
			printf("   %d ", j);
			logInstruction(inst);
            j += inst->tag; 
        }

        printf("  LineNumberTable:\n");
        printf("   line %d: %d\n\n", 0, 0);
    }
}

Class* allocClass(MemoryArea* area)
{
	int size = sizeof(Class) + sizeof(ClassEntry);
	Class* class = calloc(1, size);
	return class;
}
