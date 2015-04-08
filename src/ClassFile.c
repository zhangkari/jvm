/****************************
 * file name:   ClassFile.c
 *
 * *************************/

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
        if (1 != fread(&clsFile->magic, sizeof(clsFile->magic), 1, fp)) {
            LogE("Failed read magic");
            break;
        }

        if (1 != fread(
                    &clsFile->minor_version, 
                    sizeof(clsFile->minor_version),
                    1,
                    fp)) {
            LogE("Failed read minor_version");
            break;
        }

        if (1 != fread(
                    &clsFile->major_version, 
                    sizeof(clsFile->major_version),
                    1,
                    fp)) {
            LogE("Failed read major_version");
            break;
        }

        if (1 != fread(&clsFile->const_pool_count,
                    sizeof(clsFile->const_pool_count),
                    1,
                    fp)) {
            LogE("Failed read constant pool count");
            break;
        }

        clsFile->magic = ntohl(clsFile->magic);
        clsFile->minor_version = ntohs(clsFile->minor_version);
        clsFile->major_version = ntohs(clsFile->major_version);
        clsFile->const_pool_count = ntohs(clsFile->const_pool_count);

        clsFile->const_pool = (cp_info *)calloc(
                clsFile->const_pool_count - 1,
                sizeof (cp_info));
        if (NULL == clsFile->const_pool) {
            LogE("Failed calloc for const pool");
            break;
        }

        int i;
        for (i = 0; i < clsFile->const_pool_count - 1; ++i) {
			cp_info *info = read_cp_info(fp);
			if (NULL == info) {
				LogE("Failed read cp_info");
				break;
			}
        }

        if (i < clsFile->const_pool_count - 1) {
            break;
        }


#ifdef DEBUG
        printf("magic:%X\n", clsFile->magic);
        printf("minor version:%d\n", clsFile->minor_version);
        printf("major version:%d\n", clsFile->major_version);
        printf("constant pool count:%d\n", clsFile->const_pool_count);
#endif

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

	printf("sizeof(cp_info)=%d\n", sizeof(cp_info));

	do {
		if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
			LogE("Failed read cp_info.tag");
			break;
		}
		printf("tag=%d\n", info->tag);

		switch (info->tag) {
			case CONSTANT_Utf8:
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
				break;

			case CONSTANT_String:
				break;

			case CONSTANT_Fieldref:
				break;

			case CONSTANT_Methodref:
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

	do {
		if (1 != fread(&info->tag, sizeof(info->tag), 1, fp)) {
			LogE("Failed read utf8_info.tag");
			break;
		}

		printf("tag=%d\n", info->tag);

		if (1 != fread(&info->length, sizeof(info->length), 1, fp)) {
			printf("Failed read utf8_info.length");
			break;
		}

		printf("length=%d\n", info->length);
		printf("length=%d\n", ntohs(info->length));

	} while (0);

	free (info);
	return NULL;

}
