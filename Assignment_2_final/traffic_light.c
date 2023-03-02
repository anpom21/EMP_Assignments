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
    static tl_state = Tl_STD;

    switch (tl_state)       //Calls LED functions based on tl_state
    {
    case TL_STANDARD:
        //call standard();
        break;
    case TL_NORWAY:
        //call norway();
        break;
    case TL_RED:
        //call red();
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
        tl_state = TL_RED;
        break;
    case BE_LONG_PUSH:
        tl_sate = TL_STANDARD;
        break;
    default:
        break;
    }
}

/****************************** End Of Module *******************************/

