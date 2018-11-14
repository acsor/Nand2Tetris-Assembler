#ifndef LEXER_H
#define	LEXER_H

#include "utils.h"
#include <stdlib.h>


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

typedef enum {
	A = 0, C
} instr_type_t;

typedef struct {
	short int bits;
	// Name of the label, if any, that this A instruction was initially read
	// with.
	char label[BUFFSIZE_MED];
	// Lexer hint for whether this struct instance has been initialized
	// during the first parsing phase, or should be filled in later.
	short int loaded;
} Ainstr_t;

// TO-DO Make sure to employ a data type which has a width of two bytes
// cross-platform.
typedef short int Cinstr_t;

// Unfortunately, I can't say how space-efficient such a solution can be.
typedef struct {
	union {
		Ainstr_t a;
		Cinstr_t c;
	} instr;
	instr_type_t type;
} instr_t;


typedef enum {
	INSTR = 0, LABEL
} token_type;

typedef struct {
	char text_repr[BUFFSIZE_MED];
	// The memory location which this label is a symbol for.
	unsigned short int location;
	// Whether the `location' value of this instance has been set.
	short int loaded;
} label_t;

typedef struct {
	union {
		instr_t instr;
		label_t label;
	} data;
	token_type type;
} token_t;

typedef struct {
	token_t *tokens;
	// Index of the next `token_t' to be written.
	size_t last;
	size_t ntokens;
} tokenseq_t;


/**
 * Converts an instruction `in' in its bit string representation.
 *
 * Param `*dest': destination string to store the result in. NOTE: its length
 * is assumed to be at least 17 bytes.
 * Returns: the total number of characters written to `dest', `-1' if an error
 * occurred.
 */
int n2t_instr_to_bitstr(instr_t const in, char *const dest);
/**
 * Converts an instruction `in' in its string representation.
 */
int n2t_instr_to_str(instr_t const in, char *const dest);
/**
 * Converts an A-instruction `in' in its string representation.
 */
int n2t_Ainstr_to_str(instr_t const in, char *const dest);
/**
 * Converts a C-instruction `in' in its string representation.
 */
int n2t_Cinstr_to_str(instr_t const in, char *const dest);
/**
 * Instantiates an `instr_t' structure from `str_repr', containing its
 * human-readable textual representation.
 *
 * Returns: `1' if `str_repr' could not be converted into an `instr_t' type,
 * `0' otherwise.
 */
int n2t_str_to_instr(char const *str_repr, instr_t *dest);
/**
 * Param `norm_repr': a normalized representation for the A instruction.
 * Param `dest': `instr_t' variable on which to store the decoded instruction.
 *
 * Returns: `1' if an error occurs, `0' otherwise.
 */
int n2t_str_to_ainstr(char const *norm_repr, instr_t *dest);
/**
 * Param `norm_repr': a normalized representation for the C instruction.
 * Param `dest': `instr_t' variable on which to store the decoded instruction.
 *
 * Returns: `1' if an error occurs parsing the `dest' portion, `2' parsing the
 * `comp' portion and `3' if parsing the `jump' portion, `0' otherwise.
 */
int n2t_str_to_cinstr(char const *const norm_repr, instr_t *dest);
/**
 * Instantiates an `label_t' variable from `str_repr', containing its
 * human-readable textual representation.
 *
 * Returns: `1' if `str_repr' could not be converted into an `label_t' type,
 * `0' otherwise.
 */
int n2t_str_to_label(char const *str_repr, label_t *dest);
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
int n2t_set_dest(Cinstr_t *dest, int dest_reg);
/**
 * Retrieves the destinationn register(s) of a C-instruction computation.
 *
 * Returns: `DEST_M', `DEST_D', `DEST_MD', `DEST_A', ..., comprising any
 * possible combination of the three register.
 */
// TO-DO Test
int n2t_get_dest(Cinstr_t *in);
/**
 * Sets the jump condition of a C-instruction computation (to choose between
 * `JUMP_NONE' or `JUMP_GT', `JUMP_EQ', `JUMP_LT' or any or-ed expression
 * amongst them.
 *
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
// TO-DO Test
int n2t_set_jump(Cinstr_t *dest, int jump_cond);
/**
 * Returns: `JUMP_NONE', `JUMP_GT', `JUMP_EQ', `JUMP_GE', ..., or any other
 * combination up to `JUMP_ALWAYS'.
 */
// TO-DO Test
int n2t_get_jump(Cinstr_t *in);

/**
 * Allocates data in the heap storage for `n' `token_t' instances, returning
 * a `tokenseq_t' data type for management or `NULL' if an issue verifies.
 */
tokenseq_t* n2t_tokenseq_alloc(size_t n);
/**
 * Extends the number of `token_t' values stored by `s' by an additional `n'.
 *
 * Returns: a pointer with the same value as `s' or `NULL' if an error
 * verifies.
 */
tokenseq_t* n2t_tokenseq_extend(tokenseq_t *s, size_t n);
int n2t_tokenseq_append_instr(tokenseq_t *s, instr_t const in);
int n2t_tokenseq_append_label(tokenseq_t *s, label_t const l);
/**
 * Returns: `1' if `s' can not contain any more `token_t's, `0' otherwise.
 * Note that for a `tokenseq_t' variable `s', `s->last' points to the NEXT
 * `token_t' to be dealt with. Hence, with `20' elements and `s->last == 19',
 * `s' is NOT considered to be full (it will when `s->last >= 20').
 */
int n2t_tokenseq_full(tokenseq_t const *s);
/**
 * Frees up the dynamic data previously allocated for a `tokenseq_t' structure.
 */
void n2t_tokenseq_free(tokenseq_t *l);

/**
 * Comments and new lines are ignored.
 *
 * Param `filepath': a file path of an .asm file to tokenize.
 *
 * Returns: a `tokenseq_t' pointer, storing all the Hack-language tokens that
 * could be interpreted, or `NULL' if a reading error occurs. The return value
 * should be later freed by a call to `n2t_tokenseq_free()'.
 */
tokenseq_t* n2t_tokenize(const char *filepath);

#endif
