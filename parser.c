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
#include "utils.h"
#include "parser.h"
#include <string.h>


typedef struct {
	char id[BUFFSIZE_MED];
	uint32_t address;
} ramvar_t;

ramvar_t const DEFAULT_RAMVARS[] = {
	{"R0", RAMVAR_R0}, {"R1", RAMVAR_R1}, {"R2", RAMVAR_R2}, {"R3", RAMVAR_R3},
	{"R4", RAMVAR_R4}, {"R5", RAMVAR_R5}, {"R6", RAMVAR_R6}, {"R7", RAMVAR_R7},
	{"R8", RAMVAR_R8}, {"R9", RAMVAR_R9}, {"R10", RAMVAR_R10},
	{"R11", RAMVAR_R11}, {"R12", RAMVAR_R12}, {"R13", RAMVAR_R13},
	{"R14", RAMVAR_R14}, {"R15", RAMVAR_R15},

	{"SCREEN", RAMVAR_SCREEN}, {"KBD", RAMVAR_KBD}, {"SP", RAMVAR_SP},
	{"LCL", RAMVAR_LCL}, {"ARG", RAMVAR_ARG}, {"THIS", RAMVAR_THIS},
	{"THAT", RAMVAR_THAT},
};

static int n2t_parse_rom_labels(tokenseq_t *const s);
static int n2t_assign_rom_labels(tokenseq_t *const s);
static int n2t_assign_ram_labels(tokenseq_t *const s);

/**
 * Param `a': a `ramvar_t' array.
 * Param `n': number of elements in `a'.
 * Param `varname': RAM variable name to look for in `a'.
 *
 * Returns: the `address' field value of a `ramvar_t' object in `a' having the
 * same `id' as `varname'.
 */
static int64_t n2t_varname_to_address(
	ramvar_t const *a, size_t const n, char const *varname
);


tokenseq_t* n2t_parse(char const *filepath) {
	tokenseq_t *s;

	if ((s = n2t_tokenize(filepath)) == NULL)
		return NULL;

	n2t_parse_rom_labels(s);
	n2t_assign_rom_labels(s);
	n2t_assign_ram_labels(s);

	return s;
}

static int n2t_parse_rom_labels(tokenseq_t *s) {
	size_t i, instrcounter = 0;
	token_t *t;

	if (s == NULL)
		return 1;

	for (i = 0; i < s->next; i++) {
		t = n2t_tokenseq_index_get(s, i);

		if (t->type == LABEL && !t->data.label.loaded) {
			t->data.label.location = instrcounter;
			t->data.label.loaded = 1;
		} else if (t->type == INSTR) {
			instrcounter++;
		}
	}

	return 0;
}

static int n2t_assign_rom_labels(tokenseq_t *s) {
	size_t i;
	token_t *t;
	memloc_t *l;

	if (s == NULL)
		return 1;

	for (i = 0; i < s->next; i++) {
		t = n2t_tokenseq_index_get(s, i);

		if (
			t->type == INSTR &&
			t->data.instr.type == A &&
			t->data.instr.instr.a.memptr.loaded == 0
		) {
			l = n2t_tokenseq_find_rom_label(s, t->data.instr.instr.a.memptr);

			if (l) {
				t->data.instr.instr.a.memptr.location = l->location;
				t->data.instr.instr.a.memptr.type = ROM;
				t->data.instr.instr.a.memptr.loaded = 1;
			}
		}
	}

	return 0;
}

static int n2t_assign_ram_labels(tokenseq_t *const s) {
	size_t i, labelcounter = 16;
	int64_t default_ramvar;
	token_t *t;

	if (s == NULL)
		return 1;

	for (i = 0; i < s->next; i++) {
		t = n2t_tokenseq_index_get(s, i);

		if (
			t->type == INSTR && t->data.instr.type == A &&
			!t->data.instr.instr.a.memptr.loaded &&
			t->data.instr.instr.a.memptr.type != ROM
		) {
			default_ramvar = n2t_varname_to_address(
				DEFAULT_RAMVARS, sizeof(DEFAULT_RAMVARS) / sizeof(ramvar_t),
				t->data.instr.instr.a.memptr.label
			);

			if (default_ramvar >= 0) {
				t->data.instr.instr.a.memptr.location = default_ramvar;
			} else {
				t->data.instr.instr.a.memptr.location = labelcounter;
				labelcounter++;
			}

			t->data.instr.instr.a.memptr.type = RAM;
			t->data.instr.instr.a.memptr.loaded = 1;
		}
	}

	return 0;
}

static int64_t n2t_varname_to_address(
	ramvar_t const *a, size_t const n, char const *varname
) {
	size_t i;

	if (a == NULL)
		return -1;

	for (i = 0; i < n; i++) {
		if (!strncmp(a[i].id, varname, BUFFSIZE_MED))
			return a[i].address;
	}

	return -1;
}
