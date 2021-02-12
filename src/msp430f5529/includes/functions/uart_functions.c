/*
 * uart_functions.c
 *
 *  Created on: 17.01.2021
 *      Author: hannes
 */
#include "includes/functions/uart_functions.h"

uint16_t test_plus_minus = 80;
uint8_t cnt_sec = 0;
uint8_t cnt_min = 0;
uint8_t cnt_hour = 0;

void Uart_ECG_Wave_ST(uint16_t adc_value)
{
    adc_value = (adc_value / 20);
    uart_transmit_data_start("add 1,0,");
    uart_transmit_data_value(adc_value);
//    uart_transmit_data_start("\r\n");

    uart_transmit_data_end();
}

void Uart_ECG_Wave_LT(uint16_t adc_value)
{
    adc_value = (adc_value / 8) - 100;
    uart_transmit_data_start("add 13,0,");
    uart_transmit_data_value(adc_value);
    uart_transmit_data_end();
}

void Test_Plus_Eins()
{
    test_plus_minus = test_plus_minus + 1;
    uart_transmit_data_start("page3.puls.val=");
    uart_transmit_data_value(test_plus_minus);
    uart_transmit_data_end();
}

void Test_Minus_Eins()
{
    test_plus_minus = test_plus_minus - 1;
    uart_transmit_data_start("page3.puls.val=");
    uart_transmit_data_value(test_plus_minus);
    uart_transmit_data_end();
}

void Clear_Wave_ST()
{
    uart_transmit_data_start("cle 1,0");
    uart_transmit_data_end();
}

void Clear_Wave_LT()
{
    uart_transmit_data_start("cle 13,0");
    uart_transmit_data_end();
}

void Short_ECG_Error()
{
    uart_transmit_data_start("page 5");
    uart_transmit_data_end();
}

void Long_ECG_Error()
{
    uart_transmit_data_start("page 6");
    uart_transmit_data_end();
}

void ST_ECG()
{
    Start_ADC();
    Uart_ECG_Wave_ST(g_adc_result);
    calculate_bpm_ST();
}

void LT_ECG()
{
    Start_ADC();
    Uart_ECG_Wave_LT(g_adc_result);
    calculate_bpm_LT();
}

void ECG_Timer_LT()
{
    if (g_timer_uart_1sec == 1)
    {
        g_timer_uart_1sec = 0;
        cnt_sec++;
        if (cnt_sec > 59 && cnt_min <= 59)
        {
            cnt_min++;
            cnt_sec = 0;
        }
        uart_transmit_data_start("page3.seconds.val=");
        uart_transmit_data_value(cnt_sec);
        uart_transmit_data_end();
        if (cnt_min > 59 && cnt_sec > 59)
        {
            cnt_hour ++;
            cnt_min = 0;
            cnt_sec = 0;
        }
        uart_transmit_data_start("page3.minutes.val=");
        uart_transmit_data_value(cnt_min);
        uart_transmit_data_end();
        if (cnt_hour == 24)
        {
            cnt_hour = 0;
            cnt_min = 0;
            cnt_sec = 0;
        }
        uart_transmit_data_start("page3.hours.val=");
        uart_transmit_data_value(cnt_hour);
        uart_transmit_data_end();
    }
}

void Clear_ECG_Timer_LT(void) {
    cnt_sec = 0;
    cnt_min = 0;
    cnt_hour = 0;
    uart_transmit_data_start("page3.seconds.val=");
    uart_transmit_data_value(cnt_sec);
    uart_transmit_data_end();
    uart_transmit_data_start("page3.minutes.val=");
    uart_transmit_data_value(cnt_min);
    uart_transmit_data_end();
    uart_transmit_data_start("page3.hours.val=");
    uart_transmit_data_value(cnt_hour);
    uart_transmit_data_end();
}

void ECG_Timer_ST()
{
    if (g_timer_uart_1sec == 1)
    {
        g_timer_uart_1sec = 0;
        cnt_sec++;
        if (cnt_sec > 59 && cnt_min <= 59)
        {
            cnt_min++;
            cnt_sec = 0;
        }
        uart_transmit_data_start("page2.seconds.val=");
        uart_transmit_data_value(cnt_sec);
        uart_transmit_data_end();
        if (cnt_min > 59 && cnt_sec > 59)
        {
            cnt_min = 0;
            cnt_sec = 0;
        }
        uart_transmit_data_start("page2.minutes.val=");
        uart_transmit_data_value(cnt_min);
        uart_transmit_data_end();
    }
}

void Clear_ECG_Timer_ST(void) {
    cnt_sec = 0;
    cnt_min = 0;
    uart_transmit_data_start("page2.seconds.val=");
    uart_transmit_data_value(cnt_sec);
    uart_transmit_data_end();
    uart_transmit_data_start("page2.minutes.val=");
    uart_transmit_data_value(cnt_min);
    uart_transmit_data_end();
}
