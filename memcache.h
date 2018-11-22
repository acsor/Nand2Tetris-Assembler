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

#ifndef MEMCACHE_H
#define MEMCACHE_H

#include <stdlib.h>
#include "utils.h"


#define MEMCACHE_OFFSET(c, i)	(c->unitsize * i)
#define MEMCACHE_FULL(c)	(c->next >= c->length)
#define	MEMCACHE_DEFAULT_EXTEND 64

/**
 * The `memcache_t' structure is a facility designed to "cache" same-valued
 * objects, which is particularly useful in enclosing project during the
 * parsing phase.
 */
typedef struct {
	void *head;
	size_t unitsize;
	size_t next, length;
} memcache_t;

memcache_t* n2t_memcache_alloc(size_t units, size_t unitsize);
/**
 * Extends the number of objects stored by `c' by an additional `n'.
 *
 * Returns: `1' if an error occurs, `0' otherwise.
 */
int n2t_memcache_extend(memcache_t *c, size_t n);
/**
 * Param `objsize': size of the object to store into the cache memory.
 *
 * Returns:
 *   - `-1' if `source == NULL'
 *   - `-2' if `objsize' was strictly greater than `c->unitsize'
 *   - `-3' if `source' already existed
 *   - an integer `n >= 0' indicating the cache index the object was inserted
 *   into, in case of success.
 */
int64_t n2t_memcache_store(memcache_t *c, void const *source, size_t objsize);
/**
 * Param `mouldsize': size of the objects to compare.
 *
 * Returns: a pointer to a memory area equal to `mould' or `NULL' if none was
 * found (or an error occurs, e.g. `mouldsize > c->unitsize).
 */
void* n2t_memcache_fetch(memcache_t const *c, void const *mould, size_t mouldsize);
/**
 * Frees up the memory associated with a `memcache_t' object.
 */
void n2t_memcache_free(memcache_t *c);


#endif
