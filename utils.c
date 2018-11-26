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
#include "utils.h"
#include <string.h>


int n2t_join(char *dest, size_t const maxwrite, size_t n, ...) {
	va_list ap;
	size_t written_to = 0;
	char *s;

	dest[0] = '\0';

	va_start(ap, n);
	while (n > 0 && written_to < maxwrite) {
		s = va_arg(ap, char*);
		strncat(dest, s, maxwrite - written_to);

		written_to = MIN(maxwrite, written_to + strlen(s) + 1);
		n--;
	}
	va_end(ap);

	return written_to;
}

int n2t_replace_any(
	char const *source, char const *old, char new, char *dest
) {
	size_t pos = 0, repcount = 0;

	while (source[pos] != '\0') {
		if (index(old, source[pos])) {
			dest[pos] = new;
			repcount++;
		} else {
			dest[pos] = source[pos];
		}

		pos++;
	}

	return repcount;
}

int n2t_collapse_any(char const *source, char const *old, char *dest) {
	size_t sourcepos = 0, destpos = 0;

	while (source[sourcepos] != '\0') {
		if (index(old, source[sourcepos]) == NULL) {
			dest[destpos] = source[sourcepos];
			destpos++;
		}

		sourcepos++;
	}

	dest[destpos] = '\0';

	return destpos;
}

int	n2t_strip(char const *source, char *dest) {
	size_t const source_len = strlen(source);
	size_t lower = 0, upper = source_len;

	while (IS_SPACE(source[lower]) && lower < source_len)
		lower++;

	while (IS_SPACE(source[upper - 1]) && 0 < upper)
		upper--;

	strcpy(dest, source + lower);
	dest[upper - lower] = '\0';

	return 0;
}

int	n2t_decomment(char const *source, char *dest) {
	char *comment_begin;

	// Shall we bound the number of characters written to dest?
	strcpy(dest, source);
 	comment_begin = strstr(dest, "//");

	if (comment_begin) {
		dest[(unsigned long int)(comment_begin - dest)] = '\0';
	}

	return 0;
}

int n2t_composed_of(char const *s, char const *set) {
	size_t i;

	for (i = 0; i < strlen(s); i++) {
		if (!index(set, s[i])) {
			return 0;
		}
	}

	return 1;
}

int n2t_is_whitespace(char const *s) {
	return n2t_composed_of(s, " \t\n\r\v");
}

int n2t_is_alpha(char const *source, char const *extra) {
	char charset[strlen(ASCII_LETTERS) + strlen(extra) + 1];

	charset[0] = '\0';
	strcpy(charset, ASCII_LETTERS);
	strcat(charset, extra);

	return n2t_composed_of(source, charset);
}

int	n2t_is_numeric(char const *source) {
	return n2t_composed_of(source, "0123456789");
}

int n2t_ends_with(char const *s, char const *end) {
	size_t const slen = strlen(s), endlen = strlen(end);
	size_t i;

	if (s == NULL || end == NULL || endlen > slen)
		return 0;

	for (i = 0; i < endlen; i++) {
		if (s[slen - i - 1] != end[endlen - i - 1])
			return 0;
	}

	return 1;
}


char* n2t_filename(char *const filepath) {
	char *const o = rindex(filepath, '/');

	if (o)
		return o + 1;
	else
		return filepath;
}
