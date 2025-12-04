#include <stdbool.h>
#include <stdint.h>
#include "tlsf.h"

static uint8_t dyl_mem_pool[1024 * 64];
tlsf_t tlsf;

void mem_init(void)
{
    tlsf = tlsf_create_with_pool(dyl_mem_pool, sizeof(dyl_mem_pool));
    if (tlsf == NULL)
    {
        while(1);
    }
}

void* mem_malloc(size_t size)
{
    return tlsf_malloc(tlsf, size);
}

void mem_free(void *ptr)
{
    tlsf_free(tlsf, ptr);
}