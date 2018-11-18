#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lexer.h"
#include "utils.h"


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


typedef int (*test_function)(void*, char[], size_t);


int main (int argc, char *argv[]) {
	test_function tests[] = {
		test_n2t_strip, test_n2t_composed_of, test_n2t_decomment,
		test_n2t_replace_any, test_n2t_collapse_any,

		test_n2t_instr_to_bitstr, test_batch_back_translation
	};
	char *test_names[] = {
		"test_n2t_strip", "test_n2t_composed_of", "test_n2t_decomment",
		"test_n2t_replace_any", "test_n2t_collapse_any",

		"test_n2t_instr_to_bitstr", "test_batch_back_translation"
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


int test_n2t_strip(void *const args, char errmsg[], size_t maxwrite) {
	char const *inputs[] = {
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
		c = inputs[i];
		a.bits = inputs[i];
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

int test_batch_back_translation(void *const args, char errmsg[], size_t maxwrite) {
	char const *filenames[] = {
		"Add.asm", "Max.asm", "MaxL.asm", "Pong.asm", "PongL.asm", "Rect.asm", 
		"RectL.asm", 
	};
	char const *test_dir = "test_fixtures/test_batch_back_translation/";
	char filepath[BUFFSIZE_LARGE];
	size_t i;

	for (i = 0; i < sizeof(filenames) / sizeof(char*); i++) {
		strncpy(filepath, test_dir, BUFFSIZE_LARGE);
		strncat(filepath, filenames[i], BUFFSIZE_LARGE - strlen(filepath));

		if (test_back_translation(filepath, errmsg, maxwrite)) {
			return 1;
		}
	}

	return 0;
}

int test_back_translation(void *const args, char errmsg[], size_t maxwrite) {
	char const *filepath = args;
	char exp_repr[BUFFSIZE_LARGE],
		 actual_repr[BUFFSIZE_LARGE];
	FILE *expected;
	tokenseq_t *s;
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

	while (i < s->last && fgets(exp_repr, BUFFSIZE_LARGE, expected)) {
		n2t_strip(exp_repr, exp_repr);

		if (s->tokens[i].type == INSTR) {
			n2t_instr_to_str(
				s->tokens[i].data.instr, actual_repr, BUFFSIZE_LARGE
			);
		} else if (s->tokens[i].type == LABEL) {
			strncpy(
				actual_repr, s->tokens[i].data.label.text_repr, BUFFSIZE_LARGE
			);
		} else {
			n2t_tokenseq_free(s);
			fclose(expected);
			snprintf(
				errmsg, maxwrite,
				"Error while translating token %lu: type %u not recognized.",
				i, s->tokens[i].type
			);

			return 1;
		}

		if (strncmp(exp_repr, actual_repr, BUFFSIZE_LARGE)) {
			n2t_tokenseq_free(s);
			fclose(expected);

			snprintf(
				errmsg, maxwrite,
				"%s:%lu: `%s' was expected, but `%s' was produced.", filepath,
				i + 1, exp_repr, actual_repr
			);

			return 1;
		}

		i++;
	}

	if (i != s->last) {
		snprintf(
			errmsg, maxwrite,
			"%s: the number of tokens expected (%lu) differs from the actual one (%lu).\n",
			filepath, i, s->last
		);

		return 1;
	}

	n2t_tokenseq_free(s);
	fclose(expected);

	return 0;
}
