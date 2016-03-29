/**************************************************************
* FILE:         PCB_util.c 
* DESCRIPTION:  PCB initialization & Support Functions
* VERSION:      1.0
* AUTHOR:       Mitja Nemec
**************************************************************/
#include "PCB_util.h"

//izberi katero varianto razvojne plosce bos uporabljal
#define PICCOLO		1	//0 - control stick, 1- control card

/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
void PCB_mode_LED_speed(void)
{
	GpioDataRegs.GPBSET.bit.GPIO33 = 1;
}

/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
void PCB_mode_LED_torque(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
}

/**************************************************************
* Funckija s katero signaliziramo nacin delovanja
**************************************************************/
void PCB_mode_LED_toggle(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1;
}

/**************************************************************
* Funckija ki prizge fault LED
**************************************************************/
void PCB_fault_LED_on(void)
{
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;
}

/**************************************************************
* Funckija ki ugasne fault LED
**************************************************************/
void PCB_fault_LED_off(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
}

/**************************************************************
* Funckija ki spremeni stanje fault LED
**************************************************************/
void PCB_fault_LED_toggle(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1;
}


/**************************************************************
* Funckija ki prizge on LED
**************************************************************/
void PCB_on_LED_on(void)
{
    GpioDataRegs.GPASET.bit.GPIO0 = 1;
}

/**************************************************************
* Funckija ki ugasne on LED
**************************************************************/
void PCB_on_LED_off(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
}

/**************************************************************
* Funckija ki spremeni stanje on LED
**************************************************************/
void PCB_on_LED_toggle(void)
{
    GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
}

/**************************************************************
* Funckija ki vrne stanje on tipke
**************************************************************/
bool PCB_on_SW(void)
{
    if (GpioDataRegs.GPADAT.bit.GPIO19 == 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

/**************************************************************
* Funckija ki vrne stanje mode tipke
**************************************************************/
bool PCB_mode_SW(void)
{
    if (GpioDataRegs.GPADAT.bit.GPIO1 == 1)
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
}

/**************************************************************
* Funckija ki inicializira PCB
**************************************************************/
void PCB_init(void)
{
    EALLOW;

    /* IZHODI */
    // GPIO33
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;

    // GPIO32
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;

    // GPIO0
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;

    /* vhodi */
    // GPIO19
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;

    // GPIO1
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 0;
    EDIS;

    // postavim v privzeto stanje
    PCB_on_LED_off();
    PCB_mode_LED_speed();
    PCB_fault_LED_off();

}
