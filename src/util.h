#ifndef H_INCLUDED_UTIL
#define H_INCLUDED_UTIL

#include <stddef.h>
#include <stdio.h>

#ifdef UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif
FILE *xfopen(const char *path, const char *mode);
void *xmalloc(size_t size);
#endif
