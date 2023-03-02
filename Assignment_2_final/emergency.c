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


void tl_emergency(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Activate the emergency traficlight - turn on only the red LED
******************************************************************************/
{
  // Turn off yellow
  //GPIO_PORTF_DATA_R |= LED_YELLOW | LED_GREEN;
    LED_EMP_expansion(LED_RED);
}

/****************************** End Of Module *******************************/
