/*************************************
 *file name:	instruction.c 
 *description:	define instruction set
 *
 ************************************/

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include "comm.h"
#include "endianswap.h"
#include "instruction.h"
#include "runtime.h"
#include "utility.h"

enum {
	opcode_min = -1,
	nop,			// 0x00
	aconst_null,	// 0x01
	iconst_m1,		// 0x02
	iconst_0,		// 0x03
	iconst_1,		// 0x04
	iconst_2,		// 0x05
	iconst_3,		// 0x06
	iconst_4,		// 0x07
	iconst_5,		// 0x08
	lconst_0,		// 0x09
	lconst_1,		// 0x0a
	fconst_0,		// 0x0b
	fconst_1,		// 0x0c
	fconst_2,		// 0x0d
	dconst_0,		// 0x0e
	dconst_1,		// 0x0f
	bipush,			// 0x10
	sipush,			// 0x11
	ldc,			// 0x12
	ldc_w,			// 0x13
	ldc2_w,			// 0x14
	iload,			// 0x15
	lload,			// 0x16
	fload,			// 0x17
	dload,			// 0x18
	aload,			// 0x19
	iload_0,		// 0x1a
	iload_1,		// 0x1b
	iload_2,		// 0x1c
	iload_3,		// 0x1d
	lload_0,		// 0x1e
	lload_1,		// 0x1f
	lload_2,		// 0x20
	lload_3,		// 0x21
	fload_0,		// 0x22
	fload_1,		// 0x23
	fload_2,		// 0x24
	fload_3,		// 0x25
	dload_0,		// 0x26
	dload_1,		// 0x27
	dload_2,		// 0x28
	dload_3,		// 0x29
	aload_0,		// 0x2a
	aload_1,		// 0x2b
	aload_2,		// 0x2c
	aload_3,		// 0x2d
	iaload,			// 0x2e
	laload,			// 0x2f
	faload,			// 0x30
	daload,			// 0x31
	aaload,			// 0x32
	baload,			// 0x33
	caload,			// 0x34
	saload,			// 0x35
	istore,			// 0x36
	lstore,			// 0x37
	fstore,			// 0x38
	dstore,			// 0x39
	astore,			// 0x3a
	istore_0,		// 0x3b
	istore_1,		// 0x3c
	istore_2,		// 0x3d
	istore_3,		// 0x3e
	lstore_0,		// 0x3f
	lstore_1,		// 0x40
	lstore_2,		// 0x41
	lstore_3,		// 0x42
	fstore_0,		// 0x43
	fstore_1,		// 0x44
	fstore_2,		// 0x45
	fstore_3,		// 0x46
	dstore_0,		// 0x47
	dstore_1,		// 0x48
	dstore_2,		// 0x49
	dstore_3,		// 0x4a
	astore_0,		// 0x4b
	astore_1,		// 0x4c
	astore_2,		// 0x4d
	astore_3,		// 0x4e
	iastore,		// 0x4f
	lastore,		// 0x50
	fastore,		// 0x51
	dastore,		// 0x52
	aastore,		// 0x53
	bastore,		// 0x54
	castore,		// 0x55
	sastore,		// 0x56
	pop,			// 0x57
	pop2,			// 0x58
	dup,			// 0x59
	dup_x1,			// 0x5a
	dup_x2,			// 0x5b
	dup2,			// 0x5c
	dup2_x1,		// 0x5d
	dup2_x2,		// 0x5e
	swap,			// 0x5f
	iadd,			// 0x60
	ladd,			// 0x61
	fadd,			// 0x62
	dadd,			// 0x63
	isub,			// 0x64
	lsub,			// 0x65
	fsub,			// 0x66
	dsub,			// 0x67
	imul,			// 0x68
	lmul,			// 0x69
	fmul,			// 0x6a
	dmul,			// 0x6b
	idiv,			// 0x6c
	ldiv,			// 0x6d
	fdiv,			// 0x6e
	ddiv,			// 0x6f
	irem,			// 0x70
	lrem,			// 0x71
	frem,			// 0x72
	drem,			// 0x73
	ineg,			// 0x74
	lneg,			// 0x75
	fneg,			// 0x76
	dneg,			// 0x77
	ishl,			// 0x78
	lshl,			// 0x79
	ishr,			// 0x7a
	lshr,			// 0x7b
	iushr,			// 0x7c
	lushr,			// 0x7d
	iand,			// 0x7e
	land,			// 0x7f
	ior,			// 0x80
	lor,			// 0x81
	ixor,			// 0x82
	lxor,			// 0x83
	iinc,			// 0x84
	i2l,			// 0x85
	i2f,			// 0x86
	i2d,			// 0x87
	l2i,			// 0x88
	l2f,			// 0x89
	l2d,			// 0x8a
	f2i,			// 0x8b
	f2l,			// 0x8c
	f2d,			// 0x8d
	d2i,			// 0x8e
	d2l,			// 0x8f
	d2f,			// 0x90
	i2b,			// 0x91
	i2c,			// 0x92
	i2s,			// 0x93
	lcmp,			// 0x94
	fcmpl,			// 0x95
	fcmpg,			// 0x96
	dcmpl,			// 0x97
	dcmpg,			// 0x98
	ifeq,			// 0x99
	ifne,			// 0x9a
	iflt,			// 0x9b
	ifge,			// 0x9c
	ifgt,			// 0x9d
	ifle,			// 0x9e
	if_icmpeq,		// 0x9f
	if_icmpne,		// 0xa0
	if_icmplt,		// 0xa1
	if_icmpge,		// 0xa2
	if_icmpgt,		// 0xa3
	if_icmple,		// 0xa4
	if_acmpeq,		// 0xa5
	if_acmpne,		// 0xa6
	_goto,			// 0xa7
	jsr,			// 0xa8
	ret,			// 0xa9
	tableswitch,	// 0xaa
	lookupswitch,	// 0xab
	ireturn,		// 0xac
	lreturn,		// 0xad
	freturn,		// 0xae
	dreturn,		// 0xaf
	areturn,		// 0xb0
	_return,		// 0xb1
	getstatic,		// 0xb2
	putstatic,		// 0xb3
	getfield,		// 0xb4
	putfield,		// 0xb5
	invokevirtual,	// 0xb6
	invokespecial,	// 0xb7
	invokestatic,	// 0xb8
	invokeinterface,// 0xb9
	invokedynamic,	// 0xba
	_new,			// 0xbb
	newarray,		// 0xbc
	anewarray,		// 0xbd
	arraylength,	// 0xbe
	athrow,			// 0xbf
	checkcast,		// 0xc0
	instanceof,		// 0xc1
	monitorenter,	// 0xc2
	monitorexit,	// 0xc3
	wide,			// 0xc4
	multianewarray,	// 0xc5
	ifnull,			// 0xc6
	ifnonnull,		// 0xc7
	goto_w,			// 0xc8
	jsr_w,			// 0xc9
	breakpoint,		// 0xca
	opcode_max = 0xcb,
	impdep1 = 0xfe,
	impdep2 = 0xff,
}; 

#define DECL_FUNC(X) bool func_inst_##X(void *param)

DECL_FUNC(nop);
DECL_FUNC(aconst_null);
DECL_FUNC(iconst_m1);
DECL_FUNC(iconst_0);
DECL_FUNC(iconst_1);
DECL_FUNC(iconst_2);
DECL_FUNC(iconst_3);
DECL_FUNC(iconst_4);
DECL_FUNC(iconst_5);
DECL_FUNC(lconst_0);
DECL_FUNC(lconst_1);
DECL_FUNC(fconst_0);
DECL_FUNC(fconst_1);
DECL_FUNC(fconst_2);
DECL_FUNC(dconst_0);
DECL_FUNC(dconst_1);
DECL_FUNC(bipush);
DECL_FUNC(sipush);
DECL_FUNC(ldc);
DECL_FUNC(ldc_w);
DECL_FUNC(ldc2_w);
DECL_FUNC(iload);
DECL_FUNC(lload);
DECL_FUNC(fload);
DECL_FUNC(dload);
DECL_FUNC(aload);
DECL_FUNC(iload_0);
DECL_FUNC(iload_1);
DECL_FUNC(iload_2);
DECL_FUNC(iload_3);
DECL_FUNC(lload_0);
DECL_FUNC(lload_1);
DECL_FUNC(lload_2);
DECL_FUNC(lload_3);
DECL_FUNC(fload_0);
DECL_FUNC(fload_1);
DECL_FUNC(fload_2);
DECL_FUNC(fload_3);
DECL_FUNC(dload_0);
DECL_FUNC(dload_1);
DECL_FUNC(dload_2);
DECL_FUNC(dload_3);
DECL_FUNC(aload_0);
DECL_FUNC(aload_1);
DECL_FUNC(aload_2);
DECL_FUNC(aload_3);
DECL_FUNC(iaload);
DECL_FUNC(laload);
DECL_FUNC(faload);
DECL_FUNC(daload);
DECL_FUNC(aaload);
DECL_FUNC(baload);
DECL_FUNC(caload);
DECL_FUNC(saload);
DECL_FUNC(istore);
DECL_FUNC(lstore);
DECL_FUNC(fstore);
DECL_FUNC(dstore);
DECL_FUNC(astore);
DECL_FUNC(istore_0);
DECL_FUNC(istore_1);
DECL_FUNC(istore_2);
DECL_FUNC(istore_3);
DECL_FUNC(lstore_0);
DECL_FUNC(lstore_1);
DECL_FUNC(lstore_2);
DECL_FUNC(lstore_3);
DECL_FUNC(fstore_0);
DECL_FUNC(fstore_1);
DECL_FUNC(fstore_2);
DECL_FUNC(fstore_3);
DECL_FUNC(dstore_0);
DECL_FUNC(dstore_1);
DECL_FUNC(dstore_2);
DECL_FUNC(dstore_3);
DECL_FUNC(astore_0);
DECL_FUNC(astore_1);
DECL_FUNC(astore_2);
DECL_FUNC(astore_3);
DECL_FUNC(iastore);
DECL_FUNC(lastore);
DECL_FUNC(fastore);
DECL_FUNC(dastore);
DECL_FUNC(aastore);
DECL_FUNC(bastore);
DECL_FUNC(castore);
DECL_FUNC(sastore);
DECL_FUNC(pop);
DECL_FUNC(pop2);
DECL_FUNC(dup);
DECL_FUNC(dup_x1);
DECL_FUNC(dup_x2);
DECL_FUNC(dup2);
DECL_FUNC(dup2_x1);
DECL_FUNC(dup2_x2);
DECL_FUNC(swap);
DECL_FUNC(iadd);
DECL_FUNC(ladd);
DECL_FUNC(fadd);
DECL_FUNC(dadd);
DECL_FUNC(isub);
DECL_FUNC(lsub);
DECL_FUNC(fsub);
DECL_FUNC(dsub);
DECL_FUNC(imul);
DECL_FUNC(lmul);
DECL_FUNC(fmul);
DECL_FUNC(dmul);
DECL_FUNC(idiv);
DECL_FUNC(ldiv);
DECL_FUNC(fdiv);
DECL_FUNC(ddiv);
DECL_FUNC(irem);
DECL_FUNC(lrem);
DECL_FUNC(frem);
DECL_FUNC(drem);
DECL_FUNC(ineg);
DECL_FUNC(lneg);
DECL_FUNC(fneg);
DECL_FUNC(dneg);
DECL_FUNC(ishl);
DECL_FUNC(lshl);
DECL_FUNC(ishr);
DECL_FUNC(lshr);
DECL_FUNC(iushr);
DECL_FUNC(lushr);
DECL_FUNC(iand);
DECL_FUNC(land);
DECL_FUNC(ior);
DECL_FUNC(lor);
DECL_FUNC(ixor);
DECL_FUNC(lxor);
DECL_FUNC(iinc);
DECL_FUNC(i2l);
DECL_FUNC(i2f);
DECL_FUNC(i2d);
DECL_FUNC(l2i);
DECL_FUNC(l2f);
DECL_FUNC(l2d);
DECL_FUNC(f2i);
DECL_FUNC(f2l);
DECL_FUNC(f2d);
DECL_FUNC(d2i);
DECL_FUNC(d2l);
DECL_FUNC(d2f);
DECL_FUNC(i2b);
DECL_FUNC(i2c);
DECL_FUNC(i2s);
DECL_FUNC(lcmp);
DECL_FUNC(fcmpl);
DECL_FUNC(fcmpg);
DECL_FUNC(dcmpl);
DECL_FUNC(dcmpg);
DECL_FUNC(ifeq);
DECL_FUNC(ifne);
DECL_FUNC(iflt);
DECL_FUNC(ifge);
DECL_FUNC(ifgt);
DECL_FUNC(ifle);
DECL_FUNC(if_icmpeq);
DECL_FUNC(if_icmpne);
DECL_FUNC(if_icmplt);
DECL_FUNC(if_icmpge);
DECL_FUNC(if_icmpgt);
DECL_FUNC(if_icmple);
DECL_FUNC(if_acmpeq);
DECL_FUNC(if_acmpne);
DECL_FUNC(_goto);
DECL_FUNC(jsr);
DECL_FUNC(ret);
DECL_FUNC(tableswitch);
DECL_FUNC(lookupswitch);
DECL_FUNC(ireturn);
DECL_FUNC(lreturn);
DECL_FUNC(freturn);
DECL_FUNC(dreturn);
DECL_FUNC(areturn);
DECL_FUNC(_return);
DECL_FUNC(getstatic);
DECL_FUNC(putstatic);
DECL_FUNC(getfield);
DECL_FUNC(putfield);
DECL_FUNC(invokevirtual);
DECL_FUNC(invokespecial);
DECL_FUNC(invokestatic);
DECL_FUNC(invokeinterface);
DECL_FUNC(invokedynamic);
DECL_FUNC(_new);
DECL_FUNC(newarray);
DECL_FUNC(anewarray);
DECL_FUNC(arraylength);
DECL_FUNC(athrow);
DECL_FUNC(checkcast);
DECL_FUNC(instanceof);
DECL_FUNC(monitorenter);
DECL_FUNC(monitorexit);
DECL_FUNC(wide);
DECL_FUNC(multianewarray);
DECL_FUNC(ifnull);
DECL_FUNC(ifnonnull);
DECL_FUNC(goto_w);
DECL_FUNC(jsr_w);

/*
 * These instructions has 1 operand, but the operand need add the offset
 */
static U1 sOffsetTable[] = {
    if_icmpge,
    ifne,
    _goto,
};

/*
 * If the operand need add offset
 * 
 */
bool needAddOffset (U1 opcode)
{
    int size = sizeof (sOffsetTable) / sizeof (sOffsetTable[0]);
    int i;
    for (i = 0; i < size; ++i) {
        if (sOffsetTable[i] == opcode) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Default instruction template.
 * The instructions in the class methods must copy from them 
 */
#define INST_FUNC(X) func_inst_##X
#define _T(X) #X
#define INIT_INST(X, Y) {X, _T(X), {0}, Y, (Y + 1), INST_FUNC(X), 0}
static Instruction sInstructionTable[] = {
	INIT_INST(nop, 0),          // 0x00
	INIT_INST(aconst_null, 0),  // 0x01
	INIT_INST(iconst_m1, 0),    // 0x02
	INIT_INST(iconst_0, 0),     // 0x03
	INIT_INST(iconst_1, 0),     // 0x04
	INIT_INST(iconst_2, 0),     // 0x05
	INIT_INST(iconst_3, 0),     // 0x06
	INIT_INST(iconst_4,	0),     // 0x07
	INIT_INST(iconst_5,	0), 	// 0x08
	INIT_INST(lconst_0, 0),		// 0x09
	INIT_INST(lconst_1, 0),		// 0x0a
	INIT_INST(fconst_0, 0),		// 0x0b
	INIT_INST(fconst_1, 0),		// 0x0c
	INIT_INST(fconst_2, 0),		// 0x0d
	INIT_INST(dconst_0, 0),		// 0x0e
	INIT_INST(dconst_1,	0), 	// 0x0f
	INIT_INST(bipush, 1),		// 0x10
	INIT_INST(sipush, 2),		// 0x11
	INIT_INST(ldc, 1),			// 0x12
	INIT_INST(ldc_w, 2),		// 0x13
	INIT_INST(ldc2_w, 2),		// 0x14
	INIT_INST(iload, 1),		// 0x15
	INIT_INST(lload, 1),		// 0x16
	INIT_INST(fload, 1),		// 0x17
	INIT_INST(dload, 1),		// 0x18
	INIT_INST(aload, 1),		// 0x19
	INIT_INST(iload_0, 0),		// 0x1a
	INIT_INST(iload_1, 0),		// 0x1b
	INIT_INST(iload_2, 0),		// 0x1c
	INIT_INST(iload_3, 0),		// 0x1d
	INIT_INST(lload_0, 0),		// 0x1e
	INIT_INST(lload_1, 0),		// 0x1f
	INIT_INST(lload_2, 0),		// 0x20
	INIT_INST(lload_3, 0),		// 0x21
	INIT_INST(fload_0, 0),		// 0x22
	INIT_INST(fload_1, 0),		// 0x23
	INIT_INST(fload_2, 0),		// 0x24
	INIT_INST(fload_3, 0),		// 0x25
	INIT_INST(dload_0, 0),		// 0x26
	INIT_INST(dload_1, 0),		// 0x27
	INIT_INST(dload_2, 0),		// 0x28
	INIT_INST(dload_3, 0),		// 0x29
	INIT_INST(aload_0, 0),		// 0x2a
	INIT_INST(aload_1, 0),		// 0x2b
	INIT_INST(aload_2, 0),		// 0x2c
	INIT_INST(aload_3, 0),  	// 0x2d
	INIT_INST(iaload, 0),		// 0x2e
	INIT_INST(laload, 0),		// 0x2f
	INIT_INST(faload, 0),		// 0x30
	INIT_INST(daload, 0),		// 0x31
	INIT_INST(aaload, 0),		// 0x32
	INIT_INST(baload, 0),		// 0x33
	INIT_INST(caload, 0),		// 0x34
	INIT_INST(saload, 0),		// 0x35
	INIT_INST(istore, 1),		// 0x36
	INIT_INST(lstore, 1),		// 0x37
	INIT_INST(fstore, 1),		// 0x38
	INIT_INST(dstore, 1),		// 0x39
	INIT_INST(astore, 1),		// 0x3a
	INIT_INST(istore_0, 0),		// 0x3b
	INIT_INST(istore_1, 0),		// 0x3c
	INIT_INST(istore_2,	0), 	// 0x3d
	INIT_INST(istore_3,	0), 	// 0x3e
	INIT_INST(lstore_0, 0),		// 0x3f
	INIT_INST(lstore_1, 0),		// 0x40
	INIT_INST(lstore_2,	0), 	// 0x41
	INIT_INST(lstore_3, 0),		// 0x42
	INIT_INST(fstore_0, 0),		// 0x43
	INIT_INST(fstore_1,	0), 	// 0x44
	INIT_INST(fstore_2, 0),		// 0x45
	INIT_INST(fstore_3, 0),		// 0x46
	INIT_INST(dstore_0, 0),		// 0x47
	INIT_INST(dstore_1,	0), 	// 0x48
	INIT_INST(dstore_2,	0), 	// 0x49
	INIT_INST(dstore_3, 0),		// 0x4a
	INIT_INST(astore_0, 0),		// 0x4b
	INIT_INST(astore_1, 0),		// 0x4c
	INIT_INST(astore_2, 0),		// 0x4d
	INIT_INST(astore_3,	0), 	// 0x4e
	INIT_INST(iastore, 0),		// 0x4f
	INIT_INST(lastore, 0),		// 0x50
	INIT_INST(fastore, 0),		// 0x51
	INIT_INST(dastore, 0),		// 0x52
	INIT_INST(aastore, 0),		// 0x53
	INIT_INST(bastore, 0),		// 0x54
	INIT_INST(castore, 0),		// 0x55
	INIT_INST(sastore, 0),		// 0x56
	INIT_INST(pop, 0),			// 0x57
	INIT_INST(pop2, 0),			// 0x58
	INIT_INST(dup, 0),			// 0x59
	INIT_INST(dup_x1, 0),		// 0x5a
	INIT_INST(dup_x2, 0),		// 0x5b
	INIT_INST(dup2, 0),			// 0x5c
	INIT_INST(dup2_x1, 0),		// 0x5d
	INIT_INST(dup2_x2, 0),		// 0x5e
	INIT_INST(swap, 0),			// 0x5f
	INIT_INST(iadd, 0),			// 0x60
	INIT_INST(ladd, 0),			// 0x61
	INIT_INST(fadd, 0),			// 0x62
	INIT_INST(dadd, 0),			// 0x63
	INIT_INST(isub, 0),			// 0x64
	INIT_INST(lsub, 0),			// 0x65
	INIT_INST(fsub, 0),			// 0x66
	INIT_INST(dsub, 0),			// 0x67
	INIT_INST(imul, 0),			// 0x68
	INIT_INST(lmul, 0),			// 0x69
	INIT_INST(fmul, 0),			// 0x6a
	INIT_INST(dmul, 0),			// 0x6b
	INIT_INST(idiv, 0),			// 0x6c
	INIT_INST(ldiv, 0),			// 0x6d
	INIT_INST(fdiv, 0),			// 0x6e
	INIT_INST(ddiv, 0),			// 0x6f
	INIT_INST(irem,	0), 		// 0x70
	INIT_INST(lrem, 0),			// 0x71
	INIT_INST(frem, 0),			// 0x72
	INIT_INST(drem, 0),			// 0x73
	INIT_INST(ineg, 0),			// 0x74
	INIT_INST(lneg, 0),			// 0x75
	INIT_INST(fneg, 0),			// 0x76
	INIT_INST(dneg, 0),			// 0x77
	INIT_INST(ishl, 0),			// 0x78
	INIT_INST(lshl, 0),			// 0x79
	INIT_INST(ishr, 0),			// 0x7a
	INIT_INST(lshr, 0),			// 0x7b
	INIT_INST(iushr, 0),		// 0x7c
	INIT_INST(lushr, 0),		// 0x7d
	INIT_INST(iand, 0),			// 0x7e
	INIT_INST(land, 0),			// 0x7f
	INIT_INST(ior, 0),			// 0x80
	INIT_INST(lor, 0),			// 0x81
	INIT_INST(ixor, 0),			// 0x82
	INIT_INST(lxor, 0),			// 0x83
	INIT_INST(iinc, 2),			// 0x84
	INIT_INST(i2l, 0),			// 0x85
	INIT_INST(i2f, 0),			// 0x86
	INIT_INST(i2d, 0),			// 0x87
	INIT_INST(l2i, 0),			// 0x88
	INIT_INST(l2f, 0),			// 0x89
	INIT_INST(l2d, 0),			// 0x8a
	INIT_INST(f2i, 0),			// 0x8b
	INIT_INST(f2l, 0),			// 0x8c
	INIT_INST(f2d, 0),			// 0x8d
	INIT_INST(d2i, 0),			// 0x8e
	INIT_INST(d2l, 0),			// 0x8f
	INIT_INST(d2f, 0),			// 0x90
	INIT_INST(i2b, 0),			// 0x91
	INIT_INST(i2c, 0),			// 0x92
	INIT_INST(i2s, 0),			// 0x93
	INIT_INST(lcmp, 0),			// 0x94
	INIT_INST(fcmpl, 0),		// 0x95
	INIT_INST(fcmpg, 0),		// 0x96
	INIT_INST(dcmpl, 0),		// 0x97
	INIT_INST(dcmpg, 0),		// 0x98
	INIT_INST(ifeq, 2),			// 0x99
	INIT_INST(ifne, 2),			// 0x9a
	INIT_INST(iflt, 2),			// 0x9b
	INIT_INST(ifge, 2),			// 0x9c
	INIT_INST(ifgt, 2),			// 0x9d
	INIT_INST(ifle, 2),			// 0x9e
	INIT_INST(if_icmpeq, 2),	// 0x9f
	INIT_INST(if_icmpne, 2),	// 0xa0
	INIT_INST(if_icmplt, 2),	// 0xa1
	INIT_INST(if_icmpge, 2),	// 0xa2
	INIT_INST(if_icmpgt, 2),	// 0xa3
	INIT_INST(if_icmple, 2),	// 0xa4
	INIT_INST(if_acmpeq, 2),	// 0xa5
	INIT_INST(if_acmpne, 2),	// 0xa6
	INIT_INST(_goto, 2),		// 0xa7
	INIT_INST(jsr, 2),			// 0xa8
	INIT_INST(ret, 1),			// 0xa9
	INIT_INST(tableswitch, TABLE_SWITCH),	// 0xaa 
	INIT_INST(lookupswitch, LOOKUP_SWITCH),	// 0xab
	INIT_INST(ireturn, 0),		// 0xac
	INIT_INST(lreturn, 0),		// 0xad
	INIT_INST(freturn, 0),		// 0xae
	INIT_INST(dreturn, 0),		// 0xaf
	INIT_INST(areturn, 0),		// 0xb0
	INIT_INST(_return, 0),		// 0xb1
	INIT_INST(getstatic, 2),	// 0xb2
	INIT_INST(putstatic, 2),	// 0xb3
	INIT_INST(getfield, 2),		// 0xb4
	INIT_INST(putfield, 2),		// 0xb5
	INIT_INST(invokevirtual, 2),// 0xb6
	INIT_INST(invokespecial, 2),// 0xb7
	INIT_INST(invokestatic, 2),	// 0xb8
	INIT_INST(invokeinterface,2),// 0xb9
	INIT_INST(invokedynamic, 2),// 0xba
	INIT_INST(_new, 2),			// 0xbb
	INIT_INST(newarray, 1),		// 0xbc
	INIT_INST(anewarray, 2),	// 0xbd
	INIT_INST(arraylength, 0),	// 0xbe
	INIT_INST(athrow, 0),		// 0xbf
	INIT_INST(checkcast, 2),	// 0xc0
	INIT_INST(instanceof, 2),	// 0xc1
	INIT_INST(monitorenter, 0),	// 0xc2
	INIT_INST(monitorexit, 0),	// 0xc3
	INIT_INST(wide, WIDE),		// 0xc4
	INIT_INST(multianewarray,3),// 0xc5
	INIT_INST(ifnull, 2),		// 0xc6
	INIT_INST(ifnonnull, 2),	// 0xc7
	INIT_INST(goto_w, 4),		// 0xc8
	INIT_INST(jsr_w, 4),		// 0xc9
};

bool validate_opcode(int opcode) {
	if (opcode <= opcode_min || opcode >= opcode_max) {
		return FALSE;
	}

	return TRUE;
}

const char* stropcode(int opcode) {
	if (!validate_opcode(opcode)) {
		return "Invalid opcode";
	}

	return sInstructionTable[opcode].name;
}

/**
  * Get a instruction in the buffer
  * Param:
		code:		the byte code stream 
		codelen:	code's length
 * Return:
		A pointer of the instruction if valid opcode
		NULL if invalid opcode
 * Notice:
		do not free me
 */
const Instruction* getCachedInstruction(U1 *code, int codelen, int offset)
{
    assert (NULL != code && codelen > 0);

	U1 *buff = code;
	U1 opcode = *buff;
    assert( validate_opcode(opcode) );

    buff++;
	U1 tag = sInstructionTable[opcode].tag;
	if (1 == tag) {

        assert (codelen >= 1);

        U1 u1;
        READ_U1(u1, buff);
		sInstructionTable[opcode].operand.u1 = u1;

	} else if (2 == tag) {

        assert (codelen >= 2);

        U2 u2;
        READ_U2(u2, buff);
        if (needAddOffset (opcode)) {
            u2 += offset;
        }
		sInstructionTable[opcode].operand.u2 = u2;

	} 
    else if (TABLE_SWITCH == tag){

        offset += 1;

        int length = 1;
        int padding = 0;

        if (offset % 4 != 0) {
            padding = 4 - (offset % 4);
        }

        // buff++  before
        buff += padding;
        length += padding;

        uintptr_t ptr = (uintptr_t)buff;
        assert (0 == ptr % 4);

        Instruction *inst = sInstructionTable + opcode;
        int default_position;
        READ_U4(default_position, buff);
        inst->operand.tblSw.defaultPos = default_position + offset;
        length += 4;

        int min;
        READ_INT32(min, buff);
        inst->operand.tblSw.caseMin = min;
        length += 4;

        int max;
        READ_INT32(max, buff);
        inst->operand.tblSw.caseMax = max;
        length += 4;

        U4 pairCnt;
        pairCnt = max - min + 1;
        inst->operand.tblSw.pairCnt = pairCnt;
        CasePair *pairs = (CasePair *)calloc(pairCnt, sizeof(CasePair));

        //printf("[%d, %d],pairCnt:%d\n", min, max, pairCnt);
        assert (NULL != pairs);
        inst->operand.tblSw.pairs = pairs;

        int i;
        for (i = 0; i < pairCnt; ++i) {
            int value;
            U4 pos;
            READ_U4(pos, buff);
            value = i + min;
            pos += offset;
            (pairs + i)->value = value;
            (pairs + i)->position = pos;
            length += 4;
        }
        inst->length = length;

    } else if (LOOKUP_SWITCH == tag) {
        offset += 1;

        int length = 1;
        int padding = 0;

        if (offset % 4 != 0) {
            padding = 4 - (offset % 4);
        }

        // buff++  before
        buff += padding;
        length += padding;

        uintptr_t ptr = (uintptr_t)buff;
        assert (0 == ptr % 4);

        Instruction *inst = sInstructionTable + opcode;
        int default_position;
        READ_U4(default_position, buff);
        inst->operand.tblSw.defaultPos = default_position + offset;
        length += 4;

        U4 pairCnt;
        READ_U4(pairCnt, buff);
        length += 4;
        inst->operand.tblSw.pairCnt = pairCnt;
        CasePair *pairs = (CasePair *)calloc(pairCnt, sizeof(CasePair));

        //printf("pairCnt:%d\n", pairCnt);
        assert (NULL != pairs);
        inst->operand.tblSw.pairs = pairs;

        int i;
        for (i = 0; i < pairCnt; ++i) {
            int value;
            READ_INT32(value, buff);

            U4 pos;
            READ_U4(pos, buff);
            pos += offset;

            (pairs + i)->value = value;
            (pairs + i)->position = pos;
            length += 8;
        }
        inst->length = length;

	}
    else if (WIDE == tag) {

#if 0
        U2 index2;
        U4 index4;
        U1 opcode = *buff;
        Instruction* inst = sInstructionTable + opcode;
        buff++;
        switch (opcode) {

            ///////////////
            //
            // wide
            // <opcode>
            // indexbyte1
            // indexbyte2
            //
            ///////////////

            // load
            case iload:		    // 0x15
            case lload:		    // 0x16
            case fload:		    // 0x17
            case dload:		    // 0x18
            case aload:		    // 0x19
            // store
            case istore:		// 0x36
            case lstore:		// 0x37
            case fstore:		// 0x38
            case dstore:        // 0x39
            case astore:		// 0x3a
            // ret
            case ret:			// 0xa9
                READ_U2(index2, buff);
                buff += 2;
                inst->length = 4;
                break;

            //////////////
            //
            // wide
            // iinc
            // indexbyte1
            // indexbyte2
            // constbyte1
            // constbyte2
            //
            //////////////

            // iinc
            case iinc:
                READ_U4(index4, buff);
                buff += 4;
                inst->length = 6;
                break;

            default:
                assert(0 && "Invalid opcode after wide");
        }

#endif

        printf("WIDE not implemented yet!\n");

        Instruction* inst = sInstructionTable + opcode;
        inst->length = 1;
    }
    else if (3 == tag) {
		// printf("tag = 3\n");

	}
    else if (4 == tag) {
		// printf("tag = 4\n");

	}

	return sInstructionTable + opcode;
}

void logInstruction(const Instruction* inst) {
    if (NULL == inst) {
        return;
    }

	if (0 == inst->tag) {
		printf("%s\n", inst->name);
		return;
	}

    int i;
	int operand = 0;
    switch (inst->tag) {
        case 1:
            operand = inst->operand.u1;
            printf("%s\t%d\n", inst->name, operand);
            break;

        case 2:
            operand = inst->operand.u2;
            printf("%s\t%d\n", inst->name, operand);
            break;

        case TABLE_SWITCH:

            printf("%s  // [%d, %d]\n", inst->name, 
                    inst->operand.tblSw.caseMin, inst->operand.tblSw.caseMax);
            for (i = 0; i < inst->operand.tblSw.pairCnt; ++i) {
                printf("      %d: %d\n", 
                        (inst->operand.tblSw.pairs + i)->value,
                        (inst->operand.tblSw.pairs + i)->position); 
            }
            printf("      default:%d\n", (inst->operand.tblSw.defaultPos));
            break;


        case LOOKUP_SWITCH:
            break;

        default:
            fprintf(stderr, "Invalid tag in instruction\n");
    }

}

/**
 * Validate the instruction execute environment
 */
#define validate_inst_env(param)                            \
                                                            \
    assert(NULL != param);                                  \
                                                            \
    InstExecEnv* instEnv = (InstExecEnv *)param;            \
                                                            \
	Instruction *inst = instEnv->inst;                      \
	assert(NULL != inst);                                   \
                                                            \
	ExecEnv *env = instEnv->env;                            \
	assert(NULL != env);                                    \
                                                            \
	JavaStack *jstack = env->javaStack;                     \
	assert(NULL != jstack);                                 \
                                                            \
	StackFrame *frame = peekJavaStack(jstack);              \
	assert(NULL != frame);                                  \
                                                            \
	LocalVarTable *localTbl = frame->localTbl;              \
	assert(NULL != localTbl);                               \
                                                            \
	OperandStack *opdStack = frame->opdStack;               \
	assert(NULL != opdStack);                               \
                                                            \
	ConstPool *constPool = frame->constPool;                \
	assert(NULL != constPool);                              \
                                                            \
	U2 use = frame->use;                                    \
	assert(use == 1);                                       


//////////////////////////////////
//
// Implement instruction functions
//
//////////////////////////////////

DECL_FUNC(nop)
{
	return FALSE;
}

DECL_FUNC(aconst_null)
{
	return FALSE;
}

DECL_FUNC(iconst_m1)
{
	return FALSE;
}

DECL_FUNC(iconst_0)
{
	return FALSE;
}

DECL_FUNC(iconst_1)
{
	return FALSE;
}

DECL_FUNC(iconst_2)
{
	return FALSE;
}

DECL_FUNC(iconst_3)
{
	return FALSE;
}

DECL_FUNC(iconst_4)
{

    validate_inst_env(param);

    Slot slot;
    slot.tag = CONST_Integer;
    slot.value = 4;
    bool result = pushOperandStack(opdStack, &slot);
    assert(result);

#ifdef DEBUG
    printf("\ticonst_4\n");
#endif

	return TRUE;
}

DECL_FUNC(iconst_5)
{
	return FALSE;
}

DECL_FUNC(lconst_0)
{
	return FALSE;
}

DECL_FUNC(lconst_1)
{
	return FALSE;
}

DECL_FUNC(fconst_0)
{
	return FALSE;
}

DECL_FUNC(fconst_1)
{
	return FALSE;
}

DECL_FUNC(fconst_2)
{
	return FALSE;
}

DECL_FUNC(dconst_0)
{
	return FALSE;
}

DECL_FUNC(dconst_1)
{
	return FALSE;
}

DECL_FUNC(bipush)
{
    validate_inst_env(param);

    U1 u1 = inst->operand.u1;
    Slot slot;
    slot.tag = CONST_Integer;
    slot.value = u1;

    bool result = pushOperandStack(opdStack, &slot);
    assert(result);

#ifdef DEBUG
    printf("\tbipush  %d \n ", u1);
#endif

	return TRUE;
}

DECL_FUNC(sipush)
{
	return FALSE;
}

DECL_FUNC(ldc)
{
    validate_inst_env(param);

	U1 u1 = inst->operand.u1;
	ConstPoolEntry *constEntry = constPool->entries + u1;
	assert(NULL != constEntry);

    Slot slot;
    initSlot(&slot, constPool, constEntry);

    bool result = pushOperandStack(opdStack, &slot);
    assert(result);

#ifdef DEBUG
    printf("\tldc  %d \t\t// ", u1);
	logConstPoolEntry(constPool, constEntry);
#endif

	return TRUE;
}

DECL_FUNC(ldc_w)
{
	return FALSE;
}

DECL_FUNC(ldc2_w)
{
	return FALSE;
}

DECL_FUNC(iload)
{
    printf("iload\n");
	return FALSE;
}

DECL_FUNC(lload)
{
	return FALSE;
}

DECL_FUNC(fload)
{
	return FALSE;
}

DECL_FUNC(dload)
{
	return FALSE;
}

DECL_FUNC(aload)
{
	return FALSE;
}

DECL_FUNC(iload_0)
{
	return FALSE;
}

DECL_FUNC(iload_1)
{
    validate_inst_env(param);

    Slot *slot = localTbl->slots + 1;
    assert(NULL != slot);

    bool result = pushOperandStack(opdStack, slot);
    assert(result);

#ifdef DEBUG
    printf("\tiload_1 \n");
#endif

	return TRUE;
}

DECL_FUNC(iload_2)
{
    validate_inst_env(param);

    Slot *slot = localTbl->slots + 2;
    assert(NULL != slot);

    bool result = pushOperandStack(opdStack, slot);
    assert(result);

#ifdef DEBUG
    printf("\tiload_2 \n");
#endif

	return TRUE;

}

DECL_FUNC(iload_3)
{
	return FALSE;
}

DECL_FUNC(lload_0)
{
	return FALSE;
}

DECL_FUNC(lload_1)
{
	return FALSE;
}

DECL_FUNC(lload_2)
{
	return FALSE;
}

DECL_FUNC(lload_3)
{
	return FALSE;
}

DECL_FUNC(fload_0)
{
	return FALSE;
}

DECL_FUNC(fload_1)
{
	return FALSE;
}

DECL_FUNC(fload_2)
{
	return FALSE;
}

DECL_FUNC(fload_3)
{
	return FALSE;
}

DECL_FUNC(dload_0)
{
	return FALSE;
}

DECL_FUNC(dload_1)
{
	return FALSE;
}

DECL_FUNC(dload_2)
{
	return FALSE;
}

DECL_FUNC(dload_3)
{
	return FALSE;
}

DECL_FUNC(aload_0)
{
	return FALSE;
}

DECL_FUNC(aload_1)
{
	return FALSE;
}

DECL_FUNC(aload_2)
{
	return FALSE;
}

DECL_FUNC(aload_3)
{
    printf("aload_3\n");
	return FALSE;
}

DECL_FUNC(iaload)
{
	return FALSE;
}

DECL_FUNC(laload)
{
	return FALSE;
}

DECL_FUNC(faload)
{
	return FALSE;
}

DECL_FUNC(daload)
{
	return FALSE;
}

DECL_FUNC(aaload)
{
	return FALSE;
}

DECL_FUNC(baload)
{
	return FALSE;
}

DECL_FUNC(caload)
{
	return FALSE;
}

DECL_FUNC(saload)
{
	return FALSE;
}

DECL_FUNC(istore)
{
    printf("istore\n");
	return FALSE;
}

DECL_FUNC(lstore)
{
	return FALSE;
}

DECL_FUNC(fstore)
{
	return FALSE;
}

DECL_FUNC(dstore)
{
	return FALSE;
}

DECL_FUNC(astore)
{
	return FALSE;
}

DECL_FUNC(istore_0)
{
	return FALSE;
}

DECL_FUNC(istore_1)
{
    validate_inst_env(param);

    Slot *slot = popOperandStack(opdStack);
    (localTbl->slots + 1)->tag = slot->tag;
    (localTbl->slots + 1)->value = slot->value;
    localTbl->validCnt = 2;

#ifdef DEBUG
    printf("\tistore_1\n");
#endif

	return TRUE;
}

DECL_FUNC(istore_2)
{
    validate_inst_env(param);

    Slot *slot = popOperandStack(opdStack);
    (localTbl->slots + 2)->tag = slot->tag;
    (localTbl->slots + 2)->value = slot->value;
    localTbl->validCnt = 3;

#ifdef DEBUG
    printf("\tistore_2\n");
#endif

	return TRUE;
}

DECL_FUNC(istore_3)
{
	return FALSE;
}

DECL_FUNC(lstore_0)
{
	return FALSE;
}

DECL_FUNC(lstore_1)
{
	return FALSE;
}

DECL_FUNC(lstore_2)
{
	return FALSE;
}

DECL_FUNC(lstore_3)
{
	return FALSE;
}

DECL_FUNC(fstore_0)
{
	return FALSE;
}

DECL_FUNC(fstore_1)
{
	return FALSE;
}

DECL_FUNC(fstore_2)
{
	return FALSE;
}

DECL_FUNC(fstore_3)
{
	return FALSE;
}

DECL_FUNC(dstore_0)
{
	return FALSE;
}

DECL_FUNC(dstore_1)
{
	return FALSE;
}

DECL_FUNC(dstore_2)
{
	return FALSE;
}

DECL_FUNC(dstore_3)
{
	return FALSE;
}

DECL_FUNC(astore_0)
{
	return FALSE;
}

DECL_FUNC(astore_1)
{
	return FALSE;
}

DECL_FUNC(astore_2)
{
	return FALSE;
}

DECL_FUNC(astore_3)
{
    printf("astore_3\n");
	return FALSE;
}

DECL_FUNC(iastore)
{
	return FALSE;
}

DECL_FUNC(lastore)
{
	return FALSE;
}

DECL_FUNC(fastore)
{
	return FALSE;
}

DECL_FUNC(dastore)
{
	return FALSE;
}

DECL_FUNC(aastore)
{
	return FALSE;
}

DECL_FUNC(bastore)
{
	return FALSE;
}

DECL_FUNC(castore)
{
	return FALSE;
}

DECL_FUNC(sastore)
{
	return FALSE;
}

DECL_FUNC(pop)
{
	return FALSE;
}

DECL_FUNC(pop2)
{
	return FALSE;
}

DECL_FUNC(dup)
{
    printf("dup\n");
	return FALSE;
}

DECL_FUNC(dup_x1)
{
	return FALSE;
}

DECL_FUNC(dup_x2)
{
	return FALSE;
}

DECL_FUNC(dup2)
{
	return FALSE;
}

DECL_FUNC(dup2_x1)
{
	return FALSE;
}

DECL_FUNC(dup2_x2)
{
	return FALSE;
}

DECL_FUNC(swap)
{
	return FALSE;
}

DECL_FUNC(iadd)
{
	return FALSE;
}

DECL_FUNC(ladd)
{
	return FALSE;
}

DECL_FUNC(fadd)
{
	return FALSE;
}

DECL_FUNC(dadd)
{
	return FALSE;
}

DECL_FUNC(isub)
{
	return FALSE;
}

DECL_FUNC(lsub)
{
	return FALSE;
}

DECL_FUNC(fsub)
{
	return FALSE;
}

DECL_FUNC(dsub)
{
	return FALSE;
}

DECL_FUNC(imul)
{
	return FALSE;
}

DECL_FUNC(lmul)
{
	return FALSE;
}

DECL_FUNC(fmul)
{
	return FALSE;
}

DECL_FUNC(dmul)
{
	return FALSE;
}

DECL_FUNC(idiv)
{
	return FALSE;
}

DECL_FUNC(ldiv)
{
	return FALSE;
}

DECL_FUNC(fdiv)
{
	return FALSE;
}

DECL_FUNC(ddiv)
{
	return FALSE;
}

DECL_FUNC(irem)
{
	return FALSE;
}

DECL_FUNC(lrem)
{
	return FALSE;
}

DECL_FUNC(frem)
{
	return FALSE;
}

DECL_FUNC(drem)
{
	return FALSE;
}

DECL_FUNC(ineg)
{
	return FALSE;
}

DECL_FUNC(lneg)
{
	return FALSE;
}

DECL_FUNC(fneg)
{
	return FALSE;
}

DECL_FUNC(dneg)
{
	return FALSE;
}

DECL_FUNC(ishl)
{
	return FALSE;
}

DECL_FUNC(lshl)
{
	return FALSE;
}

DECL_FUNC(ishr)
{
	return FALSE;
}

DECL_FUNC(lshr)
{
	return FALSE;
}

DECL_FUNC(iushr)
{
	return FALSE;
}

DECL_FUNC(lushr)
{
	return FALSE;
}

DECL_FUNC(iand)
{
	return FALSE;
}

DECL_FUNC(land)
{
	return FALSE;
}

DECL_FUNC(ior)
{
	return FALSE;
}

DECL_FUNC(lor)
{
	return FALSE;
}

DECL_FUNC(ixor)
{
	return FALSE;
}

DECL_FUNC(lxor)
{
	return FALSE;
}

DECL_FUNC(iinc)
{
	return FALSE;
}

DECL_FUNC(i2l)
{
	return FALSE;
}

DECL_FUNC(i2f)
{
	return FALSE;
}

DECL_FUNC(i2d)
{
	return FALSE;
}

DECL_FUNC(l2i)
{
	return FALSE;
}

DECL_FUNC(l2f)
{
	return FALSE;
}

DECL_FUNC(l2d)
{
	return FALSE;
}

DECL_FUNC(f2i)
{
	return FALSE;
}

DECL_FUNC(f2l)
{
	return FALSE;
}

DECL_FUNC(f2d)
{
	return FALSE;
}

DECL_FUNC(d2i)
{
	return FALSE;
}

DECL_FUNC(d2l)
{
	return FALSE;
}

DECL_FUNC(d2f)
{
	return FALSE;
}

DECL_FUNC(i2b)
{
	return FALSE;
}

DECL_FUNC(i2c)
{
	return FALSE;
}

DECL_FUNC(i2s)
{
	return FALSE;
}

DECL_FUNC(lcmp)
{
	return FALSE;
}

DECL_FUNC(fcmpl)
{
	return FALSE;
}

DECL_FUNC(fcmpg)
{
	return FALSE;
}

DECL_FUNC(dcmpl)
{
	return FALSE;
}

DECL_FUNC(dcmpg)
{
	return FALSE;
}

DECL_FUNC(ifeq)
{
	return FALSE;
}

DECL_FUNC(ifne)
{
	return FALSE;
}

DECL_FUNC(iflt)
{
	return FALSE;
}

DECL_FUNC(ifge)
{
	return FALSE;
}

DECL_FUNC(ifgt)
{
	return FALSE;
}

DECL_FUNC(ifle)
{
	return FALSE;
}

DECL_FUNC(if_icmpeq)
{
	return FALSE;
}

DECL_FUNC(if_icmpne)
{
	return FALSE;
}

DECL_FUNC(if_icmplt)
{
	return FALSE;
}

DECL_FUNC(if_icmpge)
{
	return FALSE;
}

DECL_FUNC(if_icmpgt)
{
	return FALSE;
}

DECL_FUNC(if_icmple)
{
	return FALSE;
}

DECL_FUNC(if_acmpeq)
{
	return FALSE;
}

DECL_FUNC(if_acmpne)
{
	return FALSE;
}

DECL_FUNC(_goto)
{
	return FALSE;
}

DECL_FUNC(jsr)
{
	return FALSE;
}

DECL_FUNC(ret)
{
	return FALSE;
}

DECL_FUNC(tableswitch)
{
	return FALSE;
}

DECL_FUNC(lookupswitch)
{
	return FALSE;
}

DECL_FUNC(ireturn)
{
	return FALSE;
}

DECL_FUNC(lreturn)
{
	return FALSE;
}

DECL_FUNC(freturn)
{
	return FALSE;
}

DECL_FUNC(dreturn)
{
	return FALSE;
}

DECL_FUNC(areturn)
{
	return FALSE;
}

DECL_FUNC(_return)
{
    printf("return\n");
	return FALSE;
}

DECL_FUNC(getstatic)
{
    validate_inst_env(param);

	U1 u2 = inst->operand.u2;
	ConstPoolEntry *constEntry = constPool->entries + u2;
	assert(NULL != constEntry);

    Slot slot;
    initSlot(&slot, constPool, constEntry);
	assert(slot.tag == constEntry->tag);

	// Please reference here:
	// class.c line:1742
#if 0
	printf("slot value:%s\n", (char *)slot.value);
#endif

#ifdef TIME_COST_LOG
	uint64_t t1 = current_ms();
#endif

	// retrieve the class named slot.value
	Class *cls = findClass((char *)slot.value, env);
	assert (NULL != cls);

#ifdef TIME_COST_LOG
	uint64_t t2 = current_ms();
	printf("find %s cost %lu ms.\n", (char *)slot.value, t2 - t1);
#endif

	bool status = linkClass(cls, env);
	assert (status);

	status = resolveClass(cls);
	assert (status);

	status = initializeClass(cls, env);
	assert (status);

    bool result = pushOperandStack(opdStack, &slot);
    assert(result);

#ifdef DEBUG
    printf("\tgetstatic %d // ", u2);
	logConstPoolEntry(constPool, constEntry);
#endif

	return TRUE;
}

DECL_FUNC(putstatic)
{
	return FALSE;
}

DECL_FUNC(getfield)
{
	return FALSE;
}

DECL_FUNC(putfield)
{
	return FALSE;
}

DECL_FUNC(invokevirtual)
{
	validate_inst_env(param);

	U1 u2 = inst->operand.u2;
	ConstPoolEntry *constEntry = constPool->entries + u2;
	assert(NULL != constEntry);

    Slot slot;
    initSlot(&slot, constPool, constEntry);
	assert(slot.tag == constEntry->tag);

#ifdef DEBUG
    printf("\tinvokevirtual %d // ", u2);
	logConstPoolEntry(constPool, constEntry);
#endif

	return FALSE;
}

DECL_FUNC(invokespecial)
{
    printf("invokespecial\n");
	return FALSE;
}

DECL_FUNC(invokestatic)
{
	return FALSE;
}

DECL_FUNC(invokeinterface)
{
	return FALSE;
}

DECL_FUNC(invokedynamic)
{
	return FALSE;
}

DECL_FUNC(_new)
{
    validate_inst_env(param);

	U1 u2 = inst->operand.u2;
    printf("new %d\n", u2);

	printf(" Not implemented.\n");

	return FALSE;
}

DECL_FUNC(newarray)
{
	return FALSE;
}

DECL_FUNC(anewarray)
{
	return FALSE;
}

DECL_FUNC(arraylength)
{
	return FALSE;
}

DECL_FUNC(athrow)
{
	return FALSE;
}

DECL_FUNC(checkcast)
{
	return FALSE;
}

DECL_FUNC(instanceof)
{
	return FALSE;
}

DECL_FUNC(monitorenter)
{
	return FALSE;
}

DECL_FUNC(monitorexit)
{
	return FALSE;
}

DECL_FUNC(wide)
{
	return FALSE;
}

DECL_FUNC(multianewarray)
{
	return FALSE;
}

DECL_FUNC(ifnull)
{
	return FALSE;
}

DECL_FUNC(ifnonnull)
{
	return FALSE;
}

DECL_FUNC(goto_w)
{
	return FALSE;
}

DECL_FUNC(jsr_w)
{
	return FALSE;
}
