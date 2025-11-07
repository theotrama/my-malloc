#ifndef MEMLIB_H
#define MEMLIB_H

#include <stddef.h>

#define MAX_HEAP 32768  // 32 KB example heap size

extern char *mem_start;
extern char *mem_brk;
extern char *mem_max_address;
extern char *real_mem_start;

void mem_init(void);
void *mem_sbrk(int incr);
void mem_destroy(void);

#endif // MEMLIB_H

