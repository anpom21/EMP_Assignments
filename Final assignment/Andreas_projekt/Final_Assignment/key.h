/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

#ifndef _KEY_H
  #define _KEY_H
#include "emp_type.h"


// Variables til main:
//--------------------
// QueueHandle_t q_keypad;
// SemaphoreHandle_t keypad_mutex;



// Eksempel p� brug af keypad kan ses i lcd.c under lcd_example


BOOLEAN get_keyboard( INT8U* );
/*****************************************************************************
*   Input    : Pointer to variable used to store the keyboard value in
*   Output   : If a key press is stored in the queue, the function will return true.
*   Function : Returns recent key press.
******************************************************************************/
void keypad_init();
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Initialize queues and semaphores. To be used in main.
******************************************************************************/
extern void keypad_task(void *pvParameters );
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : FreeRTOS task
******************************************************************************/

#endif
