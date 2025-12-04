#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"
#include "led.h"
#include <stddef.h>

extern const led_desc_t led0;
extern const led_desc_t led1;
extern const led_desc_t led2;

void mem_init(void);
void* mem_malloc(size_t size);
void mem_free(void *ptr);

#endif