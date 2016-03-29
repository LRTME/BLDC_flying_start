/**************************************************************
* FILE:         SPI_dajalnik.h 
* DESCRIPTION:  definitions for SPI_dajalnik Initialization & Support Functions
* VERSION:      1.0
* AUTHOR:       Mitja Nemec
**************************************************************/
#ifndef   __SPI_DAJALNIK_H__
#define   __SPI_DAJALNIK_H__

// kateri SPI modul uporabljamo
#define SPI_MODUL       SpiaRegs

// katere pine uporabljamo
#define SPI_PIN_DAT     GpioCtrlRegs.GPAMUX2.bit.GPIO17
#define SPI_PIN_CLK     GpioCtrlRegs.GPAMUX2.bit.GPIO18
#define SPI_MUX_VALUE   1

// koliko bitov moramo poslati, da dobimo celotno informacijo
#define SPI_BITS        16

// kje znotraj dwords se nahaja informacija
#define SPI_MSB         15
#define SPI_LSB         5
#define SPI_PHASE       1

/**************************************************************
* Funckija ki inicializira komunikacijo
**************************************************************/
extern void SPI_init(long clock, int bits, int msb, int lsb, int clk_phase);

/**************************************************************
* Funckija ki komunicira z sPI dajalnikom položaja
* returns:  koda, ki jo vrne dajalnik
**************************************************************/
extern long SPI_getkot(void);

#endif  // end of __SPI_DAJALNIK_H__ definition

