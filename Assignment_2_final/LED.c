/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: LED.C
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
#include "gpio.h"
/*****************************    Defines    *******************************/

//position in the data register
#define START_BIT_LED   1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/


void LED_EMP_expansion(INT8U LED_color)
/*****************************************************************************
*   Input    : Desired color as seen under defines
*   Output   : Sets the desired color on the EMP expansion board
*   Function : --
******************************************************************************/
{
    GPIO_PORTF_DATA_R |= (0b111 << START_BIT_LED); //Clear the LEDs
    GPIO_PORTF_DATA_R &= ~(LED_color) << START_BIT_LED; //Set the desired color
}

void LED_EMP_expansion_2color(INT8U LED_color, INT8U LED_color_2)
/*****************************************************************************
*   Input    : Desired colors as seen under defines
*   Output   : Sets the desired colors on the EMP expansion board
*   Function : --
******************************************************************************/
{
    GPIO_PORTF_DATA_R |= (0b111 << START_BIT_LED); //Clear the LEDs
    GPIO_PORTF_DATA_R &= ~(LED_color) << START_BIT_LED; //Set the desired color
    GPIO_PORTF_DATA_R &= ~(LED_color_2) << START_BIT_LED; //Set the desired color
}

//DOES NOT WORK CURRENTLY
void LED_toggle(void)
/*****************************************************************************
*   Input    : ---
*   Output   : ---
*   Function : Toggles the LED
******************************************************************************/
{
    static previous_color;
    previous_color = GPIO_PORTF_DATA_R;
    GPIO_PORTF_DATA_R = ~(previous_color);
}
/****************************** End Of Module *******************************/
