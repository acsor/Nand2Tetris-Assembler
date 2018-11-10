#include "utils.h"
#include <string.h>


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
		if (!index(old, source[sourcepos])) {
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
	char charset[strlen(source) + strlen(extra) + 1];

	strcpy(charset, source);
	strcat(charset, extra);

	return n2t_composed_of(source, charset);
}

int	n2t_is_numeric(char const *source) {
	return n2t_composed_of(source, "0123456789");
}
