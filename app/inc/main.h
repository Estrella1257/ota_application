#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"
#include "led.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

extern const led_desc_t led0;
extern const led_desc_t led1;
extern const led_desc_t led2;

#define _STR(R) #R
#define STR(R)  _STR(R)

#define NMIN(a,b)   ((a) < (b) ? (a) : (b))
#define NMAX(a,b)   ((a) > (b) ? (a) : (b))

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x)[0])
#define OFFSET_OF(type, member) ((size_t) & ((type *)0)->member)

#define ROUND(x, y)     ((x) + (y) - 1) & ~((y) - 1)

#define TRY_CALL(f, ...) \
if (f != NULL)           \
    f(__VA_ARGS__)

#define CHECK_RET(x)    \
do {                    \
    if (!(x)) {	        \
        return;         \
    }                   \
} while (0)

#define CHECK_RETX(x,r) \
do {                    \
    if (!(x)) {	        \
        return (r);     \
    }                   \
} while (0)

#define CHECK_GO(x,e)   \
do {                    \
    if (!(x)) {	        \
        goto e;         \
    }                   \
} while (0)

#define __GET_VALUE_TEMPLATE(_nick, _type)      \
static inline _type get_##_nick(uint8_t *array) \
{                                               \
    return *(_type *)array;                     \
}

#define __GET_VALUE_INC_TEMPLATE(_nick, _type)         \
static inline _type get_##_nick##_inc(uint8_t **array) \
{                                                      \
    uint8_t *b = *array;                               \
    (*array) += sizeof(_type);                         \
    return get_##_nick(b);                             \
}

#define __PUT_VALUE_TEMPLATE(_nick, _type)                  \
static inline void put_##_nick(uint8_t *array, _type value) \
{                                                           \
    *(_type *)array = value;                                \
}

#define __PUT_VALUE_INC_TEMPLATE(_nick, _type)                     \
static inline void put_##_nick##_inc(uint8_t **array, _type value) \
{                                                                  \
    uint8_t *b = *array;                                           \
    (*array) += sizeof(_type);                                     \
    put_##_nick(b, value);                                         \
}

#define __VALUE_OPERATE_TEMPLATE(_micro) \
_micro(i8, int8_t)                       \
_micro(u8, uint8_t)                      \
_micro(i16, int16_t)                     \
_micro(u16, uint16_t)                    \
_micro(i32, int32_t)                     \
_micro(u32, uint32_t)                    \
_micro(float, float)                     \

__VALUE_OPERATE_TEMPLATE(__GET_VALUE_TEMPLATE)
__VALUE_OPERATE_TEMPLATE(__PUT_VALUE_TEMPLATE)
__VALUE_OPERATE_TEMPLATE(__GET_VALUE_INC_TEMPLATE)
__VALUE_OPERATE_TEMPLATE(__PUT_VALUE_INC_TEMPLATE)

void enable_irq(void);
void disable_irq(void);
bool is_irq(void);
bool is_cirtical(void);
bool is_user(void);
void cpu_reset(void);
uint32_t os_now(void);

void mem_init(void);
void* mem_malloc(size_t size);
void mem_free(void *ptr);

#endif