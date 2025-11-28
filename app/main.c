#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "console.h"
#include "shell.h"
#include "ringbuffer8.h"

static uint8_t rx_buff[128];
static ringbuffer8_t rxrb;
static Shell shell;
static char shell_buffer[512];

extern void board_lowlevel_init(void);

static void uart_rx_handler(uint8_t data)
{
	if (!rb8_full(rxrb))
	{
		rb8_put(rxrb, data);
	}
	
}

static signed short _shell_write(char *data,unsigned short len)
{
	for(unsigned short i = 0; i < len; i++)
	{
		uart_send((uint8_t)data[i]);
	}
	return len;
}

int main(void)
{
	board_lowlevel_init();
	uart_init();
	uart_recv_callback_register(uart_rx_handler);

	rxrb = rb8_new(rx_buff,sizeof(rx_buff));

	shell.write = _shell_write;
	shellInit(&shell,shell_buffer,sizeof(shell_buffer));

	uint8_t rxdata;
	while(1)
	{
		if (!rb8_empty(rxrb))
		{
			rb8_get(rxrb,&rxdata);
			shellHandler(&shell,rxdata);
		}
	}
}