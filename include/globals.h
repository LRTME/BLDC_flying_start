/****************************************************************
* FILENAME:     globals.h
* DESCRIPTION:  global variables header file
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/
#ifndef     __GLOBALS_H__
#define     __GLOBALS_H__

#include    "DSP28x_Project.h"

#include    "define.h"
#include    "globals.h"

// seznam globalnih spremenljivk
extern volatile enum STATE { Startup = 0, Standby, Work, Fault, Fault_sensed} state;

// nacin delovanja
extern volatile enum MODE  { Position = 0, Speed, Torque, Open_loop} mode;

// nacin delovanja
extern volatile enum TYPE  { PMSM = 0, BLDC} type;

// kašen referenèen signal imam
extern volatile enum REF_TYPE {Step = 0, Slew, Konst, Sine} ref_type;

//struktura z zastavicami napake
extern struct FAULT_FLAGS
{
    bool    overcurrent:1;
    bool    HW_overcurent:1;
    bool    overtorque:1;
    bool    overspeed:1;
    bool    undervoltage:1;
    bool    overvoltage:1;
    bool    cpu_overrun:1;
    bool    fault_registered:1;
} fault_flags;

// signaliziram, da je offset kalibriran
extern volatile bool current_offset_calibrated;

#endif // end of __GLOBALS_H__ definition
