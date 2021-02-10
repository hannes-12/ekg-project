#include <includes/config/timer_cfg.h>

void Init_Timers() {
    Init_Timer_A();
}

void Init_Timer_A() {

    // Start timer
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);

    /*** Init TIMER_A1 sourced by ACLK ***/
    Timer_A_initUpModeParam confTimerA1 = {0};
    confTimerA1.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    confTimerA1.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    confTimerA1.timerPeriod = 32768;
    confTimerA1.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    confTimerA1.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    confTimerA1.timerClear = TIMER_A_DO_CLEAR;
    confTimerA1.startTimer = true;
    Timer_A_initUpMode(TIMER_A1_BASE, &confTimerA1);

    // Start timer
    Timer_A_clearTimerInterrupt(TIMER_A2_BASE);

    /*** Init TIMER_A2 sourced by SMCLK ***/
        Timer_A_initUpModeParam confTimerA2 = {0};
        confTimerA2.clockSource = TIMER_A_CLOCKSOURCE_SMCLK; // 20447232Hz
    //    confTimerA2.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32; // 32 -> 638976Hz
        confTimerA2.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // -> 4096000 Hz / 64 = 64000 Hz
        confTimerA2.timerPeriod = 319; // 64kHz / 2 == 2 Hz
        //    confTimerA2.timerPeriod = 639; // 638976Hz / 1000Hz = 639
        // Test with approx. 2 Hz
    //    confTimerA2.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // 64 -> 319488Hz
    //    confTimerA2.timerPeriod = 63897; // approx. 2 Hz
        confTimerA2.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
        confTimerA2.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
        confTimerA2.timerClear = TIMER_A_DO_CLEAR;
        confTimerA2.startTimer = true;
        Timer_A_initUpMode(TIMER_A2_BASE, &confTimerA2);
}

//******************************************************************************
//
//This is the TIMER1_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER1_A0_VECTOR)))
#endif
void TIMER1_A0_ISR (void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN3);
	g_timer_1sec_flag = 1;
}

//******************************************************************************
//
//This is the TIMER2_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER2_A0_VECTOR)))
#endif
void TIMER2_A0_ISR (void)
{
//    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
    g_timer_1khz_flag = 1;
}
