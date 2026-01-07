#ifndef MASTERTOOLS_ALLOCATOR_H_
#define MASTERTOOLS_ALLOCATOR_H_

#include "types.h"

#if !defined MT_MAX_ALLOCATIONS
#define MT_MAX_ALLOCATIONS 32
#endif

typedef struct mt_allocator mt_allocator;
struct mt_allocator
{
    void *buckets[MT_MAX_ALLOCATIONS];
    size_t prev;
};

mt_allocator create_allocator(void);

void *allocate_aligned(mt_allocator *alloc, size_t count, size_t align);

#define allocate(alloc, count) allocate_aligned(alloc, count, sizeof(void *))

void deallocate(void *ptr);

void free_allocator(mt_allocator *alloc);

#endif
