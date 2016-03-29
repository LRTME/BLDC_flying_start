/**************************************************************
* FILE:         PCB_util.h 
* DESCRIPTION:  definitions for PCB Initialization & Support Functions
* VERSION:      1.0
* AUTHOR:       Mitja Nemec
**************************************************************/
#ifndef   PCB_UTIL_H
#define   PCB_UTIL_H

#include    "DSP28x_Project.h"
#include    "define.h"



/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
extern void PCB_mode_LED_speed(void);

/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
extern void PCB_mode_LED_torque(void);

/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
extern void PCB_mode_LED_toggle(void);

/**************************************************************
* Funckija ki prizge fault LED
**************************************************************/
extern void PCB_fault_LED_on(void);

/**************************************************************
* Funckija ki ugasne fault LED
**************************************************************/
extern void PCB_fault_LED_off(void);

/**************************************************************
* Funckija ki spremeni stanje fault LED
**************************************************************/
extern void PCB_fault_LED_toggle(void);

/**************************************************************
* Funckija ki prizge on LED
**************************************************************/
extern void PCB_on_LED_on(void);

/**************************************************************
* Funckija ki ugasne on LED
**************************************************************/
extern void PCB_on_LED_off(void);

/**************************************************************
* Funckija ki spremeni stanje on LED
**************************************************************/
extern void PCB_on_LED_toggle(void);

/**************************************************************
* Funckija ki vrne stanje on tipke
**************************************************************/
extern bool PCB_on_SW(void);

/**************************************************************
* Funckija ki vrne stanje mode tipke
**************************************************************/
extern bool PCB_mode_SW(void);

/**************************************************************
* Funckija ki inicializira tiskanino
**************************************************************/
extern void PCB_init(void);


#endif  // end of PCB_UTIL_H definition

