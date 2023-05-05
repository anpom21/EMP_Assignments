#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "tmodel.h"
#include "SPI.h"
#include "uart0.h"
#include "debug.h"
#include "status_led.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "file.h"
#include "queue.h"
#include "lcd.h"
#include "semphr.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3

// Create queues
QueueHandle_t q_uart_tx;
QueueHandle_t q_uart_rx;
QueueHandle_t q_lcd;


SemaphoreHandle_t lcd_mutex;


static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  init_gpio();
  uart0_init( 9600, 8, 1, 'n' );
  init_files();
  SPI_init();
  lcd_init();


}


int main(void)
{
    setupHardware();
    xTaskCreate( lcd_hello, "lcd_hello", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( lcd_task, "lcd_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( uart_tx_task, "uart_tx_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( uart_rx_task, "uart_rx_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( SPI_task, "SPI_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( status_led_task, "alive_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    vTaskStartScheduler();
	return 0;
}
