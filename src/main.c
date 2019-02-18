#include "pagingsim.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int page_frame_count;
	pthread_t thread_id;
	int page_fault_count;

	if (argc != 3) {
		fprintf(stderr, "usage:%s [PAGE_FRAME_COUNT] [FILE]\n",
			argv[0]);
		exit(1);
	}

	initialize(argv, &thread_id, &page_frame_count);

	page_fault_count = execute(page_frame_count);
	printf("! %d %d\n", page_frame_count, page_fault_count);
	finalize(thread_id);
	exit(0);
}
