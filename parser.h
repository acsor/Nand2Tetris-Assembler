// MIT License
// 
// Copyright (c) 2018 Oscar
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"


#define RAMVAR_R0	0
#define RAMVAR_R1 	1
#define RAMVAR_R2 	2
#define RAMVAR_R3 	3
#define RAMVAR_R4 	4
#define RAMVAR_R5 	5
#define RAMVAR_R6 	6
#define RAMVAR_R7 	7
#define RAMVAR_R8 	8
#define RAMVAR_R9 	9
#define RAMVAR_R10	10
#define RAMVAR_R11	11
#define RAMVAR_R12	12
#define RAMVAR_R13	13
#define RAMVAR_R14	14
#define RAMVAR_R15	15

#define RAMVAR_SCREEN	16384
#define RAMVAR_KBD		24576
#define RAMVAR_SP		0
#define RAMVAR_LCL		1
#define RAMVAR_ARG		2
#define RAMVAR_THIS		3
#define RAMVAR_THAT		4

/**
 * Parses the contents in `filepath' to produce a fully filled-out sequence
 * of tokens.
 *
 * Returns: a list of tokens encoding the information of an `.asm' file, or
 * `NULL' if an error occurs.
 */
tokenseq_t* n2t_parse(char const *filepath);


#endif
