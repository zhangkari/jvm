/****************************
 * file name:   ClassFile.h
 *
 * *************************/

#ifndef __CLASSFILE__H__
#define __CLASSFILE__H__

#include <comm.h>

typedef struct CONSTANT_Long_info long_info;
typedef struct CONSTANT_Utf8_info utf8_info;
typedef struct CONSTANT_Class_info class_info;
typedef struct CONSTANT_Float_info float_info;
typedef struct CONSTANT_Double_info double_info;
typedef struct CONSTANT_String_info string_info;
typedef struct CONSTANT_Integer_info integer_info;
typedef struct CONSTANT_Fieldref_info fieldref_info;
typedef struct CONSTANT_Methodref_info methodref_info;
typedef struct CONSTANT_NameAndType_info nametype_info;
typedef struct CONSTANT_MethodType_info methodtype_info;
typedef struct CONSTANT_MethodHandle_info methodhandle_info;
typedef struct CONSTANT_InvokeDynamic_info invokedynamic_info;
typedef struct CONSTANT_InterfaceMethodref_info interface_methodref_info;

typedef struct cp_info cp_info;
typedef struct field_info field_info;
typedef struct method_info method_info;
typedef struct attr_info attr_info;
typedef struct ClassFile ClassFile;

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
    void* info;
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
    uint32 bytes;
};

struct CONSTANT_Float_info {
    uint8  tag;
    uint32 bytes;
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
    uint8  *bytes;
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
    attr_info *attr; /*attr_count*/
};

struct method_info {
    uint16 access_flags;
    uint16 name_index;
    uint16 descriptor_index;
    uint16 attr_count;
    attr_info *attr; /*attr_count*/
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
    cp_info     **const_pool /*[const_pool_count - 1] */;
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

/**
 * Load class file
 */
PUBLIC ClassFile* load_class(const char *path);

PRIVATE void log_utf8_info(utf8_info *info);
PRIVATE void log_integer_info(integer_info *info);
PRIVATE void log_float_info(float_info *info);
PRIVATE void log_long_info(long_info *info);
PRIVATE void log_methodref_info(const methodref_info *info);
PRIVATE void log_string_info(const string_info *info);
PRIVATE void log_fieldref_info(const fieldref_info *info);
PRIVATE void log_nametype_info(const nametype_info *info);
PRIVATE void log_class_info(const class_info *info);
PRIVATE void log_cp_info(const cp_info *info);
PRIVATE void logClassFile(const ClassFile *file);

PRIVATE cp_info* read_cp_info(FILE *fp);
PRIVATE utf8_info* read_utf8_info(FILE *fp);
PRIVATE methodref_info* read_methodref_info(FILE *fp);
PRIVATE class_info* read_class_info(FILE *fp);
PRIVATE integer_info* read_integer_info(FILE *fp);
PRIVATE string_info* read_string_info(FILE *fp);
PRIVATE nametype_info* read_nametype_info(FILE *fp);
PRIVATE fieldref_info* read_fieldref_info(FILE *fp);

PRIVATE int read_uint16(uint16 *value, FILE *fp);
PRIVATE int read_uint32(uint32 *value, FILE *fp);

#endif
