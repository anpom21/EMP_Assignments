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
#include <stdio.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "tmodel.h"
#include "math.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "uart0.h"
#include "lcd.h"
#include "icecream.h"
#include "logging.h"

// TO DO
//------
// - Mutex for UART
// - Print log

/*****************************    Defines    *******************************/
// Console states
//---------------
#define CONSOLE_START 0
#define CONSOLE_SELECT 1
#define CONSOLE_LOG 2
#define CONSOLE_SETTINGS 3
#define CONSOLE_CHURNING_TIME 4
#define CONSOLE_FREQUENCY 5
#define CONSOLE_TIME 6


// Time seconds
//-------------
#define TIME_SEC 0
#define MSG_DELAY 800

/*****************************   Constants   *******************************/
/*****************************   Variables   *******************************/

// Mutexes
extern SemaphoreHandle_t mutex_liquid_base;
extern SemaphoreHandle_t mutex_churning_time;
extern SemaphoreHandle_t mutex_time_of_day;
extern SemaphoreHandle_t mutex_production_time;



// Gloabl
INT8U inp;

// Churning time for the different flavours
extern INT8U churning_time_vanilla;
extern INT8U churning_time_chocolate;
extern INT8U churning_time_strawberry;

// Liquid base frequencies
extern INT8U lb_frequency_milk;
extern INT8U lb_frequency_oat;
extern INT8U lb_frequency_coco;

// Number of ice creams
extern INT8U num_ice;





extern INT8U current_time_sec;
extern INT8U current_time_min;
extern INT8U current_time_hour;



INT8U console_state = CONSOLE_START;
/*****************************   Functions   *******************************/

void logging_init()
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Initialize
 ******************************************************************************/
{
    mutex_liquid_base = xSemaphoreCreateMutex();
    mutex_churning_time = xSemaphoreCreateMutex();
    mutex_time_of_day = xSemaphoreCreateMutex();
    mutex_production_time = xSemaphoreCreateMutex();
}

INT8U number_length(INT8U number)
{
    return floor(log10(abs(number))) + 1;
}

INT8U receive_input()
{
    INT8U str[4];
    INT8U str_length=0;
    INT8U ch = 0;
    INT8U result = 0;
    char space[1] = ' ';

   
    // Enter desired input and exist when 'Enter' is pressed
    while (ch != 0x0D)
    {
        if (uart0_get_q(&ch) && ch != 0x0D)
        {
            if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
            {
                str[str_length++] = ch;
                uart0_put_string(&ch);
            }           
            
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    
    INT8U i;
    for (i = 0; i < 80-str_length; i++)
    {
        uart0_put_string(&space);
    }
    
    // Turn string input into integer
    result = atoi(&str);


    return result;
}

extern void logging_task(void *pvParameters)

/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function :
 ******************************************************************************/
{
    INT8U ch;

    // Time variables
    INT8U time_length;
    INT8U time[4];
    INT8U time_hour;
    INT8U time_min;
    BOOLEAN invalid;

    // Log
    INT8U log_number = 1;
    char flavour_str[12];
    char liquid_base_str[13];
    char log_spaces[41] = "L                                        ";
    char *log_complete[100];
    INT8U i; // for loop

    INT8U str[10];
    INT8U number;
    BOOLEAN first_run = 1;
    int num;

    char int_str[10];

    INT8U cursor_x;
    while (1)
    {

        switch (console_state)
        {
        case CONSOLE_START:

            ;
            INT8U dummy_ = 0;
            
            
            if (first_run)
            {
            uart0_put_string("                                                                                "
                             "Ice Cream Machine                                                               "
                             "You have the following options:                                                 "
                             "1: See a log of the total production.                                           "
                             "2: Set the settings of the ice cream machine.                                   ");
                first_run = 0;
            }

            if (uart0_get_q(&ch))
            {
                
                uart0_put_char(ch);
                switch (ch)
                {
                case '1':
                    console_state = CONSOLE_LOG;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");

                    first_run = 1;
                    break;
                case '2':
                    console_state = CONSOLE_SETTINGS;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");

                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }
            vTaskDelay(10);

            


            

            break;
        case CONSOLE_LOG:;

            // FIRST RUN MESSAGE
            //------------------
            if (first_run)
            {
                //           {{VANILLA,  MILK,3, 22,21,12},
                //            {CHOCOLATE, OAT, 1, 14,25,44},
                //            {STRAWBERRY,COCO,2, 16,51,43},
                //            {VANILLA,   COCO,3,  2,42,22},
                //            {STRAWBERRY,OAT, 2,  5,43,35}}

                uart0_put_string("Log of recent ice cream production:                                             "
                                 "                                                                                "
                                 "#  Time      Flavour     Liquid Base  Amount                                    "
                                 "--------------------------------------------                                    ");
                

                // Log function
                INT8U row[6]; 
                char log_line[80] = "                                                                                ";

                INT8U col;
                INT8U row_number;
                for (row_number = 0; row_number < num_ice; row_number++)
                {

                    for (col = 0; col < 6; col++)
                    {
                        row[col] = get_log(row_number,col);
                    }

                    
                    

                    // Log number
                    log_line[0] = row_number + 1 + '0';
                    log_line[1] = ' ';
                    log_line[2] = ' ';

                    // Hour
                    log_line[3] = row[3] / 10 + '0';
                    log_line[4] = row[3] % 10 + '0';
                    log_line[5] = ':';
                    // Min
                    log_line[6] = row[4] / 10 + '0';
                    log_line[7] = row[4] % 10 + '0';
                    log_line[8] = ':';
                    // Seconds
                    log_line[9] = row[5] / 10 + '0';
                    log_line[10] = row[5] % 10 + '0';

                    log_line[11] = ' ';
                    log_line[12] = ' ';

                    // Flavour //
                    /////////////

                    switch (row[0])
                    {
                    case VANILLA:; // Vanilla
                        strcpy(flavour_str, "Vanilla     ");
                        flavour_str[12] = '\0';

                        for (i = 13; i < 25; i++)
                        {
                            log_line[i] = flavour_str[i - 13];
                        }

                        break;
                    case CHOCOLATE:; // Chocolate
                        strcpy(flavour_str, "Chocolate   ");
                        flavour_str[12] = '\0';

                        for (i = 13; i < 25; i++)
                        {
                            log_line[i] = flavour_str[i - 13];
                        }
                        break;
                    case STRAWBERRY:; // Strawberry
                        strcpy(flavour_str, "Strawberry  ");
                        flavour_str[12] = '\0';

                        for (i = 13; i < 25; i++)
                        {
                            log_line[i] = flavour_str[i - 13];
                        }
                        break;
                    default:

                        break;
                    }

                    // Add liquid base //
                    /////////////////////
                    switch (row[1])
                    {
                    case MILK:; // Milk
                        strcpy(liquid_base_str, "Milk         ");
                        liquid_base_str[13] = '\0';

                        for (i = 25; i < 38; i++)
                        {
                            log_line[i] = liquid_base_str[i - 25];
                        }

                        break;
                    case OAT:; // Oat
                        strcpy(liquid_base_str, "Oat          ");
                        liquid_base_str[13] = '\0';

                        for (i = 25;i < 38; i++)
                        {
                            log_line[i] = liquid_base_str[i - 25];
                        }
                        break;
                    case COCO:; // Coco
                        strcpy(liquid_base_str, "Coco         ");
                        liquid_base_str[13] = '\0';

                        for (i = 25;i < 38; i++)
                        {
                            log_line[i] = liquid_base_str[i - 25];
                        }
                        break;
                    default:

                        break;
                    }

                    // Add amount
                    log_line[38] = row[2] + '0';
                    log_line[39] = 'L';

                    // Print log line to uart
                    INT8U i;
                    for (i = 0; i < 80; i++)
                    {
                        uart0_put_char(log_line[i]);
                        vTaskDelay(1 / portTICK_RATE_MS);
                    }
                    
                }
                uart0_put_string("Press any button to continue.                                                   ");

                first_run = 0;
            }

            if (uart0_get_q(&ch))
            {
                console_state = CONSOLE_START;
                first_run = 1;
            }
            vTaskDelay(10);

            // Return when any button is pressed

            break;

        case CONSOLE_SETTINGS:;
            if (first_run)
            {
                uart0_put_string("Settings                                                                        "
                                 "You have the following options:                                                 "
                                 "1: Set churning time                                                            "
                                 "2: Set base frequency                                                           "
                                 "3: Set time                                                                     ");

                
                first_run = 0;
            }

            // SELECT SETTING
            if (uart0_get_q(&ch))
            {
                uart0_put_char(ch);
                
                switch (ch)
                {
                case '1': // Set churning time
                    // FUNCTION //

                    console_state = CONSOLE_CHURNING_TIME;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");

                    first_run = 1;
                    break;
                case '2': // Set base frequency
                    // FUNCTION //

                    console_state = CONSOLE_FREQUENCY;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");
                    first_run = 1;
                    break;
                case '3': // Set time
                    // FUNCTION //

                    console_state = CONSOLE_TIME;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }
            vTaskDelay(10);

            break;

        // CHURNING TIME
        case CONSOLE_CHURNING_TIME:
            // FIRST RUN //
            ///////////////
            if (first_run)
            {
                uart0_put_string("Churning time                                                                   "
                                 "Select the flavour for which the churning time should be changed.               "
                                 "1: Vanilla                                                                      "
                                 "2: Chocolate                                                                    "
                                 "3: Strawberry                                                                   ");

                first_run = 0;

                // SLET
                //INT8U test = '1';
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {
                uart0_put_char(ch);
                switch (ch) // OBS!
                {
                case '1': // Vanilla

                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired churning time.                                             ");

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {
                        
                        churning_time_vanilla = receive_input();

                        xSemaphoreGive(mutex_churning_time);
                    }

                    console_state = CONSOLE_START;

                    
                    uart0_put_string("Vanilla churning time updated.                                                  ");
                    first_run = 1;



                    break;
                case '2': // Chocolate
                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired churning time in seconds.                                  ");
                    

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_chocolate = receive_input();

                        xSemaphoreGive(mutex_churning_time);
                    }

                    console_state = CONSOLE_START;

                    
                    uart0_put_string("Chocolate churning time updated.                                                ");
                    first_run = 1;
                    break;
                case '3': // Strawberry
                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired churning time.                                             ");

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_strawberry = receive_input();
                        xSemaphoreGive(mutex_churning_time);
                    }

                    
                    uart0_put_string("Strawberry churning time updated.                                               ");
                    

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }
            vTaskDelay(10);

            break;
        case CONSOLE_FREQUENCY:
            // FIRST RUN //
            ///////////////
            if (first_run)
            {
                uart0_put_string("Liquid base frequency                                                           "
                                 "Select the liquid base for which the churning time should be changed.           "
                                 "1: Milk                                                                         "
                                 "2: Oat                                                                          "
                                 "3: Coconut                                                                      ");
                

                first_run = 0;
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {
                uart0_put_char(ch);
                
                switch (ch)
                {
                case '1': // Milk
                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired liquid base frequency in Hz.                               ");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_milk = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    
                    uart0_put_string("Milk liquid base frequency updated.                                             ");
                    

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                case '2': // Oat
                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired liquid base frequency.                                     ");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_oat = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    
                    uart0_put_string("Oat liquid base frequency updated.                                              ");
                    

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                case '3': // Coco
                    uart0_put_string("                                                                               ");
                    uart0_put_string("Please enter desired liquid base frequency.                                     ");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_coco = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    
                    uart0_put_string("Coconut liquid base frequency updated.                                          ");
                    

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }
            vTaskDelay(10);

            break;
        case CONSOLE_TIME:
            if (first_run)
            {
                uart0_put_string("Set time of day                                                                 "
                                 "Enter the time as follows: HH:MM                                                ");

                time_length = 0;
                
                

                first_run = 0;
                invalid = 0;
            }

            if (uart0_get_q(&ch))
            {
                
                vTaskDelay(10);
                if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
                {
                    time[time_length++] = ch - '0';
                    
                    uart0_put_char(ch);
                }

                vTaskDelay(10);
                if (time_length == 4 && invalid == 0)
                {

                    // Validate input?
                    time_hour = time[0] * 10;
                    time_hour += time[1];
                    time_min = time[2] * 10;
                    time_min += time[3];
                    
                    uart0_put_string("                                                                           ");

                    

                    if (time_hour <= 23 && time_min <= 59)
                    {
                        if (xSemaphoreTake(mutex_time_of_day, (TickType_t)10) == pdTRUE)
                        {


                            current_time_sec = TIME_SEC;
                            current_time_min = time_min;
                            current_time_hour = time_hour;
                            xSemaphoreGive(mutex_time_of_day);
                        }

                        uart0_put_string("Time of day updated.                                                            ");
                        
                        console_state = CONSOLE_START;
                        first_run = 1;
                    }
                }
                else if (time_length == 2)
                {
                   
                    uart0_put_char(':');
                }
                vTaskDelay(10);
            }
            vTaskDelay(10);

            break;

        default:
            break;
        }
    }
}
