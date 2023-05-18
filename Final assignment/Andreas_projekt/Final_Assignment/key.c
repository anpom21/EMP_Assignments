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
enum KEYPAD_states
{
  KEYPAD_RDY,
  KEYPAD_WAIT,
};
#define RDY  0
#define WAIT 1

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT8U state = RDY;
enum KEYPAD_states KEYPAD_state = KEYPAD_RDY;

// FreeRTOS
extern QueueHandle_t q_keypad;
extern SemaphoreHandle_t keypad_mutex;
/*****************************   Functions   *******************************/

void keypad_init()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Initialize queue
******************************************************************************/
{
    q_keypad = xQueueCreate(128, sizeof(INT8U));
    keypad_mutex = xSemaphoreCreateMutex();
}


INT8U row( INT8U y )
{
  INT8U result = 0;

  switch( y )
  {
    case 0x01: result = 1; break;
    case 0x02: result = 2; break;
    case 0x04: result = 3; break;
    case 0x08: result = 4; break;
  }
  return( result );
}

INT8U column( INT8U x )
{
  INT8U result = 0;

  switch( x )
  {
    case 1: result = 0x10; break;   // PE4 column 1
    case 2: result = 0x08; break;   // PE3 column 2
    case 3: result = 0x04; break;   // PE2 column 3
  }
  return( result );
}

INT8U set_column( INT8U x )
{
  INT8U result = column(x);
  
  if (result)
  {
    GPIO_PORTA_DATA_R &= 0xE3;          // Clear the 3 bits for the columns
    GPIO_PORTA_DATA_R |= result;        // Set the resulting column
  }
  
  return( result );
}

INT8U key_catch( x, y )
INT8U x, y;
{
  const INT8U matrix[3][4] = {{'*','7','4','1'},
                              {'0','8','5','2'},
                              {'#','9','6','3'}};

  return( matrix[x-1][y-1] );
}

BOOLEAN get_keyboard( INT8U *pch )
{
  BOOLEAN result = 0;
  
  if( uxQueueMessagesWaiting(q_keypad)){
      if( xSemaphoreTake( keypad_mutex, ( TickType_t ) 10 ) == pdTRUE ){
          result = xQueueReceive( q_keypad, pch, portMAX_DELAY ) == pdTRUE;
          xSemaphoreGive(keypad_mutex);
      }
  }
  return( result );
}

BOOLEAN check_rows(INT8U x)
{
    INT8U y = GPIO_PORTE_DATA_R & 0x0F;             // Save the values of the 4 bits for the rows
    if( y )                                         // If one of them are set...
    {                                               // ...we first find the row number with the function row()
        INT8U ch = key_catch( x, row(y) );          // Now that we have the row and column we look up the corresponding character using the function key_catch
           
        xQueueSend( q_keypad, &ch, portMAX_DELAY ); // Put the character in a queue so it can be used by another task
        
    
        return 1;
    }
    return 0;
}

extern void keypad_task(void *pvParameters )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  
  while (1)
  {
    switch(state)
  {
  case RDY:

    if( xSemaphoreTake( keypad_mutex, ( TickType_t ) 1 ) == pdTRUE ){
        
        set_column(1);                  // Set the bit for the column
        if (check_rows(1))                  // Check all the rows for a hit
        {                                     // If a button press is registered we go to next state so the press is only registered once
            state = WAIT;
            xSemaphoreGive(keypad_mutex);
            break;
        }
        
        set_column(2);                  // Set column 2
        if (check_rows(2))                  // Check for a hit
        {
            state = WAIT;
            xSemaphoreGive(keypad_mutex);
            break;
        }

        set_column(3);                  // Set column 3
        if (check_rows(3))                  // Check for a hit
        {
            state = WAIT;
            xSemaphoreGive(keypad_mutex);
            break;
        }
        xSemaphoreGive(keypad_mutex);
    }
    


    break;
  case WAIT:

    if( !(GPIO_PORTE_DATA_R & 0x0F) )   // We stay here until the button is released so a button press is not counted more than once
    {
      state = RDY;
      vTaskDelay( 10 / portTICK_PERIOD_MS );
    }
    break;
  }
    
  }
  
  
}
