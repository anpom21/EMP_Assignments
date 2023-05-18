/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: uart.h
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
* 150228  MoH   Module created.
*
*****************************************************************************/
#include "emp_type.h"
#ifndef _UART_H
  #define _UART_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/
/*****************************   variables   *******************************/


/*****************************   Functions   *******************************/
BOOLEAN uart0_put_q( INT8U );
BOOLEAN uart0_put_string( char a_string[]);
void uart0_put_return(INT8U length);
BOOLEAN uart0_get_q( INT8U* );


void uart_tx_task(void *pvParameters);
void uart_rx_task(void *pvParameters);


extern void uart0_init( INT32U, INT8U, INT8U, INT8U );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/


/****************************** End Of Module *******************************/
#endif

