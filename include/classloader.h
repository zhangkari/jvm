/****************************
 * file name:   classloader.h
 *
 * *************************/

#ifndef __CLASSLOADER__H__
#define __CLASSLOADER__H__

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
typedef struct ClsLoader_s ClsLoader_t;

ClsLoader_t* load_class(const char *path);

#endif
