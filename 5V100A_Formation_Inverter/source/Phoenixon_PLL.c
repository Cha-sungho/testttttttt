
#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"

int   freq_check_flag=0, freq_50Hz=0, freq_60Hz=0;
float temp_theta=0., theta=0., tmp_aa=0.;
float base_angle=(_2PI * 60. * Tsamp);
float base_50Hz_angle=(_2PI * 50. * Tsamp);
float base_60Hz_angle=(_2PI * 60. * Tsamp);
float sin_pll=0., cos_pll=0., sin_pll_line=0., cos_pll_line=0.;
float out_dir = 1., angle_Kp=0.75, angle_Ki=5.;
float Vds=0., Vqs=0., Vde=0., Vqe=0., Vde_p=0., Vqe_p=0.;
float Vde_L=0., Vqe_L=0., Err_Vde=0., Teme=2.;

//float Vde_p0=0., Vde_p1=0., Vde_p2=0., Vde_p3=0., Vde_p4=0.;
//float Vqe_p0=0., Vqe_p1=0., Vqe_p2=0., Vqe_p3=0., Vqe_p4=0.;
float Vde_p0=0., Vde_p1=0.;
float Vqe_p0=0., Vqe_p1=0.;

void Pll(void)
{
    //stationary
	Vds = ((2. * Va) - Vb - Vc) * inv3;//amplitude of voltage (==sin)
	Vqs = (out_dir * inv_root3) * (Va + (2. * Vb));//(==cos)

	//rotating
	Vde_p0 = (cos_pll_line * Vds) + (sin_pll_line * Vqs);
	Vqe_p0 = (cos_pll_line * Vqs) - (sin_pll_line * Vds);//amplitude of voltage

	Vde = ( Vde_p0 + Vde_p1) * 0.5;
	Vqe = ( Vqe_p0 + Vqe_p1) * 0.5;
//	Vde = ( Vde_p0 + Vde_p1+ Vde_p2 + Vde_p3 + Vde_p4)*0.2;
//	Vqe = ( Vqe_p0 + Vqe_p1+ Vqe_p2 + Vqe_p3 + Vqe_p4)*0.2;

	Vde_p1 = Vde_p0;
	Vqe_p1 = Vqe_p0;

//	Vde_p2 = Vde_p1;
//	Vqe_p2 = Vqe_p1;
//	Vde_p3 = Vde_p2;
//	Vqe_p3 = Vqe_p2;
//	Vde_p4 = Vde_p3;
//	Vqe_p4 = Vqe_p3;

	Vde_L = (La_V * Vde_L) + (Lb_V * (Vde + Vde_p)); Vde_p = Vde;
	Vqe_L = (La_V * Vqe_L) + (Lb_V * (Vqe + Vqe_p)); Vqe_p = Vqe;

	tmp_aa = -Vde_L;
	temp_theta += (angle_Kp * (tmp_aa - Err_Vde)) + ((angle_Ki * tmp_aa) * (Tsamp));
	Err_Vde = tmp_aa;

	if (freq_check_flag == 0)   { temp_theta += base_angle; }
	else if (freq_check_flag == 1)
	{
		if (freq_50Hz == 1)     { temp_theta += base_50Hz_angle; }
		if (freq_60Hz == 1)     { temp_theta += base_60Hz_angle; }
	}

	if (temp_theta > PI)  temp_theta -= _2PI;
	if (temp_theta < -PI) temp_theta += _2PI;

	theta = (temp_theta * (TF * Teme)) + 2048.;//1024 -> 2048 -> 4096  Teme 1 -> 2 -> 4
	if ((int)theta > 4095.) { theta = theta - 4095.; }//2047  -> 4095 -> 8191
	if ((int)theta < 0)     { theta = 0.; }

	sin_pll_line = sine_table[(int)theta];
	cos_pll_line = sine_table[(int)theta + 1024];//512 -> 1024 -> 2048

	sin_pll = (sin_pll_line*0.866) - (cos_pll_line*0.5);//sin(60)==0.866, cos(60)==0.5
	cos_pll = (cos_pll_line*0.866) + (sin_pll_line*0.5);
}





