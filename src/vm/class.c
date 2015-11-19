/****************************
 * file name:   class.c
 *
 ***************************/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "class.h"
#include "instruction.h"
#include "mem.h"

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


#define DEPRECATED_ATTR_LEN_VALUE 0

Class* defineClass(char *classname, char *data, int offset, int len, Object *class_loader) {
	if (NULL == classname || NULL == data || len <= 0) {
		return NULL;
	}

	/* change to unsigned char*
	 * for READ_U2, READ_U4, READ_U8
	 */
	unsigned char *base = (unsigned char *)data + offset;

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

	U2 cp_count;
	READ_U2(cp_count, base);

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
				constPool->entries[i].info.utf8_info.bytes = sysAlloc(length + 1);
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

	U2 acc_flags;
	READ_U2(acc_flags, base);
	class->acc_flags = acc_flags;

	char *name = NULL;
	U2 this_class;
	READ_U2(this_class, base);
	name = constPool->entries[constPool->entries[this_class].info.class_info.name_index].info.utf8_info.bytes;
	class->name = sysAlloc(strlen(name));
	strcpy(class->name, name);

	U2 super_class;
	READ_U2(super_class, base);
	if (0 == super_class) {
	} else if(super_class > 0 && super_class < constPool->length) {
		name = constPool->entries[constPool->entries[super_class].info.class_info.name_index].info.utf8_info.bytes;
		class->super_name = strdup(name);
	} else {
		printf("Invalid super class index\n");
	}

	U2 ifcount;
	READ_U2(ifcount, base);
	class->interfaces_count = ifcount;
	class->interfaces_index = sysAlloc(sizeof(U2) * ifcount);

	U2 ifidx;
	for (i = 0; i < ifcount; ++i) {
		READ_U2(ifidx, base);
		class->interfaces_index[i] = ifidx;
		name = constPool->entries[constPool->entries[ifidx].info.class_info.name_index].info.utf8_info.bytes;	
	}

	U2 field_count;
	READ_U2(field_count, base);
    class->fields_count = field_count;
	class->fields = (FieldEntry *)sysAlloc(field_count * sizeof(FieldEntry));
	assert(NULL != class->fields);

	U2 attr_count;
	for (i = 0; i < field_count; ++i) {
		READ_U2(acc_flags, base);
		class->fields[i].acc_flags = acc_flags;

		READ_U2(nameidx, base);
		name = constPool->entries[nameidx].info.utf8_info.bytes;
		class->fields[i].name = sysAlloc(strlen(name));
		strcpy(class->fields[i].name, name);

		READ_U2(typeidx, base);
		name = constPool->entries[typeidx].info.utf8_info.bytes;
		class->fields[i].type = sysAlloc(strlen(name));
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
				printf("signature:%s\n", constPool->entries[signature_idx].info.utf8_info.bytes);
			}
		}
	}

	U2 methods_count;
	READ_U2(methods_count, base);
	class->methods_count = methods_count;
	class->methods = sysAlloc(methods_count * sizeof(MethodEntry));
	assert(NULL != class->methods);

	for (i = 0; i < methods_count; ++i) {
		READ_U2(acc_flags, base);
		class->methods[i].acc_flags = acc_flags;

		READ_U2(nameidx, base);
		name = constPool->entries[nameidx].info.utf8_info.bytes;
		class->methods[i].name = sysAlloc(strlen(name));
		strcpy(class->methods[i].name, name);

		READ_U2(typeidx, base);
		name = constPool->entries[typeidx].info.utf8_info.bytes;
		class->methods[i].type = sysAlloc(strlen(name));
		strcpy(class->methods[i].type, name);

		U2 attr_name_idx;
		U4 attr_length;
		U2 method_attr_count;
		READ_U2(method_attr_count, base);
		for (j = 0; j < method_attr_count; ++j) {
			READ_U2(attr_name_idx, base);
			READ_U4(attr_length, base);
			name = constPool->entries[attr_name_idx].info.utf8_info.bytes;

			if (!strcmp(name, "Code")) {
				U2 max_stack;
				READ_U2(max_stack, base);
				class->methods[i].max_stack = max_stack;

				U2 max_locals;
				READ_U2(max_locals, base);
				class->methods[i].max_locals = max_locals;

				U4 code_length;
				READ_U4(code_length, base);
				class->methods[i].code_length = code_length;

				class->methods[i].code = sysAlloc(length);
				assert(NULL != class->methods[i].code);
				memcpy(class->methods[i].code, base, length);
				base += code_length;

				U2 excep_tbl_len;
				READ_U2(excep_tbl_len, base);
				class->methods[i].excep_tbl.length = excep_tbl_len;
				if (excep_tbl_len > 0) {
					class->methods[i].excep_tbl.entries = sysAlloc(excep_tbl_len * sizeof(ExceptionEntry));
					assert(NULL != class->methods[i].excep_tbl.entries);
					int k;
					U2 start_pc;
					U2 end_pc;
					U2 handler_pc;
					U2 catch_type;
					for (k = 0; k < excep_tbl_len; ++k) {
						READ_U2(start_pc, base);
						class->methods[i].excep_tbl.entries[k].start_pc = start_pc;	

						READ_U2(end_pc, base);
						class->methods[i].excep_tbl.entries[k].end_pc = end_pc;

						READ_U2(handler_pc, base);
						class->methods[i].excep_tbl.entries[k].handler_pc = handler_pc;

						READ_U2(catch_type, base);
						class->methods[i].excep_tbl.entries[k].catch_type = catch_type;
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
					} else if(!strcmp(name, "Exceptions")){
						U2 excep_num;
						READ_U2(excep_num, base);
						int k;
						U2 excep_idx;
						for (k = 0; k < excep_num; ++k) {
							READ_U2(excep_idx, base);
						}
					} else if(!strcmp(name, "RuntimeVisibleAnnotations")) {
						base += attr_length;
					} else if(!strcmp(name, "StackMapTable")) {
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
					} else {
						printf("Unknown Code attr:%s\n", name);
						base += attr_length;
					}
				}	// end for (Code attr's attr count)
			} else if (!strcmp(name, "Deprecated")) {
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
		}	// end for (method attr count)
	}	// end for (method count)

	U2 attr_name_idx;
	U2 attr_length;
	READ_U2(attr_count, base);

	for (i = 0; i < attr_count; ++i) {
		READ_U2(attr_name_idx, base);
		READ_U4(attr_length, base);
		char *name = constPool->entries[attr_name_idx].info.utf8_info.bytes;

		if (!strcmp(name, "SourceFile")) {
			U2 fname_idx;
			READ_U2(fname_idx, base);
			class->source_file = sysAlloc(attr_length);
			strcpy(class->source_file, constPool->entries[fname_idx].info.utf8_info.bytes);
		} else if (!strcmp(name, "InnerClasses")) {
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
	}

	/** check memory buffer overflow */
	if ((char *)base > data + offset + len) {
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

ConstPool* newConstPool(int length) {
	if (length <= 0) {
		return NULL;
	}

	ConstPool *pool = (ConstPool *)sysAlloc(sizeof (ConstPool));
	if (NULL == pool) {
		fprintf(stderr, "Failed alloc mem for ConstPool\n");
		return NULL;
	}

	pool->length = length;
	pool->entries = (ConstPoolEntry *)sysAlloc(sizeof (ConstPoolEntry) * length);
	if (NULL == pool->entries) {
		sysFree (pool);
		return NULL;
	}

	return pool;
}

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
				printf("long\t%ld\n", u8);
                break;

            case CONST_Double:
				u4high = clsEntry->constPool->entries[i].info.double_info.high_bytes;
				u4low = clsEntry->constPool->entries[i].info.double_info.low_bytes;
				u8 = u4high<<32 + u4low;
				++i;
				printf("double\t%ld\n", u8);
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
                    clsEntry->methods[i].max_stack,
                    clsEntry->methods[i].max_locals,
                    clsEntry->methods[i].args_count);

        int j;
        for (j = 0; j < clsEntry->methods[i].code_length; ++j) {
            U1 *code = (U1 *)clsEntry->methods[i].code + j;
			const Instruction* inst = getCachedInstruction(code, clsEntry->methods[i].code_length - j);
			printf("   %d ", j);
			logInstruction(inst);
            j += inst->tag; 
        }

        printf("  LineNumberTable:\n");
        printf("   line %d: %d\n\n", 0, 0);
    }
}
