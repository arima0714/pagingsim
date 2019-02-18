#include "pagingsim.h"
#include "util.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static FILE *memory_trace;
static pthread_mutex_t mutex;
STATIC struct Page *pages;

STATIC enum PageClass get_page_class(int page_index)
{
	pthread_mutex_lock(&mutex);
	enum PageClass class;
	int modified_bit = pages[page_index].modified_bit;
	int reference_bit = pages[page_index].reference_bit;

	if (!reference_bit && !modified_bit) {
		class = Class0;
	} else if (!reference_bit && modified_bit) {
		class = Class1;
	} else if (reference_bit && !modified_bit) {
		class = Class2;
	} else {
		class = Class3;
	}
	pthread_mutex_unlock(&mutex);
	return class;
}

STATIC void access_page(struct ProgramCounter *pc, int page_index)
{
	pages[page_index].modified_bit = pc->access == WRITE;
	pages[page_index].reference_bit = true;
}

STATIC void clear_page(int page_index)
{
	pages[page_index].modified_bit = false;
	pages[page_index].page_frame_no = EMPTY;
	pages[page_index].presence_bit = false;
	pages[page_index].reference_bit = false;
}

STATIC void clear_all_reference_bit(void)
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].reference_bit = false;
	}
	pthread_mutex_unlock(&mutex);
}

STATIC void *interrupt_clear_reference_bit(void *p)
{
	while (true) {
		clear_all_reference_bit();
		usleep(REFERENCE_BIT_CLEAR_USEC);
	}
	return NULL;
}

void initialize(char **argv, pthread_t * thread_id, int *page_frame_count)
{
	memory_trace = xfopen(argv[2], "r");
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		clear_page(i);
	}
	*page_frame_count = atoi(argv[1]);
	pthread_mutex_init(&mutex, NULL);
	pthread_create(thread_id, NULL, &interrupt_clear_reference_bit, NULL);
}

void finalize(pthread_t thread_id)
{
	free(pages);
	fclose(memory_trace);
	pthread_cancel(thread_id);
	pthread_join(thread_id, NULL);
}

STATIC int nru(void)
{
	int page_indexes[MAX_CLASS_COUNT] = { -1, -1, -1, -1 };
	enum PageClass class;
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		if (pages[i].presence_bit == false) {
			continue;
		}
		class = get_page_class(i);
		if (page_indexes[class] == -1) {
			page_indexes[class] = i;
			if (class == Class0) {
				break;
			}
		}
	}
	for (int i = 0; i < MAX_CLASS_COUNT; i++) {
		if (page_indexes[i] != -1) {
			return page_indexes[i];
		}
	}
	abort();
	return -1;
}

STATIC void pageout(int page_index)
{
	pthread_mutex_lock(&mutex);
	clear_page(page_index);
	pthread_mutex_unlock(&mutex);
}

STATIC void pagein(int page_frame_no, int page_index)
{
	pthread_mutex_lock(&mutex);
	pages[page_index].page_frame_no = page_frame_no;
	pages[page_index].presence_bit = true;
	pthread_mutex_unlock(&mutex);
}

STATIC int access_memory(struct ProgramCounter *pc)
{
	return fscanf(memory_trace, "%u%x%x", &pc->access, &pc->address,
		      &pc->machine_order) != EOF;
}

STATIC bool should_pageout(int page_frame_count, int used_page_frame_count)
{
	return page_frame_count <= used_page_frame_count;
}

STATIC bool is_page_fault(int page_index, int *page_fault_count)
{
	bool result = false;

	if (!pages[page_index].presence_bit) {
		(*page_fault_count)++;
		result = true;
	}
	return result;
}

STATIC int get_pageouted_page_index(void)
{
	int page_index = nru();
	return page_index;
}

int execute(int page_frame_count)
{
	static int page_fault_count;
	int page_frame_no;
	int page_index;
	int outed_page_index;
	int used_page_frame_count = 0;
	struct ProgramCounter pc;

	while (access_memory(&pc)) {
		page_index = GET_PAGE_INDEX(pc.address);
		if (is_page_fault(page_index, &page_fault_count)) {
			printf("PF %x\n", pc.address);
			if (should_pageout
			    (page_frame_count, used_page_frame_count)) {
				outed_page_index = get_pageouted_page_index();
				printf("PO %x@%d\n", outed_page_index,
				       pages[outed_page_index].page_frame_no);
				page_frame_no =
				    pages[outed_page_index].page_frame_no;
				pageout(outed_page_index);
				used_page_frame_count--;
			} else {
				page_frame_no = used_page_frame_count;
			}
			pagein(page_frame_no, page_index);
			used_page_frame_count++;
			printf("PI %x@%d\n", page_index,
			       pages[page_index].page_frame_no);
		}
		access_page(&pc, page_index);
	}
	return page_fault_count;
}
