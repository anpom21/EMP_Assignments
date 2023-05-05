/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: SPI.h
*
* PROJECT....: ECP
*
* DESCRIPTION: 
* functions and setup for SPI communication
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"

#include "gpio.h"
#include "tmodel.h"
#include "SPI.h"
#include "uart0.h"
#include "file.h"
#include "string.h"
/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

INT8U myValue = 1;
//char hello[10] = "helloWorld";
char hello[10] = "2062,2144,";
int counter = 0;
INT8U data;

extern QueueHandle_t q_uart_tx;
extern QueueHandle_t q_uart_rx;

/*****************************   Constants   *******************************/


/*************************  Function interfaces ****************************/

void SPI_init(){

    //pins used 2,3,4,5 on port A
	

    //STEP 1
   	SYSCTL_RCGCSSI_R |= (1<<0);   /*set clock enabling bit for SPI0 */
    //SYSCTL_RCGCGPIO_R |= (1<<3); /* enable clock to GPIOD for SPI1 */

    //STEP 2
    SYSCTL_RCGCGPIO_R |= (1<<0); /* enable clock to GPIOF for slave select */


    //STEP 3
	GPIO_PORTA_AFSEL_R |= (1<<2)|(1<<3)|(1<<4)|(1<<5);       /* enable alternate function of the pins*/
    
    //
    //GPIO_PORTA_PCTL_R &= ~0x0000F00F; /* assign RD0 and RD3 pins to SPI1 */
    
    //STEP 4
    GPIO_PORTA_PCTL_R &= ~(0x00FFFF00);    // clear register before assigning, not sure if reset value is zero
    GPIO_PORTA_PCTL_R |= 0x00222200;  /* assign pins to SPI1  */

    //STEP 5
    GPIO_PORTA_AMSEL_R &= ~0b00111100;      /* disable analog functionality pins, not sure if necessary??? */
    GPIO_PORTA_DIR_R |= 0b00101100; //set direction
    GPIO_PORTA_DEN_R |= 0b00111100;         /* Set PINS as digital pins */
    GPIO_PORTA_DATA_R |= (1<<3);            // set ss as high when idle

    //STEP 6
    /* Select SPI1 as a Master, POL = 0, PHA = 0, clock = 4 MHz, 8 bit data */		
    SSI0_CR1_R &= ~(1<<1);          /* disable SPI1 and configure it as a Master */
    SSI0_CR1_R = 0x00000000;
    SSI0_CC_R = 0;           /* Enable System clock Option */
    SSI0_CPSR_R = 240;       /* Select prescaler value of 4 .i.e 16MHz/100 = 0.16MHz */
    SSI0_CR0_R = 0x00007;    /* 4MHz SPI1 clock, SPI mode, 8 bit data */
    SSI0_CR0_R |= (1<<6);    // set clock to inverted mode
    //SSI0_CR0_R |= (1<<7);    // set SPH to high, this makes the first transmission happen on the first falling edge.
    SSI0_CR1_R |= 2;         /* enable SPI1 */
    
}

/* Function to get parity of number n. It returns 1
   if n has odd parity, and returns 0 if n has even
   parity */
BOOLEAN getParity(INT16U data)
{
    BOOLEAN parity = 0;
    while (data)
    {
        parity = !parity;
        data   = data & (data - 1);
    }       
    return parity;
}

INT16U insertParity(INT16U data){
    data = data << 1; //shift data to make room for parity bit
    if(getParity(data)){ //check if number of bits is odd, we use even parity.
        data |= 1;      // if set the last bit to make number of bits even
    }
    return data; 
}

INT16U removeParity(INT16U data){
    return 0;
}


void SPI_task(void *pvParameters)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Sends data on the transmit ´pin
******************************************************************************/
{
    while(1){
    //gfprintf(COM1, "%c", data);
    data = hello[counter%10];
    counter++;
    GPIO_PORTA_DATA_R &= ~(1<<3);            // set ss as low to indicate communication
    while((SSI0_SR_R & 2) == 0);
    SSI0_DR_R = data;
    while((SSI0_SR_R & 0x10));
    GPIO_PORTA_DATA_R |= (1<<3);            // set ss as high to end communication
    SPI_receive();
    }
}

void SPI_receive(){
    if(!((SSI0_SR_R & 4) == 1)){            // should check to see if the receive FIFO is not empty,
                                            // currently it just acts as if it empty
    INT8U data = SSI0_DR_R;
    //if((uxQueueMessagesWaiting(q_uart_tx))==0){       //not sure if necessary, it was used earlier to fix a bug with uart only sending one character
    gfprintf(COM1, "%c", data);
    //}
    }
}



/****************************** End Of Module *******************************/
