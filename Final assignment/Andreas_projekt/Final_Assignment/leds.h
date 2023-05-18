/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.h
*
* PROJECT....: ECP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 090215  MoH   Module created.
*
*****************************************************************************/

#ifndef LEDS_H_
#define LEDS_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define LED_GREEN   0
#define LED_YELLOW  1
#define LED_RED     2
#define LED_STATUS  3

#define LED_OFF     0
#define LED_ON      1
#define LED_TOGGLE  2
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void led_init();
/*
// To setup up timers the following must be inserted in the 'FreeRTOSConfig.h'
// file found under frt10/port/TivaM4/FreeRTOSConfig.h
#define configUSE_TIMERS 1
#define configTIMER_SERVICE_TASK_NAME "Tmr Svc"
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2)
#define configTIMER_QUEUE_LENGTH 10

And include:
#include "timers.h"

// xTimer variables for main:
xTimerHandle xTimer_led_freq;
xTimerHandle xTimer_led_dur;
*/

BOOLEAN led_flashing(INT8U led, float frequency_hz, INT8U duration_sec);
/*****************************************************************************
*   Input    : Enter which led color, then frequency in Hz, then the duration in seconds
*   Output   : Outputs true when flashing is over
*   Function : Flashes desired led with a given frequency for a given duration
*   Example use:
    // Flash red led at a frequency of 10Hz for 5s
    led_flashing(LED_RED,10,5);
******************************************************************************/


/****************************** End Of Module *******************************/
#endif /*LEDS_H_*/
