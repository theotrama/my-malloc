#include "mm.h"
#include "memlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *heap_listp = NULL;
static char *free_listp = NULL;

/* Debug flag */
int debug_enabled = 0;

/* -------------------------------
   Internal helper function prototypes
   ------------------------------- */
static void *coalesce(void *ptr);
static void *extend_heap(size_t words);
static void place(char *bp, size_t asize);
static void *find_fit(size_t asize);
static void update_free_list(char *prevFree, char *nextFree);
static void insert_free_block(char *ptr);
static void remove_free_block(char *ptr);
static void print_list(void);
static void print_free_listp(void);


void print_list() {
	char *bp;
	int block_idx = 0;
	for (bp = heap_listp; !IS_EPLGP(bp); bp = NEXT_BLKP(bp)) {
		printf("Block %d: Address -> address=%p Header -> size=%u alloc=%u", block_idx, bp,
		       GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)) );
		printf(", Footer -> size=%u alloc=%u",
		       GET_SIZE(FTRP(bp)), GET_ALLOC(FTRP(bp)));
		printf(", 		Prev -> %p, Next -> %p\n",
		       GET_PTR(bp), GET_PTR(bp + DSIZE));
		block_idx++;
	}
	printf("Block %d: Address -> address=%p Header -> size=%u alloc=%u (EPILOGUE)\n", block_idx, bp,
	       GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
}

void print_free_listp() {
	char *bp = free_listp;
	printf("\n");
	for (bp = free_listp; bp != NULL; bp = NEXT_FREE(bp)) {
		printf("free_listp: Address -> address=%p Header -> size=%u alloc=%u Prev -> prev=%p Next -> next=%p\n", bp,
		       GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), PREV_FREE(bp), NEXT_FREE(bp));
	}
	printf("\n");
}


void insert_free_block(char *ptr) {
	PUT_PTR(ptr + DSIZE, free_listp);
	PUT_PTR(ptr, NULL);

	if (free_listp != NULL) {
		PUT_PTR(free_listp, ptr);
	}

	free_listp = ptr;
}

void remove_free_block(char *ptr) {
	char *prevFree = PREV_FREE(ptr);
	if (prevFree != NULL) {
		PUT_PTR(prevFree + DSIZE, NEXT_FREE(ptr));
	} else {
		free_listp = NEXT_FREE(ptr);
	}

	char *nextFree = NEXT_FREE(ptr);
	if (nextFree != NULL) {
		PUT_PTR(nextFree, prevFree);
	}
	PREV_FREE(ptr) = NULL;
	NEXT_FREE(ptr) = NULL;
}

void update_free_list(char *prevFree, char *nextFree) {

	if (prevFree != NULL && nextFree != NULL) {
		PUT_PTR(prevFree + DSIZE, nextFree);
		PUT_PTR(nextFree, prevFree);
	} else if (prevFree != NULL) {
		PUT_PTR(prevFree + DSIZE, NULL);
	} else if (nextFree != NULL) {
		free_listp = nextFree;
		PUT_PTR(nextFree, NULL);
	} else {
		free_listp = NULL;
	}
}

static void *coalesce(void *ptr) {
        size_t isPrevAlloc = GET_ALLOC(FTRP(PREV_BLKP(ptr)));
        size_t isNextAlloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
        size_t totalSize = GET_SIZE(HDRP(ptr));

	if (isPrevAlloc && isNextAlloc) {
		insert_free_block(ptr);
        } else if (isPrevAlloc && !isNextAlloc) {
            insert_free_block(ptr);
            remove_free_block(NEXT_BLKP(ptr));
            totalSize += GET_SIZE(HDRP(NEXT_BLKP(ptr)));
            PUT(HDRP(ptr), totalSize | 0x0);
            PUT(FTRP(ptr), totalSize | 0x0);
        } else if (!isPrevAlloc && isNextAlloc) {
            totalSize += GET_SIZE(HDRP(PREV_BLKP(ptr)));
            PUT(HDRP(PREV_BLKP(ptr)), totalSize | 0x0);
            PUT(FTRP(ptr), totalSize | 0x0);
            ptr = PREV_BLKP(ptr);;
        } else {
            remove_free_block(NEXT_BLKP(ptr));
            totalSize += GET_SIZE(HDRP(PREV_BLKP(ptr))) + GET_SIZE(FTRP(NEXT_BLKP(ptr)));
            PUT(HDRP(PREV_BLKP(ptr)), totalSize | 0x0);
            PUT(FTRP(NEXT_BLKP(ptr)), totalSize | 0x0);
            ptr = PREV_BLKP(ptr);
        }

        return ptr;
}


void mm_free(void *ptr) {
	if (debug_enabled) {
		printf("FREE\nmm_free %p\n", ptr);
		printf("State before free\n");
		print_list();
		print_free_listp();
	}
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));
	coalesce(ptr);
	if (debug_enabled) {
		printf("State after free\n");
		print_list();
		print_free_listp();
	}

}

static void* extend_heap(size_t words) {
	char *bp;
	size_t size;

	size = (words % 2) ? (words+1)*WSIZE : words * WSIZE;
	bp = mem_sbrk(size);

	// update free block
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));

	// add new epilogue header
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));

	// Coalesce if adjacent to next block
	return coalesce(bp);
}

// Initial setup method for our heap
int mm_init() {
	mem_init();

	heap_listp = mem_sbrk(4*WSIZE);
	// set up alignment, prologue, and epilogue blocks
	PUT(heap_listp, 0);
	PUT(heap_listp + 1 * WSIZE, PACK(8, 1));
	PUT(heap_listp + 2 * WSIZE, PACK(8, 1));
	PUT(heap_listp + 3 * WSIZE, PACK(0, 1));

	// Move the heap_listp to right position
	heap_listp += (2 * WSIZE);

	free_listp = extend_heap(CHUNKSIZE/WSIZE);
	PUT_PTR(free_listp, NULL);
	PUT_PTR(free_listp + DSIZE, NULL);

	char *bp = free_listp;
	printf("free_listp: Address -> address=%p Header -> size=%u alloc=%u Prev -> prev=%p Next -> next=%p\n", bp,
	       GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), PREV_FREE(bp), NEXT_FREE(bp));
	return 0;
}


void *find_fit(size_t asize) {
	char *bp;
	// Loop from the start of the heap list pointer
	for (bp = free_listp; bp != NULL ; bp = NEXT_FREE(bp)) {
		if ((!GET_ALLOC(HDRP(bp))) &&  (asize <= GET_SIZE(HDRP(bp)))) {
			return bp;
		}
	}
	return NULL;
}

void place(char* bp, size_t asize) {
	size_t csize = GET_SIZE(HDRP(bp));
	// for all sizes >= 24-bytes

	char *prevFree = PREV_FREE(bp);
	char *nextFree = NEXT_FREE(bp);
	if ((csize - asize) >= (3*DSIZE)) {

		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		bp = NEXT_BLKP(bp);
		nextFree = bp;
		update_free_list(prevFree, nextFree);

		PUT(HDRP(bp), PACK(csize - asize, 0));
		PUT(FTRP(bp), PACK(csize - asize, 0));
	} else {
		// The else statement is in case blocks fits EXACTLY
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));
		update_free_list(prevFree, nextFree);
	}
	PREV_FREE(bp) = NULL;
	NEXT_FREE(bp) = NULL;
}

void *mm_malloc(size_t size) {
	if (size == 0) {
		return NULL;
	}

	if (debug_enabled) {
		printf("MALLOC\nAllocate %zd-bytes of memory\n", size);
		printf("State before malloc\n");
		print_list();
		print_free_listp();
	}

	size_t asize;
	if (size <= 2*DSIZE)
		asize = 2*DSIZE + DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
	char *bp;
	size_t extendsize;
	if ((bp = find_fit(asize)) != NULL) {
		place(bp, asize);

		if (debug_enabled) {
			printf("State after malloc\n");
			print_list();
			print_free_listp();
		}

		return bp;
	}

	extendsize = MAX(asize, CHUNKSIZE);
	bp = extend_heap(extendsize/WSIZE);

	place(bp, asize);

	if (debug_enabled) {
		printf("State after malloc\n");
		print_list();
		print_free_listp();
	}
	return bp;
}

