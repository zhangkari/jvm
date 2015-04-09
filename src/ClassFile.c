/****************************
 * file name:   ClassFile.c
 *
 * *************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ClassFile.h>
#include <comm.h>

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
				1,
                sizeof (clsFile->const_pool));
        if (NULL == clsFile->const_pool) {
            LogE("Failed calloc for const pool");
            break;
        }

        int i;
        for (i = 1; i < clsFile->const_pool_count; ++i) {
			cp_info *info = read_cp_info(fp);
			if (NULL == info) {
				LogE("Failed read cp_info");
				break;
			}

			clsFile->const_pool[i] = info;
        }

        if (i < clsFile->const_pool_count - 1) {
			LogE("read const_pool occurs errors");
            break;
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
				break;

			case CONSTANT_Float:
				break;

			case CONSTANT_Long:
				break;

			case CONSTANT_Double:
				break;

			case CONSTANT_Class:
				cls = read_class_info(fp);
				assert(NULL != cls);
				info->info = cls;
				break;

			case CONSTANT_String:
				break;

			case CONSTANT_Fieldref:
				break;

			case CONSTANT_Methodref:
				methodref = read_methodref_info(fp);
				assert(NULL != methodref);
				info->info = methodref;
				break;

			case CONSTANT_InterfaceMethodref:
				break;

			case CONSTANT_NameAndType:
				break;

			case CONSTANT_MethodHandle:
				break;

			case CONSTANT_MethodType:
				break;

			case CONSTANT_InvokeDynamic:
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

	fseek(fp, -1, SEEK_CUR);
	if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
		LogE("Failed read methodref_info.tag");
		free (info);
		return NULL;
	}
	if (1 != fread(&info->class_index, sizeof(info->class_index), 1, fp)) {
		LogE("Failed read methodref_info.class_index");
		free (info);
		return NULL;
	}
	if (1 != fread(&info->name_and_type_index, 
				sizeof(info->name_and_type_index),
				1,
				fp)) {
		LogE("Failed read methodref_info.name_and_type_index");
		free (info);
		return NULL;
	}

	info->class_index = ntohs(info->class_index);
	info->name_and_type_index = ntohs(info->name_and_type_index);

	return info;
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
		log_cp_info(file->const_pool[i]);
	}

	printf("access flags:%X\n", file->access_flags);
	printf("this class:%d\n", file->this_class);
	printf("super class:%d\n", file->super_class);
	printf("interfaces count:%d\n", file->interfaces_count);
}


PRIVATE void log_utf8_info(utf8_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf("tag=%d, type:%s, length:%d, bytes:%s\n", 
			info->tag, "Utf8_info", info->length, info->bytes);
}

PRIVATE void log_integer_info(integer_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

#if 0
	printf("tag=%d, type:%s, length:%d, bytes:%s\n", 
			info->tag, "Integer_info", info->length, info->bytes);
#endif
}

PRIVATE void log_float_info(float_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

#if 0
	printf("tag=%d, type:%s, length:%d, bytes:%s\n", 
			info->tag, "Float_info", info->length, info->bytes);
#endif
}

PRIVATE void log_long_info(long_info *info)
{
	if (NULL == info) {
		LogE("info = NULL");
		return;
	}

	printf("tag=%d, type:%s, high bytes:%d, low bytes:%d\n", 
			info->tag, "Long_info", info->high_bytes, info->low_bytes);
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

	}
}
