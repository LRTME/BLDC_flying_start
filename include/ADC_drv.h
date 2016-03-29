/**************************************************************
* FILE:         ADC_drv.c
* DESCRIPTION:  A/D driver for piccolo devices
* AUTHOR:       Mitja Nemec
* DATE:         19.1.2012
*
****************************************************************/
#ifndef     __ADC_DRV_H__
#define     __ADC_DRV_H__

#include    "DSP28x_Project.h"

// doloci kateri PWM modul prozi ADC
#define     ADC_MODUL1      EPwm2Regs

// cas vzorcenja
#define     AQ_PRESCALAR    0x15

// kje se nahajajo rezultati
#define     TOK_1_Z         (AdcResult.ADCRESULT0)
#define     TOK_2_Z         (AdcResult.ADCRESULT1)
#define     TOK_3_Z         (AdcResult.ADCRESULT2)
#define     U_DC_Z          (AdcResult.ADCRESULT3)
#define     KOMANDA_Z       (AdcResult.ADCRESULT4)
#define     NAP_1_Z         (AdcResult.ADCRESULT5)
#define     NAP_2_Z         (AdcResult.ADCRESULT6)
#define     NAP_3_Z         (AdcResult.ADCRESULT7)
#define     TOK_1_P         (AdcResult.ADCRESULT8)
#define     TOK_2_P         (AdcResult.ADCRESULT9)
#define     TOK_3_P         (AdcResult.ADCRESULT10)
#define     U_DC_P          (AdcResult.ADCRESULT11)
#define     KOMANDA_P       (AdcResult.ADCRESULT12)
#define     NAP_1_P         (AdcResult.ADCRESULT13)
#define     NAP_2_P         (AdcResult.ADCRESULT14)
#define     NAP_3_P         (AdcResult.ADCRESULT15)




/**************************************************************
* inicializiramo ADC
**************************************************************/
extern void ADC_init(void);

/**************************************************************
* Funkcija, ki pocaka da ADC konca s pretvorbo
* vzorcimo...
* return: void
**************************************************************/
extern void ADC_wait(void);

#endif /* __ADC_DRV_H__ */
