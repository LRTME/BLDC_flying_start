/****************************************************************
* FILENAME:     PER_int.h
* DESCRIPTION:  periodic interrupt header file
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/
#ifndef     __PER_INT_H__
#define     __PER_INT_H__

#include    "DSP28x_Project.h"

#include    "define.h"
#include    "globals.h"

#include    "SVM_drv.h"
#include    "ADC_drv.h"
#include    "SPI_dajalnik.h"
#include    "DLOG_gen.h"

#include    "PID_float.h"
#include    "RDIF_float.h"
#include    "CLARKE_float.h"
#include    "PARK_float.h"
#include    "IPARK_float.h"
#include    "SLEW_float.h"
#include    "ABF_float.h"
#include    "VAR_stat.h"


/**************************************************************
* Funckija, ki pripravi vse potrebno za izvajanje
* prekinitvene rutine
**************************************************************/
extern void PER_int_setup(void);

#endif // end of __PER_INT_H__ definition
