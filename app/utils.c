#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void enable_irq(void)
{
    __enable_irq();         //启用全局中断（Cortex-M 内核函数）
}

void disable_irq(void)
{
    __disable_irq();        //禁用全局中断（Cortex-M 内核函数）
}

bool is_irq(void)
{
    return (__get_IPSR() != 0);         //判断是否在中断上下文
}

bool is_cirtical(void)
{
    return (__get_PRIMASK() != 0);      //判断是否在临界区
}

bool is_user(void)
{
    return !is_irq() && is_cirtical();          //判断是否在用户（任务）上下文
}
void cpu_reset(void)
{
    NVIC_SystemReset();
}

uint32_t os_now(void)
{
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    return now;
}