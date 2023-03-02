/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: gpio.c
*
* PROJECT....: Assignment 2 - Traffic light
*
* DESCRIPTION: Setup GPIO primarily for LEDS and testing
*
* Change Log:
******************************************************************************
* Date  Id  Change
* 02-02-2023
* --------------------
* 02-03-2023  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "timers.h"
#include "gpio.h"
#include "LED.h"
/*****************************    Defines    *******************************/


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/


void alive_led(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Red LED flashes once every second
******************************************************************************/
{
    static INT8U alive_timer = TIME_500_MSEC;
    if( ! --alive_timer )
    {
      alive_timer        = TIME_500_MSEC;
      // Toggle PORT D LED
      GPIO_PORTD_DATA_R ^= 0x40;
    }

}

/****************************** End Of Module *******************************/
