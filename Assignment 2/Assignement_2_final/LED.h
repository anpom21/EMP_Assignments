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
LED_BLUE
LED_CYAN
LED_RED
LED_YELLOW
LED_MAGENTA
LED_WHITE
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
#define LED_GREEN 4
#define LED_BLUE 2
#define LED_CYAN 6
#define LED_RED 1
#define LED_YELLOW 5
#define LED_MAGENTA 3
#define LED_WHITE 7

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void LED_EMP(INT8U LED_color);
/*****************************************************************************
*   Input    : Desired color as seen under defines
*   Output   : Sets the desired color on the EMP board
*   Function : --
******************************************************************************/

void LED_EMP_expansion(INT8U LED_color);
/*****************************************************************************
*   Input    : Desired color as seen under defines
*   Output   : Sets the desired color on the EMP expansion board
*   Function : --
******************************************************************************/


/****************************** End Of Module *******************************/
#endif /* LED_H_ */
