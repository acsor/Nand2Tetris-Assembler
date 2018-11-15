#include <stdio.h>
#include <string.h>
#include "lexer.h"


int main (int argc, char *argv[]) {
	tokenseq_t *s;
	char buff[BUFFSIZE_MED];
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "%s: <file path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	s = n2t_tokenize(argv[1]);

	for (i = 0; i < s->last; i++) {
		if (s->tokens[i].type == INSTR) {
			n2t_instr_to_str(s->tokens[i].data.instr, buff);
			puts(buff);
			buff[0] = '\0';
		} else if (s->tokens[i].type == LABEL) {
			puts(s->tokens[i].data.label.text_repr);
		}
	}

	n2t_tokenseq_free(s);

	return EXIT_SUCCESS;
}
