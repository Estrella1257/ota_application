#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f4xx.h"
#include "console.h"
 
static SemaphoreHandle_t tx_busy_mux;
static SemaphoreHandle_t tx_done_sem;
static console_rx_callback_t rx_callback;

static void uart_pin_init(void)
{
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0,sizeof(GPIO_InitTypeDef));

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
}

static void uart_lowlevel_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	memset(&USART_InitStructure,0,sizeof(USART_InitTypeDef));

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART1,ENABLE);

}

static void uart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	memset(&NVIC_InitStructure,0,sizeof(NVIC_InitTypeDef));

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	NVIC_SetPriority(USART1_IRQn,7);

	memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority(DMA2_Stream7_IRQn, 7);
}

static void uart_dma_init(void)
{    
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA2_Stream7, DISABLE);
}

void console_init(void)
{
	tx_busy_mux = xSemaphoreCreateMutex();
	configASSERT(tx_busy_mux);

	tx_done_sem = xSemaphoreCreateBinary();
	configASSERT(tx_done_sem);

	uart_pin_init();
	uart_lowlevel_init();
	uart_nvic_init();
	uart_dma_init();
}

void console_write(uint8_t *data, uint16_t length)
{
	xSemaphoreTake(tx_busy_mux, portMAX_DELAY);

	// for (uint16_t i = 0; i < length; i++)
    // {
    //     USART_SendData(USART1, data[i]);
    //     while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    // }

	DMA2_Stream7->M0AR = (uint32_t)data;
	DMA2_Stream7->NDTR = length;
	DMA_Cmd(DMA2_Stream7, ENABLE);

	xSemaphoreTake(tx_done_sem, portMAX_DELAY);

	xSemaphoreGive(tx_busy_mux);
}

void console_recv_callback_register(console_rx_callback_t cb)
{
    rx_callback = cb;
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t data = USART_ReceiveData(USART1);
        if(rx_callback)
        {
            rx_callback(data);
        }
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void DMA2_Stream7_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET)              //注意函数名Flag跟IT的区别
	{
		DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
		DMA_Cmd(DMA2_Stream7,DISABLE);

		BaseType_t xHigherPriorityTaskWoken;
		xSemaphoreGiveFromISR(tx_done_sem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	
}