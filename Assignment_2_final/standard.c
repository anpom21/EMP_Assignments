/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: standard.c
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
#include "timers.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "standard.h"
#include "LED.h"



/*****************************    Defines    *******************************/
//Different states for the traffic light
#define LS_RED            0
#define LS_RED_YELLOW     1
#define LS_GREEN          2
#define LS_YELLOW         3

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT16U timer = TIME_1_SEC;  //Timer for changing the signal
INT8U light_state = LS_RED; //Initial state is red

/*****************************   Functions   *******************************/

void standard(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : The super loop.
******************************************************************************/

{
    //Whenever the timer runs out, the state and color is changed.
    if( ! --timer )
        {
          timer        = TIME_1_SEC;
          switch (light_state){
          case LS_RED:
              light_state = LS_RED_YELLOW;
              LED_EMP_expansion_2color(LED_RED, LED_YELLOW);
              break;
          case LS_RED_YELLOW:
              light_state = LS_GREEN;
              LED_EMP_expansion(LED_GREEN);
              break;
          case LS_GREEN:
              light_state = LS_YELLOW;
              LED_EMP_expansion(LED_YELLOW);
              break;
          case LS_YELLOW:
              light_state = LS_RED;
              LED_EMP_expansion(LED_RED);
              break;
          }
        }
}


/****************************** End Of Module *******************************/
