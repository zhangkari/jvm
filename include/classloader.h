/****************************
 * file name:   classloader.h
 *
 * *************************/

#ifndef __CLASSLOADER__H__
#define __CLASSLOADER__H__

typedef struct cp_info cp_info;
typedef struct field_info field_info;
typedef struct method_info method_info;
typedef struct attr_info attr_info;
typedef struct ClsLoader_s ClsLoader_t;

ClsLoader_t* load_class(const char *path);

#endif
