/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: main.c
*
* PROJECT....: EMP
*
* DESCRIPTION: Assignment 1, main module. No main.h file.
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150215  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick.h"
#include "event.h"
#include "timers.h"
#include "timers.h"
#include "norway.h"
#include "emergency.h"
#include "LED.h"
#include "alive_led.h"
#include "traffic_light.h"
#include "gpio.h"
/*****************************    Defines    *******************************/
#define RED_LED         0x02
#define YELLOW_LED      0x04
#define GREEN_LED       0x06

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern INT16S ticks;
/*****************************   Functions   *******************************/



int main(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : The super loop.
******************************************************************************/
{
  init_systick();
  init_gpio();


  GPIO_PORTF_DATA_R &= ~(0x0E);

  // Loop forever.
  while(1)
  {
    // System part of the super loop.
    // ------------------------------
    while( !ticks );

    // The following will be executed every 5ms
    ticks--;

    alive_led();

    // Application part of the super loop.
    // -----------------------------------
    event = select_button();
    traffic_light(event);
  }
  return( 0 );
}

/****************************** End Of Module *******************************/
