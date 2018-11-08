#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "utils.h"

#define ERR_SIZE 256


int test_asm_instr_to_bitstr(void *const args, char errmsg[], size_t maxwrite);
int test_asm_instr_type(void *const args, char errmsg[], size_t maxwrite);
int test_asm_strip(void *const args, char errmsg[], size_t maxwrite);
int test_asm_composed_of(void *const args, char errmsg[], size_t maxwrite);
// TO-DO Add test_asm_decomment()

typedef int (*test_function)(void*, char[], size_t);


int main (int argc, char *argv[]) {
	test_function tests[] = {
		test_asm_instr_to_bitstr, test_asm_instr_type, test_asm_strip,
		test_asm_composed_of
	};
	char *test_names[] = {
		"test_asm_instr_to_bitstr", "test_asm_instr_type", "test_asm_strip",
		"test_asm_composed_of"
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


int test_asm_instr_to_bitstr(void *const args, char errmsg[], size_t maxwrite) {
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
		asm_instr_to_bitstr(inputs[i], output);

		if (strncmp(exp_outputs[i], output, buffsize)) {
			return 1;
		}
	}

	return 0;
}

int test_asm_instr_type(void *const args, char errmsg[], size_t maxwrite) {
	instr inputs[] = {
		{0}, {1}, {4}, {1 << 15}, {(1 << 15) - 1}
	};
	instr_type exp_outputs[] = {A, A, A, C, A};
	size_t i, inputs_no = sizeof(inputs) / sizeof(instr);

	for (i = 0; i < inputs_no; i++) {
		if (exp_outputs[i] != asm_instr_type(inputs[i])) {
			return 1;
		}
	}

	return 0;
}

int test_asm_strip(void *const args, char errmsg[], size_t maxwrite) {
	char const *inputs[] = {
		"abcdefghijkl", "           abcdefghijkl", "abcdefghijkl         ",
		"     abcdefghijkl        ", "\tabcdefghijkl", "abcdefghijkl\t",
		"\tabcdefghijkl\t", "\nabcdefghijkl", "abcdefghijkl\n",
		"\nabcdefghijkl\n"
	};
	char *exp_output = "abcdefghijkl", dest[64];
	size_t i;

	for (i = 0; i < sizeof(inputs) / sizeof(char*); i++) {
		asm_strip(inputs[i], dest);

		if (strcmp(exp_output, dest)) {
			snprintf(errmsg, maxwrite, "\"%s\" != \"%s\"", exp_output, dest);
			return 1;
		}
	}

	return 0;
}

int test_asm_composed_of(void *const args, char errmsg[], size_t maxwrite) {
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
		if (asm_composed_of(inputs[i], charset_inputs[i]) != exp_outputs[i]) {
			return 1;
		}
	}

	return 0;
}
