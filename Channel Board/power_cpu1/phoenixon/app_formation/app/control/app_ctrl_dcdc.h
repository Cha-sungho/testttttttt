/*
 * app_ctrl_dcdc.h
 *
 *  Created on: Nov 10, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_CONTROL_APP_CTRL_DCDC_H_
#define PHOENIXON_APP_CONTROL_APP_CTRL_DCDC_H_

typedef enum
{
    dcdc_seq_stop = 0,
    dcdc_seq_stop_idle,
    dcdc_seq_precharge,
    dcdc_seq_precharging,
    dcdc_seq_relay_on,
    dcdc_seq_ready,
    dcdc_seq_cha_cc,
    dcdc_seq_cha_cv,
    dcdc_seq_cha_cp,
    dcdc_seq_cha_cr,
    dcdc_seq_dis_cc,
    dcdc_seq_dis_cv,
    dcdc_seq_dis_cp,
    dcdc_seq_dis_cr
} appCtrlDCDCSeq;

void app_CtrlDCDC(void);

#endif /* PHOENIXON_APP_CONTROL_APP_CTRL_DCDC_H_ */
