/*
 * app_test.c
 *
 *  Created on: 2022. 2. 5.
 *      Author: boomw
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "bsp_io.h"

#include "drv_pwm.h"
#include "app_calibration.h"

uint16_t test_io_flag = 0;

uint16_t test_io_din1 = 0;
uint16_t test_io_din2 = 0;
uint16_t test_io_mux1 = 0;
uint16_t test_io_mux2 = 0;
uint16_t test_io_mux3 = 0;
uint16_t test_io_mux4 = 0;
uint16_t test_io_fuse = 0;
uint16_t test_io_emo = 0;

uint16_t test_pwm_flag = 0;
float test_pwm_duty_a = 0.5f;
float test_pwm_duty_b = 0.5f;

/*
 * Test IO Func
 */
void app_TestIO(void)
{
    switch (test_io_flag)
    {
        case 0:     break;
        case 1:     bsp_IoOnDspLED1(1);        break;
        case 2:     bsp_IoOnDspLED1(0);        break;

        case 3:     bsp_IoOnDspLED2(1);        break;
        case 4:     bsp_IoOnDspLED2(0);        break;

        case 5:     bsp_IoOnDspLED3(1);        break;
        case 6:     bsp_IoOnDspLED3(0);        break;

        case 7:     bsp_IoOnDspLED4(1);        break;
        case 8:     bsp_IoOnDspLED4(0);        break;

        case 9:     bsp_IoOn384VLED(1);        break;
        case 10:    bsp_IoOn384VLED(0);        break;

        case 11:    bsp_IoFaultLED(1);         break;
        case 12:    bsp_IoFaultLED(0);         break;

        case 13:    bsp_IoRunLED(1);           break;
        case 14:    bsp_IoRunLED(0);           break;

        case 15:    bsp_IoEnableInput(1);      break;
        case 16:    bsp_IoEnableInput(0);      break;

        case 17:    bsp_sec_mosfet(1);         break;
        case 18:    bsp_sec_mosfet(0);         break;

        case 19:    bsp_IoEnableConv(1);       break;
        case 20:    bsp_IoEnableConv(0);       break;

        case 21:    bsp_IoEnableBattSen(1);    break;
        case 22:    bsp_IoEnableBattSen(0);    break;

        case 23:    bsp_IoEnablePreCharge(1);  break;
        case 24:    bsp_IoEnablePreCharge(0);  break;

        case 25:    bsp_IoDout(1);             break;
        case 26:    bsp_IoDout(0);             break;

        case 100:
            test_io_din1 = bsp_IoDin1();
            test_io_din2 = bsp_IoDin2();
#if(DEBUGGING != 2)
            test_io_mux1 = bsp_IoChMux1();
            test_io_mux2 = bsp_IoChMux2();
#endif
            test_io_mux3 = bsp_IoChMux3();
            test_io_mux4 = bsp_IoChMux4();
            test_io_fuse = bsp_IoFaultOutFuse();
            test_io_emo  = bsp_IoFaultEMO();
            break;

        default:
            break;
    }
}

/*
 * Test PWM Func
 */
void app_TestPWM(void)
{
    switch (test_pwm_flag)
    {
        case 0:
            break;
        case 1:
            drv_SetDCDCPWMDuty(test_pwm_duty_a, test_pwm_duty_b, 1);
            break;
        case 2:
            test_pwm_duty_a += 0.00001;
            if (test_pwm_duty_a >= 0.97) test_pwm_duty_a = 0.03;
            drv_SetDCDCPWMDuty(test_pwm_duty_a, test_pwm_duty_a, 1);
            break;

        case 3:
            drv_SetBB1PWMDuty(test_pwm_duty_a, 1);
            break;
        case 4:
            test_pwm_duty_a += 0.00001;
            if (test_pwm_duty_a >= 0.97) test_pwm_duty_a = 0.03;
            drv_SetBB1PWMDuty(test_pwm_duty_a, 1);
            break;

        case 5:
            test_pwm_flag = 0;
            drv_EnablePWM();
            break;
        case 6:
            test_pwm_flag = 0;
            drv_DisablePWM();
            break;
        default:
            break;
    }
}

//
// End of File
//
