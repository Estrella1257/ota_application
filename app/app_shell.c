#include <stdbool.h>
#include <stdint.h>
#include "console.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static Shell shell;
static char shell_buffer[512];

static QueueHandle_t shell_rx_queue;

static void shell_rx_handler(uint8_t data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;          // 用于记录是否有更高优先级的任务被唤醒
    xQueueSendFromISR(shell_rx_queue, &data, &xHigherPriorityTaskWoken);

    // 如果发送队列的操作导致一个更高优先级的任务（即 app_shell_task）从阻塞态变为就绪态，则执行上下文切换，立即运行该更高优先级的任务
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static signed short _shell_write(char *data,unsigned short len)
{
    console_write(data, len);
	return len;
}

static void app_shell_task(void *param)
{
    uint8_t rxdata;

    shell.write = _shell_write;
	shellInit(&shell,shell_buffer,sizeof(shell_buffer));
    
    while(1)
    {
        if (xQueueReceive(shell_rx_queue, &rxdata, portMAX_DELAY))
        {
            shellHandler(&shell, rxdata);
        }
    }
}

void app_shell_init(void)
{
    shell_rx_queue = xQueueCreate(128, sizeof(uint8_t));
    configASSERT(shell_rx_queue);           // 断言：确保队列创建成功



    console_recv_callback_register(shell_rx_handler);

    xTaskCreate(app_shell_task, "shell", 512, NULL, 1, NULL);
}