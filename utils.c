#include "utils.h"
#include <string.h>


int	asm_strip(char const *source, char *dest) {
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

int	asm_decomment(char const *source, char *dest) {
	char *comment_begin;

	// Shall we bound the number of characters written to dest?
	strcpy(dest, source);
 	comment_begin = strstr(dest, "//");

	if (comment_begin) {
		dest[(int)(comment_begin - source)] = '\0';
	}

	return 0;
}

int asm_composed_of(char const *s, char const *set) {
	size_t i;

	for (i = 0; i < strlen(s); i++) {
		if (!index(set, s[i])) {
			return 0;
		}
	}

	return 1;
}

int asm_is_whitespace(char const *s) {
	return asm_composed_of(s, " \t\n\r\v");
}

int asm_is_alpha(char const *source, char const *extra) {
	char charset[strlen(source) + strlen(extra) + 1];

	strcpy(charset, source);
	strcat(charset, extra);

	return asm_composed_of(source, charset);
}

int	asm_is_numeric(char const *source) {
	return asm_composed_of(source, "0123456789");
}
