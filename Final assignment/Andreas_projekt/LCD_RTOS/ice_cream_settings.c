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
#include "uart0.h"
#include "lcd.h"
/*****************************    Defines    *******************************/


/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/
INT8U churning_time;
INT8U liquid_base_frequency;

/*****************************   Functions   *******************************/

void settings_init()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Initialize 
******************************************************************************/
{

}

extern void settings_task(void *pvParameters )

/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U ch;
  
  INT8U cursor_x;
  while(1){;
        if(uart0_get_q( &ch )){

        move_LCD((cursor_x++) % 16,1);
        wr_ch_LCD( ch );

          if(cursor_x%16 == 15){
              clr_LCD();
          }
        }
        vTaskDelay(100);
    

  }
}



