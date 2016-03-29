/****************************************************************
* FILENAME:     VAR_stat.h
* DESCRIPTION:  izracuna srednjo vrednost, varianco, sigmo in R.M.S
*               signala ob spremenljivem casu vzorcenja
* AUTHOR:       Mitja Nemec
* START DATE:   25.7.2014
* VERSION:      1.0
*
****************************************************************/
#ifndef     __VAR_STAT_H__
#define     __VAR_STAT_H__

#include    "math.h"
#include    "define.h"

/* definicija konstant, ki jih potrebujemo  */

/* velikost našega okna - št vzorcev v periodi */
#define     VAR_STAT_SIZE    1000

typedef struct VAR_STAT_STRUCT
{
    float   In;                     // Input
    float   Mean;                   // output
    float   Rms;
    float   Std;
    float   Var;
    float   Sum_square;
    float   Index_value;            // Index value [0-1]               
    int     m;                      // index
    int     m_prev;                 // index previous
    float   Buffer[VAR_STAT_SIZE];  // buffer B
} VAR_stat;

typedef VAR_stat* VAR_stat_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the DC_fixed object.
-----------------------------------------------------------------------------*/                     
#define VAR_STAT_DEFAULTS   \
{                            \
    0.0,                     \
    0.0,                     \
    0.0,                     \
    0.0,                     \
    0.0,                     \
    0.0,                     \
    0,                       \
    VAR_STAT_SIZE - 1        \
}

/*------------------------------------------------------------------------------
 VAR_stat macro Definition for main function
------------------------------------------------------------------------------*/
#define VAR_STAT_MACRO(v)                                   \
{                                                           \
    if (v.Index_value < 0.0) v.Index_value = 0.0;           \
    if (v.Index_value >= 1.0) v.Index_value = 0.0;          \
    v.m = VAR_STAT_SIZE * v.Index_value;                    \
                                                            \
    if (v.m != v.m_prev)                                    \
    {                                                       \
        v.Mean = v.Mean + ((1.0/VAR_STAT_SIZE)              \
                        * (v.In - v.Buffer[v.m]));          \
                                                            \
        v.Sum_square = v.Sum_square                         \
                     - (v.Buffer[v.m] * v.Buffer[v.m])      \
                     + (v.In * v.In);                       \
        v.Rms = sqrt(fabs(v.Sum_square/VAR_STAT_SIZE));     \
                                                            \
        v.Var = fabs((v.Mean * v.Mean) - (v.Rms * v.Rms));  \
        v.Std = sqrt(v.Var);                                \
        v.Buffer[v.m] = v.In;                               \
    }                                                       \
    v.m_prev = v.m;                                         \
}

/*------------------------------------------------------------------------------
 VAR_stat macro Definition for initialization function
------------------------------------------------------------------------------*/
#define VAR_STAT_MACRO_INIT(v)                      \
{                                                   \
    for (v.m = 0; v.m < VAR_STAT_SIZE; v.m++)    \
    {                                               \
        v.Buffer[v.m] = (0.0);                      \
    }                                               \
    v.m = 0;                                        \
}

#endif // __VAR_STAT_H__
