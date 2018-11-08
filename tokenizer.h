#ifndef TOKENIZER_H
#define	TOKENIZER_H


// TO-DO Employ a data type which has a width of two bytes cross-platform.
typedef struct {
	unsigned short int bits;
} instr;

typedef enum {
	A = 0, C
} instr_type;


/**
 * Converts an instruction `in' in its bit string representation.
 *
 * Param `*dest': destination string to store the result in. NOTE: its length
 * is assumed to be at least 17 bytes.
 * Returns: the total number of characters written to `dest', `-1' if an error
 * occurred.
 */
int asm_instr_to_bitstr(instr const in, char *const dest);
/**
 * Returns: the instruction type associated with `in'.
 */
instr_type asm_instr_type(instr const in);


#endif
