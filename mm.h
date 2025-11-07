#ifndef MM_H
#define MM_H

#include <stddef.h>  // for size_t


#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<6) // 4 KB = 4096 bytes
#define MAX(x, y) ((x) > (y)? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))
#define GET_PTR(p) (*(void **)(p))
#define PUT_PTR(p, val) (*(void **)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP((char*)(bp))))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE( (char*)(bp) -DSIZE ))
#define PREV_FREE(bp) (*(char **)(char *)(bp))
#define NEXT_FREE(bp) (*(char **)(char *)(bp + DSIZE))
#define IS_EPLGP(bp) ( GET_SIZE(HDRP((char*)(bp))) == 0 )


/* -------------------------------
   Public allocator API
   ------------------------------- */
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);

/* Debug flag */
extern int debug_enabled;

#endif // MM_H

