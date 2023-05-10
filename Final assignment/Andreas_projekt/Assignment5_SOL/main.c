#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "key.h"
#include "FreeRTOS.h"

#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "task.h"
#include "gpio.h"
#include "systick.h"
#include "status_led.h"

//#include "tmodel.h"

//#include "systick.h"
#include "systick_frt.h"

//#include "rtc.h"
//#include "lcd.h"
#include "uart.h"
//#include "ui.h"
//#include "file.h"


#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3


QueueHandle_t q_keypad;
QueueHandle_t q_uart_tx;
QueueHandle_t q_uart_rx;

SemaphoreHandle_t mutex_uart_tx;
SemaphoreHandle_t mutex_uart_rx;
SemaphoreHandle_t keypad_mutex;





xTimerHandle xTimer_led_freq;
xTimerHandle xTimer_led_dur;


static void setupHardware(){
  init_gpio();
  led_init();

  uart0_init( 9600, 8, 1, 'n' );
  //init_files();
  //init_rtcs();
  keypad_init();


}

int main(void)
/*****************************************************************************
*   Input    : NONE
*   Output   : Returns 0
*   Function : Init hardware and then loop forever
******************************************************************************/
{
    setupHardware();

  //open_queue( Q_UART_TX );
  //open_queue( Q_UART_RX );
  //open_queue( Q_LCD );
  //open_queue( Q_KEY );

  //start_task( TASK_RTC, rtc_task );
  //start_task( TASK_DISPLAY_RTC, display_rtc_task );
  
  //start_task( TASK_UART_TX, uart_tx_task );
  //start_task( TASK_UART_RX, uart_rx_task );
  //start_task( TASK_UI, ui_task );
  xTaskCreate( uart_tx_task, "key_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
  xTaskCreate( uart_rx_task, "key_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
  xTaskCreate( key_task, "key_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
  //start_task( TASK_UI_KEY, ui_key_task );
  xTaskCreate( status_led_task, "alive_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

  vTaskStartScheduler();
  return( 0 );
}
