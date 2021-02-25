#include "sd_card_functions.h"

#define SD_BUFFER_MAX_SIZE      512
#define SD_CSV_ARR_LENGTH       23
#define TB_SIZE                 10
#define SD_INIT_TIMEOUT         2

unsigned char MST_Data, SLV_Data;
BYTE buffer[32];
char filename[] = "/testfile.txt";
char csvNameShortECGArr[SD_CSV_ARR_LENGTH] = "/SHORTAA.CSV";
char csvNameLongECGArr[SD_CSV_ARR_LENGTH] = "/LONGAA.CSV";
const char nextLine[] = ".\n";
char txbufferInit[] = "SD Card init successful";
char g_txbuffer[SD_BUFFER_MAX_SIZE] = "Test text for txt and csv files";
char firstLetter, secondLetter;
int result = 1;
uint16_t sdCardTimeout = 0;
unsigned int size;
unsigned int bytesWritten;
bool nameRangeOverflow = FALSE;
FRESULT file_exists = 20;
char timeBuffer[TB_SIZE];
uint16_t adcValuesCnt = 0;
uint8_t adcArrayCnt = 0;
uint8_t adcEntryCnt = 0;
bool display_sleep_mode = FALSE;
LONG_ECG_STATE_MACHINE_e g_long_ecg_state = MODE_NORMAL;
uint16_t ecg_long_array_cnt = 0;



char commaArr[1];
char adcSingleResultArr[4];
char localtimeArr[80];
char adcSingleResultArrLong[15];

void Init_FAT(void){
    errCode = 20;

    if (g_sd_card_inserted) {
        //go until f_open returns FR_OK (function successful)
        while ((errCode != FR_OK) || 
            (sdCardTimeout <= SD_INIT_TIMEOUT)) {  
            sdCardTimeout++;
            //mount drive number 0                            
            errCode = f_mount(0, &fatfs); 
            //root directory                      
            errCode = f_opendir(&dir, "/");				    	

            errCode = f_open(&file, filename, 
                FA_CREATE_ALWAYS | FA_WRITE);
            if (errCode != FR_OK) {
                //used as a debugging flag
                result = 0;
            }
            if (sdCardTimeout == SD_INIT_TIMEOUT) {
                // TODO send message to display
                SD_Card_Timeout();
            }
        }
        sdCardTimeout = 0;
        f_write(&file, txbufferInit, 
            sizeof(txbufferInit), &bytesWritten);
        f_close(&file);
        Set_SD_Icon_Display(1);
    }else{
        SD_Card_Error();
        Set_SD_Icon_Display(0);
    }
}

void SD_CreateNewCSV(void) {

    if(g_sd_card_inserted) {
        // File naming for SHORT ECG
        if (g_short_ECG_flag == 1) {

            while ((file_exists = 
                f_open(&file, csvNameShortECGArr,
                FA_OPEN_EXISTING)) == FR_OK &&
                !(nameRangeOverflow)) {

                f_close(&file);
                // Try next letter in file name
                firstLetter = csvNameShortECGArr[6];
                secondLetter = csvNameShortECGArr[7];
                if (isalpha(secondLetter) && 
                    tolower(secondLetter) != 'z') {
                    csvNameShortECGArr[7] = ++secondLetter;
                } else if (isalpha(firstLetter) && 
                    tolower(firstLetter) != 'z') {
                    csvNameShortECGArr[6] = ++firstLetter;
                    csvNameShortECGArr[7] = 'A';
                } else {
                    csvNameShortECGArr[6] = 'A';
                    csvNameShortECGArr[7] = 'A';
                    f_close(&file);
                    nameRangeOverflow = TRUE;
                }
            }

            f_open(&file, csvNameShortECGArr, 
                FA_CREATE_ALWAYS | FA_WRITE);

            // Next letter in file name
            firstLetter = csvNameShortECGArr[6];
            secondLetter = csvNameShortECGArr[7];
            if (isalpha(secondLetter) && 
                tolower(secondLetter) != 'z') {
                csvNameShortECGArr[7] = ++secondLetter;
            } else if (isalpha(firstLetter) && 
                tolower(firstLetter) != 'z') {
                csvNameShortECGArr[6] = ++firstLetter;
                csvNameShortECGArr[7] = 'A';
            } else {
                csvNameShortECGArr[6] = 'A';
                csvNameShortECGArr[7] = 'A';
            }
        // File naming for LONG EKG
        } else {
            while ((file_exists = f_open(&file, csvNameLongECGArr,
                                         FA_OPEN_EXISTING)) == FR_OK &&
            !(nameRangeOverflow)) {
                f_close(&file);
                // Try next letter in file name
                firstLetter = csvNameLongECGArr[5];
                secondLetter = csvNameLongECGArr[6];
                if (isalpha(secondLetter) && 
                    tolower(secondLetter) != 'z') {
                    csvNameLongECGArr[6] = ++secondLetter;
                } else if (isalpha(firstLetter) && 
                    tolower(firstLetter) != 'z') {
                    csvNameLongECGArr[5] = ++firstLetter;
                    csvNameLongECGArr[6] = 'A';
                } else {
                    csvNameLongECGArr[5] = 'A';
                    csvNameLongECGArr[6] = 'A';
                    f_close(&file);
                    nameRangeOverflow = TRUE;
                }
            }
            f_open(&file, csvNameLongECGArr, 
                FA_CREATE_ALWAYS | FA_WRITE);

            // Next letter in file name
            firstLetter = csvNameLongECGArr[5];
            secondLetter = csvNameLongECGArr[6];
            if (isalpha(secondLetter) && 
                tolower(secondLetter) != 'z') {
                csvNameLongECGArr[6] = ++secondLetter;
            } else if (isalpha(firstLetter) && 
                tolower(firstLetter) != 'z') {
                csvNameLongECGArr[5] = ++firstLetter;
                csvNameLongECGArr[6] = 'A';
            } else {
                csvNameLongECGArr[5] = 'A';
                csvNameLongECGArr[6] = 'A';
            }
        }
    }
}

void SD_SetTimeStamp(void){
    memset(timeBuffer,0x00,TB_SIZE);
    if (g_sys_state == ECG_SHORT){
        sprintf(timeBuffer, "%02d:%02d \n",g_cnt_min, g_cnt_sec);
    }else if (g_sys_state == ECG_LONG){
        sprintf(timeBuffer, "%02d:%02d:%02d \n",g_cnt_hour, g_cnt_min, g_cnt_sec);
    }
}

// TODO: fputs and sprintf in just one Array (see LongECG)
void SD_StartWriting(void) {
    if(g_sd_card_inserted) {
        // Set adc value
        sprintf(adcSingleResultArr, "%d", g_adc_result);
        g_tmp_return = f_puts(adcSingleResultArr, &file);

        // Set comma
        sprintf(commaArr, "%s", ",");
        g_tmp_return = f_puts(commaArr, &file);

        // Set timestamp
        SD_SetTimeStamp();
        g_tmp_return = f_puts(timeBuffer, &file);
    }
}

void SD_StopWriting(void) {
    if (g_sd_card_inserted) {
        f_close(&file);
    }
}

void SD_Energy_Saving_Long_ECG() {

    switch (g_long_ecg_state) 
    {
        // Normal mode: SD = TRUE; STORAGE = FULL; 
        //              BTN = NOT PRESSED; 5V = ON
        case MODE_NORMAL:
            // Sending to SD Card
            // TODO save in the same file
            if (g_adc_result_storage_full == TRUE) 
            {
                
                SD_CreateNewCSV();

                // Write values on SD Card
                for (ecg_long_array_cnt = 0; 
                    ecg_long_array_cnt < LONG_ECG_STORAGE_SIZE; 
                    ecg_long_array_cnt++)
                {
                    g_cnt_msec_long += 4; // 250 Hz -> 1 value per 4 msec

                    // Timestamp calculation
                    if (g_cnt_msec_long >= 1000) {
                        g_cnt_msec_long = 0;
                        g_cnt_sec_long++;
                        if (g_cnt_sec_long > 59) 
                        {
                            g_cnt_sec_long = 0;
                            g_cnt_min_long++;
                            if (g_cnt_min_long > 59) 
                            {
                                g_cnt_min_long = 0;
                                g_cnt_hour_long++;
                            }
                        } 
                    }
                    sprintf(adcSingleResultArrLong, "%d,%02d:%02d:%02d\n", g_adc_result_storage[ecg_long_array_cnt]
                                                                        , g_cnt_hour_long, g_cnt_min_long, g_cnt_sec_long);
                    // Set adc value
                    g_tmp_return = f_puts(adcSingleResultArrLong, &file);
                }

                // Close file
                SD_StopWriting();
            }
            
            if ((g_ecg_long_btn_pressed == TRUE) &&
                (g_adc_result_storage_full == FALSE))
            {
                g_long_ecg_state = MODE_5V_OFF;
            }
            else if ((g_ecg_long_btn_pressed == TRUE) &&
                    (g_adc_result_storage_full == TRUE))
            {
                g_long_ecg_state = MODE_5V_ON;
            }

            // Check if SD was inserted while turning off
            if (GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN0)) {
                Set_SD_Icon_Display(1);
            } else if (!(GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN0))){
                Set_SD_Icon_Display(0);
                SD_Card_Error();
            }

            break;



        case MODE_5V_ON:
            display_sleep_mode = TRUE;
            if (g_adc_result_storage_full == FALSE) 
            {
                g_long_ecg_state = MODE_5V_OFF;
            }

            // 5V ON
            // LED2 on PCB turn ON
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
            // 5V DC/DC turn ON
            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN6);
            // Adjust 5V flag   
            g_ecg_long_5v_on = TRUE;

            // SD Card INIT
            Init_FAT();

            // Sending to SD Card
            // TODO save in the same file
            SD_CreateNewCSV();
            f_puts(g_adc_result_storage, &file);
            SD_StopWriting();


            if (g_ecg_long_btn_pressed == FALSE)
            {
                g_long_ecg_state = MODE_NORMAL;
            }

            // When done -> change to MODE_5V_OFF
            g_long_ecg_state = MODE_5V_OFF;

            break;

        case MODE_5V_OFF:
            display_sleep_mode = TRUE;
            // 5V OFF
            // LED2 on PCB turn OFF
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
            // 5V DC/DC turn OFF
            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN6);
            // Adjust 5V flag
            g_ecg_long_5v_on = FALSE;
            if (g_adc_result_storage_full == TRUE)
            {
                g_long_ecg_state = MODE_5V_ON;
            }

            if (g_ecg_long_btn_pressed == FALSE)
            {
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
                // 5V DC/DC turn ON
                GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN6);
                 // Adjust 5V flag   
                g_ecg_long_5v_on = TRUE;
                g_long_ecg_state = MODE_NORMAL;
            }

            break;
    }
}

void Check_SD_Card_Connection()
{
    if (g_sd_state_flag == 1)
    {
        Display_Exit_Sleep_Mode();
        GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN0, GPIO_HIGH_TO_LOW_TRANSITION);
        g_sd_card_inserted = TRUE;
        Init_FAT();
        Set_SD_Icon_Display(1);
        g_sd_state_flag = 2;
    }
    else if (g_sd_state_flag == 0)
    {
        Display_Exit_Sleep_Mode();
        GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);
        g_sd_card_inserted = FALSE;
        Set_SD_Icon_Display(0);
        SD_Card_Error();
        g_sd_state_flag = 2;
    }
}
