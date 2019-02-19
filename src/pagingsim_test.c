#include "pagingsim.h"
#include "util.h"
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>

extern struct Page *pages;
int page_fault_count;

bool should_pageout(int page_frame_count, int used_page_frame_count);
void clear_page(int page_index);
void clear_all_is_referenced(void);
bool is_page_fault(int page_index, int *page_fault_count);
void pagein(int page_frame_no, int page_index);
void pageout(int page_index);
void access_page(struct ProgramCounter *pc, int page_index);
enum PageClass get_page_class(int page_index);
int nru(void);
int get_pageouted_page_index(void);

void should_pageout_test_000(void)
{
	int page_frame_count = 0;
	int used_page_frame_count = 0;
	CU_ASSERT(should_pageout(page_frame_count, used_page_frame_count));
}

void should_pageout_test_001(void)
{
	int page_frame_count = 0;
	int used_page_frame_count = 1;
	CU_ASSERT(should_pageout(page_frame_count, used_page_frame_count));
}

void should_pageout_test_002(void)
{
	int page_frame_count = 1;
	int used_page_frame_count = 0;
	CU_ASSERT(!should_pageout(page_frame_count, used_page_frame_count));
}

void should_pageout_test_003(void)
{
	int page_frame_count = 1;
	int used_page_frame_count = 1;
	CU_ASSERT(should_pageout(page_frame_count, used_page_frame_count));
}

void clear_all_is_referenced_000(void)
{
	bool check = true;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	clear_all_is_referenced();
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		if (pages[i].is_referenced == true) {
			check = false;
		}
	}
	CU_ASSERT(check == true);
}

void clear_page_000(void)
{
	bool check = true;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	clear_page(0);
	if (pages[0].is_modified != false) {
		check = false;
	} else if (pages[0].page_frame_no != EMPTY) {
		check = false;
	} else if (pages[0].is_present != false) {
		check = false;
	} else if (pages[0].is_referenced != false) {
		check = false;
	}
	CU_ASSERT(check);

}

void clear_page_001(void)
{
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	clear_page(1);
	CU_ASSERT(pages[1].is_modified == false
		  && pages[1].page_frame_no == EMPTY
		  && pages[1].is_present == false
		  && pages[1].is_referenced == false);
}

void clear_page_002(void)
{
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	clear_page(2);
	CU_ASSERT(pages[2].is_modified == false
		  && pages[2].page_frame_no == EMPTY
		  && pages[2].is_present == false
		  && pages[2].is_referenced == false);
}

void clear_page_003(void)
{
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	clear_page(3);
	CU_ASSERT(pages[3].is_modified == false
		  && pages[3].page_frame_no == EMPTY
		  && pages[3].is_present == false
		  && pages[3].is_referenced == false);
}

void is_page_fault_000(void)
{
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	page_fault_count = 0;
	pages[0].is_present = 1;
	CU_ASSERT(is_page_fault(0, &page_fault_count) == false);
}

void is_page_fault_001(void)
{
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	page_fault_count = 0;
	pages[0].is_present = 0;
	CU_ASSERT(is_page_fault(0, &page_fault_count) == true);
}

void pagein_000(void)
{
	int test_pageindex = 0;
	int test_page_frame_no = 1;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_pageindex].page_frame_no = 0;
	pages[test_pageindex].is_present = false;
	pagein(test_page_frame_no, test_pageindex);
	CU_ASSERT(pages[test_pageindex].page_frame_no == test_page_frame_no
		  && pages[test_pageindex].is_present == true);
}

void pagein_001(void)
{
	int test_pageindex = 2;
	int test_page_frame_no = 3;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_pageindex].page_frame_no = 0;
	pages[test_pageindex].is_present = false;
	pagein(test_page_frame_no, test_pageindex);
	CU_ASSERT(pages[test_pageindex].page_frame_no == test_page_frame_no
		  && pages[test_pageindex].is_present == true);
}

void pageout_000(void)
{
	bool check = true;
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	pageout(test_page_index);
	if (pages[test_page_index].is_modified != false) {
		check = false;
	} else if (pages[test_page_index].page_frame_no != EMPTY) {
		check = false;
	} else if (pages[test_page_index].is_present != false) {
		check = false;
	} else if (pages[test_page_index].is_referenced != false) {
		check = false;
	}
	CU_ASSERT(check);
}

void pageout_001(void)
{
	bool check = true;
	int test_page_index = 1;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	pageout(test_page_index);
	if (pages[test_page_index].is_modified != false) {
		check = false;
	} else if (pages[test_page_index].page_frame_no != EMPTY) {
		check = false;
	} else if (pages[test_page_index].is_present != false) {
		check = false;
	} else if (pages[test_page_index].is_referenced != false) {
		check = false;
	}
	CU_ASSERT(check);
}

void access_page_000(void)
{
	struct ProgramCounter pc;
	pc.address = 0;
	pc.machine_order = 0;
	pc.access = WRITE;
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	access_page(&pc, test_page_index);
	CU_ASSERT((pages[test_page_index].is_modified == (pc.access == WRITE)
		   && (pages[test_page_index].is_referenced == true)));
}

void access_page_001(void)
{
	struct ProgramCounter pc;
	pc.address = 0;
	pc.machine_order = 0;
	pc.access = READ;
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	access_page(&pc, test_page_index);
	CU_ASSERT((pages[test_page_index].is_modified == (pc.access == WRITE)
		   && (pages[test_page_index].is_referenced == true)));
}

void access_page_002(void)
{
	struct ProgramCounter pc;
	pc.address = 0;
	pc.machine_order = 0;
	pc.access = FETCH;
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_modified = 2;
		pages[i].page_frame_no = -1;
		pages[i].is_present = 2;
		pages[i].is_referenced = 2;
	}
	access_page(&pc, test_page_index);
	CU_ASSERT((pages[test_page_index].is_modified == (pc.access == WRITE)
		   && (pages[test_page_index].is_referenced == true)));
}

void get_page_class_000(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_page_index].is_modified = false;
	pages[test_page_index].is_referenced = false;
	pages[test_page_index].is_present = true;
	CU_ASSERT(get_page_class(test_page_index) == 0);
}

void get_page_class_001(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_page_index].is_modified = true;
	pages[test_page_index].is_referenced = false;
	pages[test_page_index].is_present = true;
	CU_ASSERT(get_page_class(test_page_index) == 1);
}

void get_page_class_002(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_page_index].is_modified = false;
	pages[test_page_index].is_referenced = true;
	pages[test_page_index].is_present = true;
	CU_ASSERT(get_page_class(test_page_index) == 2);
}

void get_page_class_003(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	pages[test_page_index].is_modified = true;
	pages[test_page_index].is_referenced = true;
	pages[test_page_index].is_present = true;
	CU_ASSERT(get_page_class(test_page_index) == 3);
}

void nru_000(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = false;
			pages[i].is_referenced = false;
		}
	}
	CU_ASSERT(nru() == test_page_index);
}

void nru_001(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = true;
			pages[i].is_referenced = false;
		}
	}
	CU_ASSERT(nru() == test_page_index);
}

void nru_002(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = false;
			pages[i].is_referenced = true;
		}
	}
	CU_ASSERT(nru() == test_page_index);
}

void nru_003(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		pages[i].is_referenced = true;
		pages[i].is_modified = true;
	}
	CU_ASSERT(nru() == test_page_index);
}

void get_pageouted_page_index_000(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = false;
			pages[i].is_referenced = false;
		}
	}
	CU_ASSERT(get_pageouted_page_index() == test_page_index);
}

void get_pageouted_page_index_001(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = true;
			pages[i].is_referenced = false;
		}
	}
	CU_ASSERT(get_pageouted_page_index() == test_page_index);
}

void get_pageouted_page_index_002(void)
{
	int test_page_index = MAX_PAGE_COUNT / 2;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		if (i != test_page_index) {
			pages[i].is_modified = true;
			pages[i].is_referenced = true;
		} else {
			pages[i].is_modified = false;
			pages[i].is_referenced = true;
		}
	}
	CU_ASSERT(get_pageouted_page_index() == test_page_index);
}

void get_pageouted_page_index_003(void)
{
	int test_page_index = 0;
	pages = xmalloc((sizeof(struct Page) * MAX_PAGE_COUNT));
	for (int i = 0; i < MAX_PAGE_COUNT; i++) {
		pages[i].is_present = true;
		pages[i].is_referenced = true;
		pages[i].is_modified = true;
	}
	CU_ASSERT(get_pageouted_page_index() == test_page_index);
}

int main()
{
	CU_pSuite should_pageout;
	CU_initialize_registry();

	should_pageout = CU_add_suite("should_pageout", NULL, NULL);
	CU_add_test(should_pageout, "should_pageout_test_000",
		    should_pageout_test_000);
	CU_add_test(should_pageout, "should_pageout_test_001",
		    should_pageout_test_001);
	CU_add_test(should_pageout, "should_pageout_test_002",
		    should_pageout_test_002);
	CU_add_test(should_pageout, "should_pageout_test_003",
		    should_pageout_test_003);

	CU_pSuite clear_all_is_referenced;
	clear_all_is_referenced =
	    CU_add_suite("clear_all_is_referenced", NULL, NULL);
	CU_add_test(clear_all_is_referenced, "clear_all_is_referenced_000",
		    clear_all_is_referenced_000);

	CU_pSuite clear_page;
	clear_page = CU_add_suite("clear_page", NULL, NULL);
	CU_add_test(clear_page, "clear_page_000", clear_page_000);
	CU_add_test(clear_page, "clear_page_001", clear_page_001);
	CU_add_test(clear_page, "clear_page_002", clear_page_002);
	CU_add_test(clear_page, "clear_page_003", clear_page_003);

	CU_pSuite pagein;
	pagein = CU_add_suite("pagein", NULL, NULL);
	CU_add_test(pagein, "pagein_000", pagein_000);
	CU_add_test(pagein, "pagein_001", pagein_001);

	CU_pSuite is_page_fault;
	is_page_fault = CU_add_suite("is_page_fault", NULL, NULL);
	CU_add_test(is_page_fault, "is_page_fault_000", is_page_fault_000);
	CU_add_test(is_page_fault, "is_page_fault_001", is_page_fault_001);

	CU_pSuite pageout;
	pageout = CU_add_suite("pageout", NULL, NULL);
	CU_add_test(pageout, "pageout_000", pageout_000);
	CU_add_test(pageout, "pageout_001", pageout_001);

	CU_pSuite access_page;
	access_page = CU_add_suite("access_page", NULL, NULL);
	CU_add_test(access_page, "access_page_000", access_page_000);
	CU_add_test(access_page, "access_page_001", access_page_001);
	CU_add_test(access_page, "access_page_002", access_page_002);

	CU_pSuite get_page_class;
	get_page_class = CU_add_suite("get_page_class", NULL, NULL);
	CU_add_test(get_page_class, "get_page_class_000", get_page_class_000);
	CU_add_test(get_page_class, "get_page_class_001", get_page_class_001);
	CU_add_test(get_page_class, "get_page_class_002", get_page_class_002);
	CU_add_test(get_page_class, "get_page_class_003", get_page_class_003);

	CU_pSuite nru;
	nru = CU_add_suite("nru", NULL, NULL);
	CU_add_test(nru, "nru_000", nru_000);
	CU_add_test(nru, "nru_001", nru_001);
	CU_add_test(nru, "nru_002", nru_002);
	CU_add_test(nru, "nru_003", nru_003);

	CU_pSuite get_pageouted_page_index;
	get_pageouted_page_index =
	    CU_add_suite("get_pageouted_page_index", NULL, NULL);
	CU_add_test(get_pageouted_page_index, "get_pageouted_page_index_000",
		    get_pageouted_page_index_000);
	CU_add_test(get_pageouted_page_index, "get_pageouted_page_index_001",
		    get_pageouted_page_index_001);
	CU_add_test(get_pageouted_page_index, "get_pageouted_page_index_002",
		    get_pageouted_page_index_002);
	CU_add_test(get_pageouted_page_index, "get_pageouted_page_index_003",
		    get_pageouted_page_index_003);

	CU_console_run_tests();
	CU_cleanup_registry();

	return 0;
}
