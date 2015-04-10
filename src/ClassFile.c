/****************************
 * file name:   ClassFile.c
 *
 * *************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ClassFile.h>
#include <comm.h>

#define MAGIC_NUMBER 0XCAFEBABE

PUBLIC ClassFile* load_class(const char *path)
{
    if (NULL == path) {
        fprintf(stderr, "path is NULL\n");
        return NULL;
    }

    FILE *fp = fopen(path, "rb");
    if (NULL == fp) {
        fprintf(stderr, "Fatal error:%s not exists\n", path);
        return NULL;
    }

    ClassFile *clsFile = (ClassFile *)calloc(sizeof(*clsFile), 1);
    if (NULL == clsFile) {
        LogE("Failed calloc mem for clsFile");
        fclose(fp);
        return NULL;
    }

    do {
		if (read_uint32(&clsFile->magic, fp) < 0) {
            LogE("Failed read magic");
            break;
        }
        clsFile->magic = ntohl(clsFile->magic);
		if (MAGIC_NUMBER != clsFile->magic) {
			printf("Error:%s is invalid.\n", path);
			break;
		}

		if (read_uint16(&clsFile->minor_version, fp) < 0) {
            LogE("Failed read minor_version");
            break;
        }
        clsFile->minor_version = ntohs(clsFile->minor_version);

		if (read_uint16(&clsFile->major_version, fp) < 0) {
            LogE("Failed read major_version");
            break;
        }
        clsFile->major_version = ntohs(clsFile->major_version);

		if (read_uint16(&clsFile->const_pool_count, fp) < 0) {
            LogE("Failed read constant pool count");
            break;
        }
        clsFile->const_pool_count = ntohs(clsFile->const_pool_count);

        clsFile->const_pool = (cp_info **)calloc(
				clsFile->const_pool_count,
                sizeof (cp_info *));
        if (NULL == clsFile->const_pool) {
            LogE("Failed calloc for const pool");
            break;
        }

        int i;
        for (i = 1; i < clsFile->const_pool_count; ++i) {
			cp_info *info = read_cp_info(fp);
			assert (NULL != info);
			clsFile->const_pool[i] = info;
        }

		if (read_uint16(&clsFile->access_flags, fp) < 0) {
			LogE("Failed read access_flags");
			break;
		}
		clsFile->access_flags = ntohs(clsFile->access_flags);

		if (read_uint16(&clsFile->this_class, fp) < 0) {
			LogE("Failed read this_class");
			break;
		}
		clsFile->this_class = ntohs(clsFile->this_class);

		if (read_uint16(&clsFile->super_class, fp) < 0) {
			LogE("Failed read super_class");
			break;
		}
		clsFile->super_class = ntohs(clsFile->super_class);

		if (read_uint16(&clsFile->interfaces_count, fp) < 0) {
			LogE("Failed read interfaces_count");
			break;
		}
		clsFile->interfaces_count = ntohs(clsFile->interfaces_count);

		clsFile->interfaces = (uint16 *)calloc(clsFile->interfaces_count,
				sizeof(uint16));
		if (NULL == clsFile->interfaces) {
			LogE("Failed calloc mem for interfaces");
			break;
		}

		for (i = 0; i < clsFile->interfaces_count; ++i) {
			if (read_uint16(clsFile->interfaces + i, fp) < 0) {
				LogE("Failed read interfaces[%d]", i);
				assert (0 != 0);
			}

			clsFile->interfaces[i] = ntohs(clsFile->interfaces[i]);
		}

		if (read_uint16(&clsFile->field_count, fp) < 0) {
			LogE("Failed read field_count");
			break;
		}

		clsFile->field_count = ntohs(clsFile->field_count);
		clsFile->fields = (field_info **)calloc(clsFile->field_count, sizeof(field_info *));
		if (NULL == clsFile->fields) {
			LogE("Failed calloc mem for fields");
			break;
		}
		for (i = 0; i < clsFile->field_count; ++i) {
			field_info *info = read_field_info(fp);
			assert (NULL != info);
			clsFile->fields[i] = info;
		}

		if (read_uint16(&clsFile->methods_count, fp) < 0) {
			LogE("Failed read methods_count");
			break;
		}
		clsFile->methods_count = ntohs(clsFile->methods_count);
		clsFile->methods = (method_info **)calloc(clsFile->methods_count, sizeof(method_info *));
		for (i = 0; i < clsFile->methods_count; ++i) {
			method_info *info = read_method_info(fp);
			assert (NULL != info);
			clsFile->methods[i] = info;
		}

		if (read_uint16(&clsFile->attributes_count, fp) < 0) {
			LogE("Failed read attributes_count");
			break;
		}
		clsFile->attributes_count = ntohs(clsFile->attributes_count);

		clsFile->attributes = (attr_info **)calloc(clsFile->attributes_count, sizeof(attr_info *));
		for (i = 0; i < clsFile->attributes_count; ++i) {
			clsFile->attributes[i] = read_attr_info(fp);		
		}

#ifdef DEBUG
		logClassFile(clsFile);
#endif

		fclose(fp);
        return clsFile;

    } while (0);


    free(clsFile);
    fclose(fp);

    return NULL;
}

PRIVATE cp_info* read_cp_info(FILE *fp) {
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	cp_info *info = (cp_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for cp_info");
		return NULL;
	}

	utf8_info *utf8 = NULL;
	methodref_info *methodref = NULL;
	class_info *cls = NULL;
    integer_info *integer = NULL;
    string_info *string = NULL;
    fieldref_info *fieldref = NULL;
    nametype_info *nametype = NULL;

	do {
		if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
			LogE("Failed read cp_info.tag");
			break;
		}

		switch (info->tag) {
			case CONSTANT_Utf8:
				utf8 = read_utf8_info(fp);
				assert (NULL != utf8);
				info->info = utf8;
				break;

			case CONSTANT_Integer:
                integer = read_integer_info(fp);
                assert (NULL != integer);
                info->info = integer;
				break;

			case CONSTANT_Float:
                printf("Float\n");
				break;

			case CONSTANT_Long:
                printf("Long\n");
				break;

			case CONSTANT_Double:
                printf("Double\n");
				break;

			case CONSTANT_Class:
				cls = read_class_info(fp);
				assert(NULL != cls);
				info->info = cls;
				break;

			case CONSTANT_String:
				printf("string\n");
                string = read_string_info(fp);
                assert(NULL != string);
                info->info = string;
				break;

			case CONSTANT_Fieldref:
				printf("Fieldref\n");
                fieldref = read_fieldref_info(fp);
                assert(NULL != fieldref);
                info->info = fieldref;
				break;

			case CONSTANT_Methodref:
				printf("methodref\n");
				methodref = read_methodref_info(fp);
				assert(NULL != methodref);
				info->info = methodref;
				break;

			case CONSTANT_InterfaceMethodref:
                printf("InterfaceMethodRef\n");
				break;

			case CONSTANT_NameAndType:
                nametype = read_nametype_info(fp);
                assert(NULL != nametype);
                info->info = nametype;
				break;

			case CONSTANT_MethodHandle:
                printf("MethodHandle\n");
				break;

			case CONSTANT_MethodType:
                printf("MethodType\n");
				break;

			case CONSTANT_InvokeDynamic:
                printf("InvokeDynamic\n");
				break;
		}

		return info;

	} while (0);

	free (info);
	return NULL;
}

PRIVATE utf8_info* read_utf8_info(FILE *fp) {
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	utf8_info *info = (utf8_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for utf8 info");
		return NULL;
	}


	fseek(fp, -1, SEEK_CUR);
	do {
		if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
			LogE("Failed read utf8_info.tag");
			break;
		}

		if (1 != fread(&info->length, sizeof(info->length), 1, fp)) {
			printf("Failed read utf8_info.length");
			break;
		}
		info->length = ntohs(info->length);

		info->bytes = (uint8 *)calloc(info->length, sizeof(info->bytes));
		if (NULL == info->bytes) {
			break;
		}
		if (info->length != fread(info->bytes, 
					sizeof(*info->bytes),
					info->length,
					fp)) {
			LogE("Failed read utf8_info.bytes");
			free (info->bytes);
			break;
		}

		return info;

	} while (0);

	free (info);
	return NULL;
}

PRIVATE methodref_info* read_methodref_info(FILE *fp) {
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	methodref_info *info = (methodref_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for methodref info");
		return NULL;
	}

	do {
		fseek(fp, -1, SEEK_CUR);
		if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
			LogE("Failed read methodref_info.tag");
			break;
		}
		if (read_uint16(&info->class_index, fp) < 0) {
			LogE("Failed read methodref_info.class_index");
			break;
		}
		info->class_index = ntohs(info->class_index);

		if (read_uint16(&info->name_and_type_index, fp) < 0) {
			LogE("Failed read methodref_info.name_and_type_index");
			break;
		}
		info->name_and_type_index = ntohs(info->name_and_type_index);
		return info;

	} while (0);

	free (info);
	return NULL;
}

PRIVATE class_info* read_class_info(FILE *fp) {
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	class_info *cls = (class_info *)calloc(1, sizeof(*cls));
	if (NULL == cls) {
		LogE("Failed calloc mem for class_info");
		return NULL;
	}

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&cls->tag, sizeof(cls->tag), 1, fp)) {
		LogE("Failed read class_info.tag");
		free (cls);
		return NULL;
	}

	if (1 != fread(&cls->name_index, sizeof(cls->name_index), 1, fp)) {
		LogE("Failed read class_info.name_index");
		free (cls);
		return NULL;
	}
	cls->name_index = ntohs(cls->name_index);
	return cls;
}

PRIVATE int read_uint16(uint16 *value, FILE *fp) {
	VALIDATE_NOT_NULL2(value, fp);
	if (1 != fread(value, sizeof(*value), 1, fp)) {
		return -1;
	}

	return 0;
}

PRIVATE int read_uint32(uint32 *value, FILE *fp) {
	VALIDATE_NOT_NULL2(value, fp);
	if (1 != fread(value, sizeof(*value), 1, fp)) {
		return -1;
	}

	return 0;
}

PRIVATE void logClassFile(const ClassFile *file)
{
	if (NULL == file) {
		LogE("file = NULL");
		return;
	}

	printf("magic:%X\n", file->magic);
	printf("minor version:%u\n", file->minor_version);
	printf("major version:%u\n", file->major_version);

	printf("const pool count:%u\n", file->const_pool_count);
	int i;
	for (i = 1; i < file->const_pool_count; ++i) {
		printf("const #%-2d = ", i);
		log_cp_info(file->const_pool[i]);
	}

	printf("access flags:%X\n", file->access_flags);
	printf("this class:%d\n", file->this_class);
	printf("super class:%d\n", file->super_class);

	printf("interfaces count:%d\n", file->interfaces_count);
	for (i = 0; i < file->interfaces_count; ++i) {
		printf("interfaces:%d\n", file->interfaces[i]);
	}

	printf("field count:%d\n", file->field_count);
	for (i = 0; i < file->field_count; ++i) {
		log_field_info(file->fields[i]);
	}

	printf("methods count:%d\n", file->methods_count);
	for (i = 0; i < file->methods_count; ++i) {
		log_method_info(file->methods[i]);
	}

	printf("attributes count:%d\n", file->attributes_count);
	for (i = 0; i < file->attributes_count; ++i) {
		log_attr_info(file->attributes[i]);
	}
}


PRIVATE void log_utf8_info(utf8_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s\t\t%s;\n", "Asciz", info->bytes); 
}

PRIVATE void log_integer_info(integer_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s;\t\tbytes:%d\n", 
			"Integer", info->bytes);
}

PRIVATE void log_float_info(float_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s, bytes:%d\n", 
			"Float", info->bytes);
}

PRIVATE void log_long_info(long_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s, high bytes:%d, low bytes:%d\n", 
			"Long", info->high_bytes, info->low_bytes);
}


PRIVATE void log_cp_info(const cp_info *info)
{
	switch (info->tag) {
		case CONSTANT_Utf8:
			log_utf8_info((utf8_info *)info->info);
			break;

		case CONSTANT_Integer:
			log_integer_info((integer_info *)info->info);
			break;

		case CONSTANT_Float:
			log_float_info((float_info *)info->info);
			break;

		case CONSTANT_Long:
			log_long_info((long_info *)info->info);
			break;
        
        case CONSTANT_Double:
            break;

        case CONSTANT_Class:
            log_class_info((class_info *)info->info);
            break;

        case CONSTANT_String:
            log_string_info((string_info *)info->info);
            break;

        case CONSTANT_Fieldref:
            log_fieldref_info((fieldref_info *)info->info);
            break;

        case CONSTANT_Methodref:
            log_methodref_info((methodref_info*)info->info);
            break;

        case CONSTANT_InterfaceMethodref:
			printf("InterfaceMethodref\n");
            break;

        case CONSTANT_NameAndType:
            log_nametype_info((nametype_info *)info->info);
            break;

        case CONSTANT_MethodHandle:
			printf("MethodHanle\n");
            break;

        case CONSTANT_InvokeDynamic:
			printf("InvokeDynamic\n");
            break;
	}
}

PRIVATE integer_info* read_integer_info(FILE *fp)
{
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	integer_info *info = (integer_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for integer_info");
		return NULL;
	}

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
		LogE("Failed read integer_info.tag");
		free (info);
		return NULL;
	}
    
    if (1 != fread(&info->bytes, sizeof(info->bytes), 1, fp)) {
        LogE("Failed read integer_info.bytes");
        free (info);
        return NULL;
    }
    info->bytes = ntohl(info->bytes);

    return info;
}

PRIVATE void log_methodref_info(const methodref_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s;\t\t#%d;#%d;\n", "MethodRef", 
			info->class_index, info->name_and_type_index);
}

PRIVATE string_info* read_string_info(FILE *fp)
{
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	string_info *info = (string_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for string_info");
		return NULL;
	}

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
		LogE("Failed read string_info.tag");
		free (info);
		return NULL;
	}

    if (1 != fread(&info->string_index, sizeof(info->string_index),
                1, fp)) {
        LogE("Failed read string_info.string_index");
        free (info);
        return NULL;
    }
    info->string_index = ntohs(info->string_index);

    return info;
}

PRIVATE void log_string_info(const string_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s\t\t#%d;\n",
            "String", info->string_index);

}

PRIVATE fieldref_info* read_fieldref_info(FILE *fp)
{
 	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	fieldref_info *info = (fieldref_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for fieldref_info");
		return NULL;
	}

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
		LogE("Failed read fieldref_info.tag");
		free (info);
		return NULL;
	}
   
    if (1 != fread(&info->class_index, 
                sizeof(info->class_index),
                1,
                fp)) {
        LogE("Failed read fieldref_info.class_index");
        free(info);
        return NULL;
    }
    info->class_index = ntohs(info->class_index);

    if (1 != fread(&info->name_and_type_index, 
                sizeof(info->name_and_type_index),
                1,
                fp)) {
        LogE("Failed read fieldref_info.name_and_type_index");
        free(info);
        return NULL;
    }
    info->name_and_type_index = ntohs(info->name_and_type_index);

    return info;
}

PRIVATE void log_fieldref_info(const fieldref_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s;\t\t#%d;#%d;\n", 
            "FieldRef", info->class_index,
            info->name_and_type_index);
}

PRIVATE nametype_info* read_nametype_info(FILE *fp)
{
 	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	nametype_info *info = (nametype_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for nametype_info");
		return NULL;
	}

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
		LogE("Failed read nametype_info.tag");
		free (info);
		return NULL;
	}
   
    if (1 != fread(&info->name_index, 
                sizeof(info->name_index),
                1,
                fp)) {
        LogE("Failed read name_info.name_index");
        free(info);
        return NULL;
    }
    info->name_index = ntohs(info->name_index);

    if (1 != fread(&info->descriptor_index, 
                sizeof(info->descriptor_index),
                1,
                fp)) {
        LogE("Failed read nametype_info.descriptor_index");
        free(info);
        return NULL;
    }
    info->descriptor_index = ntohs(info->descriptor_index);

    return info;
}

PRIVATE void log_nametype_info(const nametype_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf(" %s;\t#%d;#%d;\n", 
            "NameAndType", info->name_index, info->descriptor_index);
}

PRIVATE void log_class_info(const class_info *info)
{
    if (NULL == info) {
        LogE("info = NULL");
        return;
    }

    printf(" %s;\t\t#%d\n",
            "Class", info->name_index);
}

PRIVATE field_info* read_field_info(FILE *fp)
{
	if (NULL == fp) {
		return NULL;
	}

	field_info *info = (field_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for field_info");
		return NULL;
	}

	do {
		if (read_uint16(&info->access_flags, fp) < 0) {
			LogE("Failed read field_info.access_flags");
			break;
		}
		info->access_flags = ntohs(info->access_flags);

		if (read_uint16(&info->name_index, fp) < 0) {
			LogE("Failed read field_info.name_index");
			break;
		}
		info->name_index = ntohs(info->name_index);

		if (read_uint16(&info->descriptor_index, fp) < 0) {
			LogE("Failed read field_info.descriptor_index");
			break;
		}
		info->descriptor_index = ntohs(info->descriptor_index);

		if (read_uint16(&info->attr_count, fp) < 0) {
			LogE("Failed read field_info.attr_count");
			break;
		}
		info->attr_count = ntohs(info->attr_count);

		info->attr = (attr_info **)calloc(1, sizeof(*info->attr));
		if (NULL == info->attr) {
			LogE("Failed calloc mem for field_info.attr");
			break;
		}

		int i;
		for (i = 0; i < info->attr_count; ++i) {
			attr_info *attr = read_attr_info(fp);
			assert (NULL != attr);
			info->attr[i] = attr;
		}

		return info;

	} while (0);

	free (info);
	return NULL;
}

PRIVATE attr_info* read_attr_info(FILE *fp)
{
	if (NULL == fp) {
		LogE("fp = NULL");
		return NULL;
	}

	attr_info *info = (attr_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for attr_info");
		return NULL;
	}

	do {
		if (read_uint16(&info->attr_name_index, fp) < 0) {
			LogE("Failed read attr_info.attr_name_index");
			break;
		}
		info->attr_name_index = ntohs(info->attr_name_index);

		if (read_uint32(&info->attr_length, fp) < 0) {
			LogE("Failed read attr_info.attr_length");
			break;
		}
		info->attr_length = ntohl(info->attr_length);

		info->info = (uint8 *)calloc(info->attr_length, sizeof(*info->info));
		if (NULL == info->info) {
			LogE("Failed calloc mem for attr_info.info");
			break;
		}

		if (info->attr_length != fread(info->info, sizeof(*info->info), info->attr_length, fp)) {
			LogE("Failed read attr_info.info");
			break;
		}

		return info;

	} while (0);

	free (info);

	return NULL;
}

PRIVATE void log_field_info (field_info *info) {
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf("access flags:%X\n", info->access_flags);
	printf("name index:%d\n", info->name_index);
	printf("descriptor_index:%d\n", info->descriptor_index);
	printf("attr count:%d\n", info->attr_count);
	int i;
	for (i = 0; i < info->attr_count; ++i) {
		log_attr_info(info->attr[i]);
	}
}

PRIVATE void log_attr_info (attr_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf("attr name index:%d\n", info->attr_name_index);
	printf("attr length:%d\n", info->attr_length);
	printf("attr info:%s\n", info->info);
}

PRIVATE void log_method_info (method_info *info) {
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf("access flags:%X\n", info->access_flags);
	printf("name index:%d\n", info->name_index);
	printf("descriptor_index:%d\n", info->descriptor_index);
	printf("attr count:%d\n", info->attr_count);
	int i;
	for (i = 0; i < info->attr_count; ++i) {
		log_attr_info(info->attr[i]);
	}
}

PRIVATE method_info* read_method_info(FILE *fp)
{
	if (NULL == fp) {
		return NULL;
	}

	method_info *info = (method_info *)calloc(1, sizeof(*info));
	if (NULL == info) {
		LogE("Failed calloc mem for field_info");
		return NULL;
	}

	do {
		if (read_uint16(&info->access_flags, fp) < 0) {
			LogE("Failed read method_info.access_flags");
			break;
		}
		info->access_flags = ntohs(info->access_flags);

		if (read_uint16(&info->name_index, fp) < 0) {
			LogE("Failed read method_info.name_index");
			break;
		}
		info->name_index = ntohs(info->name_index);

		if (read_uint16(&info->descriptor_index, fp) < 0) {
			LogE("Failed read method_info.descriptor_index");
			break;
		}
		info->descriptor_index = ntohs(info->descriptor_index);

		if (read_uint16(&info->attr_count, fp) < 0) {
			LogE("Failed read method_info.attr_count");
			break;
		}
		info->attr_count = ntohs(info->attr_count);

		info->attr = (attr_info **)calloc(1, sizeof(*info->attr));
		if (NULL == info->attr) {
			LogE("Failed calloc mem for method_info.attr");
			break;
		}

		int i;
		for (i = 0; i < info->attr_count; ++i) {
			attr_info *attr = read_attr_info(fp);
			assert (NULL != attr);
			info->attr[i] = attr;
		}

		return info;

	} while (0);

	free (info);
	return NULL;
}

