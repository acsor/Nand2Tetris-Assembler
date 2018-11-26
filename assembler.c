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
#include <string.h>
#include "lexer.h"
#include "parser.h"


int main (int argc, char *argv[]) {
	FILE *output;
	char output_path[BUFFSIZE_LARGE], buff[BITSTR_BUFFSIZE];
	tokenseq_t *s;
	token_t *t;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "%s: <file path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!n2t_ends_with(argv[1], ".asm")) {
		fprintf(
			stderr, "%s: `%s' does not have an `.asm' extension.\n", argv[0],
			argv[1]
		);
		return EXIT_FAILURE;
	}

	strncpy(output_path, n2t_filename(argv[1]), BUFFSIZE_LARGE);
	*index(output_path, '.') = '\0';
	strncat(output_path, ".hack", BUFFSIZE_LARGE - strlen(output_path));

	if ((output = fopen(output_path, "wt")) == NULL) {
		fprintf(
			stderr, "%s: could not open `%s' for writing. Exiting.\n", argv[0],
			output_path
		);
		return EXIT_FAILURE;
	}

	if ((s = n2t_parse(argv[1])) == NULL) {
		fprintf(
			stderr, "%s: `%s' is an invalid `.asm' file.\n", argv[0], argv[1]
		);
		fclose(output);

		return EXIT_FAILURE;
	}

	for (i = 0; i < s->next; i++) {
		t = n2t_tokenseq_index_get(s, i);

		if (t->type == INSTR) {
			n2t_instr_to_bitstr(t->data.instr, buff);
		} else if (t->type == LABEL) {
			continue;
		} else {
			fprintf(
				stderr, "No known instrucion type %u. Quitting\n", t->type
			);
			fclose(output);
			n2t_tokenseq_free(s);

			return EXIT_FAILURE;
		}

		fprintf(output, "%s\n", buff);
	}

	n2t_tokenseq_free(s);
	fclose(output);

	return EXIT_SUCCESS;
}
