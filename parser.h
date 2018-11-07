#ifndef PARSER_H
#define	PARSER_H

// TO-DO Employ a data type which has a width of two bytes cross-platform.
typedef struct {
	unsigned short int address;
} a_instruction;

typedef struct {
	unsigned short int bits;
} c_instruction;

#endif
