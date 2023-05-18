/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: icecream.h
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
 * 150321  MoH   Module created.
 *
 *****************************************************************************/

#ifndef _ICECREAM_H
#define _ICECREAM_H
#include "emp_type.h"

// FLAVOURS
#define VANILLA 1
#define CHOCOLATE 2
#define STRAWBERRY 3

// LIQUID BASE
#define MILK 0
#define OAT 1
#define COCO 2

//extern void icecream_task(void *pvParameters);
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : FreeRTOS task
 ******************************************************************************/
void locked_task(void *pvParameters);
void flavour_task(void *pvParameters);
void liquid_task(void *pvParameters);
void choose_amount_task(void *pvParameters);
void place_cup_task(void *pvParameters);
void producing_task(void *pvParameters);
void logging_task(void *pvParameters);

INT8U get_log(INT8U row, INT8U col);
void ice_cream_init();

#endif
