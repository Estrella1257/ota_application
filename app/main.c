#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "console.h"
#include "shell.h"

static volatile uint8_t rx_data;
static Shell shell;
static char shell_buffer[512];

extern void board_lowlevel_init(void);

static void uart_rx_handler(uint8_t data)
{
	rx_data=data;
}

static signed short _shell_read(char *data,unsigned short len)
{
	if(rx_data != 0)
	{
		*data = rx_data;
		rx_data = 0;
		return 1;
	}
	return 0;
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

	shell.read = _shell_read;
	shell.write = _shell_write;
	shellInit(&shell,shell_buffer,sizeof(shell_buffer));

	while(1)
	{
		shellTask(&shell);
	}
}