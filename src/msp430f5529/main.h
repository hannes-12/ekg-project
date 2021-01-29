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
#include "includes/config/spi_cfg.h"
#include "includes/config/sd-card-cfg/diskio.h"
#include "includes/config/sd-card-cfg/ff.h"
#include "includes/config/sd-card-cfg/ffconf.h"
#include "includes/config/sd-card-cfg/HAL_SDCard.h"
#include "includes/config/sd-card-cfg/mmc.h"
#include "includes/functions/uart_functions.h"
#include "includes/functions/bpm_function.h"
#include "includes/functions/sd_card_functions.h"
#include "integer.h"

typedef enum
{
    SYS_INIT,
    SYS_NO_ACTION,  // "Leerlauf" - ohne Funktion
    ECG_SHORT,
    ECG_LONG,
    ENERGY_SAVING_MODE,
    IDLE_STATE,
    SYS_ERROR,
    SYS_DIRTY_START,
    SYS_BAD_KEY
} STATE_MACHINE_e;

/***** GLOBALE VARS *****/
extern uint8_t g_short_ECG_flag;
extern uint8_t g_long_ECG_flag;
extern uint8_t g_timer_1khz_flag;
extern uint8_t g_timer_1sec_flag;
extern uint16_t g_adc_result;
extern uint8_t g_tmp_return;
extern STATE_MACHINE_e g_sys_state;

#endif /* MAIN_H_ */
