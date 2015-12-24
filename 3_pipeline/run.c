/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: pipeline stages                                  */
/*                                                             */
/* Purpose: Do stuffs..                                        */
/*                                                             */
/***************************************************************/

IF_ID if_stage() {
    IF_ID next_latch;
    if (!CURRENT_STATE.PC) { // Stall
        memset(&next_latch, 0, sizeof(next_latch));
        return next_latch;
    }

    next_latch.NPC = CURRENT_STATE.PC;
    next_latch.INSTR = INST_INFO[(CURRENT_STATE.PC - MEM_TEXT_START) >> 2];
    return next_latch;
}

ID_EX id_stage() {
    IF_ID latch = CURRENT_STATE.if_id;
    ID_EX next_latch;

    next_latch.NPC = latch.NPC;

    instruction *inst = &latch.INSTR;
    short op = OPCODE (inst);
    short func = FUNC (inst);

    next_latch.REG1 = CURRENT_STATE.REGS[RS (inst)];
    next_latch.REG2 = CURRENT_STATE.REGS[RT (inst)];

    next_latch.RS = RS (inst);
    next_latch.RT = RT (inst);
    next_latch.RD = RD (inst);
    next_latch.IMM = IMM (inst);
    next_latch.SHAMT = SHAMT (inst);

    if (op == 0x0 || op == 0x3) {
      next_latch.DEST = RD (inst);
    }
    else {
      next_latch.DEST = RT (inst);
    }

    next_latch.ALUSrc = (op == 0x4 || op == 0x5 || op == 0x9 || op == 0xb ||
        op == 0xc || op == 0xd || op == 0xf || op == 0x23 || op == 0x2b);
    next_latch.ALUNeg = (op == 0x27 || op == 0x5);

    next_latch.Branch = (op == 0x4 || op == 0x5);
    next_latch.MemRead = (op == 0x23);
    next_latch.MemWrite = (op == 0x2b);

    next_latch.RegWrite = !(op == 0x2b || op == 0x4 || op == 0x05 ||
        op == 0x2 || (op == 0x0 && func == 0x8));
    next_latch.MemtoReg = (op == 0x23);

    switch (op) {
        case 0x0:
            switch(func) {
                case 0x21:  //ADDU
                    next_latch.ALUOp = 1;
                    break;
                case 0x24:  //AND
                    next_latch.ALUOp = 3;
                    break;
                case 0x27:  //NOR
                    next_latch.ALUOp = 4;
                    next_latch.ALUNeg = 1;
                    break;
                case 0x25:  //OR
                    next_latch.ALUOp = 4;
                    break;
                case 0x2B:  //SLTU
                    next_latch.ALUOp = 7;
                    break;
                case 0x0:   //SLL
                    next_latch.ALUOp = 5;
                    break;
                case 0x2:   //SRL
                    next_latch.ALUOp = 6;
                    break;
                case 0x23:  //SUBU
                    next_latch.ALUOp = 2;
                    break;

                case 0x8:   //JR
                    next_latch.ALUOp = 0;
                    CURRENT_STATE.PC = CURRENT_STATE.REGS[next_latch.RS];
                    break;

                default:
                    printf("Unknown function code type: %d\n", FUNC(inst));
                    break;
            }
            break;

        case 0x9:               //(0x001001)ADDIU
            next_latch.ALUOp = 1;
            break;

        case 0xc:               //(0x001100)ANDI
            next_latch.ALUOp = 3;
            break;

        case 0xf:               //(0x001111)LUI
            next_latch.ALUOp = 8;
            break;
        case 0xd:               //(0x001101)ORI
            next_latch.ALUOp = 4;
            break;
        case 0xb:               //(0x001011)SLTIU
            next_latch.ALUOp = 7;
            break;
        case 0x23:              //(0x100011)LW
            next_latch.ALUOp = 1;
            break;
        case 0x4:               //(0x000100)BEQ
        case 0x5:               //(0x000101)BNE
            next_latch.ALUOp = 2;

            break;

        case 0x2:               //(0x000010)J
            next_latch.ALUOp = 0;
            CURRENT_STATE.PC = ((next_latch.NPC & 0xf0000000) | (next_latch.IMM << 2));
            break;

        case 0x3:               //(0x000011)JAL
            next_latch.ALUOp = 9;
            CURRENT_STATE.REGS[31] = next_latch.NPC + 4; // TODO: 이거 여기서하는지 EX에서 하는지
            CURRENT_STATE.PC = ((next_latch.NPC & 0xf0000000) | (next_latch.IMM << 2));
            break;

        default:
            printf("Unknown instruction type: %d\n", OPCODE(inst));
            break;

    }
    return next_latch;
}

EX_MEM ex_stage(){
    ID_EX latch = CURRENT_STATE.id_ex;
    EX_MEM next_latch;

    // load-use

    EX_MEM ex_mem = CURRENT_STATE.ex_mem;

    if (ex_mem.MemRead) {
        if (latch.DEST == latch.RT) {
            if (latch.RS == ex_mem.DEST) {
                CURRENT_STATE.LoadUse = 1;
                return ex_mem;
            }
        }
        else {
            if ((latch.RS == ex_mem.DEST) || (latch.RT == ex_mem.DEST)) {
                CURRENT_STATE.LoadUse = 1;
                return ex_mem;
            }
        }
    }

    next_latch.DEST = latch.DEST;

    next_latch.Branch = latch.Branch;
    next_latch.MemRead = latch.MemRead;
    next_latch.MemWrite = latch.MemWrite;

    next_latch.RegWrite = latch.RegWrite;
    next_latch.MemtoReg = latch.MemtoReg;

    switch (latch.ALUOp) {
        case 1: // add
          if (latch.ALUSrc) {
              next_latch.ALU_OUT = latch.REG1 + latch.IMM;
          }
          else {
              next_latch.ALU_OUT = latch.REG1 + latch.REG2;
          }
          break;

        case 2: // sub
          next_latch.ALU_OUT = latch.REG1 - latch.REG2;
          if (latch.ALUNeg) {
              next_latch.ALU_OUT = !(next_latch.ALU_OUT); // bne
          }
          break;

        case 3: // and
          if (latch.ALUSrc) {
              next_latch.ALU_OUT = latch.REG1 & latch.IMM;
          }
          else {
              next_latch.ALU_OUT = latch.REG1 & latch.REG2;
          }
          break;

        case 4: // or
          if (latch.ALUSrc) {
              next_latch.ALU_OUT = latch.REG1 | latch.IMM;
          }
          else {
              next_latch.ALU_OUT = latch.REG1 | latch.REG2;
          }

          if (latch.ALUNeg) {
              next_latch.ALU_OUT = !(next_latch.ALU_OUT);
          }
          break;

        case 5: // shift left
          next_latch.ALU_OUT = latch.REG2 << latch.SHAMT;
          break;

        case 6: // shift right
          next_latch.ALU_OUT = latch.REG2 >> latch.SHAMT;
          break;

        case 7: // set less than
          if (latch.ALUSrc) {
              if (latch.REG1 < latch.IMM) {
                  next_latch.ALU_OUT = 1;
              }
              else {
                  next_latch.ALU_OUT = 0;
              }
          }
          else {
              if (latch.REG1 < latch.REG2) {
                  next_latch.ALU_OUT = 1;
              }
              else {
                  next_latch.ALU_OUT = 0;
              }
          }
          break;

        case 8: // lui
          next_latch.ALU_OUT = latch.IMM << 16;

          break;

        case 9: // jal
          break;

        default:
          break;
    }
    if (latch.MemWrite) {
        next_latch.MemValue = latch.REG2;
    }
}

MEM_WB mem_stage(){
    EX_MEM latch = CURRENT_STATE.ex_mem;
    MEM_WB next_latch;

    next_latch.RegWrite = latch.RegWrite;
    next_latch.MemtoReg = latch.MemtoReg;
    next_latch.DEST = latch.DEST;
    next_latch.ALU_OUT = latch.ALU_OUT;

    if (latch.MemWrite)
    {
        mem_write_32(latch.ALU_OUT, latch.MemValue);
    }
    else if (latch.MemRead)
    {
        next_latch.MEM_OUT = mem_read_32(latch.ALU_OUT);// Read from memory
    }

    return next_latch;

}

void wb_stage(){
    MEM_WB latch = CURRENT_STATE.mem_wb;

    if (latch.RegWrite)
    {
        CURRENT_STATE.REGS[latch.DEST] = latch.ALU_OUT;
    }
    else if (latch.MemtoReg)
    {
        CURRENT_STATE.REGS[latch.DEST] = latch.MEM_OUT;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
    instruction *inst;
    int i;              // for loop
    uint32_t dest;

    /* pipeline */
    for ( i = PIPE_STAGE - 1; i > 0; i--)
        CURRENT_STATE.PIPE[i] = CURRENT_STATE.PIPE[i-1];
    CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;

    inst = get_inst_info(CURRENT_STATE.PC);
    CURRENT_STATE.PC += BYTES_PER_WORD; // PC = nPC

    // WB Stage
    wb_stage();

    // MEM Stage
    CURRENT_STATE.mem_wb = mem_stage();

    if (!CURRENT_STATE.LoadUse) {
        CURRENT_STATE.LoadUse = 0;
        // EX Stage
        CURRENT_STATE.ex_mem = ex_stage();

        // ID Stage
        CURRENT_STATE.id_ex = id_stage();

        // IF Stage
        CURRENT_STATE.if_id = if_stage();
    }


    // Fetch Stall caused by Control Hazard
    if (CURRENT_STATE.STALL) {
        CURRENT_STATE.PC -= BYTES_PER_WORD; // PC = nPC 취소
        CURRENT_STATE.STALL -= 1;
    }

    // Flush
    if (CURRENT_STATE.FLUSH) {
        for (i=0; i<CURRENT_STATE.FLUSH; i++) {
            CURRENT_STATE.PIPE[i] = 0;
            switch (i) {
              case 1:
                memset(&CURRENT_STATE.if_id, 0, sizeof(CURRENT_STATE.if_id));
              case 2:
                memset(&CURRENT_STATE.id_ex, 0, sizeof(CURRENT_STATE.id_ex));
            }
        }
    }

    if (CURRENT_STATE.PC < MEM_REGIONS[0].start || CURRENT_STATE.PC >= (MEM_REGIONS[0].start + (NUM_INST * 4)))
        RUN_BIT = FALSE;
}
