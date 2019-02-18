#ifndef H_INCLUDED_PAGINGSIM
#define H_INCLUDED_PAGINGSIM

#define EMPTY -1
#define GET_PAGE_INDEX(VIRTUAL_ADDRESS) ((VIRTUAL_ADDRESS) / (PAGE_SIZE))
#define MAX_CLASS_COUNT 4
#define MAX_PAGE_COUNT 1048576
#define PAGE_SIZE 4096
#define REFERENCE_BIT_CLEAR_USEC 20000

#include <pthread.h>
#include <stdbool.h>

enum AccessType {
	READ, WRITE, FETCH
};

enum PageClass {
	Class0, Class1, Class2, Class3
};

struct ProgramCounter {
	enum AccessType access;
	unsigned int address;
	unsigned int machine_order;
};

struct Page {
	bool modified_bit;
	bool presence_bit;
	bool reference_bit;
	int page_frame_no;
};

void initialize(char **argv, pthread_t * thread_id, int *page_frame_count);
void finalize(pthread_t thread_id);
int execute(int page_frame_count);

#endif
