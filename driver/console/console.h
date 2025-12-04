#ifndef __CONSOLE_UART_H__
#define __CONSOLE_UART_H__

#include <stdint.h>

typedef void (*console_rx_callback_t)(uint8_t data);
 
void console_init(void);
void console_write(uint8_t *data, uint16_t length);
void console_recv_callback_register(console_rx_callback_t cb);

#endif