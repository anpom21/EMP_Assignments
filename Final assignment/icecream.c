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
// SYSTEM FILES
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "tmodel.h"

// USER FILES
#include "adc.h"
#include "buttons.h"
#include "clock.h"
#include "file.h"
#include "icecream.h"
#include "key.h"
#include "lcd.h"
#include "leds.h"
#include "rotary_encoder.h"
#include "string.h"
#include "uart0.h"

// FREE-RTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "projdefs.h"
#include "portmacro.h"
#include "timers.h"

/*****************************    Defines    *******************************/
//******** states************/
#define LOCKED 0
#define FLAVOUR 1
#define LIQUID 2
#define AMOUNT 3
#define CUP 4
#define PRODUCTION 5
#define LOGGING 6

#define LOCKED 0
#define UNLOCKED 1
#define PASSWORD_SIZE 4

// FLAVOURS
#define VANILLA 1
#define CHOCOLATE 2
#define STRAWBERRY 3

// LIQUID BASE
#define MILK 0
#define OAT 1
#define COCO 2
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern QueueHandle_t q_uart_tx;
extern QueueHandle_t q_uart_rx;

extern SemaphoreHandle_t lcd_mutex;

extern TaskHandle_t Tflavour;
extern TaskHandle_t Tlocked;
extern TaskHandle_t Tliquid;
extern TaskHandle_t TchooseAmount;
extern TaskHandle_t TplaceCup;
extern TaskHandle_t Tproducing;
extern TaskHandle_t Tlogging;

// for encoder
extern INT8U rot_cnt;
extern INT8U rot_pressed;

// run_time
extern INT8U run_time_sec;
extern INT8U run_time_min;
extern INT8U run_time_hour;

extern INT8U current_time_sec;
extern INT8U current_time_min;
extern INT8U current_time_hour;

INT8U hour;
INT8U min;
INT8U sec;

INT8U ice_state = LOCKED;

// LOCKED
INT8U key;
INT8U cursor_x = 0;
INT16U keys_pressed = 0;

// AMOUNT
INT8U icecream_amount = 0; // in Liters

// FLAVOUR
INT8U chosen_flavour = 0;

// LIQUID BASE
INT8U liquid_base = 0;

INT8U time_of_day = 0;

typedef struct
{
    INT8U icecream_flavour;
    INT8U icecream_liquid;
    INT8U icecream_amount;
    INT8U production_time_hour;
    INT8U production_time_min;
    INT8U production_time_sec;
} icecream;

icecream current_ice;
icecream produced[100];
INT8U num_ice = 0;

/*****************************   Functions   *******************************/

INT8U ascii_to_num(INT8U num)
{
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

void add_to_code(INT8U num)
{
    switch (cursor_x % 4)
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

void locked_task(void *pvParameters)
{
    while (1)
    {

        gfprintf(COM1, "hour: %u, min: %u, sec: %u\r", run_time_hour, run_time_min, run_time_sec);
        lcd_write("LOCKED", 0, 0);
        if (get_keyboard(&key))
        {
            key = ascii_to_num(key);                         // convert the key pressed from ascii to int
            add_to_code(key);                                // add the new key to the string of pressed digits
            lcd_write("*", (cursor_x++) % PASSWORD_SIZE, 1); // note the key on the LCD
            cursor_x = cursor_x % PASSWORD_SIZE;             // done so that modulo doesnt need to be taken all the time
            if (cursor_x == 0)
            { // if 4 digits have been pressed
                if (keys_pressed % 8 == 0)
                { // check if valid password
                    // ice_state = FLAVOUR;
                    clr_LCD(); // be nice and clean after yourself :D
                    vTaskResume(Tflavour);
                    vTaskSuspend(NULL);
                }
                else
                {
                    clr_LCD();
                }
            }
        }
    }
}

void flavour_task(void *pvParameters)
{
    while (1)
    {
        lcd_write("1:van  2:choco", 0, 0);
        lcd_write("3:straw", 0, 1);
        if (get_keyboard(&key))
        {
            key = ascii_to_num(key);
            if (key == 1 || key == 2 || key == 3)
            {
                // ice_state = AMOUNT;
                chosen_flavour = key;
                clr_LCD();
                vTaskResume(Tliquid);
                vTaskSuspend(NULL);
            }
        }
    }
}

void liquid_task(void *pvParameters)
{
    // liquid_base = 0;
    while (1)
    {
        liquid_base = rot_cnt % 3;
        switch (rot_cnt % 3)
        {
        case MILK:
            lcd_write("milk", 0, 0);
            break;
        case OAT:
            lcd_write("oat ", 0, 0);
            break;
        case COCO:
            lcd_write("coco", 0, 0);
            break;
        default:
            break;
        }
        if (rot_pressed)
        {

            clr_LCD();
            vTaskResume(TchooseAmount);
            vTaskSuspend(NULL);
        }
    }
}

void choose_amount_task(void *pvParameters)
{
    INT16U adc_val;
    while (1)
    {
        adc_val = get_adc(); // adc max value is 4095
        if (adc_val < ADC_MAX_VAL / 3)
        {
            icecream_amount = 1;
        }
        else if (adc_val < ADC_MAX_VAL / 3 * 2)
        {
            icecream_amount = 2;
        }
        else
        {
            icecream_amount = 3;
        }

        // 1: Take mutex
        if (xSemaphoreTake(lcd_mutex, (TickType_t)10) == pdTRUE)

            // 2: Move cursor position to desired location
            move_LCD(0, 0);

        // 2.1: Display can be cleared when necessary (should not be cleared every time it is written to, only when a new 'menu' should be displayed.)
        // clr_LCD();

        // 3: Write to LCD with a string or char
        switch (icecream_amount)
        {
        case 1:
            lcd_write("amount 1l", 0, 0);
            break;
        case 2:
            lcd_write("amount 2l", 0, 0);
            break;
        case 3:
            lcd_write("amount 3l", 0, 0);
            break;
        default:
            break;
        }

        // 4: Give back mutex
        xSemaphoreGive(lcd_mutex);
        if (get_keyboard(&key))
        {
            clr_LCD();
            vTaskResume(TplaceCup);
            vTaskSuspend(NULL);
        }
    }
}

void place_cup_task(void *pvParameters)
{
    BOOLEAN forgot_cup = FALSE;
    while (1)
    {
        if (xSemaphoreTake(lcd_mutex, (TickType_t)10) == pdTRUE)
        {
            if (!forgot_cup)
            {
                lcd_write("place cup and", 0, 0);
                lcd_write("press start", 0, 1);
            }
            else if (forgot_cup)
            {

                lcd_write("PLACE CUP!!", 0, 0);
            }
            xSemaphoreGive(lcd_mutex);
        }

        if (sw1_pushed() && sw2_pushed())
        {
            clr_LCD();
            forgot_cup = FALSE; // clean up for next time
            vTaskResume(Tproducing);
            vTaskSuspend(NULL);
        }
        else if (sw2_pushed())
        {
            clr_LCD();
            forgot_cup = TRUE;
        }
    }
};

void log_data()
{
    current_ice.icecream_flavour = chosen_flavour;
    current_ice.icecream_amount = icecream_amount;
    current_ice.icecream_liquid = liquid_base;
    current_ice.production_time_hour = current_time_hour;
    current_ice.production_time_min = current_time_min;
    current_ice.production_time_sec = current_time_sec;

    produced[num_ice] = current_ice;
    num_ice++;
    gfprintf(COM1, "flavour: %u, amount: %u, liquid: %u\r", current_ice.icecream_flavour, current_ice.icecream_amount, current_ice.icecream_liquid);
}

void producing_task(void *pvParameters)
{
    INT8U production_time = 0;
    float production_frequency = 1;
    INT8U timer_counter = 0;
    BOOLEAN finished = FALSE;

    while (1)
    {
        lcd_write("producing...", 0, 0);

        switch (liquid_base)
        {
        case MILK:
            production_frequency = 1;
            break;
        case OAT:
            production_frequency = 2;
            break;
        case COCO:
            production_frequency = 4;
            break;
        default:
            break;
        }

        switch (chosen_flavour)
        {
        case VANILLA:
            production_time = 5;
            break;
        case CHOCOLATE:
            production_time = 7;
            break;
        case STRAWBERRY:
            production_time = 10;
            break;
        default:
            break;
        }
        production_time *= icecream_amount;
        switch (chosen_flavour)
        {
        case VANILLA:
            finished = led_flashing(LED_YELLOW, production_frequency, production_time);
            break;
        case CHOCOLATE:
            finished = led_flashing(LED_GREEN, production_frequency, production_time);
            break;
        case STRAWBERRY:
            finished = led_flashing(LED_RED, production_frequency, production_time);
            break;
        default:
            break;
        }

        if (finished)
        {
            clr_LCD();
            // ice_state = LOCKED;
            // log_data();
            vTaskResume(Tlocked);
            vTaskSuspend(NULL);
        }
    }
}

INT8U *get_log(INT8U index)
{
    INT8U logged_icecream[6];
    logged_icecream[0] = produced[index].icecream_flavour;
    logged_icecream[1] = produced[index].icecream_liquid;
    logged_icecream[2] = produced[index].icecream_amount;
    logged_icecream[3] = produced[index].production_time_hour;
    logged_icecream[4] = produced[index].production_time_min;
    logged_icecream[5] = produced[index].production_time_sec;
    return logged_icecream;
}

/*extern void icecream_task(void *pvParameters ){
    ice_state = LOCKED;
    while(1){
    switch (ice_state)
    {
    case LOCKED:
        locked();
        break;
    case FLAVOUR:
        flavour();
        break;
    case AMOUNT:
        choose_amount();
        break;
    case CUP:
        place_cup();
        break;
    case PRODUCTION:
        producing();
    default:
        break;
    }
    }
}*/