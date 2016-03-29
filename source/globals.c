/****************************************************************
* FILENAME:     globals.c
* DESCRIPTION:  global variables
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/
#include "globals.h"

// spremenljivka stanja
volatile enum STATE state = Startup;

// nacin delovanja
volatile enum MODE mode = Speed;

// kašen referenèen signal imam
volatile enum REF_TYPE ref_type = Konst;

//struktura z zastavicami napake
struct FAULT_FLAGS fault_flags =
{
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE
};

// signaliziram, da je offset kalibriran
volatile bool current_offset_calibrated = FALSE;




