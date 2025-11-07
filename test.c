#include <stdio.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"

void test_scenario_zero() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: test_scenario_zero\n");
	void *p1 = mm_malloc(8);
	void *p2 = mm_malloc(32);
	void *p3 = mm_malloc(32);
	void *p4 = mm_malloc(48);
	void *p5 = mm_malloc(24);
	void *p6 = mm_malloc(28);
	void *p7 = mm_malloc(256);
	void *p8 = mm_malloc(64);
	mm_free(p6);
	mm_free(p7);
	mm_free(p8);
	mm_free(p2);
	mm_free(p4);
	mm_free(p3);
	mm_free(p5);


	mem_destroy();

}


void test_allocate_larger_than_current_heap_size() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: test_allocate_larger_than_current_heap_size\n");

	void *p1 = mm_malloc(1232);
	mem_destroy();
}

void test_malloc_bunch_of_heap() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: test_malloc_bunch_of_heap\n");
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);
	mm_malloc(1232);

	mem_destroy();
}


void test_malloc_small() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: test_malloc_small\n");
	mm_malloc(1);

	mem_destroy();
}

void test_malloc_and_free_case_prev_and_next_alloc() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: test_malloc_and_free_case_prev_and_next_alloc\n");
	void *p1 = mm_malloc(16);
	void *p2 = mm_malloc(16);
	void *p3 = mm_malloc(16);
	mm_free(p2);
	mem_destroy();
}


void test_malloc_and_free_case_prev_alloc_and_not_next_alloc() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: prevAlloc && !nextAlloc\n");
	void *p1 = mm_malloc(16);
	void *p2 = mm_malloc(16);
	void *p3 = mm_malloc(16);
	mm_free(p3);
	mm_free(p2);
	mem_destroy();
}


void test_malloc_and_free_case_not_prev_alloc_and_next_alloc() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: !prevAlloc && nextAlloc\n");
	void *p1 = mm_malloc(16);
	void *p2 = mm_malloc(16);
	void *p3 = mm_malloc(16);
	void *p4 = mm_malloc(16);
	mm_free(p2);
	mm_free(p3);
	mem_destroy();
}


void test_malloc_and_free_case_not_prev_alloc_and_not_next_alloc() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: !prevAlloc && !nextAlloc\n");
	void *p1 = mm_malloc(16);
	void *p2 = mm_malloc(64);
	void *p3 = mm_malloc(64);
	void *p4 = mm_malloc(32);
	void *p5 = mm_malloc(32);
	mm_free(p2);
	mm_free(p4);
	mm_free(p3);
	mem_destroy();
}

void test_free_next_to_prologue_and_epilogue() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: free next to prologue and epilogue\n");
	void *p1 = mm_malloc(56);
	mm_free(p1);
	mem_destroy();
}


void test_lots_of_malloc_and_free() {
	mm_init();

	printf("\n\n________________________________________\n");
	printf("TEST CASE: lots of malloc and free\n");
	void *p1 = mm_malloc(16);
	void *p2 = mm_malloc(64);
	void *p3 = mm_malloc(64);
	void *p4 = mm_malloc(32);
	void *p5 = mm_malloc(32);
	void *p6 = mm_malloc(32);
	void *p7 = mm_malloc(32);
	void *p8 = mm_malloc(32);
	void *p9 = mm_malloc(32);
	void *p10 = mm_malloc(32);
	void *p11 = mm_malloc(32);
	void *p12 = mm_malloc(32);
	void *p13 = mm_malloc(32);
	void *p14 = mm_malloc(32);
	void *p15 = mm_malloc(32);
	void *p16 = mm_malloc(32);
	void *p17 = mm_malloc(32);
	void *p18 = mm_malloc(32);
	mm_free(p2);
	mm_free(p4);
	mm_free(p3);
	mm_free(p18);
	mm_free(p10);
	mm_free(p11);
	mm_free(p12);
	mm_free(p13);
	mem_destroy();
}


int main(int argc, char **argv) {
	if (argc == 2 && strcmp(argv[1], "-d") == 0) {
		debug_enabled = 1;
	}
	test_scenario_zero();
	test_allocate_larger_than_current_heap_size();
	test_malloc_bunch_of_heap();
	test_malloc_small();
	test_malloc_and_free_case_prev_and_next_alloc();
	test_malloc_and_free_case_prev_alloc_and_not_next_alloc();
	test_malloc_and_free_case_not_prev_alloc_and_not_next_alloc();
	test_malloc_and_free_case_not_prev_alloc_and_not_next_alloc();
	test_free_next_to_prologue_and_epilogue();
	test_lots_of_malloc_and_free();
	return 0;
}
