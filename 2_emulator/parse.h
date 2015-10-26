/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.h                                                   */
/*                                                             */
/***************************************************************/

#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>

#include "util.h"

extern int text_size;
extern int data_size;

/* functions */
instruction	parsing_instr(const char *buffer, const int index);
void		parsing_data(const char *buffer, const int index);

#endif
