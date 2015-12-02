/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.h                                                     */
/*                                                             */
/***************************************************************/

#ifndef _RUN_H_
#define _RUN_H_

#include <stdio.h>

#include "util.h"

#define OPCODE(INST)		(INST)->opcode
#define SET_OPCODE(INST, VAL)	(INST)->opcode = (short)(VAL)

#define FUNC(INST)		(INST)->func_code
#define SET_FUNC(INST, VAL)	(INST)->func_code = (short)(VAL)

#define RS(INST)		(INST)->r_t.r_i.rs
#define SET_RS(INST, VAL)	(INST)->r_t.r_i.rs = (unsigned char)(VAL)

#define RT(INST)		(INST)->r_t.r_i.rt
#define SET_RT(INST, VAL)	(INST)->r_t.r_i.rt = (unsigned char)(VAL)

#define RD(INST)		(INST)->r_t.r_i.r_i.r.rd
#define SET_RD(INST, VAL)	(INST)->r_t.r_i.r_i.r.rd = (unsigned char)(VAL)

#define FS(INST)		RD(INST)
#define SET_FS(INST, VAL)	SET_RD(INST, VAL)
 
#define FT(INST)		RT(INST)
#define SET_FT(INST, VAL)	SET_RT(INST, VAL)

#define FD(INST)		SHAMT(INST)
#define SET_FD(INST, VAL)	SET_SHAMT(INST, VAL)

#define SHAMT(INST)		(INST)->r_t.r_i.r_i.r.shamt
#define SET_SHAMT(INST, VAL)	(INST)->r_t.r_i.r_i.r.shamt = (unsigned char)(VAL)

#define IMM(INST)		(INST)->r_t.r_i.r_i.imm
#define SET_IMM(INST, VAL)	(INST)->r_t.r_i.r_i.imm = (short)(VAL)

#define BASE(INST)		RS(INST)
#define SET_BASE(INST, VAL)	SET_RS(INST, VAL)

#define IOFFSET(INST)		IMM(INST)
#define SET_IOFFSET(INST, VAL)	SET_IMM(INST, VAL)
#define IDISP(INST)		(SIGN_EX (IOFFSET (INST) << 2))

#define COND(INST)		RS(INST)
#define SET_COND(INST, VAL)	SET_RS(INST, VAL)

#define CC(INST)		(RT(INST) >> 2)
#define ND(INST)		((RT(INST) & 0x2) >> 1)
#define TF(INST)		(RT(INST) & 0x1)

#define TARGET(INST)		(INST)->r_t.target
#define SET_TARGET(INST, VAL)	(INST)->r_t.target = (mem_addr)(VAL)

#define ENCODING(INST)		(INST)->encoding
#define SET_ENCODING(INST, VAL)	(INST)->encoding = (int32)(VAL)

#define EXPR(INST)		(INST)->expr
#define SET_EXPR(INST, VAL)	(INST)->expr = (imm_expr*)(VAL)

#define SOURCE(INST)		(INST)->source_line
#define SET_SOURCE(INST, VAL)	(INST)->source_line = (char *)(VAL)

/* Sign Extension */
#define SIGN_EX(X) (((X) & 0x8000) ? ((X) | 0xffff0000) : (X))

#define COND_UN		0x1
#define COND_EQ		0x2
#define COND_LT		0x4
#define COND_IN		0x8

/* Minimum and maximum values that fit in instruction's imm field */
#define IMM_MIN		0xffff8000
#define IMM_MAX 	0x00007fff

#define UIMM_MIN  	(unsigned)0
#define UIMM_MAX  	((unsigned)((1<<16)-1))

#define BRANCH_INST(TEST, TARGET, NULLIFY)	\
{						\
    if (TEST)					\
    {						\
	uint32_t target = (TARGET);		\
	JUMP_INST(target)			\
    }						\
}

#define JUMP_INST(TARGET)			\
{						\
    CURRENT_STATE.PC = (TARGET);		\
}

#define LOAD_INST(DEST_A, LD, MASK)		\
{						\
    LOAD_INST_BASE (DEST_A, (LD & (MASK)))	\
}

#define LOAD_INST_BASE(DEST_A, VALUE)		\
{						\
    *(DEST_A) = (VALUE);			\
}

/* functions */
instruction*	get_inst_info(uint32_t pc);
void		process_instruction();

#endif
