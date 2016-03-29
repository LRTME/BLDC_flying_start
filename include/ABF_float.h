/****************************************************************
* FILENAME:     ABF_float.h
* DESCRIPTION:  Alfa-Beta filter
* AUTHOR:       David Kavreèiè
* START DATE:   16.1.2009
****************************************************************/
#ifndef     __ABF_FLOAT__
#define     __ABF_FLOAT__

#include    "define.h"

#define     ABF_FLOAT_SAMPLING_FREQ  SWITCH_FREQ

// Diskretizacijski korak trackerja
#define     ABF_Ts              (1.0 / ABF_FLOAT_SAMPLING_FREQ)


typedef struct ABF_FLOAT_STRUCT
{
	float ThetaInK;              // Izmerjen Theta(k)
	float ThetaOutK;             // Ocenjen  Theta(k)
	float ThetaOutK_1;           //          Theta(k-1)
	float ThetaDotOutK;          // Ocenjena ThetaDot(k)
	float ThetaDotOutK_1;        //          ThetaDot(k-1)
	float ErrorThetaK;           // Napaka med dejanskim in izmerjenim kotom
	float Alpha;                 // Parametra trackerja
	float Beta;
	float Sampling_period;

} ABF_float;

#define ABF_FLOAT_DEFAULTS  \
{                           \
	0.0,                    \
	0.0,                    \
	0.0,                    \
	0.0,                    \
	0.0,                    \
	0.0,                    \
	0.0,                    \
    0.0,                    \
	0.0                     \
}

extern void ABF_float_calc(ABF_float *v);

#endif  // __ABF_FLOAT__
