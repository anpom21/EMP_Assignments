
/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
#include "rotary_encoder.h"

/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U rot_cnt;
INT8U rot_pressed;
/*****************************   Functions   *******************************/

void rotary_enc_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
    /*
    //DIGIswitch enable
    GPIO_PORTA_DIR_R &= ~0b11100000;
    GPIO_PORTA_DEN_R |= 0b11100000;

    //led green enable
    GPIO_PORTF_DIR_R |= 0x08;
    GPIO_PORTF_DEN_R |= 0x08;
    */
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   //Enable port F
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;   //Enable port A
    GPIO_PORTF_DIR_R |= 0x0e;               //Set onboard LEDS as outputs
    GPIO_PORTF_DEN_R |= 0x0e;               //Enable onboard LEDs

    GPIO_PORTA_DIR_R &= ~0b11100000;        //Rotary encoder input
    GPIO_PORTA_DEN_R |= 0b11100000;         //Enable rotary encoder
    GPIO_PORTA_PUR_R = 0x80;                //Pullup resistor on rotary button

}


void rotary_enc_task(void *pvParameters)
/*
 * Input: Rotary encoder.
 * Output: rot_cnt variable.
 */
{
    rotary_enc_init();
    INT8U AB_new = 0x00; //Format: 0b000000AB
    INT8U AB_last = 0x00;
    INT8U YY;
    rot_cnt = 0x00;
    rot_pressed = 0x00;

    while(1) {

        AB_new = (GPIO_PORTA_DATA_R & 0x60)>>5;     //Get rotary encoder inputs

        if(AB_new != AB_last) {                     //has a change in signal occurred
            YY = AB_new ^ AB_last;                  //YY = XOR between old and new signal
            if ((AB_new & 0x01) == ((AB_new & 0x02)>>1)) //If A == B
            {
                if(YY == 0x01) {            //If B has changed
                    rot_cnt--;
                }
                else if(YY == 0x02) {       //If A has changed
                    rot_cnt++;
                }
            }
            AB_last = AB_new;               //update AB_last
        }


        if ((GPIO_PORTA_DATA_R & 0x80) == 0 && rot_pressed == 0x00) {//button clicked for one cycle
            rot_pressed = 1;
            //Evt. kode der skal køre hver gang knappen bliver trykket
        }
        else if((GPIO_PORTA_DATA_R & 0x80) && rot_pressed == 0x01)
            rot_pressed = 0; //button pulled down again

        /*
        cnt = cnt % 3;
        GPIO_PORTF_DATA_R |= 0x06;
        GPIO_PORTF_DATA_R &= ~((cnt & 0x07)<<1);
         */
        vTaskDelay(1 / portTICK_RATE_MS);   //1ms delay
    }
}


/****************************** End Of Module *******************************/

/*
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x02;
        vTaskDelay(1000 / portTICK_RATE_MS);



        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x04;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x06;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x08;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x0a;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x0c;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x0e;
        vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_PORTF_DATA_R |= 0x0e;
        GPIO_PORTF_DATA_R &= ~0x00;
        vTaskDelay(1000 / portTICK_RATE_MS);
        */




/*
*       if(GPIO_PORTA_DATA_R & 0x40)
            GPIO_PORTF_DATA_R |= 0x08;
        else
            GPIO_PORTF_DATA_R &= ~0x08;

        if(GPIO_PORTA_DATA_R & 0x20)
            GPIO_PORTF_DATA_R |= 0x04;
        else
            GPIO_PORTF_DATA_R &= ~0x04;
 */

