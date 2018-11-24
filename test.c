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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "memcache.h"


#define	TEST_DIR_ROOT "test_fixtures/"

// utils.h
int test_n2t_strip(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_composed_of(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_decomment(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_replace_any(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_collapse_any(void *const args, char errmsg[], size_t maxwrite);

// lexer.h
int test_n2t_instr_to_bitstr(void *const args, char errmsg[], size_t maxwrite);
/**
 * Reads a filepath from `args', interpreting it as an .asm file to parse,
 * interpret, translate and confront with the original input.
 */
int test_back_translation(void *const args, char errmsg[], size_t maxwrite);
/**
 * Picks a selection of .asm files, reading, interpreting and retranslating
 * them, checking that the original and the computed instructions match.
 *
 * Invokes `test_back_translation()' as a subroutine.
 */
int test_batch_back_translation(void *const args, char errmsg[], size_t maxwrite);

// memcache.h
int test_n2t_memcache_fetch(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_memcache_index_fetch(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_memcache_extend(void *const args, char errmsg[], size_t maxwrite);

// assembler.c
/**
 * Param `args': a `*char[]' pointer having:
 * 	- The filepath to an `.asm' file to be read in as first element.
 * 	- The filepath to an `.hack' file to be checked with as a second element.
 */
int test_assembler(void *const args, char errmsg[], size_t maxwrite);
int test_assembler_batch(void *const args, char errmsg[], size_t maxwrite);


typedef int (*test_function)(void*, char[], size_t);


int main (int argc, char *argv[]) {
	test_function tests[] = {
		test_n2t_strip, test_n2t_composed_of, test_n2t_decomment,
		test_n2t_replace_any, test_n2t_collapse_any,

		test_n2t_instr_to_bitstr, test_batch_back_translation,

		test_n2t_memcache_fetch, test_n2t_memcache_extend,
		test_n2t_memcache_index_fetch,

		test_assembler_batch
	};
	char *test_names[] = {
		"test_n2t_strip", "test_n2t_composed_of", "test_n2t_decomment",
		"test_n2t_replace_any", "test_n2t_collapse_any",

		"test_n2t_instr_to_bitstr", "test_batch_back_translation",

		"test_n2t_memcache_fetch", "test_n2t_memcache_extend",
		"test_n2t_memcache_index_fetch",

		"test_assembler_batch"
	};
	char errmsg[BUFFSIZE_VLARGE];
	size_t const tests_no = sizeof(tests) / sizeof(test_function);
	size_t i, failed_no = 0;

	for (i = 0; i < tests_no; i++) {
		errmsg[0] = '\0';
		printf("Testing `%s()'... ", test_names[i]);

		if (tests[i](NULL, errmsg, BUFFSIZE_VLARGE)) {
			if (errmsg[0] == '\0') {
				strncpy(errmsg, "???", BUFFSIZE_VLARGE);
			}

			printf("FAILED.\n\t`%s'\n", errmsg);
			failed_no++;
		} else {
			puts("OK.");
		}
	}

	printf(
		"\nRUN %lu TESTS (%lu OK, %lu FAILED).\n",
		tests_no, tests_no - failed_no, failed_no
	);

	return EXIT_SUCCESS;
}


//utils.h
int test_n2t_strip(void *const args, char errmsg[], size_t maxwrite) {
	char *inputs[] = {
		"abcdefghijkl", "           abcdefghijkl", "abcdefghijkl         ",
		"     abcdefghijkl        ", "\tabcdefghijkl", "abcdefghijkl\t",
		"\tabcdefghijkl\t", "\nabcdefghijkl", "abcdefghijkl\n",
		"\nabcdefghijkl\n"
	};
	char *exp_output = "abcdefghijkl", dest[64];
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(char*); i++) {
		n2t_strip(inputs[i], dest);

		if (strcmp(exp_output, dest)) {
			snprintf(errmsg, maxwrite, "\"%s\" != \"%s\"", exp_output, dest);
			return 1;
		}
	}

	return 0;
}

int test_n2t_composed_of(void *const args, char errmsg[], size_t maxwrite) {
	char const *inputs[] = {
		"azbcyaxbzcyx", "abczwy", "01236789012346789", "004958aa",
		"////?---___", "a//__--?"
	};
	char const *charset_inputs[] = {
		"abcxyz", "abcxzy", "0123456789", "0123456789", "/?-_", "/?-_"
	};
	int exp_outputs[] = {
		1, 0, 1, 0, 1, 0,
	};
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(char*); i++) {
		if (n2t_composed_of(inputs[i], charset_inputs[i]) != exp_outputs[i]) {
			return 1;
		}
	}

	return 0;
}

int test_n2t_decomment(void *const args, char errmsg[], size_t maxwrite) {
	char const *inputs[] = {
		"abcdef", "//abcdef", "abcdef   //ghijkl", "abcdef//ghijkl",
		"//abcdef//ghijkl", "    //abcdef", "\t\t//abcdef"
	};
	char *exp_output[] = {
		"abcdef", "", "abcdef   ", "abcdef", "", "    ", "\t\t"
	};
	size_t const buffsize = 32;
	char buff[buffsize];
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(char*); i++) {
		if (n2t_decomment(inputs[i], buff)) {
			return 1;
		}

		if (strcmp(buff, exp_output[i])) {
			return 1;
		}
	}

	return 0;
}

int test_n2t_replace_any(void *const args, char errmsg[], size_t maxwrite) {
	char const *input = "abc def ghi jkl";
	char const *old[] = {
		"abc", "def", "ghi", "jkl", " ", "abcdefghijkl "
	};
	char const new = 'x';
	char const *exp_outputs[] = {
		"xxx def ghi jkl", "abc xxx ghi jkl", "abc def xxx jkl",
		"abc def ghi xxx", "abcxdefxghixjkl", "xxxxxxxxxxxxxxx"
	};
	char buff[BUFFSIZE_MED];
	size_t i;

	for (i = 0; i < sizeof(exp_outputs) / sizeof(char*); i++) {
		n2t_replace_any(input, old[i], new, buff);

		if (strcmp(exp_outputs[i], buff)) {
			return 1;
		}
	}

	return 0;
}

int test_n2t_collapse_any(void *const args, char errmsg[], size_t maxwrite) {
	char const *input = "abc def ghi jkl \n\t\r\v";
	char const *old[] = {
		"abc", "def", "ghi", "jkl", " ", "abcdefghijkl\n\t\r\v "
	};
	char const *exp_outputs[] = {
		" def ghi jkl \n\t\r\v", "abc  ghi jkl \n\t\r\v",
		"abc def  jkl \n\t\r\v", "abc def ghi  \n\t\r\v",
		"abcdefghijkl\n\t\r\v", ""
	};
	char buff[BUFFSIZE_MED];
	size_t i;

	for (i = 0; i < sizeof(exp_outputs) / sizeof(char*); i++) {
		n2t_collapse_any(input, old[i], buff);

		if (strncmp(exp_outputs[i], buff, BUFFSIZE_MED)) {
			snprintf(
				errmsg, maxwrite, "Expected `%s', but got `%s'",
				exp_outputs[i], buff
			);
			return 1;
		}
	}

	return 0;
}


//lexer.h
int test_n2t_instr_to_bitstr(
	void *const args, char errmsg[], size_t maxwrite
) {
	short int inputs[] = {0, 1, 4, 1 << 15, (1 << 15) - 1};
	instr_t in;
	Cinstr_t c;
	Ainstr_t a;
	size_t const buffsize = 17;
	char *exp_outputs[] = {
		"0000000000000000", "0000000000000001", "0000000000000100",
		"1000000000000000", "0111111111111111",
	};
	char output[buffsize];
	size_t i, inputs_no = sizeof(inputs) / sizeof(short int);

	for (i = 0; i < inputs_no; i++) {
		a.memptr.location = c = inputs[i];
		in.instr.c = c;
		in.instr.a = a;

		in.type = A;
		n2t_instr_to_bitstr(in, output);

		if (strncmp(exp_outputs[i], output, buffsize))
			return 1;

		in.type = C;
		n2t_instr_to_bitstr(in, output);

		if (strncmp(exp_outputs[i], output, buffsize))
			return 1;
	}

	return 0;
}

int test_batch_back_translation(
	void *const args, char errmsg[], size_t maxwrite
) {
	char const *filenames[] = {
		"Add.asm", "Max.asm", "MaxL.asm", "Pong.asm", "PongL.asm", "Rect.asm",
		"RectL.asm"
	};
	char test_dir[BUFFSIZE_LARGE], filepath[BUFFSIZE_LARGE];
	size_t i;

	n2t_join(test_dir, BUFFSIZE_LARGE, 2, TEST_DIR_ROOT,
		"test_batch_back_translation/");

	for (i = 0; i < sizeof(filenames) / sizeof(char*); i++) {
		n2t_join(filepath, BUFFSIZE_LARGE, 2, test_dir, filenames[i]);

		if (test_back_translation(filepath, errmsg, maxwrite))
			return 1;
	}

	return 0;
}

int test_back_translation(void *const args, char errmsg[], size_t maxwrite) {
	char const *filepath = args;
	char exp_repr[BUFFSIZE_LARGE],
		 actual_repr[BUFFSIZE_LARGE];
	FILE *expected;
	tokenseq_t *s;
	token_t *t;
	size_t i = 0;

	if ((expected = fopen(filepath, "rt")) == NULL) {
		snprintf(errmsg, maxwrite, "Error while opening `%s': ", filepath);
		strncat(errmsg, strerror(errno), maxwrite - strlen(errmsg));

		return 1;
	}

	if ((s = n2t_tokenize(filepath)) == NULL) {
		fclose(expected);
		snprintf(errmsg, maxwrite, "Error while tokenizing `%s'.", filepath);

		return 1;
	}

	while (i < s->next && fgets(exp_repr, BUFFSIZE_LARGE, expected)) {
		n2t_strip(exp_repr, exp_repr);
		t = n2t_tokenseq_index_get(s, i);

		if (t == NULL) {
			snprintf(
				errmsg, maxwrite, "%s:%lu: token was NULL at line %lu.",
				filepath, i + 1, i + 1
			);

			n2t_tokenseq_free(s);
			fclose(expected);

			return 1;
		}

		if (t->type == INSTR) {
			n2t_instr_to_str(t->data.instr, actual_repr, BUFFSIZE_LARGE);
		} else if (t->type == LABEL) {
			snprintf(actual_repr, BUFFSIZE_LARGE, "(%s)", t->data.label.label);
		} else {
			snprintf(
				errmsg, maxwrite,
				"%s:%lu: error while translating token %s:%lu: type %u not"\
				" recognized.",
				filepath, i + 1, exp_repr, i + 1, t->type
			);

			n2t_tokenseq_free(s);
			fclose(expected);

			return 1;
		}

		if (strncmp(exp_repr, actual_repr, BUFFSIZE_LARGE)) {
			snprintf(
				errmsg, maxwrite,
				"%s:%lu: `%s' was expected, but `%s' was produced.", filepath,
				i + 1, exp_repr, actual_repr
			);

			n2t_tokenseq_free(s);
			fclose(expected);

			return 1;
		}

		i++;
	}

	if (i != s->next) {
		snprintf(
			errmsg, maxwrite,
			"%s: the number of tokens expected (%lu) differs from the actual"\
			" one (%u).\n", filepath, i, s->next
		);

		return 1;
	}

	n2t_tokenseq_free(s);
	fclose(expected);

	return 0;
}


// memcache.h
int test_n2t_memcache_fetch(void *const args, char errmsg[], size_t maxwrite) {
	size_t nmemb = 3E3, membsize = 20, i;
	memcache_t *c = n2t_memcache_alloc(nmemb, membsize);
	void *e;
	char const *source[] = {
		"aaaaaaaaaa", "bbbbbbbbbb", "cccccccccc", "dddddddddd", "eeeeeeeeee",
		"ffffffffff", "gggggggggg", "hhhhhhhhhh", "iiiiiiiiii", "jjjjjjjjjj"
	};
	size_t const strsize = 11, sourcesize = 10;

	for (i = 0; i < nmemb; i++) {
		n2t_memcache_store(c, source[i % sourcesize], strsize);
	}

	for (i = 0; i < nmemb; i++) {
		e = n2t_memcache_fetch(c, source[i % sourcesize], strsize);

		if (memcmp(source[i % sourcesize], e, strsize)) {
			snprintf(
				errmsg, maxwrite, "`%s' didn't match with `%s'.",
				source[i % sourcesize], (char*) e
			);

			return 1;
		}
	}

	n2t_memcache_free(c);

	return 0;
}

int test_n2t_memcache_index_fetch(
	void *const args, char errmsg[], size_t maxwrite
) {
	size_t nmemb = 3000, membsize = sizeof(int64_t), i;
	memcache_t *c = n2t_memcache_alloc(nmemb, membsize);
	int64_t temp = 0;
	void *e;

	for (i = 0; i < nmemb; i++) {
		n2t_memcache_store(c, &temp, sizeof(temp));
		// We pick arbitrary values -- all non-odd integers.
		temp += 2;
	}

	if (c->next != nmemb) {
		snprintf(
			errmsg, maxwrite, "Added %u elements, but %lu had to.", c->next,
			nmemb
		);
		n2t_memcache_free(c);

		return 1;
	}

	temp = 0;
	for (i = 0; i < c->next; i++) {
		e = n2t_memcache_index_fetch(c, i);

		if (memcmp(e, &temp, sizeof(temp))) {
			snprintf(
				errmsg, maxwrite, "Expected value %ld, got %ld.", temp,
				*((int64_t*) e)
			);

			n2t_memcache_free(c);
			return 1;
		}

		temp += 2;
	}

	n2t_memcache_free(c);
	return 0;
}

int test_n2t_memcache_extend(
	void *const args, char errmsg[], size_t maxwrite
) {
	size_t nmemb = 1000, membsize = 1, last;
	int8_t const check = 0xFF;
	int8_t *actual;
	memcache_t *c = n2t_memcache_alloc(1, membsize);

	for (last = c->length; last < nmemb; last++) {
		// This might ensure no segmentation faults are caused.
		memset(c->head + MEMCACHE_OFFSET(c, last - 1), check, membsize);
		n2t_memcache_extend(c, 1);

		if (c->length != last + 1) {
			snprintf(
				errmsg, maxwrite, "`c->length' = %u, but %lu was expected.",
				c->length, last + 1
			);

			return 1;
		}

		actual = c->head + MEMCACHE_OFFSET(c, last - 1);
		
		if (*actual != check) {
			snprintf(
				errmsg, maxwrite, "Value at index %lu = %d != %d.",
				last - 1, *actual, check
			);

			return 1;
		}
	}

	n2t_memcache_free(c);

	return 0;
}


// assembler.c
int test_assembler_batch(void *const args, char errmsg[], size_t maxwrite) {
	char *filenames[] = {
		"Add.asm", "Max.asm", "MaxL.asm", "Pong.asm", "PongL.asm", "Rect.asm",
		"RectL.asm"
	};
	char
		test_dir[BUFFSIZE_LARGE],
		filepath[BUFFSIZE_LARGE], hack_filepath[BUFFSIZE_LARGE],
		*subcall_args[2];
	size_t filenames_no = sizeof(filenames) / sizeof(char*);
	size_t i;

	n2t_join(
		test_dir, BUFFSIZE_LARGE, 2, TEST_DIR_ROOT, "test_assembler_batch/"
	);

	for (i = 0; i < filenames_no; i++) {
		n2t_join(filepath, BUFFSIZE_LARGE, 2, test_dir, filenames[i]);
		n2t_join(hack_filepath, BUFFSIZE_LARGE, 2, test_dir, filenames[i]);

		*index(hack_filepath, '.') = '\0';
		strcat(hack_filepath, ".hack");

		subcall_args[0] = filepath;
		subcall_args[1] = hack_filepath;

		if (test_assembler(subcall_args, errmsg, maxwrite))
			return 1;
	}

	return 0;
}

int test_assembler(void *const args, char errmsg[], size_t maxwrite) {
	char **argv = args;
	char
		exp[BITSTR_BUFFSIZE + 2], actual[BITSTR_BUFFSIZE + 1],
		instr_repr[BUFFSIZE_MED];
	tokenseq_t *s;
	token_t *t;

	FILE *exp_stream;
	size_t i, actual_lineno = 0, exp_lineno = 0;

	if ((s = n2t_parse(argv[0])) == NULL) {
		snprintf(errmsg, maxwrite, "Could not parse `%s'.", argv[0]);

		return 1;
	}

	if ((exp_stream = fopen(argv[1], "rt")) == NULL) {
		snprintf(errmsg, maxwrite, "Could not open `%s'.", argv[1]);
		n2t_tokenseq_free(s);

		return 1;
	}

	for (i = 0; i < s->next; i++) {
		t = n2t_tokenseq_index_get(s, i);

		if (t->type == INSTR) {
			// `BITSTR_BUFFSIZE + 1' is due to the newline.
			fgets(exp, BITSTR_BUFFSIZE + 1, exp_stream);
			n2t_strip(exp, exp);
			n2t_instr_to_bitstr(t->data.instr, actual);

			actual_lineno++;
			exp_lineno++;
		} else if (t->type == LABEL) {
			continue;
		} else {
			snprintf(
				errmsg, maxwrite, "No known instrucion type `%u'. Quitting.",
				t->type
			);
			fclose(exp_stream);
			n2t_tokenseq_free(s);

			return 1;
		}

		if (strncmp(actual, exp, BITSTR_BUFFSIZE)) {
			n2t_instr_to_str(t->data.instr, instr_repr, BUFFSIZE_MED);
			snprintf(
				errmsg, maxwrite, "%s:%lu: wrong bitstring output: expected"
				" `%s', got `%s'. Alleged instruction: `%s'.", argv[1],
				actual_lineno, exp, actual, instr_repr
			);

			fclose(exp_stream);
			n2t_tokenseq_free(s);

			return 1;
		}
	}

	while (fgets(exp, BITSTR_BUFFSIZE, exp_stream))
		exp_lineno++;

	if (exp_lineno != actual_lineno) {
		snprintf(
			errmsg, maxwrite, "%s: %lu machine language lines were expected,"
			" but %lu produced.", argv[1], exp_lineno, actual_lineno
		);

		fclose(exp_stream);
		n2t_tokenseq_free(s);

		return 1;
	}

	fclose(exp_stream);
	n2t_tokenseq_free(s);

	return 0;
}
