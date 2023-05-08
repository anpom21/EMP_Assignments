/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: ui.c
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
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "ui.h"
#include "rtc.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U i;
INT8U  InBuf[128];

/*****************************   Functions   *******************************/


void ui_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U ch;

  if( get_queue( Q_UART_RX, &ch, WAIT_FOREVER ))                        // if there is received something from the uart
  {
    if( i < 128 )
      InBuf[i++] = ch;                                                  // put it in our buffer
    put_queue( Q_UART_TX, ch, WAIT_FOREVER );                           // write the same char back to see it in the terminal
    if( ch == '\r' )                                                    // if the current char is 'Enter' (return) we look at the entire message
    {
      if( InBuf[0] == '2' )                                             // if the first char is '2' it is a GET CLOCK command
      {
          put_queue( Q_UART_TX, '\n', WAIT_FOREVER );
          put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
          put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
          put_queue( Q_UART_TX, ' ', WAIT_FOREVER );
          put_queue( Q_UART_TX, '2', WAIT_FOREVER );                    // write the command 2 followed by the current time to the terminal'
          put_queue( Q_UART_TX, ' ', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_hour()/10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_hour()%10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_min()/10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_min()%10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_sec()/10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_sec()%10+'0', WAIT_FOREVER );
      }

      if(( InBuf[0] == '1' ) && ( i >= 7 ))                             // if the first char is '1' it is a SET CLOCK command
      {
          put_queue( Q_UART_TX, '\n', WAIT_FOREVER );
          put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
          put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
          put_queue( Q_UART_TX, ' ', WAIT_FOREVER );
          put_queue( Q_UART_TX, '1', WAIT_FOREVER );                    // write the command 1 and use the values in the buffer to set the time
          put_queue( Q_UART_TX, ' ', WAIT_FOREVER );
          set_hour( (InBuf[1]-'0')*10+InBuf[2]-'0');
          set_min( (InBuf[3]-'0')*10+InBuf[4]-'0');
          set_sec( (InBuf[5]-'0')*10+InBuf[6]-'0');
          put_queue( Q_UART_TX, get_hour()/10+'0', WAIT_FOREVER );      // write the current time to the terminal
          put_queue( Q_UART_TX, get_hour()%10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_min()/10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_min()%10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_sec()/10+'0', WAIT_FOREVER );
          put_queue( Q_UART_TX, get_sec()%10+'0', WAIT_FOREVER );
      }

      put_queue( Q_UART_TX, '\n', WAIT_FOREVER );
      put_queue( Q_UART_TX, '\r', WAIT_FOREVER );
      put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
      put_queue( Q_UART_TX, '>',  WAIT_FOREVER );
      put_queue( Q_UART_TX, ' ', WAIT_FOREVER );
      i = 0;
    }
  }

}


/****************************** End Of Module *******************************/












