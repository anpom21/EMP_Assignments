/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "tmodel.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
/*****************************    Defines    *******************************/
/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
/*****************************   Functions   *******************************/


BOOLEAN sw1_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );     // SW1 at PF4
}

BOOLEAN sw2_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x01) );     // SW2 at PF0
}

void button_task(void *pvParameters){

  while (1)
  {
    /* code */
  }
  
}

