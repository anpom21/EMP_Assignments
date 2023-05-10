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

// TO DO
//------
// - Mutex for UART
// - Print log

/*****************************    Defines    *******************************/
// Console states
//---------------
#define CONSOLE_START 0
#define CONSOLE_SELECT 1
#define CONSOLE_SELECT_MSG 11
#define CONSOLE_LOG 2
#define CONSOLE_LOG_MSG 8
#define CONSOLE_SETTINGS 3
#define CONSOLE_SETTINGS_MSG 7
#define CONSOLE_CHURNING_TIME 4
#define CONSOLE_CHURRING_TIME_MSG 9
#define CONSOLE_FREQUENCY 5
#define CONSOLE_FREQUENCY_MSG 5
#define CONSOLE_TIME 6
#define CONSOLE_TIME_MSG 10

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
INT8U churning_time_vanilla;
INT8U churning_time_chocolate;
INT8U churning_time_strawberry;

// Liquid base frequencies
INT8U lb_frequency_milk;
INT8U lb_frequency_oat;
INT8U lb_frequency_coco;

// Log
// 6 elementer (flav,liq,amount,hour,min,sec)
// 3xTid

// FLAVOURS
#define VANILLA 1
#define CHOCOLATE 2
#define STRAWBERRY 3

// LIQUID BASE
#define MILK 0
#define OAT 1
#define COCO 2

INT8U matrix[3][4] = {{'*', '7', '4', '1'},
                      {'0', '8', '5', '2'},
                      {'#', '9', '6', '3'}};

INT8U log_[5][6] = {{VANILLA, MILK, 3, 22, 21, 12},
                    {CHOCOLATE, OAT, 1, 14, 25, 44},
                    {STRAWBERRY, COCO, 2, 16, 51, 43},
                    {VANILLA, COCO, 3, 2, 42, 22},
                    {STRAWBERRY, OAT, 2, 5, 43, 35}};

INT8U console_state = CONSOLE_START;
/*****************************   Functions   *******************************/

void settings_init()
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
    INT8U str_length;
    INT8U ch = 0;
    INT8U result = 0;

    //uart0_get_q(&ch);
    // Enter desired input and exist when 'Enter' is pressed;
    while (ch != 0x0D)
    {
        if (uart0_get_q(&ch) && ch != 0x0D)
        {
            str[str_length++] = ch;
            GPIO_PORTF_DATA_R ^= 0x08;
            uart0_put_q(ch);

        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    // Turn UART input into integer

    result = atoi(&str);
    inp = result;
    //*out = str;

    return result;
}

extern void settings_task(void *pvParameters)

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
            // char msg[] =
            uart0_put_string("                                                                                "
                             "Ice Cream Machine                                                               "
                             "You have the following options:                                                 "
                             "1: See a log of the total production.                                           "
                             "2: Set the settings of the ice cream machine.                                   ");

            // uart0_put_return(strlen(msg));
            vTaskDelay(MSG_DELAY + 200 / portTICK_RATE_MS);

            console_state = CONSOLE_SELECT;
            break;
        case CONSOLE_SELECT:

            if (uart0_get_q(&ch))
            {
                uart0_put_q(ch);
                vTaskDelay(50 / portTICK_RATE_MS);
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
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }

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
                vTaskDelay(300 / portTICK_RATE_MS);

                // Log function
                INT8U row[6]; // = {CHOCOLATE, OAT, 1, 14,25,44};
                char log_line[80] = "                                                                                ";

                INT8U k;
                INT8U row_number;

                for (row_number = 0; row_number < 5; row_number++)
                {

                    for (k = 0; k < 6; k++)
                    {
                        row[k] = log_[row_number][k];
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

                        for (k = 13; k < 25; k++)
                        {
                            log_line[k] = flavour_str[k - 13];
                        }

                        break;
                    case CHOCOLATE:; // Chocolate
                        strcpy(flavour_str, "Chocolate   ");
                        flavour_str[12] = '\0';

                        for (k = 13; k < 25; k++)
                        {
                            log_line[k] = flavour_str[k - 13];
                        }
                        break;
                    case STRAWBERRY:; // Strawberry
                        strcpy(flavour_str, "Strawberry  ");
                        flavour_str[12] = '\0';

                        for (k = 13; k < 25; k++)
                        {
                            log_line[k] = flavour_str[k - 13];
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

                        for (k = 25; k < 38; k++)
                        {
                            log_line[k] = liquid_base_str[k - 25];
                        }

                        break;
                    case OAT:; // Oat
                        strcpy(liquid_base_str, "Oat          ");
                        liquid_base_str[13] = '\0';

                        for (k = 25; k < 38; k++)
                        {
                            log_line[k] = liquid_base_str[k - 25];
                        }
                        break;
                    case COCO:; // Coco
                        strcpy(liquid_base_str, "Coco         ");
                        liquid_base_str[13] = '\0';

                        for (k = 25; k < 38; k++)
                        {
                            log_line[k] = liquid_base_str[k - 25];
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
                        uart0_put_q(log_line[i]);
                    }
                    vTaskDelay(500 / portTICK_RATE_MS);
                }
                vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                uart0_put_string("Press any button to continue.                                                   ");

                first_run = 0;
            }

            if (uart0_get_q(&ch))
            {
                console_state = CONSOLE_START;
            }

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

                vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                first_run = 0;
            }

            // SELECT SETTING
            if (uart0_get_q(&ch))
            {
                uart0_put_q(ch);
                vTaskDelay(50 / portTICK_RATE_MS);
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
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                    first_run = 1;
                    break;
                case '3': // Set time
                    // FUNCTION //

                    console_state = CONSOLE_TIME;

                    uart0_put_string("                                                                               ");
                    uart0_put_string("                                                                                ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }

            break;

        // CHURNING TIME
        case CONSOLE_CHURNING_TIME:
            // FIRST RUN //
            ///////////////
            if (first_run)
            {
                uart0_put_string("Churning time                                                                   "
                                 "Select the flavour for which the churning time should be changed                "
                                 "1: Vanilla                                                                      "
                                 "2: Chocolate                                                                    "
                                 "3: Strawberry                                                                   ");
                vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                first_run = 0;

                // SLET
                //INT8U test = '1';
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {
                uart0_put_q(ch);
                vTaskDelay(50 / portTICK_RATE_MS);
                switch (ch) // OBS!
                {
                case '1': // Vanilla

                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);




                    break;
                case '2': // Chocolate
                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_chocolate = receive_input();

                        xSemaphoreGive(mutex_churning_time);
                    }

                    console_state = CONSOLE_START;

                    uart0_put_return(number_length(churning_time_chocolate));
                    vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                    uart0_put_string("Chocolate churning time updated.                                                ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                    first_run = 1;
                    break;
                case '3': // Strawberry
                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_strawberry = receive_input();
                        xSemaphoreGive(mutex_churning_time);
                    }

                    vTaskDelay(MSG_DELAY * 3 / portTICK_RATE_MS);
                    //uart0_put_return(number_length(churning_time_strawberry));
                    vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                    uart0_put_string("Strawberry churning time updated.                                               ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }

            break;
        case CONSOLE_FREQUENCY:
            // FIRST RUN //
            ///////////////
            if (first_run)
            {
                uart0_put_string("Liquid base frequency                                                           "
                                 "Select the liquid base for which the churning time should be changed            "
                                 "1: Milk                                                                         "
                                 "2: Oat                                                                          "
                                 "3: Coco                                                                         ");
                vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                first_run = 0;
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {
                uart0_put_q(ch);
                vTaskDelay(50 / portTICK_RATE_MS);
                switch (ch)
                {
                case '1': // Milk
                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_milk = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    uart0_put_return(number_length(lb_frequency_milk));
                    vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                    uart0_put_string("Milk liquid base frequency updated.                                             ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                case '2': // Oat
                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_oat = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    uart0_put_return(number_length(lb_frequency_oat));
                    vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                    uart0_put_string("Oat liquid base frequency updated.                                              ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                case '3': // Coco
                    uart0_put_string("                                                                               ");
                    vTaskDelay(100 / portTICK_RATE_MS);

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_coco = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    uart0_put_return(number_length(lb_frequency_coco));
                    vTaskDelay(MSG_DELAY * 2 / portTICK_RATE_MS);
                    uart0_put_string("Coco liquid base frequency updated.                                             ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    console_state = CONSOLE_START;
                    first_run = 1;
                    break;
                default:

                    vTaskDelay(1);
                    uart0_put_string(" Invalid key press.                                                            ");

                    break;
                }
            }

            break;
        case CONSOLE_TIME:
            if (first_run)
            {
                uart0_put_string("Set time of day                                                                 "
                                 "Enter the time as follows HH:MM.                                                ");
                vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                time_length = 0;

                first_run = 0;
                invalid = 0;
            }

            if (uart0_get_q(&ch))
            {
                uart0_put_q(ch);

                if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
                {
                    time[time_length++] = ch - '0';
                }
                else
                {
                    time_length++;
                    invalid = 1;
                }

                if (time_length == 4 && invalid == 0)
                {

                    // Validate input?
                    time_hour = time[0] * 10;
                    time_hour += time[1];
                    time_min = time[2] * 10;
                    time_min += time[3];

                    vTaskDelay(100 / portTICK_RATE_MS);
                    uart0_put_string("                                                                           ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);

                    first_run = 1;

                    if (time_hour <= 23 && time_min <= 59)
                    {
                        if (xSemaphoreTake(mutex_time_of_day, (TickType_t)10) == pdTRUE)
                        {

                            // set_time(time_hour,time_min,TIME_SEC);
                            xSemaphoreGive(mutex_time_of_day);
                        }

                        uart0_put_string("Time of day updated.                                                          ");
                        vTaskDelay(100 / portTICK_RATE_MS);

                        console_state = CONSOLE_START;
                    }
                }
                else if (time_length == 4)
                {
                    // Reset timer input
                    invalid = 0;
                    time_length = 0;

                    vTaskDelay(100 / portTICK_RATE_MS);
                    uart0_put_string("                                                                               ");
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                }
                else if (time_length == 2)
                {
                    uart0_put_q(':');
                    vTaskDelay(MSG_DELAY / portTICK_RATE_MS);
                }
            }

            break;

        default:
            break;
        }
    }
}
