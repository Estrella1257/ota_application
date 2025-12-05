#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "console.h"

extern void board_lowlevel_init(void);
extern void app_shell_init(void);
//extern void app_logger_init(void);


static void component_init(void)
{
    console_init();
    app_shell_init();
    app_logger_init();
}

static void application_init(void)
{

}

static void sys_init(void *args)
{
    component_init();
    application_init();

    vTaskDelete(NULL);
}

void main(void)
{
    board_lowlevel_init();

    xTaskCreate(sys_init, "sysinit", 1024, NULL, configMAX_PRIORITIES - 1, NULL);

    vTaskStartScheduler();
}