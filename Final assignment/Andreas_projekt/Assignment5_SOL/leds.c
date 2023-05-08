/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 090215  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "leds.h"
#include "FreeRTOS.h"
#include "projdefs.h"
#include "timers.h"

/******************************* Defines ***********************************/

#define TIMER_IDLE      0
#define TIMER_RUNNING   1
#define TIMER_DONE      2
/***************************** Variables ***********************************/
extern xTimerHandle xTimer_led_freq;
extern xTimerHandle xTimer_led_dur;

INT8U global_led;
INT8U timer_state = TIMER_IDLE;
/***************************** Functions ***********************************/
static void led_freq_callback(xTimerHandle pxTimer){

  turn_led(global_led,LED_TOGGLE);

};



static void led_dur_callback(xTimerHandle pxTimer){
  xTimerStop(xTimer_led_freq,10);
  turn_led( global_led, LED_OFF );
  timer_state = TIMER_DONE;
};



void led_init(){
  // Initialize frequency timer
  xTimer_led_freq = xTimerCreate(
  "timer_frequency", /* name */
  pdMS_TO_TICKS(200), /* period/time */
  pdTRUE, /* auto reload */
  (void*)1, /* timer ID */
  led_freq_callback); /* callback */

  // Initialize duration timer
  xTimer_led_dur = xTimerCreate(
  "timer_duration", /* name */
  pdMS_TO_TICKS(5000), /* period/time */
  pdFALSE, /* auto reload */
  (void*)1, /* timer ID */
  led_dur_callback); /* callback */
  
  // Turn ON all LEDs
  turn_led( LED_RED, LED_ON );
  turn_led( LED_YELLOW, LED_ON );
  turn_led( LED_GREEN, LED_ON );

}




BOOLEAN led_flashing(INT8U led, float frequency_hz, INT8U duration_sec){
  BOOLEAN result = 0;

  global_led = led;

  // Check if timer is idle
  if(timer_state == TIMER_IDLE){
    // Change time period to the entered frequency
    xTimerChangePeriod(xTimer_led_freq,pdMS_TO_TICKS(1000 * ( 1/frequency_hz )),10);

    // Change time period to the entered duration
    xTimerChangePeriod(xTimer_led_dur,pdMS_TO_TICKS(1000 * duration_sec),10);

    // Start timers
    xTimerStart(xTimer_led_freq, 10);
    xTimerStart(xTimer_led_dur, 10);

    // Change timer state
    timer_state = TIMER_RUNNING;
  }else if (timer_state == TIMER_DONE)
  {
    result = 1;
    timer_state = TIMER_IDLE;
  }
  return result;

}



BOOLEAN turn_led( name, action )
INT8U name;
INT8U action;
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  BOOLEAN Result;

  switch( name )
  {
    case LED_GREEN:
	  switch( action )
	  {
	    case LED_ON:
	      GPIO_PORTF_DATA_R &= 0xF7;
	      break;
	    case LED_OFF:
	      GPIO_PORTF_DATA_R |= 0x08;
    	  break;
	    case LED_TOGGLE:
	      GPIO_PORTF_DATA_R ^= 0x08;
	      break;
	  }
	  Result = !(GPIO_PORTF_DATA_R & 0x02 );
	  break;
	case LED_YELLOW:
	  switch( action )
	  {
	    case LED_ON:
	      GPIO_PORTF_DATA_R &= 0xFB;
	      break;
	    case LED_OFF:
    	  GPIO_PORTF_DATA_R |= 0x04;
    	  break;
	    case LED_TOGGLE:
	      GPIO_PORTF_DATA_R ^= 0x04;
	      break;
	  }
	  Result = !(GPIO_PORTF_DATA_R & 0x01 );
	  break;
	case LED_RED:
	  switch( action )
	  {
	    case LED_ON:
	      GPIO_PORTF_DATA_R &= 0xFD;
	      break;
	    case LED_OFF:
    	  GPIO_PORTF_DATA_R |= 0x02;
    	  break;
	    case LED_TOGGLE:
	      GPIO_PORTF_DATA_R ^= 0x02;
	      break;
	  }
	  Result = !(GPIO_PORTF_DATA_R & 0x40 );
	  break;
	case LED_STATUS:
	  switch( action )
	  {
	    case LED_ON:
	      GPIO_PORTD_DATA_R &= 0xCF;
	      break;
	    case LED_OFF:
    	  GPIO_PORTD_DATA_R |= 0x40;
    	  break;
	    case LED_TOGGLE:
	      GPIO_PORTD_DATA_R ^= 0x40;
	      break;
	  }
	  Result = !(GPIO_PORTD_DATA_R & 0x01 );
	  break;
  }
  return( Result );
}	



