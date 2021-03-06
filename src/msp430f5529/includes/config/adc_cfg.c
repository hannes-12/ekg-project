#include <adc_cfg.h>

void Init_ADC()
{
    //Configure internal reference
    //If ref generator busy, WAIT
    while (REF_ACTIVE == Ref_isRefGenBusy(REF_BASE))
        ;
    //Select internal ref = 1.5V
    Ref_setReferenceVoltage(REF_BASE,
                            REF_VREF1_5V);
    //Internal Reference ON
    Ref_enableReferenceVoltage(REF_BASE);

    //Delay (~75us) for Ref to settle
    __delay_cycles(75);

    //Enable A/D channel A0
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P6, GPIO_PIN4);

    // Battery surveillance
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P6, GPIO_PIN5);

    //Configure Memory Buffer
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vr+ = Vref+ (int)
     * Vr- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam adc_cfg = {0};
    adc_cfg.memoryBufferControlIndex = ADC12_A_MEMORY_0;
    adc_cfg.inputSourceSelect = ADC12_A_INPUT_A4;
    adc_cfg.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC; // AVCC = 3V
    adc_cfg.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    adc_cfg.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;

    ADC12_A_configureMemoryParam adc_akku_cfg = {0}; // configuration for 2nd Channel for battery surveillance
    adc_akku_cfg.memoryBufferControlIndex = ADC12_A_MEMORY_1;
    adc_akku_cfg.inputSourceSelect = ADC12_A_INPUT_A5;
    adc_akku_cfg.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC; // AVCC = 3V
    adc_akku_cfg.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    adc_akku_cfg.endOfSequence = ADC12_A_ENDOFSEQUENCE;

    //Init the ADC12_A Module
    /*
     * Base address of ADC12_A Module
     * Use internal ADC12_A bit as sample/hold signal
     * to start conversion
     * USE ACLK 32kHz as clock source / EDIT SMCLK
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
                 ADC12_A_SAMPLEHOLDSOURCE_SC,
                 ADC12_A_CLOCKSOURCE_ACLK, // SMCLK
                 ADC12_A_CLOCKDIVIDER_1);

    /*
     * Base address of ADC12_A Module
     * For memory buffers 0-7 sample/hold for 64
     * clock cycles
     * For memory buffers 8-15 sample/hold for 4
     * clock cycles (default)
     * Disable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
                               ADC12_A_CYCLEHOLD_4_CYCLES,
                               ADC12_A_CYCLEHOLD_4_CYCLES,
                               ADC12_A_MULTIPLESAMPLESENABLE); // multisampling enabled

    ADC12_A_configureMemory(ADC12_A_BASE, &adc_cfg);

    ADC12_A_configureMemory(ADC12_A_BASE, &adc_akku_cfg);

    //Enable memory buffer 0 interrupt
    ADC12_A_clearInterrupt(ADC12_A_BASE,
                           ADC12IFG0);
    ADC12_A_enableInterrupt(ADC12_A_BASE,
                            ADC12IE0);

    __enable_interrupt();

    ADC12_A_enable(ADC12_A_BASE);
}

void Start_ADC()
{
    //Enable/Start sampling and conversion
    /*
     * Base address of ADC12_A Module
     * Start the conversion into memory buffer 0
     * Use the single-channel, single-conversion mode
     */

    ADC12_A_startConversion( // Conversion with two channels
        ADC12_A_BASE,
        ADC12_A_MEMORY_0,
        ADC12_A_SEQOFCHANNELS);

    //LPM0, ADC12_A_ISR will force exit
    __bis_SR_register(LPM0_bits + GIE);
    //for Debugger
    __no_operation();
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_A_ISR(void)
{
    switch (__even_in_range(ADC12IV, 34))
    {
    case 0:
        break; //Vector  0:  No interrupt
    case 2:
        break; //Vector  2:  ADC overflow
    case 4:
        break; //Vector  4:  ADC timing overflow
    case 6:    //Vector  6:  ADC12IFG0

        //Get ECG Value
        g_adc_result = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_0);

        if (g_sys_state == ECG_LONG)
        {
            g_adc_result_storage[g_adc_result_cnt] = g_adc_result;

            // Counter has values 0..999 -> LONG_ECG_STORAGE_SIZE - 1
            if (g_adc_result_cnt < (LONG_ECG_STORAGE_SIZE - 1))
            {
                g_adc_result_cnt++;
            }
            else
            {
                memcpy(g_adc_result_storage_cpy, g_adc_result_storage, 2000);
                g_adc_result_cnt = 0;
                // Flag -> storage is ready to be sent
                // Array will not be cleared
                // Flag == true -> array has new values
                g_adc_result_storage_full = TRUE;
            }
        }

        //Get accumulator Voltage
        g_akku_vol = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_1);

        //Set flag that new values are available
        g_adc_new_values = true;

        //Exit active CPU
        __bic_SR_register_on_exit(LPM0_bits);
        break;
    case 8:
        break; //Vector  8:  ADC12IFG1
    case 10:
        break; //Vector 10:  ADC12IFG2
    case 12:
        break; //Vector 12:  ADC12IFG3
    case 14:
        break; //Vector 14:  ADC12IFG4
    case 16:
        break; //Vector 16:  ADC12IFG5
    case 18:
        break; //Vector 18:  ADC12IFG6
    case 20:
        break; //Vector 20:  ADC12IFG7
    case 22:
        break; //Vector 22:  ADC12IFG8
    case 24:
        break; //Vector 24:  ADC12IFG9
    case 26:
        break; //Vector 26:  ADC12IFG10
    case 28:
        break; //Vector 28:  ADC12IFG11
    case 30:
        break; //Vector 30:  ADC12IFG12
    case 32:
        break; //Vector 32:  ADC12IFG13
    case 34:
        break; //Vector 34:  ADC12IFG14
    default:
        break;
    }
}
