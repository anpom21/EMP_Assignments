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
#include "manager_ui.h"
#include "file.h"
#include "string2.h"

// TO DO
//------
// - Mutex for UART
// - Print log
typedef INT8U *FILE;
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
    INT8U str_length = 0;
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
                gfprintf(COM1, "%c", ch);
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    INT8U i;

    gfprintf(COM1, "\n\r");

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

    // Log
    INT8U log_number = 1;
    char flavour_str[12];
    char liquid_base_str[13];
    char log_spaces[41] = "L                                        ";
    char *log_complete[100];
    INT8U i; // for loop

    INT8U str[10];
    INT8U number;
    BOOLEAN run_once = 1;
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

            if (run_once)
            {
                gfprintf(COM1, "\n\r");
                gfprintf(COM1, "Ice Cream Machine\n\r");
                gfprintf(COM1, "You have the following options:\n\r");
                gfprintf(COM1, "1: See a log of the total production.\n\r");
                gfprintf(COM1, "2: Set the settings of the ice cream machine.\n\r");

                run_once = 0;
            }

            if (uart0_get_q(&ch))
            {

                switch (ch)
                {
                case '1':

                    console_state = CONSOLE_LOG;
                    gfprintf(COM1, "%c", ch);
                    gfprintf(COM1, "\n\r\n\r");

                    run_once = 1;
                    break;
                case '2':
                    console_state = CONSOLE_SETTINGS;
                    gfprintf(COM1, "%c", ch);
                    gfprintf(COM1, "\n\r\n\r");

                    run_once = 1;
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
            if (run_once)
            {
                //           {{VANILLA,  MILK,3, 22,21,12},
                //            {CHOCOLATE, OAT, 1, 14,25,44},
                //            {STRAWBERRY,COCO,2, 16,51,43},
                //            {VANILLA,   COCO,3,  2,42,22},
                //            {STRAWBERRY,OAT, 2,  5,43,35}}

                gfprintf(COM1, "Log of recent ice cream production:\n\r");
                gfprintf(COM1, "\n\r");
                gfprintf(COM1, "#  Time      Flavour     Liquid Base  Amount\n\r");
                gfprintf(COM1, "--------------------------------------------\n\r");
                vTaskDelay(50 / portTICK_PERIOD_MS);
                // Log function
                INT8U row[6];
                char time_str[6];

                INT8U col;
                INT8U row_number;
                for (row_number = 0; row_number < num_ice; row_number++)
                {

                    for (col = 0; col < 6; col++)
                    {
                        row[col] = get_log(row_number, col);
                    }

                    // Log time
                    // Hour
                    time_str[0] = row[3] / 10 + '0';
                    time_str[1] = row[3] % 10 + '0';
                    time_str[2] = ':';
                    // Min
                    time_str[3] = row[4] / 10 + '0';
                    time_str[4] = row[4] % 10 + '0';
                    time_str[5] = ':';

                    gfprintf(COM1, "%u  %s%c%c  ",
                             row_number + 1, // Row number
                             time_str,
                             row[5] / 10 + '0',
                             row[5] % 10 + '0'); // Time

                    // Flavour //
                    /////////////

                    switch (row[0])
                    {
                    case VANILLA:; // Vanilla
                        gfprintf(COM1, "Vanilla     ");
                        break;
                    case CHOCOLATE:; // Chocolate
                        gfprintf(COM1, "Chocolate   ");
                        break;
                    case STRAWBERRY:; // Strawberry
                        gfprintf(COM1, "Strawberry  ");
                        break;
                    default:

                        break;
                    }

                    // Add liquid base //
                    /////////////////////
                    switch (row[1])
                    {
                    case MILK:; // Milk
                        gfprintf(COM1, "Milk         ");
                        break;
                    case OAT:; // Oat
                        gfprintf(COM1, "Oat          ");
                        break;
                    case COCO:; // Coco
                        gfprintf(COM1, "Coconut      ");
                        break;
                    default:

                        break;
                    }

                    // Add amount
                    gfprintf(COM1, "%uL \n\r", row[2]);
                    // Time to empty UART queue
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                gfprintf(COM1, "Press any button to continue. \n\r", row[2]);

                run_once = 0;
            }

            if (uart0_get_q(&ch))
            {
                console_state = CONSOLE_START;
                run_once = 1;
            }
            vTaskDelay(10);

            // Return when any button is pressed

            break;

        case CONSOLE_SETTINGS:;
            if (run_once)
            {

                gfprintf(COM1, "\n\r");
                gfprintf(COM1, "Settings\n\r");
                gfprintf(COM1, "You have the following options:\n\r");
                gfprintf(COM1, "1: Set churning time\n\r");
                gfprintf(COM1, "2: Set base frequency\n\r");
                gfprintf(COM1, "3: Set time\n\r");

                run_once = 0;
            }

            // SELECT SETTING
            if (uart0_get_q(&ch))
            {
                gfprintf(COM1, "%c\n\r\n\r", ch);

                switch (ch)
                {
                case '1': // Set churning time

                    console_state = CONSOLE_CHURNING_TIME;
                    run_once = 1;
                    break;
                case '2': // Set base frequency

                    console_state = CONSOLE_FREQUENCY;
                    run_once = 1;
                    break;
                case '3': // Set time

                    console_state = CONSOLE_TIME;
                    run_once = 1;
                    break;
                default:

                    gfprintf(COM1, "Invalid key press\n\r");
                    vTaskDelay(1);

                    break;
                }
            }
            vTaskDelay(10);

            break;

        // CHURNING TIME
        case CONSOLE_CHURNING_TIME:
            // FIRST RUN //
            ///////////////
            if (run_once)
            {
                gfprintf(COM1, "\n\r");
                gfprintf(COM1, "\n\r");

                gfprintf(COM1, "Churning time\n\r");
                gfprintf(COM1, "Select the flavour for which the churning time should be changed.\n\r");
                gfprintf(COM1, "1: Vanilla\n\r");
                gfprintf(COM1, "2: Chocolate\n\r");
                gfprintf(COM1, "3: Strawberry\n\r");

                run_once = 0;
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {
                uart0_put_char(ch);
                switch (ch) // OBS!
                {
                case '1': // Vanilla

                    gfprintf(COM1, "\n\rPlease enter desired churning time in seconds.\n\r");

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_vanilla = receive_input();

                        xSemaphoreGive(mutex_churning_time);
                    }

                    console_state = CONSOLE_START;

                    gfprintf(COM1, "Vanilla churning time updated.\n\r");
                    run_once = 1;

                    break;
                case '2': // Chocolate
                    gfprintf(COM1, "\n\rPlease enter desired churning time in seconds.\n\r");

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_chocolate = receive_input();

                        xSemaphoreGive(mutex_churning_time);
                    }

                    console_state = CONSOLE_START;

                    gfprintf(COM1, "Chocolate churning time updated.\n\r");
                    run_once = 1;
                    break;
                case '3': // Strawberry

                    gfprintf(COM1, "\n\rPlease enter desired churning time in seconds.\n\r");

                    if (xSemaphoreTake(mutex_churning_time, (TickType_t)10) == pdTRUE)
                    {

                        churning_time_strawberry = receive_input();
                        xSemaphoreGive(mutex_churning_time);
                    }

                    gfprintf(COM1, "Strawberry churning time updated.\n\r");

                    console_state = CONSOLE_START;
                    run_once = 1;
                    break;
                default:

                    vTaskDelay(1);
                    gfprintf(COM1, "Invalid key press\n\r");

                    break;
                }
            }
            vTaskDelay(10);

            break;
        case CONSOLE_FREQUENCY:
            // FIRST RUN //
            ///////////////
            if (run_once)
            {

                gfprintf(COM1, "Liquid base frequency\n\r");
                gfprintf(COM1, "Select the liquid base for which the churning time should be changed.\n\r");
                gfprintf(COM1, "1: Milk\n\r");
                gfprintf(COM1, "2: Oat\n\r");
                gfprintf(COM1, "3: Coconut\n\r");

                run_once = 0;
            }

            // SELECT SETTING //
            ////////////////////

            if (uart0_get_q(&ch))
            {

                switch (ch)
                {
                case '1': // Milk
                    gfprintf(COM1, "%c\n\r", ch);
                    gfprintf(COM1, "Please enter desired liquid base frequency in Hz.\n\r");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_milk = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    gfprintf(COM1, "Milk liquid base frequency updated.\n\r");

                    console_state = CONSOLE_START;
                    run_once = 1;
                    break;
                case '2': // Oat
                    gfprintf(COM1, "%c\n\r", ch);
                    gfprintf(COM1, "Please enter desired liquid base frequency in Hz.\n\r");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_oat = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    gfprintf(COM1, "Oat liquid base frequency updated.\n\r");

                    console_state = CONSOLE_START;
                    run_once = 1;
                    break;
                case '3': // Coco
                    gfprintf(COM1, "%c\n\r", ch);
                    gfprintf(COM1, "Please enter desired liquid base frequency in Hz.\n\r");

                    if (xSemaphoreTake(mutex_liquid_base, (TickType_t)10) == pdTRUE)
                    {

                        lb_frequency_coco = receive_input();
                        xSemaphoreGive(mutex_liquid_base);
                    }

                    gfprintf(COM1, "Coconut liquid base frequency updated.\n\r");

                    console_state = CONSOLE_START;
                    run_once = 1;
                    break;
                default:

                    vTaskDelay(1);
                    gfprintf(COM1, "Invalid key press\n\r");

                    break;
                }
            }
            vTaskDelay(10);

            break;
        case CONSOLE_TIME:
            if (run_once)
            {

                gfprintf(COM1, "\n\rSet time of day\n\r");
                gfprintf(COM1, "Enter the time as follows: HH:MM\n\r");

                time_length = 0;
                run_once = 0;
            }

            if (uart0_get_q(&ch))
            {

                vTaskDelay(10);
                if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
                {
                    time[time_length++] = ch - '0';
                    gfprintf(COM1, "%c", ch);
                }

                vTaskDelay(10);
                if (time_length == 4)
                {

                    // Extract user input to variables
                    time_hour = time[0] * 10;
                    time_hour += time[1];
                    time_min = time[2] * 10;
                    time_min += time[3];

                    if (time_hour <= 23 && time_min <= 59)
                    {
                        if (xSemaphoreTake(mutex_time_of_day, (TickType_t)10) == pdTRUE)
                        {

                            current_time_sec = TIME_SEC;
                            current_time_min = time_min;
                            current_time_hour = time_hour;
                            xSemaphoreGive(mutex_time_of_day);
                        }

                        gfprintf(COM1, "\n\rTime of day updated.\n\r", ch);

                        console_state = CONSOLE_START;
                        run_once = 1;
                    }
                }
                else if (time_length == 2)
                {

                    gfprintf(COM1, ":", ch);
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
