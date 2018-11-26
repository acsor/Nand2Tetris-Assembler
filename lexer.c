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
#include "lexer.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// A mapping from destination information (the `dest' fragment) to its
// normalized textual representation.
static char const *INDEX_TO_DEST[] = {
	"", "M", "D", "MD", "A", "AM", "AD", "AMD"
};
// A mapping from instruction codes (the `comp' fragment) to their normalized
// textual representation.
static char const *INDEX_TO_COMP[] = {
	"D&A",	//0
	"",
	"D+A",	//2
	"", "", "", "",
	"A-D",	//7
	"", "", "", "",
	"D",	//12
	"!D",	//13
	"D-1",	//14
	"-D",	//15
	"", "", "",
	"D-A",	//19
	"",
	"D|A",	//21
	"", "", "", "", "", "", "", "", "",
	"D+1",	//31
	"", "", "", "", "", "", "", "", "", "",
	"0",	//42
	"", "", "", "", "",
	"A",	//48
	"!A",	//49
	"A-1",	//50
	"-A",	//51
	"", "", "",
	"A+1",	//55
	"", "",
	"-1",	//58
	"", "", "", "",
	"1",	//58
	"D&M",	//59
	"",
	"D+M",	//61
	"", "", "", "",
	"M-D",	//66
	"", "", "", "", "", "", "", "", "", "", "",
	"D-M",	//83
	"",
	"D|M",	//85
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "",
	"M",	//112
	"!M",	//113
	"M-1",	//114
	"-M",	//115
	"", "", "",
	"M+1",	//119
	"", "", "", "", "", "", ""
};
// A mapping from jump information (the `jump' fragment) to its
// normalized textual representation.
static char const *INDEX_TO_JUMP[] = {
	"", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"
};

/**
 * Parses the computation portion of a C-instruction.
 *
 * Param `norm_repr': a normalized representation for this ALU instruction.
 * Returns: the integer value associated to the ALU instruction or `COMP_ERROR'
 * if no correct parsing could be performed.
 */
static word_t n2t_parse_Cinstr_comp(char const *norm_repr);


int n2t_str_to_instr(char const *str_repr, instr_t *dest) {
	char normalized[strlen(str_repr) + 1];

	if (n2t_strip(str_repr, normalized))
		return 1;

	if (normalized[0] == '@') {
		dest->type = A;
		return n2t_str_to_Ainstr(normalized, &dest->instr.a);
	} else {
		dest->type = C;
		return n2t_str_to_Cinstr(normalized, &dest->instr.c);
	}
}

int n2t_instr_to_bitstr(instr_t in, char *const dest) {
	word_t bitmask = 1 << 15,
		   i = 0;
	word_t const bits =
		(in.type == A) ? n2t_Ainstr_bits(in.instr.a): in.instr.c;

	while (bitmask > 0 && i < 16) {
		dest[i] = bits & bitmask ? '1': '0';

		bitmask >>= 1;
		i++;
	}

	dest[i] = '\0';

	return i + 1;
}

int n2t_instr_to_str(instr_t const in, char *const dest, size_t maxwrite) {
	if (in.type == A) {
		return n2t_Ainstr_to_str(in.instr.a, dest, maxwrite);
	} else if (in.type == C) {
		return n2t_Cinstr_to_str(in.instr.c, dest, maxwrite);
	} else {
		return -1;
	}
}


int n2t_str_to_Ainstr(char const *norm_repr, Ainstr_t *dest) {
	if (norm_repr[0] != '@') {
		return 1;	// Not an A-instruction.
	}

	if (n2t_is_numeric(norm_repr + 1)) {
		// `[1-9]\d*' digits.
		// TO-DO: even multi-digit figures beginning with `0' are allowed!
		dest->memptr.location = atoi(norm_repr + 1);
		dest->memptr.loaded = 1;
		snprintf(
			dest->memptr.label, BUFFSIZE_MED, "%u", dest->memptr.location
		);
		dest->memptr.type = RAM;
	} else if (
			!IS_IN(norm_repr[1], "0123456789") &&
			n2t_composed_of(norm_repr + 1, LABEL_CHARSET)
	) {
		// @R0, @R1, ..., @SP, @THIS, ..., @LABEL, @label, @...
		dest->memptr.loaded = 0;
		strncpy(dest->memptr.label, norm_repr + 1, BUFFSIZE_MED);
		dest->memptr.type = UNKNOWN;
	} else {
		return 1;
	}

	return 0;
}

int n2t_Ainstr_to_str(Ainstr_t const in, char *const dest, size_t maxwrite) {
	if (in.memptr.label[0] != '\0') {
		snprintf(dest, maxwrite, "@%s", in.memptr.label);
	} else {
		snprintf(dest, maxwrite, "@%d", in.memptr.location);
	}

	return 0;
}

word_t n2t_Ainstr_bits(Ainstr_t const in) {
	if (in.memptr.loaded) {
		// If bit 16 was set to 1:
		if (in.memptr.location & (1 << 15))
			return AINSTR_ERROR;

		return in.memptr.location;
	}
	
	return AINSTR_ERROR;
}


int n2t_str_to_Cinstr(char const *const norm_repr, Cinstr_t *dest) {
	char const
		*const dest_field_tail = index(norm_repr, SYM_EQ),
		*const jump_field_head = index(norm_repr, SYM_SEMIC);
	char comp_field[strlen(norm_repr) + 1];
	word_t comp_encoding;
	size_t dest_offset = 0;

	char parsed_dest[4] = "   ";
	size_t parsed_dest_index = 0;

	*dest = 0;

	// Parse the `dest' part.
	// If '=' = SYM_EQ was not found in `norm_repr', `dest_field_tail' will be
	// `NULL'.
	while (norm_repr + dest_offset < dest_field_tail) {
		if (!IS_SPACE(norm_repr[dest_offset])) {
			if (index(parsed_dest, norm_repr[dest_offset])) {
				return 1;	// We already parsed `*norm_repr'!
			}

			switch (norm_repr[dest_offset]) {
				case 'M':
					n2t_set_dest(dest, DEST_M);
					parsed_dest[parsed_dest_index] = 'M';
					break;
				case 'D':
					n2t_set_dest(dest, DEST_D);
					parsed_dest[parsed_dest_index] = 'D';
					break;
				case 'A':
					n2t_set_dest(dest, DEST_A);
					parsed_dest[parsed_dest_index] = 'A';
					break;
				default:
					return 1;
			}

			parsed_dest_index++;
		}

		dest_offset++;
	}

	// Parse the `jump' part.
	if (jump_field_head) {
		if (!strcmp(jump_field_head + 1, "JGT")) {
			n2t_set_jump(dest, JUMP_GT);
		} else if (!strcmp(jump_field_head + 1, "JEQ")) {
			n2t_set_jump(dest, JUMP_EQ);
		} else if (!strcmp(jump_field_head + 1, "JGE")) {
			n2t_set_jump(dest, JUMP_GE);
		} else if (!strcmp(jump_field_head + 1, "JLT")) {
			n2t_set_jump(dest, JUMP_LT);
		} else if (!strcmp(jump_field_head + 1, "JNE")) {
			n2t_set_jump(dest, JUMP_NE);
		} else if (!strcmp(jump_field_head + 1, "JLE")) {
			n2t_set_jump(dest, JUMP_LE);
		} else if (!strcmp(jump_field_head + 1, "JMP")) {
			n2t_set_jump(dest, JUMP_ALWAYS);
		} else {
			return 3;
		}
	}

	// Parse the `comp' part.
	if (dest_field_tail)
		strcpy(comp_field, dest_field_tail + 1);
	else
		strcpy(comp_field, norm_repr);

	if (jump_field_head)
		*index(comp_field, SYM_SEMIC) = '\0';

	n2t_strip(comp_field, comp_field);
	n2t_collapse_any(comp_field, " \t\n", comp_field);

	if ((comp_encoding = n2t_parse_Cinstr_comp(comp_field)) != COMP_ERROR) {
		n2t_set_comp(dest, comp_encoding);
	} else {
		return 2;
	}

	*dest |= (0x7 << 13);

	return 0;
}

int n2t_Cinstr_to_str(Cinstr_t const in, char *const dest, size_t maxwrite) {
	word_t const
		dest_reg = n2t_get_dest(in),
		comp = n2t_get_comp(in),
		jump = n2t_get_jump(in);
	
	dest[0] = '\0';

	// Write the dest part.
	if (dest_reg < 0 || DEST_AMD < dest_reg) {
		return 1;
	} else if (dest_reg) {	// `0 < dest_reg <= 7 = DEST_AMD'.
		strncat(dest, INDEX_TO_DEST[dest_reg], maxwrite);
		strncat(dest, "=", maxwrite);
	}

	// Write the comp part.
	if (comp < 0 || COMP_MPLUS1 < comp) {
		return 2;
	} else {
		strncat(dest, INDEX_TO_COMP[comp], maxwrite);
	}

	// Write the jump part.
	if (jump < 0 || JUMP_ALWAYS < jump) {
		return 3;
	} else if (jump) {	// `0 < jump <= 7 = JUMP_ALWAYS'.
		strncat(dest, ";", maxwrite);
		strncat(dest, INDEX_TO_JUMP[jump], maxwrite);
	}

	return 0;
}

int n2t_set_dest(Cinstr_t *dest, word_t dest_reg) {
	if (DEST_NONE <= dest_reg && dest_reg <= DEST_AMD) {
		*dest |= dest_reg << 3;

		return 0;
	}

	return 1;
}

word_t n2t_get_dest(Cinstr_t const in) {
	return (in & (0x7 << 3)) >> 3;
}

int n2t_set_comp(Cinstr_t *in, word_t comp_instr) {
	if (
		comp_instr < 0
		|| COMP_MPLUS1 < comp_instr
		|| !strcmp(INDEX_TO_COMP[comp_instr], "")
    ) {
		return 1;
	} else {
		*in |= comp_instr << 6;
		return 0;
	}
}

word_t n2t_get_comp(Cinstr_t const in) {
	return (in & (0x7F << 6)) >> 6;
}

int n2t_set_jump(Cinstr_t *dest, word_t jump_cond) {
	if (JUMP_NONE <= jump_cond && jump_cond <= JUMP_ALWAYS) {
		*dest |= jump_cond;

		return 0;
	} else {
		return 1;
	}
}

word_t n2t_get_jump(Cinstr_t const in) {
	return in & 0x7;
}


int n2t_str_to_label(char const *str_repr, memloc_t *dest) {
	size_t const len = strlen(str_repr);

	if (str_repr[0] != '(' || str_repr[len - 1] != ')')
		return 1;
	if (len - 2 >= BUFFSIZE_MED)	// Line too long.
		return 1;

	strncpy(dest->label, str_repr + 1, len - 2);
	dest->label[len - 2] = '\0';

	if (n2t_composed_of(dest->label, LABEL_CHARSET)) {
		dest->type = ROM;
		dest->loaded = 0;

		return 0;
	} else {
		dest->label[0] = '\0';

		return 1;
	}
}


// tokenseq_t
tokenseq_t* n2t_tokenseq_alloc(size_t n) {
	tokenseq_t *o;

	if (n <= 0)
		return NULL;

	o = malloc(sizeof(tokenseq_t));
	if (o == NULL)
		return NULL;

	o->tokens = calloc(n, sizeof(uint32_t));
	if (o->tokens == NULL) {
		free(o);
		return NULL;
	}

	o->ntokens = n;
	o->next = 0;

	o->tokens_multiton = n2t_memcache_alloc(n, sizeof(token_t));

	if (o->tokens_multiton == NULL) {
		free(o->tokens);
		free(o);

		return NULL;
	}

	return o;
}

int n2t_tokenseq_append_token_index(tokenseq_t *s, uint32_t index) {
	if (s == NULL)
		return 1;

	if (n2t_tokenseq_full(s)) {
		n2t_tokenseq_extend(s, BUFFSIZE_MED);
	}

	s->tokens[s->next] = index;
	s->next++;

	return 0;
}

int n2t_tokenseq_cache_token(tokenseq_t *s, token_t const t) {
	if (s == NULL)
		return 1;

	return n2t_memcache_store(s->tokens_multiton, &t, sizeof(token_t));
}

token_t* n2t_tokenseq_index_get(tokenseq_t const *s, uint32_t index) {
	if (s == NULL)
		return NULL;
	if (index >= s->next)
		return NULL;

	return n2t_memcache_index_fetch(
		s->tokens_multiton, s->tokens[index]
	);
}

memloc_t* n2t_tokenseq_find_rom_label(tokenseq_t const *s, memloc_t mould) {
	token_t *t;
	size_t i;

	for (i = 0; i < s->tokens_multiton->next; i++) {
		t = n2t_memcache_index_fetch(s->tokens_multiton, i);

		if (
			t->type == LABEL &&
			!strncmp(mould.label, t->data.label.label, BUFFSIZE_MED)
		)
			return &t->data.label;
	}

	return NULL;
}

tokenseq_t* n2t_tokenize(const char *filepath) {
	FILE *fin;
	char buff[BUFFSIZE_LARGE];

	tokenseq_t *seq;
	token_t t;
	int64_t cacheindex;

	memset(&t, 0, sizeof(token_t));

	if ((fin = fopen(filepath, "r")) == NULL) {
		return NULL;
	}

	if ((seq = n2t_tokenseq_alloc(BUFFSIZE_LARGE)) == NULL) {
		fclose(fin);
		return NULL;
	}
	
	while (fgets(buff, BUFFSIZE_LARGE, fin)) {
		n2t_decomment(buff, buff);
		n2t_strip(buff, buff);

		// If `buff' contains nothing after taking away comments and
		// whitespaces:
		if (buff[0] == '\0')
			continue;

		if (n2t_str_to_instr(buff, &t.data.instr) == 0) {
			t.type = INSTR;
		} else if (n2t_str_to_label(buff, &t.data.label) == 0) {
			t.type = LABEL;
		} else {
			// We couldn't parse in any possible way `buff'.
			fclose(fin);
			n2t_tokenseq_free(seq);

			return NULL;
		}

		cacheindex = n2t_memcache_index_of(
			seq->tokens_multiton, &t, sizeof(token_t)
		);
		// If `t' was already present in the multiton store:
		if (cacheindex >= 0) {
			n2t_tokenseq_append_token_index(seq, cacheindex);
		} else {
			n2t_tokenseq_cache_token(seq, t);
			n2t_tokenseq_append_token_index(
				seq,
				n2t_memcache_index_of(seq->tokens_multiton, &t, sizeof(token_t))
			);
		}

		memset(&t, 0, sizeof(token_t));
	}

	fclose(fin);

	return seq;
}

int n2t_tokenseq_full(tokenseq_t const *s) {
	return s->next >= s->ntokens;
}

tokenseq_t* n2t_tokenseq_extend(tokenseq_t *s, size_t n) {
	uint32_t *t;

	if (n > 0) {
		t = realloc(s->tokens, sizeof(uint32_t) * (s->ntokens + n));

		if (t == NULL) {
			return NULL;
		} else {
			s->tokens = t;
			s->ntokens += n;
		}
	}

	return s;
}

void n2t_tokenseq_free(tokenseq_t *l) {
	n2t_memcache_free(l->tokens_multiton);
	free(l->tokens);
	free(l);
}


static word_t n2t_parse_Cinstr_comp(char const *norm_repr) {
	size_t i;

	for (i = 0; i <= COMP_MPLUS1; i++) {
		if (strcmp(norm_repr, INDEX_TO_COMP[i]) == 0)
			return i;
	}

	return COMP_ERROR;
}
