#include "memlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

char *mem_start = NULL;
char *mem_brk = NULL;
char *mem_max_address = NULL;
char *real_mem_start = NULL;

void mem_init(void) {
	real_mem_start = mmap(NULL, MAX_HEAP, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (real_mem_start == MAP_FAILED) {
		fprintf(stderr, "mem_init: mmap failed.\n");
		exit(1);
	}
	mem_start = real_mem_start;
	mem_brk = (char*)mem_start;
	mem_max_address = (char*)(mem_start + MAX_HEAP);
}

void mem_destroy(void) {
	if (real_mem_start != NULL) {
		if (munmap(mem_start, MAX_HEAP) != 0) {
            		perror("mem_destroy: munmap failed");
        	}
	}
	real_mem_start = NULL;
	mem_start = NULL;
	mem_brk = NULL;
	mem_max_address = NULL;
}

void *mem_sbrk(int incr) {

	char *old_brk = mem_brk;

	if ((incr < 0) || ((mem_brk + incr) > mem_max_address)) {
		printf("Ran out of memory...\n");
		exit(-1);
	}
	mem_brk += incr;
	return (void*)old_brk;
}
