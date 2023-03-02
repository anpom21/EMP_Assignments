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
<<<<<<< HEAD:Assignement_2_final/main.c
#include "event.h"
#include "timers.h"
=======
#include "timers.h"
#include "norway.h"
#include "emergency.h"
#include "LED.h"
<<<<<<< HEAD
#include "alive_led.h"
=======
>>>>>>> 6d51886ff24d4081056edc4009c1ccbadfff9f33:Assignment_2_final/main.c
>>>>>>> 241ebd631a88a54a1f87aa377fdc79d195c43a25


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
<<<<<<< HEAD:Assignement_2_final/main.c
    event = select_button();
    switch (event){
    case BE_SINGLE_PUSH:
        GPIO_PORTF_DATA_R &= ~(0b00001100);
        GPIO_PORTF_DATA_R |= 0b00000010;
        break;
    case BE_DOUBLE_PUSH:
        GPIO_PORTF_DATA_R &= ~(0b00001010);
        GPIO_PORTF_DATA_R |= 0b00000100;
        break;
    case BE_LONG_PUSH:
        GPIO_PORTF_DATA_R &= ~(0b00000110);
        GPIO_PORTF_DATA_R |= 0b00001000;
        break;
    default:
        break;
    }
=======

<<<<<<< HEAD
    tl_norway();

=======
    tl_emergency();
>>>>>>> 6d51886ff24d4081056edc4009c1ccbadfff9f33:Assignment_2_final/main.c
>>>>>>> 241ebd631a88a54a1f87aa377fdc79d195c43a25
  }
  return( 0 );
}

/****************************** End Of Module *******************************/
