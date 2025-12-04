#include <stddef.h>
#include "main.h"

void *malloc(size_t size)
{
    return mem_malloc(size);
}

void free(void *ptr)
{
    mem_free(ptr);
}