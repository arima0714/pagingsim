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
	bool is_modified = pages[page_index].is_modified;
	bool is_referenced = pages[page_index].is_referenced;

	if (!is_referenced && !is_modified) {
		class = Class0;
	} else if (!is_referenced && is_modified) {
		class = Class1;
	} else if (is_referenced && !is_modified) {
		class = Class2;
	} else {
		class = Class3;
	}
	pthread_mutex_unlock(&mutex);
	return class;
}

STATIC void access_page(struct ProgramCounter *pc, int page_index)
{
	pages[page_index].is_modified = pc->access == WRITE;
	pages[page_index].is_referenced = true;
}

STATIC void clear_page(int page_index)
{
	pages[page_index].is_modified = false;
	pages[page_index].page_frame_no = EMPTY;
	pages[page_index].is_present = false;
	pages[page_index].is_referenced = false;
}

STATIC void clear_all_is_referenced(void)
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_referenced = false;
	}
	pthread_mutex_unlock(&mutex);
}

STATIC void *interrupt_clear_is_referenced(void *p)
{
	while (true) {
		clear_all_is_referenced();
		usleep(REFERENCE_CLEAR_USEC);
	}
	return NULL;
}

void initialize(char **argv, pthread_t * thread_id, int *page_frame_count,
		pthread_attr_t * thread_attr)
{
	*page_frame_count = xatoi(argv[1]);
	if (*page_frame_count <= 0) {
		perror("Set PAGE_FRAME_COUNT more than 0.");
		exit(1);
	}
	memory_trace = xfopen(argv[2], "r");
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		clear_page(i);
	}
	pthread_mutex_init(&mutex, NULL);
	pthread_attr_init(thread_attr);
	pthread_attr_setdetachstate(thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(thread_id, thread_attr, &interrupt_clear_is_referenced,
		       NULL);
}

void finalize(pthread_t thread_id)
{
	free(pages);
	fclose(memory_trace);
	pthread_cancel(thread_id);
}

STATIC int nru(void)
{
	int page_indexes[MAX_CLASS_COUNT] = { -1, -1, -1, -1 };
	enum PageClass class;

	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		if (!pages[i].is_present) {
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
	pages[page_index].is_present = true;
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

STATIC bool is_page_fault(int page_index)
{
	return !pages[page_index].is_present;
}

STATIC int get_pageouted_page_index(void)
{
	int page_index = nru();
	return page_index;
}

int execute(int page_frame_count)
{
	int page_fault_count = 0;
	int page_frame_no;
	int page_index;
	int outed_page_index;
	int used_page_frame_count = 0;
	struct ProgramCounter pc;

	while (access_memory(&pc)) {
		page_index = GET_PAGE_INDEX(pc.address);
		if (is_page_fault(page_index)) {
			page_fault_count++;
			printf("PF %x\n", pc.address);
			if (should_pageout
			    (page_frame_count, used_page_frame_count)) {
				outed_page_index = get_pageouted_page_index();
				page_frame_no =
				    pages[outed_page_index].page_frame_no;
				printf("PO %x@%d\n", outed_page_index,
				       page_frame_no);
				pageout(outed_page_index);
				used_page_frame_count--;
			} else {
				// ページフレームはフレーム番号が小さいものから順に使われていくので、
				// それと同時に1ずつ増加するused_page_frame_countをpage_frame_noに入れて問題ない
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
