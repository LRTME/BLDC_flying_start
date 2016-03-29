/****************************************************************
* FILENAME:     FLT_int.h
* DESCRIPTION:  fault handler header file
* AUTHOR:       Mitja Nemec
*
****************************************************************/
#ifndef     __FLT_INT_H__
#define     __FLT_INT_H__

#include    "DSP28x_Project.h"

#include    "define.h"
#include    "globals.h"

/**************************************************************
* funkcija, ki inicializira prekinitve in pripadajoce rutine
**************************************************************/
extern void FLT_int_setup(void);

extern void FLT_int_enable(void);

extern void FLT_int_disable(void);

#endif  // end of __FLT_INT_H__ definition

