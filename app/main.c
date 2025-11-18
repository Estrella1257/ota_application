#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static void blink_led(void *args)
{
	const led_desc_t *led=args;
	led_init(led);
	while (1)
	{
		led_on(led);
		vTaskDelay(200);
		led_off(led);
		vTaskDelay(200);		
	}
	
}

int main(void)
{
	xTaskCreate(blink_led,"blink_led",256,(void *)&led0,1,NULL);
	xTaskCreate(blink_led,"blink_led",256,(void *)&led1,1,NULL);
	xTaskCreate(blink_led,"blink_led",256,(void *)&led2,1,NULL);

	vTaskStartScheduler();
 
	return 0;
}

                                              
