#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "SPI.h"
#include "adc.h"
#include "clock.h"
#include "file.h"
#include "gpio.h"
#include "icecream.h"
#include "key.h"
#include "lcd.h"
#include "leds.h"
#include "rotary_encoder.h"
#include "status_led.h"
#include "tmodel.h"
#include "uart0.h"

// FREE-RTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "systick_frt.h"
#include "task.h"
#include "timers.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO 1
#define MED_PRIO 2
#define HIGH_PRIO 3

#define UART_BITS 9

QueueHandle_t q_uart_tx;
QueueHandle_t q_uart_rx;
QueueHandle_t q_lcd;
QueueHandle_t q_key;
QueueHandle_t q_keypad;

SemaphoreHandle_t keypad_mutex;
SemaphoreHandle_t lcd_mutex;

TaskHandle_t Tflavour;
TaskHandle_t Tlocked;
TaskHandle_t TchooseAmount;
TaskHandle_t Tliquid;
TaskHandle_t TplaceCup;
TaskHandle_t Tproducing;
TaskHandle_t Tenc;
TaskHandle_t Tpot;
TaskHandle_t Tkey;

TaskHandle_t Tlogging;
TaskHandle_t Tuart_rx;
TaskHandle_t Tuart_tx;

xTimerHandle xTimer_led_freq;
xTimerHandle xTimer_led_dur;

static void setupHardware(void)
/*****************************************************************************
 *   Input    :  -
 *   Output   :  -
 *   Function :
 *****************************************************************************/
{
    // TODO: Put hardware configuration and initialisation in here

    // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
    // enc_init();
    init_adc();
    init_files();
    init_gpio();
    init_systick();
    keypad_init();
    lcd_init();
    led_init();
    uart0_init(9600, UART_BITS, 1, 'n');
    rotary_enc_init();
    // setup queues
}

void suspend_task(void *pvParameters)
{
    // vTaskSuspend(Tkey);
    // vTaskSuspend(Tuart_rx);
    // vTaskSuspend(Tuart_tx);
    // vTaskSuspend(Tenc);
    vTaskSuspend(Tflavour);
    vTaskSuspend(Tliquid);
    vTaskSuspend(TchooseAmount);
    vTaskSuspend(TplaceCup);
    vTaskSuspend(Tproducing);
    vTaskSuspend(NULL);
}

int main(void)
{
    setupHardware();
    // xTaskCreate( SPI_task, "SPI_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate(keypad_task, "keypad_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tkey);
    xTaskCreate(lcd_task, "lcd_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate(status_led_task, "alive_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate(uart_rx_task, "uart_rx_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tuart_rx);
    xTaskCreate(uart_tx_task, "uart_tx_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tuart_tx);
    xTaskCreate(rotary_enc_task, "enc_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tenc);

    xTaskCreate(locked_task, "locked_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tlocked);
    xTaskCreate(flavour_task, "flavour_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tflavour);
    xTaskCreate(liquid_task, "liquid_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tliquid);
    xTaskCreate(choose_amount_task, "amount", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &TchooseAmount);
    xTaskCreate(place_cup_task, "place_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &TplaceCup);
    xTaskCreate(producing_task, "producing_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tproducing);
    xTaskCreate(clock_task, "clock_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);

    xTaskCreate(suspend_task, "susp_task", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);
    // xTaskCreate( logging_task, "logging_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, &Tlogging);
    // vTaskSuspend(Tlogging);

    // xTaskCreate( icecream_task, "icecream_task", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

    vTaskStartScheduler();
    return 0;
}
