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
* 050128  KA    Module created.
*
*****************************************************************************/

#ifndef _ICE_CREAM_SETTINGS_H
  #define _ICE_CREAM_SETTINGS_H

/***************************** Include files *******************************/
#include "emp_type.h"
/*****************************    Defines    *******************************/


/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void settings_init();


void settings_task(void *pvParameters );
/*****************************************************************************
*   Input    : Strings can be put directly in as the string input.
*              Chars need to be passed by address ex lcd_write(&ch,0,0)
*              For x and y the desired LCD position should be used
*   Output   : -
*   Function : Initialization
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

