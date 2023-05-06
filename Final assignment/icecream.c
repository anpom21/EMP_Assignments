/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: icecream.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
//SYSTEM FILES
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "tmodel.h"

//USER FILES
#include "adc.h"
#include "file.h"
#include "icecream.h"
#include "key.h"
#include "lcd.h"
#include "string.h"
#include "uart0.h"

//FREE-RTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "projdefs.h"
#include "portmacro.h"

/*****************************    Defines    *******************************/
//******** states************/
#define LOCKED 0
#define FLAVOUR 1
#define LIQUID 2
#define AMOUNT 3
#define PROCESS 4
#define PRODUCTION 5
#define LOGGING 6

#define LOCKED 0
#define UNLOCKED 1
#define PASSWORD_SIZE 4

/*****************************   Constants   *******************************/

 

/*****************************   Variables   *******************************/
extern QueueHandle_t q_uart_tx;
extern QueueHandle_t q_uart_rx;

extern SemaphoreHandle_t lcd_mutex;
INT8U ice_state = LOCKED;

//LOCKED
INT8U key;
INT8U cursor_x = 0;
INT8U password[PASSWORD_SIZE] = {54, 57, 54, 57};
INT16U keys_pressed = 0;
BOOLEAN unlocked = 0;



//AMOUNT
INT16U adc_val;
INT8U icecream_amount = 0;      //in Liters


/*****************************   Functions   *******************************/

INT8U ascii_to_num(INT8U num){
    INT8U result;
    switch (num)
    {
    case '0':
        result = 0;
        break;
    case '1':
        result = 1;
        break;
    case '2':
        result = 2;
        break;
    case '3':
        result = 3;
        break;
    case '4':
        result = 4;
        break;
    case '5':
        result = 5;
        break;
    case '6':
        result = 6;
        break;
    case '7':
        result = 7;
        break;    
    case '8':
        result = 8;
        break;
    case '9':
        result = 9;
        break;
    default:
        result = 99;
        break;
    }
    return result;
}

void add_to_code(INT8U num){
    switch (cursor_x%4)
    {
    case 0:
        keys_pressed = 1000 * num;
        break;
    case 1:
        keys_pressed += 100 * num;
        break;
    case 2:
        keys_pressed += 10 * num;
        break;
    case 3:
        keys_pressed += 1 * num;
        break;
    default:
        break;
    }
}

void locked(){
    if( xSemaphoreTake( lcd_mutex, ( TickType_t ) 10 ) == pdTRUE ){
    if (unlocked == LOCKED)
    lcd_write("LOCKED",0,0);
    //else if (unlocked == UNLOCKED)
    //{
    //    lcd_write("UNLOCKED",0,0);
    //}
    cursor_x = cursor_x % PASSWORD_SIZE;
    if (get_keyboard(&key))
        {
        key = ascii_to_num(key);
        add_to_code(key);
        gfprintf(COM1, "key: %u, cursor: %u, keys_pressed: %u\r", key, cursor_x, keys_pressed);
                    
            lcd_write("*",(cursor_x++) % PASSWORD_SIZE, 1);


            if(cursor_x % PASSWORD_SIZE == 0){
            if(keys_pressed % 8 == 0){
              ice_state = AMOUNT;
              clr_LCD();
            }
            else {
                clr_LCD();
            }
          }
        }
    }
    xSemaphoreGive( lcd_mutex );
}

void choose_amount(){
  while (1)
  {
    adc_val = get_adc();        //adc max value is 4095
    if(adc_val < ADC_MAX_VAL/3){
        icecream_amount = 1;
    }
    else if (adc_val < ADC_MAX_VAL/3 * 2)
    {
        icecream_amount = 2;
    }
    else{
        icecream_amount = 3;
    }

      // 1: Take mutex
      if( xSemaphoreTake( lcd_mutex, ( TickType_t ) 10 ) == pdTRUE )

        // 2: Move cursor position to desired location
        move_LCD( 0, 0 );  

        // 2.1: Display can be cleared when necessary (should not be cleared every time it is written to, only when a new 'menu' should be displayed.)
        //clr_LCD();

        // 3: Write to LCD with a string or char
        switch (icecream_amount)
        {
        case 1:
            lcd_write("amount 1l",0,0);
            break;
        case 2:
            lcd_write("amount 2l",0,0);
            break;
        case 3:
            lcd_write("amount 3l",0,0);
            break;
        default:
            break;
        }

        // 4: Give back mutex
        xSemaphoreGive( lcd_mutex );
  }
}

extern void icecream_task(void *pvParameters ){    
    ice_state = LOCKED;
    while(1)
    switch (ice_state)
    {
    case LOCKED:
        locked();
        break;
    case AMOUNT:
        choose_amount();
        break;
    
    default:
        break;
    }
}