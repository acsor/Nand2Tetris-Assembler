#ifndef LEXER_H
#define	LEXER_H


#define SYM_EQ		'='
#define SYM_SEMIC	';'

#define	DEST_NONE 0
#define	DEST_M 1
#define	DEST_D 2
#define	DEST_MD 3
#define	DEST_A 4
#define	DEST_AM 5
#define	DEST_AD 6
#define	DEST_AMD 7

#define JUMP_NONE 0
#define JUMP_GT 1
#define JUMP_EQ 2
#define JUMP_GE 3
#define JUMP_LT 4
#define JUMP_NE 5
#define JUMP_LE 6
#define JUMP_ALWAYS 7

#define	TEXT_REPR_SIZE 64


// TO-DO Employ a data type which has a width of two bytes cross-platform.
typedef struct {
	unsigned short int bits;
	// Human-readable version of the instruction (e.g., as may be read from a
	// source file).
	char text_repr[TEXT_REPR_SIZE];
	// Lexer hint for whether this struct instance has been initialized
	// during the first parsing phase, or should be filled in later. (Mostly
	// useful for A-instructions.)
	short int loaded;
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
int n2t_instr_to_bitstr(instr const in, char *const dest);
/**
 * Instantiates an `instr' structure from `str_repr', containing its
 * human-readable textual representation.
 *
 * Returns: `1' if `str_repr' could not be converted into an `instr' type, `0'
 * otherwise.
 */
int n2t_str_to_instr(char const *str_repr, instr *dest);
/**
 * Sets the destination register(s) of a C-instruction computation (to choose
 * between `A', `D' or `M').
 *
 * Param `dest_reg': can assume either `DEST_A', `DEST_D' or `DEST_M' values,
 * or an or-ed expression amongst them.
 *
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
// TO-DO Test
int n2t_set_dest(instr *dest, int dest_reg);
/**
 * Retrieves the destinationn register(s) of a C-instruction computation.
 *
 * Returns: `DEST_M', `DEST_D', `DEST_MD', `DEST_A', ..., comprising any
 * possible combination of the three register.
 */
// TO-DO Test
int n2t_get_dest(instr *dest);
/**
 * Sets the jump condition of a C-instruction computation (to choose between
 * `JUMP_NONE' or `JUMP_GT', `JUMP_EQ', `JUMP_LT' or any or-ed expression
 * amongst them.
 *
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
// TO-DO Test
int n2t_set_jump(instr *dest, int jump_cond);
/**
 * Returns: `JUMP_NONE', `JUMP_GT', `JUMP_EQ', `JUMP_GE', ..., or any other
 * combination up to `JUMP_ALWAYS'.
 */
// TO-DO Test
int n2t_get_jump(instr *dest);
/**
 * Returns: the instruction type associated with `in'.
 */
instr_type n2t_instr_type(instr const in);


#endif
