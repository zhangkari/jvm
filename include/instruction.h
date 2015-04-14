/*************************************
 *file name:	instruction.h 
 *description:	define instruction set
 *
 ************************************/

#ifndef __INSTRUCTION__H__
#define __INSTRUCTION__H__

typedef enum {
	ins_min = -1,
	ins_nop,			// 0x00
	ins_aconst_null,	// 0x01
	ins_iconst_m1,		// 0x02
	ins_iconst_0,		// 0x03
	ins_iconst_1,		// 0x04
	ins_iconst_2,		// 0x05
	ins_iconst_3,		// 0x06
	ins_iconst_4,		// 0x07
	ins_iconst_5,		// 0x08
	ins_lconst_0,		// 0x09
	ins_lconst_1,		// 0x0a
	ins_fconst_0,		// 0x0b
	ins_fconst_1,		// 0x0c
	ins_fconst_2,		// 0x0d
	ins_dconst_0,		// 0x0e
	ins_dconst_1,		// 0x0f
	ins_bipush,			// 0x10
	ins_sipush,			// 0x11
	ins_ldc,			// 0x12
	ins_ldc_w,			// 0x13
	ins_ldc2_w,			// 0x14
	ins_iload,			// 0x15
	ins_lload,			// 0x16
	ins_fload,			// 0x17
	ins_dload,			// 0x18
	ins_aload,			// 0x19
	ins_iload_0,		// 0x1a
	ins_iload_1,		// 0x1b
	ins_iload_2,		// 0x1c
	ins_iload_3,		// 0x1d
	ins_lload_0,		// 0x1e
	ins_lload_1,		// 0x1f
	ins_lload_2,		// 0x20
	ins_faload_0,		// 0x21
	ins_fload_0,		// 0x22
	ins_fload_1,		// 0x23
	ins_fload_2,		// 0x24
	ins_fload_3,		// 0x25
	ins_dload_0,		// 0x26
	ins_dload_1,		// 0x27
	ins_dload_2,		// 0x28
	ins_dload_3,		// 0x29
	ins_aload_0,		// 0x2a
	ins_aload_1,		// 0x2b
	ins_aload_2,		// 0x2c
	ins_aload_3,		// 0x2d
	ins_iaload,			// 0x2e
	ins_laload_3,		// 0x2f
	ins_faload_3,		// 0x30
	ins_daload_3,		// 0x31
	ins_aaload_3,		// 0x32
	ins_baload_3,		// 0x33
	ins_caload_3,		// 0x34
	ins_saload_3,		// 0x35
	ins_istore,			// 0x36
	ins_lstore,			// 0x37
	ins_fstore,			// 0x38
	ins_dstore,			// 0x39
	ins_astore,			// 0x3a
} ins_opcode;

#endif
