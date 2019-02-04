#include "pagingsim.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char **argv)
{
	int page_frame_count;
	pthread_t thread_id;

	if (argc != 3) {
		fprintf(stderr, "usage:%s [PAGE_FRAME_COUNT] [FILE]\n",
			argv[0]);
		exit(1);
	}

	page_frame_count = atoi(argv[1]);

	initialize(argv[2], &thread_id);

	printf("! %d %d\n", page_frame_count, execute(page_frame_count));
	finalize(thread_id);
	exit(0);
}
