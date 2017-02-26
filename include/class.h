/****************************************************************
 * file name:   class.h
 * description:	define class structure & functions to load class 
 * author:		kari.zhang
 *
 * modifications:
 *	1.Review code by kari.zhang @ 2015-12-22
 * *************************************************************/

#ifndef __CLASS__H__
#define __CLASS__H__

#include "comm.h"
#include "instruction.h"

#define STACK_MAX_DEPTH 256

typedef enum tag_value {
    CONST_Utf8			= 1,
    CONST_Integer		= 3,
    CONST_Float			= 4,
    CONST_Long			= 5,
    CONST_Double		= 6,
    CONST_Class			= 7,
    CONST_String		= 8,
    CONST_Fieldref		= 9,
    CONST_Methodref		= 10,
    CONST_IfMethodref	= 11,
    CONST_NameAndType	= 12, 
    CONST_MethodHandle	= 15,
    CONST_MethodType	= 16,
    CONST_InvokeDynamic	= 18,

	// this type not defined in JVM specification and exclude the
	// primer type (such as int, float, long, double), but include the
	// types all defined above.
	// I use this type to distinguish primer type & reference type.
	// Maybe this type will never be used.
	ReferenceType		= 127
} TypeTag;

typedef enum access_flags {
    ACC_PUBLIC      = 0x0001,
    ACC_PRIVATE     = 0x0002,
    ACC_PROTECTED   = 0x0004,
    ACC_STATIC      = 0x0008,
    ACC_FINAL       = 0x0010,
    ACC_SUPER       = 0x0020,
    ACC_SYNCHRONIZED= 0x0020, 
    ACC_VOLATILE    = 0x0040,
    ACC_BRIDGE      = 0x0040,
    ACC_TRANSLENT   = 0x0080,
	ACC_VARARGS		= 0x0080, 
    ACC_NATIVE      = 0x0100, 
    ACC_INTERFACE   = 0x0200,
    ACC_ABSTRACT    = 0x0400,
    ACC_STRICT      = 0x0800,
    ACC_SYNTHETIC   = 0x1000,
    ACC_ANNOTATION  = 0x2000,
    ACC_ENUM        = 0x4000,
} AccTag;

// Do not modify the assigned value !!!
enum Class_State {
	CLASS_BAD		= 0x00,
	CLASS_LOADING	= 0x01,
	CLASS_LOADED	= 0x02,
	CLASS_LINKING	= 0x03,
	CLASS_LINKED	= 0x04,
	CLASS_RESOLVING	= 0x05,
	CLASS_RESOLVED	= 0x06,
	CLASS_INITING	= 0x07,
	CLASS_INITED	= 0x08
};

enum Prime_Class_Type {
	PRIM_VOID		= 0x00,
	PRIM_BYTE		= 0x01,
	PRIM_BOOLEAN	= 0x02,
	PRIM_CHAR		= 0x03,
	PRIM_SHORT		= 0x04,
	PRIM_INT		= 0x05,
	PRIM_FLOAT		= 0x06,
	PRIM_LONG		= 0x07,
	PRIM_DOUBLE		= 0x08
};

typedef struct ConstPoolEntry {
	U1 tag;
	union {
		struct {
			U2 name_index;
		} class_info;

		struct {
			U2 class_index;
			U2 name_type_index;
		} fieldref_info;

		struct {
			U2 class_index;
			U2 name_type_index;
		} methodref_info;

		struct {
			U2 class_index;
			U2 name_type_index;
		} ifmethodref_info;

		struct {
			U2 string_index;
		} string_info;

		struct {
			U4 bytes;
		} integer_info;

		struct {
			U4 bytes;
		} float_info;

		struct {
			U4 high_bytes;
			U4 low_bytes;
		} long_info;

		struct {
			U4 high_bytes;
			U4 low_bytes;
		} double_info;

		struct {
			U2 name_index;
			U2 type_index;
		} nametype_info;

		struct {
			U2 	 length;
			char *bytes;
		} utf8_info;

		struct {
			U1 ref_kind;
			U2 ref_index;
		} methodhandle_info;

		struct {
			U2 type_index;
		} methodtype_info;

		struct {
			U2 bootstrap_method_attr_index;
			U2 name_type_index;
		} invokedynamic_info;
	} info;
} ConstPoolEntry;

typedef struct ConstPool {
	U2 length;
	ConstPoolEntry *entries;
} ConstPool;

typedef struct ExceptionEntry {
    U2 start_pc;
    U2 end_pc;
    U2 handler_pc;
    U2 catch_type;
} ExceptionEntry;

typedef struct ExceptionTable {
	U4 length;
	struct ExceptionEntry* entries;
} ExceptionTable;

/*
 I need leave some explanations of jvm memory model.
 At first, I design a reference handle will hold the pointer of Class type. Therefore, Object struct will hold a Class struct pointer. This make Object struct and Class struct look the same. However, If use this memory model, the jvm GC will be very difficult to realize. So I change another memory model.
In the new memory model, I use a Reference Handle to hold the Class struct and Object struct.
Now I should redesign the Object struct.
 */

typedef struct Object Object;
typedef struct Object Class;
/*******************************************
 * model in memory (assume in 32-bits CPU)
 *
 * ----------------------------------
 * | 4 bytes | 4 bytes | ClassEntry |
 * ----------------------------------
 * |   lock  |   cls   |  clsEntry  |
 * ----------------------------------
 *
 * cls always be NULL in Class
 * cls pointed it's Class type in Object
 *******************************************/
struct Object {
	uintptr_t lock;			// this
	Class *cls;			    // class type(may be NULL in Class type)
	/* classEntry */		// classEntry ( see allocClass() )
}; 

typedef struct FieldEntry {
	Class *class;
	char *name;	
	char *type;
	char *signature;
	U2	acc_flags;
	U2	constant;
} FieldEntry;

typedef struct MethodEntry {
	Class           *class;
	char            *name;
	char            *type;
	char            *signature; 
	U2	            acc_flags;
	U2				max_stack;
	U2				max_locals;
	U2	            args_count;
	U4	            code_length;
	void            *code;
    U4              instCnt;
    Instruction     **instTbl;
	ExceptionTable  excep_tbl;
} MethodEntry;

#define RESERVE_SIZE 4
typedef struct ClassEntry {
	uintptr_t reserve[RESERVE_SIZE];	// to keep version until now
	char *name;							// class name
	char *signature;					// class signature
	char *super_name;					// super class name
	char *source_file;					// file name of this class
	Class *super;						// super class 
	U1 state;							// class state
	U2 acc_flags;						// class access flag
	U2 fields_count;					// filed count
	FieldEntry  *fields;				// fields
	ConstPool *constPool;				// constant pool
	U2 methods_count;					// methods count
	MethodEntry *methods;				// methods
	U2 interfaces_count;				// interface count
	Class **interfaces;					// interfaces
	U2 *interfaces_index;				//
	Object *class_loader;				// 
} ClassEntry;

#define CLASS_CE(cls) ( (ClassEntry *) (cls + 1) )

extern Class* allocClass();
extern ConstPool* newConstPool(int length);
extern Class* defineClass(const char *clsname, const char *data, int len);
extern Class* findClassImpl(const char *classname, Class * const *list, int size);
extern bool linkClassImpl(Class *class, const ExecEnv *env);
extern bool resolveClass(Class *class);

extern FieldEntry* findField(Class *class, char *name, char *type);
extern MethodEntry* findMethod(Class *class, char *name, char *type);
extern MethodEntry* lookupVirtualMethod(Class *class, char *name, char *type);

/*
 * Load a Class from a .class file
 * Parameters:
 *		path:		path of *.class
 *		classname:	such as System (may represent java.lang.System)
 * Return:
 *		if Error: NULL
 *		if OK	: base address of Class
 */
extern Class* loadClassFromFile(const char *path, const char *classname);

/*
 * Load a Class from a .jar file
 * Parameters:
 *		path:		path of *.jar (such as /jdk/jre/lib/rt.jar)
 * Return:
 *		if Error: NULL
 *		if OK	: base address of Class
 */
extern int loadClassFromJar(char *path, Class ***classes);

/*
 * Extract instructions from byte codes
 * Parameters:
 *		MethodEntry:	method
 */
extern void extractInstructions(MethodEntry *method);

/**
 * Log the information of ConstPool
 */
extern void logConstPool(const ConstPool *pool);

/**
 * Log the information of ConstPoolEntry
 */
extern void logConstPoolEntry(const ConstPool *pool, 
		const ConstPoolEntry* constEntry);

/**
 * Log the information of ClassEntry
 */
extern void logClassEntry(ClassEntry* clsEntry);

/**
 * Log the information of MethodEntry
 */
extern void logMethodEntry(MethodEntry* method);

#endif
