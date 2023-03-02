/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.:button.h
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
#include "traffic_light.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void traffic_light(INT8U event)
/*****************************************************************************
*   Input    : Button Event
*   Output   :
*   Function : Rotates between states based on conditions
******************************************************************************/
{
    static tl_state = TL_STANDARD;

    switch (tl_state)       //Calls LED functions based on tl_state
    {
    case TL_STANDARD:
        standard();
        break;
    case TL_NORWAY:
        tl_norway();
        break;
    case TL_EMERGENCY:
        tl_emergency();
        break;
    default:
        break;
    }

    switch (event)          //Updates tl_state based on button events
    {
    case BE_SINGLE_PUSH:
        tl_state = TL_NORWAY;
        break;
    case BE_DOUBLE_PUSH:
        tl_state = TL_EMERGENCY;
        break;
    case BE_LONG_PUSH:
        tl_sate = TL_STANDARD;
        break;
    default:
        break;
    }
}

/****************************** End Of Module *******************************/

