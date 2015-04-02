/****************************
 * file name:   ClassFile.c
 *
 * *************************/

#include <stdio.h>
#include <stdlib.h>

#include <ClassFile.h>
#include <comm.h>

enum tag_value {
    CONSTANT_Utf8       = 1,
    CONSTANT_Integer    = 3,
    CONSTANT_Float      = 4,
    CONSTANT_Long       = 5,
    CONSTANT_Double     = 6,
    CONSTANT_Class      = 7,
    CONSTANT_String     = 8,
    CONSTANT_Fieldref   = 9,
    CONSTANT_Methodref  = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12, 
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18,
};
struct cp_info {
    uint8 tag;
    uint8 info[];
};

struct CONSTANT_Class_info {
    uint8  tag;
    uint16 name_index;
};

struct CONSTANT_Fieldref_info {
    uint8  tag;
    uint16 class_index;
    uint16 name_and_type_index;
};

struct CONSTANT_Methodref_info {
    uint8  tag;
    uint16 class_index;
    uint16 name_and_type_index;
};

struct CONSTANT_String_info {
    uint8  tag;
    uint16 string_index;
};

struct CONSTANT_Integer_info {
    uint8  tag;
    uint16 bytes;
};

struct CONSTANT_Float_info {
    uint8  tag;
    uint16 bytes;
};

struct CONSTANT_Long_info {
    uint8  tag;
    uint32 high_bytes;
    uint32 low_bytes;
};

struct CONSTANT_Double_info {
    uint8  tag;
    uint32 high_bytes;
    uint32 low_bytes;
};

struct CONSTANT_NameAndType_info {
    uint8  tag;
    uint16 name_index;
    uint16 descriptor_index;
};

struct CONSTANT_Utf8_info {
    uint8  tag;
    uint16 length;
    uint8  bytes[ 0 /*length*/];
};

struct CONSTANT_MethodHandle_info {
    uint8  tag;
    uint8  reference_kind;
    uint16 reference_index;
};

struct CONSTANT_MethodType_info {
    uint8  tag;
    uint16 descriptor_index;
};

struct CONSTANT_InvokeDynamic_info {
    uint8  tag;
    uint16 bootstrap_method_attr_index;
    uint16 name_and_type_index;
};

struct field_info {
    uint16 access_flags;
    uint16 name_index;
    uint16 descriptor_index;
    uint16 attr_count;
    //    attr_info attr[ 0 /*attr_count*/];
};

struct method_info {
    uint16 access_flags;
    uint16 name_index;
    uint16 descriptor_index;
    uint16 attr_count;
    //    attr_info attr[ 0 /*attr_count*/ ];
};


enum access_flags {
    ACC_PUBLIC      = 0x0001,
    ACC_PRIVATE     = 0x0002,
    ACC_PROTECTED   = 0x0004,
    ACC_STATIC      = 0x0008,
    ACC_FINAL       = 0x0010,
    ACC_SUPER       = 0x0020,

    ACC_SYNCHRONIZED = 0x0020, // method_info

    ACC_VOLATILE    = 0x0040,
    ACC_BRIDGE      = 0x0040,  // method_info
    ACC_TRANSLENT   = 0x0080,
    ACC_NATIVE      = 0x0100, 
    ACC_INTERFACE   = 0x0200,
    ACC_ABSTRACT    = 0x0400,
    ACC_STRICT      = 0x0800, // strictfp
    ACC_SYNTHETIC   = 0x1000,
    ACC_ANNOTATION  = 0x2000,
    ACC_ENUM        = 0x4000,
};

struct attr_info {
    uint16 attr_name_index;
    uint32 attr_length;
    uint8  info[ 0 /*attr_length*/];
};

struct ClassFile {
    uint32      magic;
    uint16      minor_version;
    uint16      major_version;
    uint16      const_pool_count;
    cp_info     *const_pool /*[const_pool_count - 1] */;
    uint16      access_flags;
    uint16      this_class;
    uint16      super_class;
    uint16      interfaces_count;
    uint16      interfaces[ 0 /*interface_count*/];
    uint16      field_count;
    field_info  fields[ 0 /*fileds_count*/]; 
    uint16      methods_count;
    method_info methods[ 0 /*methods_count*/];
    uint16      attributes_count;
    attr_info   attributes[ 0 /*attributes_count*/]; 
};

ClassFile* load_class(const char *path)
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


        utf8_info utf8;
        integer_info integerinfo;
        float_info floatinfo;
        long_info longinfo;
        double_info doubleinfo;
        class_info clsinfo;
        string_info strinfo;
        fieldref_info fieldref;
        methodref_info methodref;

        char *buff = NULL;

        int i;
        for (i = 0; i < clsFile->const_pool_count - 1; ++i) {
            uint8 tag;
            fread(&tag, sizeof(tag), 1, fp);
            printf("tag=%d\n", tag);

            switch (tag) {
                case CONSTANT_Utf8:
                    printf("Utf8\n");
                    fread(&utf8, sizeof(utf8), 1, fp);
                    buff = (char *)calloc(utf8.length, 1);
                    fread(buff, utf8.length, 1, fp);
                    printf("buff:%s\n", buff);
                    break;

                case CONSTANT_Integer:
                    printf("Integer\n");
                    fread(&integerinfo, sizeof(integerinfo), 1, fp);
                    break;

                case CONSTANT_Float:
                    printf("Float\n");
                    fread(&floatinfo, sizeof(floatinfo), 1, fp);
                    break;

                case CONSTANT_Long:
                    printf("Long\n");
                    fread(&longinfo, sizeof(longinfo), 1, fp);
                    break;

                case CONSTANT_Double:
                    printf("Double\n");
                    fread(&doubleinfo, sizeof(doubleinfo), 1, fp);
                    break;

                case CONSTANT_Class:
                    printf("Class\n");
                    fread(&clsinfo, sizeof(clsinfo), 1, fp);
                    break;

                case CONSTANT_String:
                    printf("String\n");
                    fread(&strinfo, sizeof(strinfo), 1, fp);
                    break;

                case CONSTANT_Fieldref:
                    printf("Filedref\n");
                    fread(&fieldref, sizeof(fieldref), 1, fp);
                    break;

                case CONSTANT_Methodref:
                    printf("Methodref\n");
                    fread(&methodref, sizeof(methodref), 1, fp);
                    break;

                case CONSTANT_InterfaceMethodref:
                    printf("InterfaceMethod ref\n");
                    break;

                case CONSTANT_NameAndType:
                    printf("NameAndType\n");
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

                default:
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
