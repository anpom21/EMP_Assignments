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


#define YELLOW_LED          0x04
#define RED_AND_GREEN_LED   0x0A
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/


void tl_norway(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Activate the Norway traffic light sequence - yellow light flashing
*              once every second
******************************************************************************/
{
  // Initialize light timer
  static INT16U  light_timer = TIME_1_SEC;

  // Check if timer has run out
  if( ! --light_timer )
  {
      // Reset timer
      light_timer        = TIME_1_SEC;

      // Turn off green and red LED
      GPIO_PORTF_DATA_R |= RED_AND_GREEN_LED;

      // Toggle yellow LED
      GPIO_PORTF_DATA_R ^= YELLOW_LED;
  }

}

/****************************** End Of Module *******************************/
