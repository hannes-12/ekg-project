/*
 * main.h
 *
 *  Created on: 8 Nov 2020
 *      Author: ivan
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <msp430.h>
#include <includes/driverlib/driverlib.h>
#include <includes/driverlib/std_types.h>
#include "includes/config/adc_cfg.h"
#include "includes/config/uart_cfg.h"
#include "includes/config/gpio_cfg.h"
#include "includes/config/clk_cfg.h"
#include "includes/config/timer_cfg.h"
#include "includes/config/fir_filter.h"
#include "includes/config/iir_filter.h"


/***** GLOBALE VARS *****/

uint8_t uart_puls_counter = 0;
uint8_t adc_start_stop = 0;
uint8_t timer_start_stop = 0;

uint16_t adc_result = 0;

#endif /* MAIN_H_ */
