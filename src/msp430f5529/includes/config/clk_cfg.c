/*
 * clk_cfg.c
 *
 *  Created on: 8 Nov 2020
 *      Author: ivan
 */

#include <includes/config/clk_cfg.h>


//Target frequency for MCLK in kHz
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   20000 // 4000   // 20 MHz
#define FLLREF_KHZ                          32      
// MCLK/FLLRef Ratio
// 20000 kHz / 32 kHz = 625 kHz
#define UCS_MCLK_FLLREF_RATIO       UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ/FLLREF_KHZ

//*****************************************************************************
//
//Variable to store current Clock values
//
//*****************************************************************************
uint32_t clockValue = 0;

//*****************************************************************************
//
//Variable to store status of Oscillator fault flags
//
//*****************************************************************************
uint16_t status;

/*
 * Setup Clocking
 * ACLK = aux clock, slow & low-power;
 * REFO = 32768 Hz, trimmed to 3,5%;
 * */
void Init_CLK() {

    //Set VCore = 1
    PMM_setVCore(PMM_CORE_LEVEL_2);

    UCS_initClockSignal (
        UCS_ACLK,               // Configure ACLK
        UCS_REFOCLK_SELECT,     // Set to REFO source
        UCS_CLOCK_DIVIDER_1     // Set clock divider to 1
    );

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(
        UCS_FLLREF,
//            UCS_MCLK,
        UCS_REFOCLK_SELECT,
//        UCS_XT2CLK_SELECT,
        UCS_CLOCK_DIVIDER_1
        );


//    //Set DCO FLL reference = REFO
//    UCS_initClockSignal(
////        UCS_FLLREF,
//            UCS_MCLK,
////        UCS_REFOCLK_SELECT,
//        UCS_XT2CLK_SELECT,
//        UCS_CLOCK_DIVIDER_1
//        );
//
//    UCS_initClockSignal(
////        UCS_FLLREF,
//            UCS_SMCLK,
////        UCS_REFOCLK_SELECT,
//        UCS_XT2CLK_SELECT,
//        UCS_CLOCK_DIVIDER_1
//        );

    //Set Ratio and Desired MCLK Frequency and initialize DCO
    UCS_initFLLSettle(
        UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
        UCS_MCLK_FLLREF_RATIO
        );

    //Verify if the Clock settings are as expected
    clockValue = UCS_getSMCLK();    // clockValue = 4MHz
    clockValue = UCS_getMCLK();     // clockValue = 4MHz
    clockValue = UCS_getACLK();     // clockValue = 32768d
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(UNMI_VECTOR)))
#endif
void NMI_ISR(void)
{
  do {
    // If it still can't clear the oscillator fault flags after the timeout,
    // trap and wait here.
    status = UCS_clearAllOscFlagsWithTimeout(1000);
  } while(status != 0);
}
