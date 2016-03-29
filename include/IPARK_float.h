/****************************************************************
* FILENAME:     IPARK_float.h             
* DESCRIPTION:  izracuna inverzno parkovo transformacijo 
* AUTHOR:       Mitja Nemec
* START DATE:   30.3.2013
* VERSION:      1.0
*
****************************************************************/
#ifndef     __IPARK_FLOAT_H__
#define     __IPARK_FLOAT_H__

#include    "math.h"

typedef struct IPARK_FLOAT_STRUCT
{  
    float  Alpha;       // Output: stationary d-axis stator variable
	float  Beta;		// Output: stationary q-axis stator variable
	float  Angle;		// Input: rotating angle (pu)
	float  Ds;			// Input: rotating d-axis stator variable
	float  Qs;			// Input: rotating q-axis stator variable
} IPARK_float;	            

/*-----------------------------------------------------------------------------
Default initalizer for the IPARK object.
-----------------------------------------------------------------------------*/                   
#define IPARK_FLOAT_DEFAULTS    \
{                               \
    0.0,                        \
    0.0,                        \
    0.0,                        \
    0.0,                        \
    0.0                         \
}
/*------------------------------------------------------------------------------
	Inverse PARK Transformation Macro Definition
------------------------------------------------------------------------------*/
#define IPARK_FLOAT_CALC(v)									\
{														    \
    v.Alpha = (v.Ds * cos(v.Angle)) - (v.Qs * sin(v.Angle));\
    v.Beta = (v.Qs * cos(v.Angle)) + (v.Ds * sin(v.Angle)); \
}

#endif // __IPARK_FLOAT_H__

