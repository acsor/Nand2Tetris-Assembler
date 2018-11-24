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
#ifndef LEXER_H
#define	LEXER_H

#include "utils.h"
#include "memcache.h"
#include <stdlib.h>
#include <stdint.h>


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

#define	COMP_0 (32 + 8 + 2)
#define	COMP_1 (63)
#define	COMP_MINUS1 (32 + 16 + 8 + 2)
#define	COMP_D (8 + 4)
#define	COMP_A (32 + 16)
#define	COMP_NOTD (8 + 4 + 1)
#define	COMP_NOTA (32 + 16 + 1)
#define	COMP_MINUSD (8 + 4 + 2 + 1)
#define	COMP_MINUSA (32 + 16 + 2 + 1)
#define	COMP_DPLUS1 (16 + 8 + 4 + 2 + 1)
#define	COMP_APLUS1 (32 + 16 + 4 + 2 + 1)
#define	COMP_DMINUS1 (8 + 4 + 2)
#define	COMP_AMINUS1 (32 + 16 + 2)
#define	COMP_DPLUSA (2)
#define	COMP_DMINUSA (16 + 2 + 1)
#define	COMP_AMINUSD (4 + 2 + 1)
#define	COMP_DANDA (0)
#define	COMP_DORA (16 + 4 + 1)
#define	COMP_M (64 + 32 + 16)
#define	COMP_NOTM (64 + 32 + 16 + 1)
#define	COMP_MINUSM (64 + 32 + 16 + 2 + 1)
#define	COMP_MPLUS1 (64 + 32 + 16 + 8 + 4 + 2)
#define	COMP_MMINUS1 (64 + 32 + 16 + 2)
#define	COMP_DPLUSM (64 + 2)
#define	COMP_DMINUSM (64 + 16 + 2 + 1)
#define	COMP_MMINUSD (64 + 4 + 2 + 1)
#define	COMP_DANDM (64)
#define	COMP_DORM (64 + 16 + 4 + 1)
// Used to signal errors on return values, e.g.
#define COMP_ERROR (64 + 32 + 16 + 8 + 4 + 2 + 1)

#define JUMP_NONE 0
#define JUMP_GT 1
#define JUMP_EQ 2
#define JUMP_GE 3
#define JUMP_LT 4
#define JUMP_NE 5
#define JUMP_LE 6
#define JUMP_ALWAYS 7


typedef enum {
	UNKNOWN = 0, ROM, RAM
} memtype_t;

#define LABEL_CHARSET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.$_0123456789"
/**
 * `memloc_t' encodes a memory location (either from the ROM instruction
 * memory, or from the RAM) optionally assigning a name (a label) to it.
 *
 * The `loaded' field is a parser hint for whether this `memloc_t' instance
 * needs further processing during the parsing phase, or is already done.
 */
typedef struct {
	uint16_t location;
	char label[BUFFSIZE_MED];
	uint8_t loaded;
	memtype_t type;
} memloc_t;


// A word in the Hack architecture is 16 bits, that we assing here once and for
// all.
typedef uint16_t word_t;

typedef enum {
	A = 0, C
} instr_type_t;

// An A-instruction holds a reference to a memory location -- whether in ROM or
// RAM. Instead of storing the instruction bits directly in the structure, we
// compute them only when needed, memorizing the minimum necessary.
//
// `memptr' references that memory location that will be later used to
// compue the 16-bit representation of the instruction.
typedef struct {
	memloc_t memptr;
} Ainstr_t;

typedef word_t Cinstr_t;

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
} token_type_t;

typedef struct {
	union {
		instr_t instr;
		memloc_t label;
	} data;
	token_type_t type;
} token_t;

/**
 * `tokenseq_t' needs some explanation: the `tokens' variable stores ADDRESSES
 * to `token_t' instances stored elsewhere, namely in `tokens_multiton'. This
 * is not only to avoid greater memory loads, but to facilitate the parsing
 * phase in which the parser only has to update objects' info only once, and
 * see the change propagate to all the other copies stored in `tokens'.
 */
typedef struct {
	uint32_t *tokens;
	// Index of the next `token_t' to be written.
	uint32_t next;
	uint32_t ntokens;

	memcache_t *tokens_multiton;
} tokenseq_t;


/**
 * Instantiates an `instr_t' structure from `str_repr', containing its
 * human-readable textual representation.
 *
 * Returns: `1' if `str_repr' could not be converted into an `instr_t' type,
 * `0' otherwise.
 */
int n2t_str_to_instr(char const *str_repr, instr_t *dest);
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
 *
 * Returns: the return value of either `n2t_Ainstr_to_str()' or
 * `n2t_Cinstr_to_str()', or `-1' if an error occurs before delegating the
 * call.
 */
int n2t_instr_to_str(instr_t const in, char *const dest, size_t maxwrite);

#define	AINSTR_ERROR (1 << 15)
/**
 * Param `norm_repr': a normalized representation for the A instruction.
 * Param `dest': `Ainstr_t' variable on which to store the decoded instruction.
 *
 * Returns: `1' if an error occurs, `0' otherwise.
 */
int n2t_str_to_Ainstr(char const *norm_repr, Ainstr_t *dest);
/**
 * Converts an A-instruction `in' in its string representation.
 *
 * Returns: `1' if an error occurs, `0' otherwise.
 */
int n2t_Ainstr_to_str(Ainstr_t const in, char *const dest, size_t maxwrite);
/**
 * Returns: a bitvector representing the A instruction `in' or `AINSTR_ERROR'
 * if an error occurs.
 */
word_t n2t_Ainstr_bits(Ainstr_t const in);

/**
 * Param `norm_repr': a normalized representation for the C instruction.
 * Param `dest': `instr_t' variable on which to store the decoded instruction.
 *
 * Returns: `1' if an error occurs parsing the `dest' portion, `2' parsing the
 * `comp' portion and `3' if parsing the `jump' portion, `0' otherwise.
 */
int n2t_str_to_Cinstr(char const *const norm_repr, Cinstr_t *dest);
/**
 * Converts a C-instruction `in' in its string representation. The output will
 * be deprived from whitespaces, what is called a "normalized" representation
 * (in this project).
 *
 * Returns: `0' if no errors occur, `1' if an error verifies while writing the
 * `dest' segment, `2' while writing the `comp' segment or `3' while writing
 * the `jump' segment.
 */
int n2t_Cinstr_to_str(Cinstr_t const in, char *const dest, size_t maxwrite);
/**
 * Sets the destination register(s) of a C-instruction computation (to choose
 * between `A', `D' or `M').
 *
 * Param `dest_reg': can assume either `DEST_A', `DEST_D' or `DEST_M' values,
 * or an or-ed expression amongst them.
 *
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
int n2t_set_dest(Cinstr_t *dest, word_t dest_reg);
/**
 * Retrieves the destinationn register(s) of a C-instruction computation.
 *
 * Returns: `DEST_M', `DEST_D', `DEST_MD', `DEST_A', ..., comprising any
 * possible combination of the three register.
 */
word_t n2t_get_dest(Cinstr_t const in);
/**
 * Sets the comp condition of a C-instruction (to choose between any available
 * expression, see the reference).
 * 
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
int n2t_set_comp(Cinstr_t *in, word_t comp_instr);
/**
 * Returns: the related `comp' code of a C-instruction.
 */
word_t n2t_get_comp(Cinstr_t const in);
/**
 * Sets the jump condition of a C-instruction computation (to choose between
 * `JUMP_NONE' or `JUMP_GT', `JUMP_EQ', `JUMP_LT' or any or-ed expression
 * amongst them.
 *
 * Returns: `1' if invalid arguments were supplied, `0' otherwise.
 */
int n2t_set_jump(Cinstr_t *dest, word_t jump_cond);
/**
 * Returns: `JUMP_NONE', `JUMP_GT', `JUMP_EQ', `JUMP_GE', ..., or any other
 * combination up to `JUMP_ALWAYS'.
 */
word_t n2t_get_jump(Cinstr_t const in);


/**
 * Sets a `memloc_t' variable from `str_repr', containing its human-readable
 * textual representation.
 *
 * Returns: `1' if `str_repr' could not be converted into an `memloc_t' type,
 * `0' otherwise.
 */
int n2t_str_to_label(char const *str_repr, memloc_t *dest);


#define DEFAULT_EXTEND_SIZE 64
/**
 * Allocates data in the heap storage for `n' `token_t' instances, returning
 * a `tokenseq_t' data type for management or `NULL' if an issue verifies.
 */
tokenseq_t* n2t_tokenseq_alloc(size_t n);
int n2t_tokenseq_append_token_index(tokenseq_t *s, uint32_t index);
int n2t_tokenseq_cache_token(tokenseq_t *s, token_t const t);
/**
 * Returns: a R/W pointer to a `token_t' variable located at index `index'
 * within the token sequence.
 */
token_t* n2t_tokenseq_index_get(tokenseq_t const *s, uint32_t index);
/**
 * Returns: the first `memloc_t' ROM label having the same label as `mould', or
 * `NULL' if none is found. Other fields are not compared.
 */
memloc_t* n2t_tokenseq_find_rom_label(tokenseq_t const *s, memloc_t mould);
/**
 * Comments and new lines are ignored.
 *
 * Param `filepath': a file path of an .asm file to tokenize.
 *
 * Returns: a `tokenseq_t' pointer, storing all the Hack-language tokens to be
 * interpreted, or `NULL' if a reading error occurs. The return value should be
 * later freed by a call to `n2t_tokenseq_free()'.
 */
tokenseq_t* n2t_tokenize(const char *filepath);
/**
 * Returns: `1' if `s' can not contain any more `token_t's, `0' otherwise.
 * Note that for a `tokenseq_t' variable `s', `s->next' points to the NEXT
 * `token_t' to be dealt with. Hence, with `20' elements and `s->next == 19',
 * `s' is NOT considered to be full (it will when `s->next >= 20').
 */
int n2t_tokenseq_full(tokenseq_t const *s);
/**
 * Extends the number of `token_t' values stored by `s' by an additional `n'.
 *
 * Returns: a pointer with the same value as `s' or `NULL' if an error
 * verifies.
 */
tokenseq_t* n2t_tokenseq_extend(tokenseq_t *s, size_t n);
/**
 * Frees up the dynamic data previously allocated for a `tokenseq_t' structure.
 */
void n2t_tokenseq_free(tokenseq_t *l);

#endif
