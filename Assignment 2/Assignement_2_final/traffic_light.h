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

#ifndef TRAFFIC_LIGHT_H_
#define TRAFFIC_LIGHT_H_

/***************************** Include files *******************************/
#include "norge.c"
#include "emp_type.h"
#include "event.h"
/*****************************    Defines    *******************************/
#define TL_STANDARD     0
#define TL_NORWAY       1
#define TL_RED          2

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void traffic_light(INT8U event);
/*****************************************************************************
*   Input    : Button Event
*   Output   :
*   Function : Rotates between states based on conditions
******************************************************************************/


/****************************** End Of Module *******************************/




#endif /* TRAFFIC_LIGHT_H_ */
