/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.:button.c
*
* PROJECT....: Assignment 2 - Traffic light
*
* DESCRIPTION: listens to button pushes and sets the event value
*           to match correspondingly.
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
#include "event.h"
#include "button.h"
#include "timers.h"
/*****************************    Defines    *******************************/
#define BS_IDLE           0
#define BS_FIRST_PUSH     1
#define BS_FIRST_RELEASE  2
#define BS_SECOND_PUSH    3
#define BS_LONG_PUSH      4
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
INT8U button_pushed() //returns 1 if SW1 is pushed
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );
}

INT8U select_button(void)
/*****************************************************************************
*   Input    : -
*   Output   : Button Event
*   Function : Listens to button
******************************************************************************/
{
    static INT8U button_state = BS_IDLE;
    static INT16U button_timer;
    static INT8U button_event = GE_NO_EVENT;

    switch(button_state) {
        case BS_IDLE:
            if(button_pushed()) {
                button_state = BS_FIRST_PUSH;
                button_timer = TIME_2_SEC;
            }
            break;
        case BS_FIRST_PUSH:
            if (!button_pushed()) {
                button_state = BS_FIRST_RELEASE;
                button_timer = TIME_100_MSEC;
            }
            else if (! --button_timer ) {
                button_state = BS_LONG_PUSH;
                button_event = BE_LONG_PUSH;
            }
            break;
        case BS_FIRST_RELEASE:
            if (! --button_timer) {
                button_state = BS_IDLE;
                button_event = BE_SINGLE_PUSH;
            }
            else if (button_pushed()) {
                button_state = BS_SECOND_PUSH;
                button_timer = TIME_2_SEC;
            }
            break;
        case BS_SECOND_PUSH:
            if (! --button_timer) {
                button_event = BE_LONG_PUSH;
                button_state = BS_LONG_PUSH;
            }
            else if (! button_pushed()) {
                button_event = BE_DOUBLE_PUSH;
                button_state = BS_IDLE;
            }
            break;
        case BS_LONG_PUSH:
            if (! button_pushed()) {
                button_state = BS_IDLE;
            }
            break;
        default:
            break;
    }
    return(button_event);
}

/****************************** End Of Module *******************************/
