/****************************************************************
* FILENAME:     RDIF_float.h             
* DESCRIPTION:  izracuna srednjo vrednost signala  
* AUTHOR:       Mitja Nemec
* START DATE:   12.8.2010
* VERSION:      1.0
*
****************************************************************/
#ifndef     __RDIF_FLOAT_H__
#define     __RDIF_FLOAT_H__

#include    "define.h"
/* definicija konstant, ki jih potrebujemo  */

/* vzorcna frekvenca signala */
#define     RDIF_SAMPLING_FREQ  SAMPLE_FREQ

/* frekvenca odvoda */
#define     RDIF_FREQ           200

// velikost bufferja
#define     RDIF_BUFF_SIZE      (RDIF_SAMPLING_FREQ/RDIF_FREQ)

typedef struct RDIF_FLOAT_STRUCT
{
    float   in;                    // vhod
    float   out;                   // odvod
    float   diff;                  // cista razlika
    int     m;                     // sample index
    float   buffer[RDIF_BUFF_SIZE]; // buffer
} RDIF_float;

/*-----------------------------------------------------------------------------
Default initalizer for the DC_float object.
-----------------------------------------------------------------------------*/                     
#define RDIF_FLOAT_DEFAULTS \
{                           \
    0.0,                    \
    0.0,                    \
    0.0,                    \
    0                       \
}

/*------------------------------------------------------------------------------
 DC Macro Definition for main function
------------------------------------------------------------------------------*/
#define RDIF_FLOAT_CALC(v)                                     \
{                                                               \
    v.diff = v.in - v.buffer[v.m];                              \
    v.buffer[v.m] = v.in;                                       \
    v.out = v.diff;                                             \
    v.m++;                                                      \
    if (v.m == RDIF_BUFF_SIZE)                                  \
    {                                                           \
        v.m = 0;                                                \
    }                                                           \
}

/*------------------------------------------------------------------------------
 DFT Macro Definition for initialization function
------------------------------------------------------------------------------*/
#define RDIF_FLOAT_INIT(v)                      \
{                                               \
    for (v.m = 0; v.m < RDIF_BUFF_SIZE; v.m++)  \
    {                                           \
        v.buffer[v.m] = 0.0;                    \
    }                                           \
    v.m = 0;                                    \
}

#endif // __DIFF_FLOAT_H__
