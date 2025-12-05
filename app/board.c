#include "stm32f4xx.h"
#include "led.h"
#include "main.h"

void board_lowlevel_init(void)
{
    SCB->VTOR = 0x08010000;

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);

    mem_init();
}

const led_desc_t led0 =
{
    .clk_sourse = RCC_AHB1Periph_GPIOE,
    .port = GPIOE,
    .pin = GPIO_Pin_5,
    .on_lvl = Bit_RESET,
    .off_lvl = Bit_SET,
};

const led_desc_t led1 =
{
    .clk_sourse = RCC_AHB1Periph_GPIOE,
    .port = GPIOE,
    .pin = GPIO_Pin_6,
    .on_lvl = Bit_RESET,
    .off_lvl = Bit_SET,
};

const led_desc_t led2 =
{
    .clk_sourse = RCC_AHB1Periph_GPIOC,
    .port = GPIOC,
    .pin = GPIO_Pin_13,
    .on_lvl = Bit_RESET,
    .off_lvl = Bit_SET,
};