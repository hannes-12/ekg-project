#include "main.h"

/* Function declarations */
void Init_Watchdog(void);
void EnableGlobalInterrupt(void);

/* Function definitions */
void main(void){
    /* Init MSP430 BEGIN */
    Init_Watchdog();
    Init_GPIO();
    Init_CLK();
    // TODO Timers
    Init_UART();
    Init_ADC();
    EnableGlobalInterrupt();
    /* Init MSP430 END */

    P1DIR |= BIT0;                            // P1.0 set as output

    unsigned int i, j;
      while(1)                                  // continuous loop
      {
        P1OUT ^= BIT0;                          // XOR P1.0
        for(i=65000;i>0;i--)                    // Delay
            for(j=2;j>0;j--);
      }

	while(1) {
//	    Test_UART();
	    Test_ADC();
	}
}

/*** Configure Watchdog Timer ***/
void Init_Watchdog(void) {
	WDT_A_hold(WDT_A_BASE);
}

/*** Global Interrupt Enable ***/
void EnableGlobalInterrupt() {
    // Enter interrupt
    __bis_SR_register(GIE);
    // For debugger
    __no_operation();
}








