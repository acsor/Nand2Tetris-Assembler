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


int main (int argc, char *argv[]) {
	tokenseq_t *s;
	char buff[BUFFSIZE_MED];
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "%s: <file path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	s = n2t_tokenize(argv[1]);

	for (i = 0; i < s->next; i++) {
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
