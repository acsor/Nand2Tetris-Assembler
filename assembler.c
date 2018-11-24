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
	tokenseq_t *s;
	char buff[BITSTR_BUFFSIZE];
	token_t *t;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "%s: <file path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((s = n2t_parse(argv[1])) == NULL) {
		fprintf(
			stderr, "%s: `%s' is an invalid `.asm' file.\n", argv[0], argv[1]
		);
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
			n2t_tokenseq_free(s);

			return EXIT_FAILURE;
		}

		puts(buff);
	}

	n2t_tokenseq_free(s);

	return EXIT_SUCCESS;
}
