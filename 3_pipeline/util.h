/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Basic Information */
#define MEM_TEXT_START  0x00400000
#define MEM_TEXT_SIZE   0x00100000
#define MEM_DATA_START  0x10000000
#define MEM_DATA_SIZE   0x00100000
#define MIPS_REGS       32
#define BYTES_PER_WORD  4
#define PIPE_STAGE      5

typedef struct IF_ID_Struct {
  instruction INSTR;

  uint32_t NPC;
} IF_ID;

typedef struct ID_EX_Struct {
  uint32_t NPC;

  uint32_t REG1;    // Register value 1
  uint32_t REG2;    // Register value 2

  char RS;
  char RD;
  char RT;
  uint32_t IMM;     // Immediate Field
  char SHAMT;
  char DEST;

  char ALUOp;
  // 1: add (ADDIU, ADDU, LW, SW)
  // 2: sub (SUBU, BEQ, BNE)
  // 3: AND (AND, ANDI)
  // 4: OR (OR, ORI, NOR)
  // 5: Shift Left (SLL)
  // 6: Shift Right (SRL)
  // 7: Set Less Than (SLTIU, SLTU)
  // 8: Shift Left 16 (LUI)
  // 9: JAL (JAL)
  char ALUSrc; // Second Operand from Immediate -> 1
  char ALUNeg; // NOR or BNE -> 1

  char Branch; // branch -> 1
  char MemRead;
  char MemWrite;

  char RegWrite;
  char MemtoReg;
  char LoadUse;
} ID_EX;


typedef struct EX_MEM_Struct {
  uint32_t MemValue;
  uint32_t DEST;
  uint32_t ALU_OUT;
  uint32_t BR_TARGET;

  char Branch;
  char MemRead;
  char MemWrite;

  char RegWrite;
  char MemtoReg;
} EX_MEM;

typedef struct MEM_WB_Struct {
  uint32_t PC;
  uint32_t DEST;
  uint32_t MEM_OUT;
  uint32_t ALU_OUT;

  char RegWrite;
  char MemtoReg;
} MEM_WB;


typedef struct inst_s {
    short opcode;

    /*R-type*/
    short func_code;

    union {
        /* R-type or I-type: */
        struct {
            unsigned char rs;
            unsigned char rt;

            union {
                short imm;

                struct {
                    unsigned char rd;
                    unsigned char shamt;
                } r;
            } r_i;
        } r_i;
        /* J-type: */
        uint32_t target;
    } r_t;

    uint32_t value;

    //int32 encoding;
    //imm_expr *expr;
    //char *source_line;
} instruction;

typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;

typedef struct CPU_State_Struct {
    uint32_t PC;                /* program counter */
    uint32_t REGS[MIPS_REGS];   /* register file */
    uint32_t PIPE[PIPE_STAGE];  /* pipeline stage */

    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;

    uint32_t STALL;       /* The number of needed stalls. Fetch when 0 */
    uint32_t FLUSH;
    char LoadUse;
} CPU_State;

/* For PC * Registers */
extern CPU_State CURRENT_STATE;

/* For Instructions */
extern instruction *INST_INFO;
extern int NUM_INST;

/* For Memory Regions */
extern mem_region_t MEM_REGIONS[2];

/* For Execution */
extern int RUN_BIT;     /* run bit */
extern int INSTRUCTION_COUNT;

/* Functions */
char**          str_split(char *a_str, const char a_delim);
int             fromBinary(char *s);
uint32_t        mem_read_32(uint32_t address);
void            mem_write_32(uint32_t address, uint32_t value);
void            cycle();
void            run(int num_cycles);
void            go();
void            mdump(int start, int stop);
void            rdump();
void            init_memory();
void            init_inst_info();

/* YOU IMPLEMENT THIS FUNCTION */
void    process_instruction();

#endif
