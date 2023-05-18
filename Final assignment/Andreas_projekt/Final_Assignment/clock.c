/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 * MODULENAME.: run_time.c
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

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "key.h"
#include "glob_def.h"
#include "tmodel.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "projdefs.h"
#include "portmacro.h"
// #include "switches.h"
#include "leds.h"

INT8U run_time_sec;
INT8U run_time_min;
INT8U run_time_hour;

INT8U current_time_sec;
INT8U current_time_min;
INT8U current_time_hour;

void clock_task(void *pvParameters)
{
    while (1)
    {
        run_time_sec++;

        if (run_time_sec % 60 == 0)
        {
            run_time_min++;
            run_time_sec = 0;
            if (run_time_min % 60 == 0)
            {
                run_time_hour++;
                run_time_min = 0;
                if (run_time_hour % 24 == 0)
                {
                    run_time_hour = 0;
                }
            }
        }
        current_time_sec++;

        if (current_time_sec % 60 == 0)
        {
            current_time_min++;
            current_time_sec = 0;
            if (current_time_min % 60 == 0)
            {
                current_time_hour++;
                current_time_min = 0;
                if (current_time_hour % 24 == 0)
                {
                    current_time_hour = 0;
                }
            }
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
