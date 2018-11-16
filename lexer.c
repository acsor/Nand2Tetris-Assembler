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
	"!D",	//12
	"",
	"-D",	//14
	"", "", "",
	"D-A",	//18
	"",
	"D|A",	//20
	"", "", "", "", "", "",
	"D-1",	//28
	"", "",
	"D+1",	//31
	"", "", "", "", "", "", "", "", "", "",
	"0",	//42
	"", "", "", "", "",
	"A",	//48
	"!A",	//44
	"A-1",	//45
	"-A",	//46
	"", "", "",
	"A-1",	//50
	"", "",
	"-1",	//53
	"",
	"A+1",	//55
	"", "",
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
	"", "", "", "", "", "",
	"M",	//92
	"!M",	//93
	"M-1",	//94
	"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	"",
	"-M",	//115
	"", "", "", "", "", "", "", "", "", "",
	"M+1"	//126
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
 * Returns: the integer value associated to the ALU instruction or `-1' if no
 * correct parsing could be performed.
 */
static int n2t_parse_cinstr_comp(char const *norm_repr);


int n2t_instr_to_bitstr(instr_t const in, char *const dest) {
	int bitmask = 1 << 15, i = 0;
	short int const bits = (in.type == A) ? in.instr.a.bits: in.instr.c;

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

int n2t_Ainstr_to_str(Ainstr_t const in, char *const dest, size_t maxwrite) {
	if (in.label[0] != '\0') {
		// TO-DO snprintf() doesn't actually return the number of characters
		// written.
		return snprintf(dest, maxwrite, "@%s", in.label);
	} else {
		// TO-DO snprintf() doesn't actually return the number of characters
		// written.
		return snprintf(dest, maxwrite, "@%u", in.bits);
	}
}

int n2t_Cinstr_to_str(Cinstr_t const in, char *const dest, size_t maxwrite) {
	short int const
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

int n2t_str_to_instr(char const *str_repr, instr_t *dest) {
	char normalized[strlen(str_repr) + 1];

	if (n2t_strip(str_repr, normalized)) {
		return 1;
	}

	if (normalized[0] == '@') {
		return n2t_str_to_ainstr(normalized, dest);
	} else {
		return n2t_str_to_cinstr(normalized, dest);
	}
}

int n2t_str_to_ainstr(char const *norm_repr, instr_t *dest) {
	if (norm_repr[0] != '@') {
		return 1;	// Not an A-instruction.
	}

	if (n2t_is_numeric(norm_repr + 1) && norm_repr[1] != '0') {
		// `[1-9]\d*' digits.
		dest->instr.a.bits = atoi(norm_repr + 1);
		dest->instr.a.loaded = 1;
	} else if (	// @R0, @R1, ..., @R15
		norm_repr[1] == 'R' && n2t_is_numeric(norm_repr + 2) &&\
		atoi(norm_repr + 2) < 16
	) {
		strncpy(dest->instr.a.label, norm_repr + 1, BUFFSIZE_MED);
		dest->instr.a.bits = atoi(norm_repr + 2);
		dest->instr.a.loaded = 1;
	} else if (n2t_is_alpha(norm_repr + 1, ".$_0123456789")) {
		// @LABEL, @label, @...
		strncpy(dest->instr.a.label, norm_repr + 1, BUFFSIZE_MED);
		dest->instr.a.loaded = 0;
	} else {
		return 1;
	}

	dest->type = A;

	return 0;
}

int n2t_str_to_cinstr(char const *const norm_repr, instr_t *dest) {
	char const
		*const dest_field_tail = index(norm_repr, '='),
		*const jump_field_head = index(norm_repr, ';');
	char comp_field[strlen(norm_repr) + 1];
	int comp_encoding;
	size_t dest_offset = 0;

	char parsed_dest[4] = "   ";
	size_t parsed_dest_index = 0;

	dest->instr.c = 0;

	// Parse the `dest' part.
	// If '=' was not found in `norm_repr', `dest_field_tail' will be `NULL'.
	while (norm_repr + dest_offset < dest_field_tail) {
		if (!IS_SPACE(norm_repr[dest_offset])) {
			if (index(parsed_dest, norm_repr[dest_offset])) {
				return 1;	// We already parsed `*norm_repr'!
			}

			switch (norm_repr[dest_offset]) {
				case 'M':
					n2t_set_dest(&dest->instr.c, DEST_M);
					parsed_dest[parsed_dest_index] = 'M';
					break;
				case 'D':
					n2t_set_dest(&dest->instr.c, DEST_D);
					parsed_dest[parsed_dest_index] = 'D';
					break;
				case 'A':
					n2t_set_dest(&dest->instr.c, DEST_A);
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
			n2t_set_jump(&dest->instr.c, JUMP_GT);
		} else if (!strcmp(jump_field_head + 1, "JEQ")) {
			n2t_set_jump(&dest->instr.c, JUMP_EQ);
		} else if (!strcmp(jump_field_head + 1, "JGE")) {
			n2t_set_jump(&dest->instr.c, JUMP_GE);
		} else if (!strcmp(jump_field_head + 1, "JLT")) {
			n2t_set_jump(&dest->instr.c, JUMP_LT);
		} else if (!strcmp(jump_field_head + 1, "JNE")) {
			n2t_set_jump(&dest->instr.c, JUMP_NE);
		} else if (!strcmp(jump_field_head + 1, "JLE")) {
			n2t_set_jump(&dest->instr.c, JUMP_LE);
		} else if (!strcmp(jump_field_head + 1, "JMP")) {
			n2t_set_jump(&dest->instr.c, JUMP_ALWAYS);
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
		*index(comp_field, ';') = '\0';

	n2t_strip(comp_field, comp_field);
	n2t_collapse_any(comp_field, " \t", comp_field);

	if ((comp_encoding = n2t_parse_cinstr_comp(comp_field)) > -1) {
		n2t_set_comp(&dest->instr.c, comp_encoding);
	} else {
		return 2;
	}

	dest->instr.c |= (7 << 13);
	dest->type = C;

	return 0;
}

int n2t_str_to_label(char const *str_repr, label_t *dest) {
	size_t const len = strlen(str_repr);
	char copy[len - 1];

	if (str_repr[0] != '(' || str_repr[len - 1] != ')') {
		return 1;
	}

	strncpy(copy, str_repr + 1, len - 2);
	copy[len - 2] = '\0';

	if (n2t_is_alpha(copy, ".$_0123456789")) {
		strncpy(dest->text_repr, str_repr, BUFFSIZE_MED);

		return 0;
	} else {
		return 1;
	}
}

int n2t_set_dest(Cinstr_t *dest, int dest_reg) {
	if (DEST_NONE <= dest_reg && dest_reg <= DEST_AMD) {
		*dest |= dest_reg << 3;

		return 0;
	}

	return 1;
}

int n2t_get_dest(Cinstr_t const in) {
	return (in & (0x7 << 3)) >> 3;
}

int n2t_set_comp(Cinstr_t *in, short int comp_instr) {
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

int n2t_get_comp(Cinstr_t in) {
	return (in & (0x7F << 6)) >> 6;
}

int n2t_set_jump(Cinstr_t *dest, int jump_cond) {
	if (JUMP_NONE <= jump_cond && jump_cond <= JUMP_ALWAYS) {
		*dest |= jump_cond;

		return 0;
	} else {
		return 1;
	}
}

int n2t_get_jump(Cinstr_t in) {
	return in & 0x7;
}

tokenseq_t* n2t_tokenseq_alloc(size_t n) {
	tokenseq_t *o;

	if (n <= 0)
		return NULL;

	o = malloc(sizeof(tokenseq_t));
	if (o == NULL)
		return NULL;

	o->tokens = calloc(n, sizeof(token_t));
	if (o->tokens == NULL) {
		free(o);
		return NULL;
	}

	o->ntokens = n;
	o->last = 0;

	return o;
}

tokenseq_t* n2t_tokenseq_extend(tokenseq_t *s, size_t n) {
	token_t *t;

	if (n > 0) {
		t = realloc(s->tokens, sizeof(token_t) * (s->ntokens + n));

		if (t == NULL) {
			return NULL;
		} else {
			s->tokens = t;
			s->ntokens += n;
		}
	}

	return s;
}

int n2t_tokenseq_append_instr(tokenseq_t *s, instr_t const in) {
	if (s == NULL)
		return 1;

	if (in.type == A) {
		s->tokens[s->last].data.instr.instr.a = in.instr.a;
		s->tokens[s->last].data.instr.type = A;
	} else if (in.type == C) {
		s->tokens[s->last].data.instr.instr.c = in.instr.c;
		s->tokens[s->last].data.instr.type = C;
	} else {
		return 1;
	}
	s->tokens[s->last].type = INSTR;

	s->last++;

	return 0;
}

int n2t_tokenseq_append_label(tokenseq_t *s, label_t const l) {
	if (s == NULL)
		return 1;

	strncpy(
		s->tokens[s->last].data.label.text_repr, l.text_repr, BUFFSIZE_MED
	);
	s->tokens[s->last].data.label.location = l.location;
	s->tokens[s->last].type = LABEL;
	s->last++;

	return 0;
}

int n2t_tokenseq_full(tokenseq_t const *s) {
	return s->last >= s->ntokens;
}

void n2t_tokenseq_free(tokenseq_t *l) {
	free(l->tokens);
	free(l);
}

tokenseq_t* n2t_tokenize(const char *filepath) {
	FILE *fin;
	char buff[BUFFSIZE_LARGE];

	tokenseq_t *seq;
	instr_t i;
	label_t l;

	if ((fin = fopen(filepath, "r")) == NULL) {
		return NULL;
	}

	if ((seq = n2t_tokenseq_alloc(BUFFSIZE_MED)) == NULL) {
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

		if (n2t_tokenseq_full(seq)) {
			// Double the size at each new refill.
			n2t_tokenseq_extend(seq, seq->ntokens);
		}

		if (n2t_str_to_instr(buff, &i) == 0) {
			n2t_tokenseq_append_instr(seq, i);
			memset(&i, 0, sizeof(instr_t));
		} else if (n2t_str_to_label(buff, &l) == 0) {
			n2t_tokenseq_append_label(seq, l);
			memset(&l, 0, sizeof(label_t));
		}
	}

	fclose(fin);

	return seq;
}

static int n2t_parse_cinstr_comp(char const *norm_repr) {
	size_t i;

	for (i = 0; i <= COMP_MPLUS1; i++) {
		if (!strcmp(norm_repr, INDEX_TO_COMP[i]))
			return i;
	}

	return -1;
}
