/**************************************************************
* FILE:         SPI_dajalnik.c 
* DESCRIPTION:  SPI_dajalnik Initialization & Support Functions
* VERSION:      1.0
* AUTHOR:       Mitja Nemec
**************************************************************/
#include 	"DSP28x_Project.h"
#include	"SPI_dajalnik.h"
#include    "define.h"


int SPI_msb = SPI_MSB;
int SPI_lsb = SPI_LSB;
int SPI_bits = SPI_BITS;
int SPI_phase = SPI_PHASE;

int SPI_level = 0;

long SPI_koda = 0;

/**************************************************************
* Funckija ki inicializira komunikacijo
**************************************************************/
void SPI_init(long clock, int bits, int msb, int lsb, int clk_phase)
{
    long SpiaRegs_clock = 1;
    long spi_brr = 1;

    int  spi_psc = 0;

    SPI_msb = msb;
    SPI_lsb = lsb;
    SPI_bits = bits;
    SPI_phase = clk_phase;

    // za pravilno nastavitev Baud registra, da dosežemo pravo uro
    if (SysCtrlRegs.LOSPCP.bit.LSPCLK == 0)
    {
        spi_psc = 1;
    }
    else
    {
        spi_psc = SysCtrlRegs.LOSPCP.bit.LSPCLK << 1;
    }

    // izraèunam vrednost za baud rate register
    SpiaRegs_clock = ((long)CPU_FREQ) / spi_psc;

    spi_brr = (SpiaRegs_clock / clock) - 1;

    if (spi_brr < 0)
    {
        spi_brr = 0;
    }

    // lahko imam problem, èe je spi_brr veèji od 127
    if (spi_brr > 127)
    {
        spi_brr = 127;
    }

    // najprej nastavmo GPIO pine
    EALLOW;
    SPI_PIN_DAT = SPI_MUX_VALUE;
    SPI_PIN_CLK = SPI_MUX_VALUE;

    EDIS;

    // zdaj pa inicializiramo SPI vmesnik za FIFO delovanje
    SPI_MODUL.SPICCR.bit.SPISWRESET = 0;     // reset SPI

    // clock polarity
    SPI_MODUL.SPICCR.bit.CLKPOLARITY = 1;    // clock polarity
    SPI_MODUL.SPICCR.bit.SPILBK = 0;         // loopback disabled
    SPI_MODUL.SPICCR.bit.SPICHAR = 15;       // bit lenght
    SPI_MODUL.SPICTL.bit.OVERRUNINTENA = 0;  // no overrun interrupta

    // clock phase
    SPI_MODUL.SPICTL.bit.CLK_PHASE = SPI_phase;      // clock phase
    SPI_MODUL.SPICTL.bit.MASTER_SLAVE = 1;   // SPI is master
    SPI_MODUL.SPICTL.bit.TALK = 1;           // enable transmit
    SPI_MODUL.SPICTL.bit.SPIINTENA = 0;      // no interrupts
    SPI_MODUL.SPISTS.bit.BUFFULL_FLAG = 1;   // clear flags
    SPI_MODUL.SPISTS.bit.INT_FLAG = 1;       // clear flags
    SPI_MODUL.SPISTS.bit.OVERRUN_FLAG = 1;   // clear flags

    SPI_MODUL.SPIBRR = spi_brr;          // SPICLK =   Mhz

    SPI_MODUL.SPIFFTX.bit.SPIRST = 0;        // SPI FIFO reset
    SPI_MODUL.SPIFFTX.bit.SPIFFENA = 1;      // enable FIFO
    SPI_MODUL.SPIFFTX.bit.TXFIFO = 0;        // reset FIFO cnt
    SPI_MODUL.SPIFFTX.bit.TXFIFO = 1;        // enable FIFO cnt
    SPI_MODUL.SPIFFTX.bit.TXFFIENA = 0;      // no FIFO interrupts
    SPI_MODUL.SPIFFCT.bit.TXDLY = 0;         // 0 delay bits
    SPI_MODUL.SPIFFTX.bit.TXFFINTCLR = 1;    // clear flags
    SPI_MODUL.SPIFFRX.bit.RXFIFORESET = 0;   // put out of reset
    SPI_MODUL.SPIFFRX.bit.RXFFIENA = 0;      // no RX interrupts
    SPI_MODUL.SPIFFRX.bit.RXFFINTCLR = 1;    // clear flags
    SPI_MODUL.SPIFFRX.bit.RXFFOVFCLR = 1;    // clear flags
    SPI_MODUL.SPIFFRX.bit.RXFIFORESET = 1;   // put out of reset
    SPI_MODUL.SPIFFTX.bit.SPIRST = 1;        // SPI FIFO out of reset
    SPI_MODUL.SPICCR.bit.SPISWRESET = 1;     // put out ofreset SPI

    // zazenem komunikacijo
    SPI_getkot();
    DELAY_US(50);
    SPI_getkot();
    DELAY_US(50);
    SPI_getkot();
    DELAY_US(50);

}

/**************************************************************
* Funckija ki komunicira z sPI dajalnikom položaja
* returns:  koda, ki jo vrne dajalnik
**************************************************************/
#pragma CODE_SECTION(SPI_getkot, "ramfuncs");
long SPI_getkot(void)
{
    long temp1 = 0;
    long temp2 = 0;


    long temp;
    long koda;

    if (SPI_bits > 16)
    {
        // èe me èaka veè podatkov kot dva, potem preberem dummy
        SPI_level = SpiaRegs.SPIFFRX.bit.RXFFST;
        while (SpiaRegs.SPIFFRX.bit.RXFFST > 2)
        {
            temp1 = SpiaRegs.SPIRXBUF;
        }

        // preberem podatek
        temp1 = SpiaRegs.SPIRXBUF;
        temp2 = SpiaRegs.SPIRXBUF;

        // izlocim podatek
        SPI_koda = 0;
        SPI_koda = (temp1 << 16);
        SPI_koda = SPI_koda | temp2;

        temp = SPI_koda & ((((long)1) << (SPI_msb)) - 1);

        koda = temp >> SPI_lsb;

        // posljem nov paket, da znova dobim podatek
        SpiaRegs.SPITXBUF = 0x0000;
        SpiaRegs.SPITXBUF = 0x0000;
    }
    else
    {
        // preberem podatek
        SPI_koda = SpiaRegs.SPIRXBUF;

        // izlocim podatek
        koda = SPI_koda;

        koda = koda & ((((long)1) << (SPI_msb)) - 1);

        koda = koda >> SPI_lsb;

        // posljem nov paket, da znova dobim podatek
        SpiaRegs.SPITXBUF = 0x0000;
    }
    return(koda);
}
