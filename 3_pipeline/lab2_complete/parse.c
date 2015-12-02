/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
    char opcode[7],rs[6],rt[6],rd[6],shamt[6],imm[17],target[27],func_code[7] = {0};
    int word;
    word = fromBinary(buffer);
    mem_write_32(MEM_TEXT_START + index, word);
    sscanf(buffer, "%6s", opcode);

    instr.value = word;
    instr.opcode = (short)fromBinary(opcode);

    switch(instr.opcode)
    {
	//Type I
	case 0x9:		//(0x001001)ADDIU
	case 0xc:		//(0x001100)ANDI
	case 0xf:		//(0x001111)LUI	
	case 0xd:		//(0x001101)ORI
	case 0xb:		//(0x001011)SLTIU
	case 0x23:		//(0x100011)LW	
	case 0x2b:		//(0x101011)SW
	case 0x4:		//(0x000100)BEQ
	case 0x5:		//(0x000101)BNE
	    sscanf(buffer,"%6s%5s%5s%16s",opcode,rs,rt,imm);
	    instr.r_t.r_i.rs = (unsigned char)fromBinary(rs);
	    instr.r_t.r_i.rt = (unsigned char)fromBinary(rt);
	    instr.r_t.r_i.r_i.imm = (short)fromBinary(imm);
	    break;

	    //TYPE R
	case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
	    sscanf(buffer,"%6s%5s%5s%5s%5s%6s",opcode,rs,rt,rd,shamt,func_code);
	    instr.func_code = (short)fromBinary(func_code);

	    //JR exception
	    if(instr.func_code == 0x8)
		instr.r_t.r_i.rs = (unsigned char)fromBinary(rs);
	    else
	    {
		instr.r_t.r_i.rs = (unsigned char)fromBinary(rs);
		instr.r_t.r_i.rt = (unsigned char)fromBinary(rt);
		instr.r_t.r_i.r_i.r.rd = (unsigned char)fromBinary(rd);
		instr.r_t.r_i.r_i.r.shamt = (unsigned char)fromBinary(shamt);
	    }
	    break;

	    //TYPE J
	case 0x2:		//(0x000010)J
	case 0x3:		//(0x000011)JAL
	    sscanf(buffer,"%6s%26s",opcode,target);
	    instr.r_t.target = fromBinary(target);
	    break;

	default:
	    printf("Not available instruction\n");
	    assert(0);
    }

    return instr;
}

void parsing_data(const char *buffer, const int index)
{
    uint32_t word;
    word = fromBinary(buffer);
    mem_write_32(MEM_DATA_START + index, word);
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
	printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
	printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	switch(INST_INFO[i].opcode)
	{
	    //Type I
	    case 0x9:		//(0x001001)ADDIU
	    case 0xc:		//(0x001100)ANDI
	    case 0xf:		//(0x001111)LUI	
	    case 0xd:		//(0x001101)ORI
	    case 0xb:		//(0x001011)SLTIU
	    case 0x23:		//(0x100011)LW	
	    case 0x2b:		//(0x101011)SW
	    case 0x4:		//(0x000100)BEQ
	    case 0x5:		//(0x000101)BNE
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
		break;

    	    //TYPE R
	    case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
		printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
		printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
		break;

    	    //TYPE J
	    case 0x2:		//(0x000010)J
	    case 0x3:		//(0x000011)JAL
		printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
		break;

	    default:
		printf("Not available instruction\n");
		assert(0);
	}
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
	printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
	printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
