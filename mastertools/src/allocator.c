#include "allocator.h"

#include "logging.h"
#include "simd/rvm.h"
#include "types.h"

#include <immintrin.h>
#include <stdlib.h>
#include <string.h>

typedef struct header header;
struct header
{
    size_t count;
    bool free;
};

mt_allocator create_allocator(void)
{
    mt_allocator alloc;
    memset(alloc.buckets, 0, MT_MAX_ALLOCATIONS * sizeof(void *));
    alloc.prev = 0;
    return alloc;
}

static inline header *get_header(void *bucket)
{
    return ((header *)bucket) - 1;
}

static inline void *try_allocate(mt_allocator *alloc, void **it, size_t count, size_t align)
{
    void *bucket = *it;
    if (!bucket)
    {
        bucket = _mm_malloc(count + sizeof(header), align);
        header *head = bucket;
        head->count = count;
        head->free = false;

        alloc->prev += 1;
        return head + 1;
    }

    header *head = get_header(bucket);
    if (head->free)
    {
        if (head->count < count)
        {
            _mm_free(head);
            head = _mm_malloc(count + sizeof(header), align);
            head->count = count;
            *it = head + 1;
        }

        head->free = false;
        alloc->prev += 1;
        return *it;
    }

    return NULL;
}

void *allocate_aligned(mt_allocator *alloc, size_t count, size_t align)
{
    for (size_t i = alloc->prev; i < MT_MAX_ALLOCATIONS; ++i)
    {
        void *ptr = try_allocate(alloc, alloc->buckets + i, count, align);
        if (ptr)
        {
            return ptr;
        }
    }

    for (size_t i = 0; i < alloc->prev; ++i)
    {
        void *ptr = try_allocate(alloc, alloc->buckets + i, count, align);
        if (ptr)
        {
            return ptr;
        }
    }

    LERROR("Maximum allocation count of %d exceeded.", MT_MAX_ALLOCATIONS);
    abort(); // maybe later add recovery logic
}

// pointer must have originated from 'allocate'
void deallocate(void *ptr)
{
    get_header(ptr)->free = true;
}

void free_allocator(mt_allocator *alloc)
{
    for (int i = 0; i < MT_MAX_ALLOCATIONS; ++i)
    {
        void *bucket = alloc->buckets[i];
        if (bucket)
        {
            header *header = get_header(bucket);
            _mm_free(header);
        }
    }
}
