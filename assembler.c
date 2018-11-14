#include <stdio.h>
#include <string.h>
#include "lexer.h"


int main (int argc, char *argv[]) {
	tokenseq_t *s;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "%s: <file path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	s = n2t_tokenize(argv[1]);

	for (i = 0; i < s->last; i++) {
		if (s->tokens[i].type == INSTR) {
			printf("%s\n", s->tokens[i].data.instr.text_repr);
		} else if (s->tokens[i].type == LABEL) {
			printf("%s\n", s->tokens[i].data.label.text_repr);
		}
	}

	n2t_tokenseq_free(s);

	return EXIT_SUCCESS;
}
