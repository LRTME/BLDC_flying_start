/****************************************************************
* FILENAME:     PID_float.h             
* DESCRIPTION:  declarations of Initialization & Support Functions.
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
****************************************************************/
#ifndef __PID_FLOAT_H__
#define __PID_FLOAT_H__

typedef struct PID_FLOAT_STRUCT
{
    float Ref;              // Input: Reference input
    float Fdb;              // Input: Feedback input
    float Err;              // Variable: Error
    float Kp;               // Parameter: Proportional gain
    float Ki;               // Parameter: Integral gain = 1/Ti
    float Kd;               // Parameter: Derivative gain
    float Up;               // Variable: Proportional output
    float Ui;               // Variable: Integral output
    float Ud;               // Variable: Derivative output
    float OutMax;           // Parameter: Maximum output
    float OutMin;           // Parameter: Minimum output
    float Out;              // Output: PID output
    float Fdb1;             // History: Previous Feedback input
    float Sampling_period;  // Parameter: sampling time
} PID_float;            

/*-----------------------------------------------------------------------------
Default initalizer for the PID_float object.
-----------------------------------------------------------------------------*/                     
#define PID_FLOAT_DEFAULTS  \
{           \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0,    \
    0.0     \
}

/*------------------------------------------------------------------------------
 PID Macro Definition
------------------------------------------------------------------------------*/
#define PID_FLOAT_CALC(v)                                   \
{                                                           \
    v.Err = v.Ref - v.Fdb;                                  \
    v.Up = v.Kp * v.Err;                                    \
    v.Ud = v.Kd * (v.Fdb1 - v.Fdb) / v.Sampling_period;     \
    v.Fdb1 = v.Fdb;                                         \
    v.Out = v.Up + v.Ui + v.Ud;                             \
    if (v.Out > v.OutMax)                                   \
    {                                                       \
        v.Out = v.OutMax;                                   \
        if  (v.Ui < 0.0)                                    \
        {                                                   \
            v.Ui = v.Up * v.Sampling_period * v.Ki + v.Ui;  \
        }                                                   \
    }                                                       \
    else if (v.Out < v.OutMin)                              \
    {                                                       \
        v.Out = v.OutMin;                                   \
        if  (v.Ui > 0.0)                                    \
        {                                                   \
            v.Ui = v.Up * v.Sampling_period * v.Ki + v.Ui;  \
        }                                                   \
    }                                                       \
    else                                                    \
    {                                                       \
        v.Ui = v.Up * v.Sampling_period * v.Ki + v.Ui;      \
    }                                                       \
}    
#endif // __PID_FLOAT_H__
