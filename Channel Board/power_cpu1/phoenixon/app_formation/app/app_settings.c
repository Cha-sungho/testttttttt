/*
 * app_settings.c
 *
 *  Created on: 2021. 12. 4.
 *      Author: JOO
 */

#include <math.h>

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "app_settings.h"
#include "app_safety.h"

//Need to setting
float set_pu_out_amps                   = 100.0f;       //Unit A
float set_pu_margin_out_amps            = 0.1f;         //Unit %
float set_pu_out_volts                  = 5.0f;         //Unit V
float set_pu_margin_out_volts           = 0.1f;         //Unit %

float set_pu_dc_link_pri_volts          = 384.0f;       //Unit V
float set_pu_margin_dc_link_pri_volts   = 0.2f;         //Unit %
float set_pu_dc_link_sec_volts          = 12.0f;        //Unit V
float set_pu_margin_dc_link_sec_volts   = 0.2f;         //Unit %

float set_pu_out_L                      = 0.00039f;     //Unit H
float set_pu_out_buck_C                 = 0.00004f;     //Unit F
float set_pu_out_boost_C                = 0.000045f;    //Unit F

float set_pu_dc_out_cap_volts           = 6.0f;
float set_pu_margin_dc_out_cap_volts    = 0.2f;         //Unit %

float set_pu_battary_volts_max          = 4.3f;
float set_pu_battary_volts_min          = 0.4f;

float set_pu_ctrl_t = T_SAMP;           //50us
float set_pu_ctrl_f = F_SAMP;           //20kHz

//To be calculation
float set_calc_grid_volts_max;
float set_calc_grid_amps_max;
float set_calc_system_out_power_kw;
float set_calc_dc_link_pri_volts_max;
float set_calc_dc_link_pri_volts_min;
float set_calc_dc_link_sec_volts_max;
float set_calc_dc_link_sec_volts_min;
float set_calc_dc_out_cap_volts_max;
float set_calc_dc_out_cap_volts_min;
float set_calc_battary_volts_max;
float set_calc_battary_volts_min;
float set_calc_output_current_max;
float set_calc_output_current_min;

void app_InitSettings(void)
{
    set_calc_dc_link_pri_volts_max  = set_pu_dc_link_pri_volts * (1.0f + set_pu_margin_dc_link_pri_volts);
    set_calc_dc_link_pri_volts_min  = set_pu_dc_link_pri_volts * (1.0f - set_pu_margin_dc_link_pri_volts);

    set_calc_dc_link_sec_volts_max  = set_pu_dc_link_sec_volts * (1.0f + set_pu_margin_dc_link_sec_volts);
    set_calc_dc_link_sec_volts_min  = set_pu_dc_link_sec_volts * (1.0f - set_pu_margin_dc_link_sec_volts);

    set_calc_dc_out_cap_volts_max   = set_pu_dc_out_cap_volts * (1.0f + set_pu_margin_dc_out_cap_volts);
    set_calc_dc_out_cap_volts_min   = set_pu_dc_out_cap_volts * (1.0f - set_pu_margin_dc_out_cap_volts);

    set_calc_battary_volts_max      = set_pu_battary_volts_max;
    set_calc_battary_volts_min      = set_pu_battary_volts_min;

    set_calc_output_current_max     = (set_pu_out_amps) * (1.0f + set_pu_margin_out_amps);
    set_calc_output_current_min     = (set_pu_out_amps) * (1.0f - set_pu_margin_out_amps);

    app_ResetSafetyVal();
}

//
// End of File
//
