/****************************************************************
* FILENAME:     SVM_drv.h
* DESCRIPTION:  Space Vector Modulation
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.1
****************************************************************/
#ifndef     __SVM_DRV_H__
#define     __SVM_DRV_H__

#include    "DSP28x_Project.h"

#include    "define.h"

/* definicije */
// stevilke PWM modulov, ki krmilijo mostic
// ne pozabi spremeniti GPIO mux registrov
#define     SVM_MODUL1      EPwm2Regs
#define     SVM_MODUL2      EPwm3Regs
#define     SVM_MODUL3      EPwm4Regs

// perioda casovnika (v procesorskih cikilh) 
#define     SVM_PERIOD      ((CPU_FREQ/SWITCH_FREQ) - 1)

// dead time (v nano sekundah)
#define     SVM_DEAD_TIME   500.0


// nacin delovanja casovnika ob emulation stop
// (0-stop at zero, 1-stop immediately, 2-run free)
#define     SVM_DEBUG_MODE  0

// definicije za status mostica
enum SVM_STATE { DISABLE=0, ENABLE, BOOTSTRAP, TRIP };

/**************************************************************
* nastavi mrtvi cas v nanosekundah
**************************************************************/
extern void SVM_dead_time(float dead_time);

/**************************************************************
* Inicializacija PWM modula za SVM
* returns:  
**************************************************************/
extern void SVM_init(void);

/**************************************************************
* Pozene casovnike za SVM modul
* returns:  
**************************************************************/
extern void SVM_start(void);

/**************************************************************
* Na podlagi Ualpha in Ubeta nastavi PWM module
* returns:  
* arg1:     napetost Ualpha, globalni IQ format [-1,+1]
* arg2:     napetost Ubeta, globalni IQ format [-1,+1]
**************************************************************/
extern void SVM_update(float Ualpha, float Ubeta);

/**************************************************************
* Na podlagi sektor in vklopnega razmerja nastavi PWM module
* returns:  
* arg1:     sektor [1-6]
* arg2:     vklopno razmerje
**************************************************************/
extern void SVM_update_six(float duty, int sektor);

/**************************************************************
* Na podlagi sektor in vklopnega razmerja nastavi PWM module
* returns:  
* arg1:     vklopno razmerje
* arg2:     sektor [1-6]
**************************************************************/
extern void SVM_update_bldc(float duty, int sektor);

/**************************************************************
* Funkcija, ki na podlagi vklopnega razmerja
* vklopi doloèene tranzistorje
* return: void
* arg1: vklopno razmerje od -1.0 do +1.0 (format IQ)
**************************************************************/
extern void SVM_update_DC(float duty);

/**************************************************************
* Na vrne vklopno razmerje veje
* returns:
**************************************************************/
extern float SVM_get_duty_leg1(void);
extern float SVM_get_duty_leg2(void);
extern float SVM_get_duty_leg3(void);

/**************************************************************
* Izklopi vse izhode
* returns:  
**************************************************************/
extern void SVM_disable(void);

/**************************************************************
* vklopi vse izhode
* returns:  
**************************************************************/
extern void SVM_enable(void);

/**************************************************************
* Izklopi vse tranzistorje - urgentno
* returns:
**************************************************************/
extern void SVM_trip(void);

/**************************************************************
* vrne status (delnam/ne delam)
* returns:  
**************************************************************/
extern int  SVM_status(void);

/**************************************************************
* Vklopi spodnje tranzistorje
* returns:  
**************************************************************/
extern void SVM_bootstrap(void);

/**************************************************************
* Vklopi zgornje tranzistorje
* returns:
**************************************************************/
extern void SVM_high(void);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene periode
* in je natancna na cikel natancno
* return: void
* arg1: zelena perioda
**************************************************************/
extern void SVM_period(float perioda);

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene frekvence
* in je natancna na cikel natancno
* return: void
* arg1: zelena frekvenca
**************************************************************/
extern void SVM_frequency(float frekvenca);

#endif  // SVM_DRV_H
