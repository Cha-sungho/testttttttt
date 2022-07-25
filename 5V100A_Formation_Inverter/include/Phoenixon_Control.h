//###########################################################################
//
// FILE:   Phoenixon_Control.h
//
//###########################################################################

#ifndef __CONTROL_H__
#define __CONTROL_H__

/******************/
/* absolute value */
/******************/
//#define abs(var)    		(((var) > 0.) ? (var) : -(var))

/******************************/
/*        Limit Value         */
/******************************/
#define Max(in,lim)	(in = (in > lim)  ? lim  : in)
#define Min(in,lim)	(in = (in < -lim) ? -lim : in)
#define MaxMin(in,lim)	(in = (in > lim) ? lim : ((in < -lim) ? -lim : in))
#define MaxMin2(in, min, max)	((in > (max)) ? (max) : ((in < (min)) ? (min) : in))


/*********************************************/
/*               PI controller               */
/*********************************************/

/* Y(n)	= Y(n-1) + kp*X(n) + ki*T*X(n) - kp*X(n-1) */
#define DPICON(out, ref, in, iterm, kp, ki, Ts, lim)\
{  float err, tmp;\
   err = ref - in;\
   iterm += ki*Ts*err;\
   tmp = kp*err + iterm;\
   out = MaxMin(tmp, lim);\
   tmp = out - kp*err;\
   iterm = MaxMin(tmp, lim);\
}

#define DPICON2(out, ref, in, iterm, kp, ki, Ts, lim)\
{  float err, tmp;\
   err = ref - in;\
   iterm += ki*Ts*err;\
   tmp = kp*err + iterm;\
   out = MaxMin2(tmp, 0, lim);\
   tmp = out - kp*err;\
   iterm = MaxMin2(tmp, 0, lim);\
}

#define DDPICON2(out, ref, in, iterm, kp, ki, Ts, lim_Min, lim_Max)\
{  float err, tmp;\
   err = ref - in;\
   iterm += ki*Ts*err;\
   tmp = kp*err + iterm;\
   out = MaxMin2(tmp, lim_Min, lim_Max);\
   tmp = out - kp*err;\
   iterm = MaxMin2(tmp, lim_Min, lim_Max);\
}

#define DPICON3(out, ref, in, iterm, kp, ki, Ts, lim)\
{  float err, tmp;\
   err = -ref + in;\
   iterm += ki*Ts*err;\
   tmp = kp*err + iterm;\
   out = MaxMin2(tmp, 0, lim);\
   tmp = out - kp*err;\
   iterm = MaxMin2(tmp, 0, lim);\
}

#define DPICON4(out, ref, in, iterm, kp, ki, Ts, lim)\
{  float err, tmp;\
   err = -ref + in;\
   iterm += ki*Ts*err;\
   tmp = kp*err + iterm;\
   out = MaxMin(tmp, lim);\
   tmp = out - kp*err;\
   iterm = MaxMin(tmp, lim);\
}

#define DPIDCON2(out, ref, in, iterm, kp, ki, kd, Ts, lim)\
{  double err, tmp, dterm;\
	err = ref - in;\
	iterm += ki*Ts*err;\
	dterm = kd*err/Ts;\
	tmp = kp*err + iterm + dterm;\
	out = MaxMin2(tmp, 0, lim);\
	tmp = out - kp*err;\
	iterm = MaxMin2(tmp, 0, lim);\
}

#define SVPWM(Vaa,Vbb,Vcc,T,Vdc,Ta,Tb,Tc,Tmax,Tmin,Te,To,Toff,Tr,Ts,Tt)\
{\
  float tmp;\
  tmp = T / Vdc;\
  Ta = (Vaa * tmp);\
  Tb = (Vbb * tmp);\
  Tc = (Vcc * tmp);\
  Tmax = Ta;\
  Tmin = Ta;\
  if(Tb > Tmax) Tmax = Tb;\
  if(Tc > Tmax) Tmax = Tc;\
  if(Tb < Tmin) Tmin = Tb;\
  if(Tc < Tmin) Tmin = Tc;\
  Te = Tmax - Tmin;\
  To = T - Te;\
  Toff = ((To * 0.5) - Tmin);\
  if(To < 0.){\
   tmp=T / Te;\
   Ta *= tmp;\
   Tb *= tmp;\
   Tc *= tmp;\
   Toff = (-Tmin * tmp);\
  }\
  Tr = (int)(Ta + Toff);\
  Ts = (int)(Tb + Toff);\
  Tt = (int)(Tc + Toff);\
}

#define SVPWM3L(Vaa,Vbb,Vcc,T,Vdc,Ta,Tb,Tc,Tmax,Tmin,Te,To,Toff,Toff_ad,Tr,Ts,Tt)\
{\
  float tmp;\
  tmp = T / Vdc;\
  Ta = (Vaa * tmp);\
  Tb = (Vbb * tmp);\
  Tc = (Vcc * tmp);\
  Tmax = Ta;\
  Tmin = Ta;\
  if(Tb > Tmax) Tmax = Tb;\
  if(Tc > Tmax) Tmax = Tc;\
  if(Tb < Tmin) Tmin = Tb;\
  if(Tc < Tmin) Tmin = Tc;\
  Te = Tmax - Tmin;\
  To = T - Te;\
  Toff = ((To * 0.5) - Tmin)+Toff_ad;\
  if(To < 0.){\
   tmp=T / Te;\
   Ta *= tmp;\
   Tb *= tmp;\
   Tc *= tmp;\
   Toff = (-Tmin * tmp);\
  }\
  Tr = (int)(Ta + Toff);\
  Ts = (int)(Tb + Toff);\
  Tt = (int)(Tc + Toff);\
}

#define Vdq_Vabc(Vlink, Vaas, Vbbs, Vccs, Vdde, Vqqe)\
{\
	float Voa, Vob, Voc, Voaa, Vobb, Vocc, Vmax, Vmin, Vsn, Vadd_sn;\
	Voa = Vdde;\
	Vob = 0.5 * (((out_dir * root3) * Vqqe) - Vdde);\
	Voc = -(Voa + Vob);\
	Vmax = Voa;\
	Vmin = Voa;\
	if(Vob > Vmax) Vmax = Vob;\
	if(Voc > Vmax) Vmax = Voc;\
	if(Vob < Vmin) Vmin = Vob;\
	if(Voc < Vmin) Vmin = Voc;\
	Vsn = - (Vmax + Vmin) * 0.5;\
	Voa = Voa + Vsn;\
	Vob = Vob + Vsn;\
	Voc = Voc + Vsn;\
	Voaa = (int)(Voa)%(int)(Vdc *0.5);\
	Vobb = (int)(Vob)%(int)(Vdc *0.5);\
	Vocc = (int)(Voc)%(int)(Vdc *0.5);\
	Vmax = Voaa;\
	Vmin = Voaa;\
	if(Vobb > Vmax) Vmax = Vobb;\
	if(Vocc > Vmax) Vmax = Vocc;\
	if(Vobb < Vmin) Vmin = Vobb;\
	if(Vocc < Vmin) Vmin = Vocc;\
	Vadd_sn = (Vlink * 0.25)-(Vmax + Vmin) * 0.5;\
	Vaas = Voa + Vadd_sn;\
	Vbbs = Vob + Vadd_sn;\
	Vccs = Voc + Vadd_sn;\
}

#endif




