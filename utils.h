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
#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdarg.h>


#define	BUFFSIZE_MICRO 16
#define	BUFFSIZE_SMALL 32
#define	BUFFSIZE_MED 64
#define	BUFFSIZE_LARGE 128
#define	BUFFSIZE_VLARGE 256
#define	BUFFSIZE_XLARGE 512

#define ASCII_LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define	MIN(a, b)	(a < b ? a: b)
#define IS_IN(c, s)	(index(s, c) != NULL)
#define	IS_SPACE(c)	(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v')


/**
 * Joins `n' `char*' arguments to `dest', writing at most `maxwrite' bytes of
 * data.
 *
 * Returns: `-1' if an error occurs, the number of characters written to `dest'
 * otherwise. No error is reported if `maxwrite' is exceeded, but its value
 * returned.
 */
int n2t_join(char *dest, size_t const maxwrite, size_t n, ...);
/**
 * Replaces any of the characters in `old' from `source' with `new', storing
 * the result on to `dest'.
 * It is assumed that `dest' has as much storage as source does, or more.
 *
 * Returns: the number of charaters replaced or `-1' if an error verified.
 */
int n2t_replace_any(char const *source, char const *old, char new, char *dest);
/**
 * Removes any of the characters in `old' from `source', storing the result on
 * to `dest'.
 *
 * Returns: the number of charaters removed or `-1' if an error verified.
 */
int n2t_collapse_any(char const *source, char const *old, char *dest);
/**
 * Stores into `dest' `source' stripped of whitespaces (`\n', `\t', ...).
 *
 * Returns: `1' if an error occurred, `0' otherwise.
 */
int	n2t_strip(char const *source, char *dest);
/**
 * Strips away any single-line comments from `source', storing its altered form
 * to `dest'. It is assumed that `dest' has as much (or more) storage as
 * `source' does.
 *
 * Returns: `1' if an error occurred, `0' otherwise.
 */
int	n2t_decomment(char const *source, char *dest);
/**
 * Returns: `1' if `source' is composed entirely of `a-zA-Z' characters and
 * additional ones specified by `extra'.
 */
int n2t_is_alpha(char const *source, char const *extra);
/**
 * Returns: `1' if `source' is composed entirely of numeric characters, `0'
 * otherwise.
 */
int	n2t_is_numeric(char const *source);
/**
 * Returns: `1' if `s' is composed entirely of whitespaces, `0' otherwise.
 */
int n2t_is_whitespace(char const *s);
/**
 * Returns: `1' if `s' is composed of characters only found in `set', `0'
 * othewrise.
 */
int n2t_composed_of(char const *s, char const *set);
/**
 * Returns: `1' if `s' is terminated by `end', `0' otherwise.
 */
int n2t_ends_with(char const *s, char const *end);

/**
 * Returns: the file name portion of a file path encoded in `filepath'. The
 * primitives limits to just considering slash characters.
 */
char* n2t_filename(char *const filepath);


#endif
