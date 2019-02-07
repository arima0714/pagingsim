#include "util.h"
#include <stdio.h>
#include <stdlib.h>

FILE *xfopen(const char *path, const char *mode)
{
	FILE *f = fopen(path, mode);
	if (!f) {
		perror("failed to reading file\n");
		exit(1);
	}
	return f;
}

void *xmalloc(size_t size)
{
	void *p = malloc(size);
	if (!p) {
		perror("Memory could not be allocated\n");
		exit(1);
	}
	return p;
}
