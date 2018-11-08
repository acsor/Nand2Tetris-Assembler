#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"


int test_asm_instr_to_bitstr(void *const args);
int test_asm_instr_type(void *const args);


typedef int (*test_function)(void*);


int main (int argc, char *argv[]) {
	test_function tests[] = {
		test_asm_instr_to_bitstr, test_asm_instr_type
	};
	char *test_names[] = {
		"test_asm_instr_to_bitstr", "test_asm_instr_type"
	};
	size_t i, tests_no = sizeof(tests) / sizeof(test_function);

	for (i = 0; i < tests_no; i++) {
		printf("Testing `%s()'... ", test_names[i]);

		if (tests[i](NULL)) {
			puts("FAILED.");
		} else {
			puts("OK.");
		}
	}

	return EXIT_SUCCESS;
}


int test_asm_instr_to_bitstr(void *const args) {
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

int test_asm_instr_type(void *const args) {
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
