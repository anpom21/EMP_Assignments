/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: event.h
*
* PROJECT....: Assignment 2 - Traffic light
*
* DESCRIPTION: Defines button event states.
*
* Change Log:
******************************************************************************
* Date  Id  Change
* 02-02-2023
* --------------------
* 02-03-2023  MoH   Module created.
*
*****************************************************************************/


#ifndef EVENT_H_
#define EVENT_H_



/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
//initial event
#define GE_NO_EVENT     0

//Button events
#define BE_SINGLE_PUSH  1
#define BE_DOUBLE_PUSH  2
#define BE_LONG_PUSH    3
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/

#endif /* EVENT_H_ */
