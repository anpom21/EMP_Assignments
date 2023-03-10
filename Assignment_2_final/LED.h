/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: LED.h
*
* PROJECT....: Assignment 2 - Traffic light
*
* DESCRIPTION: Setup GPIO primarily for LEDS and testing
*
The following options exist for colors:
LED_GREEN
LED_RED
LED_YELLOW
*
* Change Log:
******************************************************************************
* Date  Id  Change
* 02-02-2023
* --------------------
* 02-03-2023  MoH   Module created.
*
*****************************************************************************/
#ifndef LED_H_
#define LED_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define LED_NO_COLOR 0
#define LED_GREEN 0b100
#define LED_RED 0b001
#define LED_YELLOW 0b010


/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void LED_EMP_expansion(INT8U LED_color);
/*****************************************************************************
*   Input    : Desired color as seen under defines
*   Output   : Sets the desired color on the EMP expansion board
*   Function : --
******************************************************************************/

void LED_EMP_expansion_2color(INT8U LED_color, INT8U LED_color_2);
/*****************************************************************************
*   Input    : Desired colors as seen under defines
*   Output   : Sets the desired colors on the EMP expansion board
*   Function : --
******************************************************************************/

//does not work currently
void LED_toggle(void);
/*****************************************************************************
*   Input    : ---
*   Output   : ---
*   Function : Toggles the LED
******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* LED_H_ */
