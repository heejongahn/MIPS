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

/**************************************************************/
/*                                                            */
/* Procedure : parsing_instr                                  */
/*                                                            */
/* Purpose   : parse binary format instrction and return the  */
/*             instrcution data                               */
/*                                                            */
/* Argument  : buffer - binary string for the instruction     */
/*             index  - order of the instruction              */
/*                                                            */
/**************************************************************/
instruction parsing_instr(const char *buffer, const int index) {
    instruction instr = {0, };

    /* Your code */

    return instr;
}

/**************************************************************/
/*                                                            */
/* Procedure : parsing_data                                   */
/*                                                            */
/* Purpose   : parse binary format data and store data into   */
/*             the data region                                */
/*                                                            */
/* Argument  : buffer - binary string for data                */
/*             index  - order of data                         */
/*                                                            */
/**************************************************************/
void parsing_data(const char *buffer, const int index) {

    /* Your code */

}
