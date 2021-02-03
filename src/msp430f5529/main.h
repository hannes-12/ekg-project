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
#include "includes/config/median_cfg.h"



extern sMedianFilter_t medianFilter;
extern uint8_t g_median_value;

//uint16_t uart_timer_one_sec = 0;
extern uint8_t adc_ready;
//uint8_t flag = 0;
//uint8_t centisecond = 0;
extern bool enable_functionality;
extern uint8_t one_sec;
extern uint16_t akku_vol;
extern uint16_t adc_result;
extern bool buzzer_flag;


#endif /* MAIN_H_ */

