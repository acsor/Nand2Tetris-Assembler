#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "utils.h"

#define ERR_SIZE 256


// utils.h
int test_n2t_strip(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_composed_of(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_decomment(void *const args, char errmsg[], size_t maxwrite);

// tokenizer.h
int test_n2t_instr_to_bitstr(void *const args, char errmsg[], size_t maxwrite);
int test_n2t_instr_type(void *const args, char errmsg[], size_t maxwrite);

typedef int (*test_function)(void*, char[], size_t);


int main (int argc, char *argv[]) {
	test_function tests[] = {
		test_n2t_strip, test_n2t_composed_of, test_n2t_decomment,

		test_n2t_instr_to_bitstr, test_n2t_instr_type
	};
	char *test_names[] = {
		"test_n2t_strip", "test_n2t_composed_of", "test_n2t_decomment",

		"test_n2t_instr_to_bitstr", "test_n2t_instr_type"
	};
	char errmsg[ERR_SIZE];
	size_t i, tests_no = sizeof(tests) / sizeof(test_function);

	for (i = 0; i < tests_no; i++) {
		printf("Testing `%s()'... ", test_names[i]);

		if (tests[i](NULL, errmsg, ERR_SIZE)) {
			printf("FAILED.\n\t`%s'\n", errmsg);
		} else {
			puts("OK.");
		}
	}

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

int test_n2t_instr_to_bitstr(
	void *const args, char errmsg[], size_t maxwrite
) {
	instr inputs[] = {
		{0}, {1}, {4}, {1 << 15}, {(1 << 15) - 1}
	};
	size_t const buffsize = 17;
	char *exp_outputs[] = {
		"0000000000000000", "0000000000000001", "0000000000000100",
		"1000000000000000", "0111111111111111",
	};
	char output[buffsize];
	size_t i, inputs_no = sizeof(inputs) / sizeof(instr);

	for (i = 0; i < inputs_no; i++) {
		n2t_instr_to_bitstr(inputs[i], output);

		if (strncmp(exp_outputs[i], output, buffsize)) {
			return 1;
		}
	}

	return 0;
}

int test_n2t_instr_type(void *const args, char errmsg[], size_t maxwrite) {
	instr inputs[] = {
		{0}, {1}, {4}, {1 << 15}, {(1 << 15) - 1}
	};
	instr_type exp_outputs[] = {A, A, A, C, A};
	size_t i, inputs_no = sizeof(inputs) / sizeof(instr);

	for (i = 0; i < inputs_no; i++) {
		if (exp_outputs[i] != n2t_instr_type(inputs[i])) {
			return 1;
		}
	}

	return 0;
}
