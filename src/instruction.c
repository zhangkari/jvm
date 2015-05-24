/*************************************
 *file name:	instruction.c 
 *description:	define instruction set
 *
 ************************************/

#include <comm.h>
#include <instruction.h>

bool validate_opcode(int opcode)
{
	if (opcode > opcode_min && opcode < opcode_max) {
		return true;
	} else {
		return false;
	}
}

const char* stropcode(int opcode)
{
	if (validate_opcode(opcode)) {
		return opcode_name[opcode];
	} else {
		return "Invalid opcode";
	}
}

const char* const opcode_name[] ={
	"nop",			
	"aconst_null",	
	"iconst_m1",		
	"iconst_0",		
	"iconst_1",		
	"iconst_2",		
	"iconst_3",		
	"iconst_4",		
	"iconst_5",		
	"lconst_0",		
	"lconst_1",		
	"fconst_0",		
	"fconst_1",		
	"fconst_2",		
	"dconst_0",		
	"dconst_1",		
	"bipush",			
	"sipush",			
	"ldc",			
	"ldc_w",			
	"ldc2_w",			
	"iload",			
	"lload",			
	"fload",			
	"dload",			
	"aload",			
	"iload_0",		
	"iload_1",		
	"iload_2",		
	"iload_3",		
	"lload_0",		
	"lload_1",		
	"lload_2",		
	"faload_0",		
	"fload_0",		
	"fload_1",		
	"fload_2",		
	"fload_3",		
	"dload_0",		
	"dload_1",		
	"dload_2",		
	"dload_3",		
	"aload_0",		
	"aload_1",		
	"aload_2",		
	"aload_3",		
	"iaload",			
	"lasore",		
	"faload",			
	"daload",			
	"aaload",			
	"baload",			
	"caload",			
	"saload",			
	"istore",			
	"lstore",			
	"fstore",			
	"dstore",			
	"astore",			
	"istore_0",		
	"istore_1",		
	"istore_2",		
	"istore_3",		
	"lstore_0",		
	"lstore_1",		
	"lstore_2",		
	"lstore_3",		
	"fstore_0",		
	"fstore_1",		
	"fstore_2",		
	"fstore_3",		
	"dstore_0",		
	"dstore_1",		
	"dstore_2",		
	"dstore_3",		
	"astore_0",		
	"astore_1",		
	"astore_2",		
	"astore_3",		
	"iastore",		
	"lastore",		
	"fastore",		
	"dastore",		
	"aastore",		
	"bastore",		
	"castore",		
	"sastore",		
	"pop",			
	"pop2",			
	"dup",			
	"dup_x1",			
	"dup_x2",			
	"dup2",			
	"dup2_x1",		
	"dup2_x2",		
	"swap",			
	"iadd",			
	"ladd",			
	"fadd",			
	"dadd",			
	"isub",			
	"lsub",			
	"fsub",			
	"dsub",			
	"imul",			
	"lmul",			
	"fmul",			
	"dmul",			
	"idiv",			
	"ldiv",			
	"fdiv",			
	"ddiv",			
	"irem",			
	"lrem",			
	"frem",			
	"drem",			
	"ineg",			
	"lneg",			
	"fneg",			
	"dneg",			
	"ishl",			
	"lshl",			
	"ishr",			
	"lshr",			
	"iushr",			
	"lushr",			
	"iand",			
	"land",			
	"ior",			
	"lor",			
	"ixor",			
	"lxor",			
	"iinc",			
	"i2l",			
	"i2f",			
	"i2d",			
	"l2i",			
	"l2f",			
	"l2d",			
	"f2i",			
	"f2l",			
	"f2d",			
	"d2i",			
	"d2l",			
	"d2f",			
	"i2b",			
	"i2c",			
	"i2s",			
	"lcmp",			
	"fcmpl",			
	"fcmpg",			
	"dcmpl",			
	"dcmpg",			
	"ifeq",			
	"ifne",			
	"iflt",			
	"ifge",			
	"ifgt",			
	"ifle",			
	"if_icmpeq",		
	"if_icmpne",		
	"if_icmplt",		
	"if_icmpge",		
	"if_icmpgt",		
	"if_icmple",		
	"if_acmpeq",		
	"if_acmpne",		
	"goto",			
	"jsr",			
	"ret",			
	"ableswitch",	
	"lookupswitch",	
	"ireturn",		
	"lreturn",		
	"freturn",		
	"dreturn",		
	"areturn",		
	"return",		
	"getstatic",		
	"putstatic",		
	"getfield",		
	"putfield",		
	"invokevirtual",	
	"invokespecial",	
	"invokestatic",	
	"invokeinterface",
	"invokedynamic",	
	"new",			
	"newarray",		
	"anewarray",		
	"arraylength",	
	"athrow",			
	"checkcast",		
	"instanceof",		
	"moniorenter",	
	"moniorexit",	
	"wide",			
	"mulianewarray",	
	"ifnull",			
	"ifnonnull",		
	"goto_w",			
	"jsr_w",			
	"breakpoint"
};

uint8 operand_len[] = {
    0,  // 0x00 nop
    0,  // 0x01 
    0,  // 0x02
    0,  // 0x03
    0,  // 0x04
    0,  // 0x05
    0,  // 0x06
    0,  // 0x07
    0,  // 0x08
    0,  // 0x09
    0,  // 0x0a
    0,  // 0x0b
    0,  // 0x0c
    0,  // 0x0d
    0,  // 0x0e
    0,  // 0x0f dconst_1
    1,  // 0x10 bipush
    2,  // 0x11 sipush
    1,  // 0x12 idc
    2,  // 0x13 ldc_w
    2,  // 0x14 ldc2_w
    1,  // 0x15
    1,  // 0x16
    1,  // 0x17
    1,  // 0x18
    1,  // 0x19 aload
    0,  // 0x1a
    0,  // 0x1b
    0,  // 0x1c
    0,  // 0x1d
    0,  // 0x1e
    0,  // 0x1f
    0,  // 0x20
    0,  // 0x21
    0,  // 0x22
    0,  // 0x23
    0,  // 0x24
    0,  // 0x25
    0,  // 0x26
    0,  // 0x27
    0,  // 0x28
    0,  // 0x29
    0,  // 0x2a
    0,  // 0x2b
    0,  // 0x2c
    0,  // 0x2d
    0,  // 0x2e
    0,  // 0x2f
    0,  // 0x30
    0,  // 0x31
    0,  // 0x32
    0,  // 0x33
    0,  // 0x34
    0,  // 0x35 saload
    1,  // 0x36 istore
    1,  // 0x37
    1,  // 0x38
    1,  // 0x39
    1,  // 0x3a astore
    0,  // 0x3b istore_0
    0,  // 0x3c
    0,  // 0x3d
    0,  // 0x3e
    0,  // 0x3f
    0,  // 0x40
    0,  // 0x41
    0,  // 0x42
    0,  // 0x43
    0,  // 0x44
    0,  // 0x45
    0,  // 0x46
    0,  // 0x47
    0,  // 0x48
    0,  // 0x49
    0,  // 0x4a
    0,  // 0x4b
    0,  // 0x4c
    0,  // 0x4d
    0,  // 0x4e
    0,  // 0x4f
    0,  // 0x50
    0,  // 0x51
    0,  // 0x52
    0,  // 0x53
    0,  // 0x54
    0,  // 0x55
    0,  // 0x56
    0,  // 0x57
    0,  // 0x58
    0,  // 0x59
    0,  // 0x5a
    0,  // 0x5b
    0,  // 0x5c
    0,  // 0x5d
    0,  // 0x5e
    0,  // 0x5f
    0,  // 0x60
    0,  // 0x61
    0,  // 0x62
    0,  // 0x63
    0,  // 0x64
    0,  // 0x65
    0,  // 0x66
    0,  // 0x67
    0,  // 0x68
    0,  // 0x69
    0,  // 0x6a
    0,  // 0x6b
    0,  // 0x6c
    0,  // 0x6d
    0,  // 0x6e
    0,  // 0x6f
    0,  // 0x70
    0,  // 0x71
    0,  // 0x72
    0,  // 0x73
    0,  // 0x74
    0,  // 0x75
    0,  // 0x76
    0,  // 0x77
    0,  // 0x78
    0,  // 0x79
    0,  // 0x7a
    0,  // 0x7b
    0,  // 0x7c
    0,  // 0x7d
    0,  // 0x7e
    0,  // 0x7f
    0,  // 0x80
    0,  // 0x81
    0,  // 0x82
    0,  // 0x83 ixor
    1,  // 0x84 iinc
    0,  // 0x85 i2l
    0,  // 0x86
    0,  // 0x87
    0,  // 0x88
    0,  // 0x89
    0,  // 0x8a
    0,  // 0x8b
    0,  // 0x8c
    0,  // 0x8d
    0,  // 0x8e
    0,  // 0x8f
    0,  // 0x90
    0,  // 0x91
    0,  // 0x92
    0,  // 0x93
    0,  // 0x94
    0,  // 0x95
    0,  // 0x96
    0,  // 0x97
    0,  // 0x98 dcmpg
    2,  // 0x99 ifeq
    2,  // 0x9a
    2,  // 0x9b
    2,  // 0x9c
    2,  // 0x9d
    2,  // 0x9e
    2,  // 0x9f
    2,  // 0xa0
    2,  // 0xa1
    2,  // 0xa2
    2,  // 0xa3
    2,  // 0xa4
    2,  // 0xa5
    2,  // 0xa6
    2,  // 0xa7
    2,  // 0xa8 jsr
    1,  // 0xa9 ret
    4,  // 0xaa tableswitch
    4,  // 0xab lookupswitch
    0,  // 0xac ireturn
    0,  // 0xad
    0,  // 0xae
    0,  // 0xaf
    0,  // 0xb0
    0,  // 0xb1
    2,  // 0xb2 getstatic
    2,  // 0xb3
    2,  // 0xb4
    2,  // 0xb5
    2,  // 0xb6
    2,  // 0xb7
    2,  // 0xb8
    2,  // 0xb9
    2,  // 0xba
    2,  // 0xbb _new
    1,  // 0xbc newarray
    2,  // oxbd anewarray
    0,  // 0xbe arraylength
    0,  // 0xbf athrow
    2,  // 0xc0 checkcast
    2,  // 0xc1 instanceof
    0,  // 0xc2 moniterenter
    0,  // 0xc3 moniterexit
    3,  // 0xc4 wide
    3,  // 0xc5 multianewarray
    2,  // 0xc6 ifnull
    2,  // 0xc7 ifnonnull
    4,  // 0xc8 goto_w
    4   // 0xc9 jsr_w
};
