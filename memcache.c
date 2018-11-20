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
#include "memcache.h"
#include <string.h>

memcache_t* n2t_memcache_alloc(size_t units, size_t unitsize) {
	memcache_t *o;

	if (units < 1 || unitsize < 1)
		return NULL;

	if ((o = malloc(sizeof(memcache_t))) == NULL)
		return NULL;
	
	o->head = calloc(units, unitsize);

	if (o->head == NULL) {
		free(o);
		return NULL;
	}

	o->unitsize = unitsize;
	o->next = 0;
	o->length = units;
	
	return o;
}

memcache_t* n2t_memcache_extend(memcache_t *c, size_t n) {
	void *updated_head;

	if (n > 0) {
		updated_head = realloc(c->head, c->unitsize * (c->length + n));

		if (updated_head == NULL)
			return NULL;

		c->head = updated_head;
		c->length += n;
	}

	return c;
}

int n2t_memcache_add(memcache_t *c, void const *source, size_t objsize) {
	if (objsize > c->unitsize) {
		return 1;
	} else if (n2t_memcache_fetch(c, source, objsize) != NULL) {
		return 2;
	} else if (MEMCACHE_FULL(c)) {
		return 3;
	} else {
		memcpy(
			c->head + MEMCACHE_OFFSET(c, c->next),
			source, MIN(c->unitsize, objsize)
		);
		// Set the remaining memory to `0', if any.
		memset(
			c->head + MEMCACHE_OFFSET(c, c->next) + MIN(c->unitsize, objsize),
			0,
			c->unitsize - MIN(c->unitsize, objsize)
		);
		c->next++;

		return 0;
	}
}

void* n2t_memcache_fetch(memcache_t const *c, void const *mould, size_t mouldsize) {
	size_t const cmpsize = MIN(c->unitsize, mouldsize);
	size_t i;

	if (mouldsize > c->unitsize)
		return NULL;

	for (i = 0; i < c->length; i++) {
		if (memcmp(c->head + MEMCACHE_OFFSET(c, i), mould, cmpsize) == 0) {
			return c->head + MEMCACHE_OFFSET(c, i);
		}
	}

	return NULL;
}

void n2t_memcache_free(memcache_t *c) {
	free(c->head);
	free(c);
}
