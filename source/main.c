/****************************************************************
* FILENAME:     main.c
* DESCRIPTION:  initialization code
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/
#include "main.h"

/**************************************************************
* Funkcija, ki se izvede inicializacijo
**************************************************************/
void main(void)
{
    // PLL, in ura
    InitSysCtrl();
    
    // ce sta Load in Run naslova potem se izvaja program iz FLASH-a
    if (&RamfuncsLoadStart != &RamfuncsRunStart)
    {
        // skopiram ta hitro kodo iz FLASH-a v RAM
        // Copy Ramfuncs section to run addres
        MemCopy(&IQfuncsLoadStart, &IQfuncsLoadEnd, &IQfuncsRunStart);
        MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

        // Inicializiramo Flash - cakalna stanja
        InitFlash();
    }    

    // GPIO - najprej
    InitGpio();

    // generic init of PIE
    InitPieCtrl();

    // basic vector table
    InitPieVectTable();

    // inicializiram ADC
    ADC_init();

    // inicializiram SVM
    SVM_init();

    // inicializiram digitalne vhode in izhode
    PCB_init();

    // inicializiram SCI vmesnik
    SCI_init(57600L);

    // inicializiram SPI dajalnik kota
    SPI_init(1000000L, 16, 15, 3, 1);

    // inicializiram peridoièno prekinitev za regulacijo motorja
    PER_int_setup();

    // pocakam, da se vse stabilizira
    DELAY_US(100000);

    // omogocim prekinitve
    EINT;
    ERTM;

    // pozenem casovnik, ki bo prozil ADC in prekinitev
    SVM_start();

    // kalibriram tokovne sonde
    while (current_offset_calibrated == FALSE)
    {
        /* DO NOTHING */
    }

    // napolnim bootstrap kondenzatorje
    SVM_bootstrap();
    DELAY_US(20000);

    // onemogocim mostic in preidem v zagon
    SVM_disable();
    PCB_fault_LED_off();
    PCB_on_LED_off();
    state = Startup;
    //state = Work;

    // grem v neskoncno zanko, ki se izvaja v ozadju
    BACK_loop();
}   // end of main
