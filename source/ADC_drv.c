/**************************************************************
* FILE:         ADC_drv.c
* DESCRIPTION:  A/D driver for piccolo devices
* AUTHOR:       Mitja Nemec
* DATE:         19.1.2012
*
****************************************************************/
#include "ADC_drv.h"

/**************************************************************
* inicializiramo ADC
**************************************************************/
void ADC_init(void)
{
    // *IMPORTANT*
    // The Device_cal function, which copies the ADC calibration values from TI reserved
    // OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
    // Boot ROM. If the boot ROM code is bypassed during the debug process, the
    // following function MUST be called for the ADC to function according
    // to specification. The clocks to the ADC MUST be enabled before calling this
    // function.
    // See the device data manual and/or the ADC Reference
    // Manual for more information.

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    (*Device_cal)();
    EDIS;

    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
    // after power up to give all analog circuits time to power up and settle

    // Please note that for the delay function below to operate correctly the
    // CPU_RATE define statement in the DSP2802x_Examples.h file must
    // contain the correct CPU clock period in nanoseconds.
    EALLOW;
    AdcRegs.ADCCTL1.bit.ADCBGPWD = 1;       // Power ADC BG
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Power reference
    AdcRegs.ADCCTL1.bit.ADCPWDN = 1;        // Power ADC - vklop analognega vezja znotraj procesorja
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC - omogocimo delovanje adc-ja
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Select interal BG - izberemo notranjo referenco
    AdcRegs.ADCCTL1.bit.TEMPCONV = 1;       // izberemo notranji temperaturni senzor na adcin5 vhodu
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;
    EDIS;

    DELAY_US(1000L);         // Delay before converting ADC channels

    // Configure ADC
    EALLOW;
    
    // ADC trigger setup
    ADC_MODUL1.ETSEL.bit.SOCASEL = ET_CTR_ZERO;   //sproži na nic
    ADC_MODUL1.ETPS.bit.SOCAPRD = ET_1ST;        //ob vsakem prvem dogodku
    ADC_MODUL1.ETCLR.bit.SOCA = 1;               //clear possible flag
    ADC_MODUL1.ETSEL.bit.SOCAEN = 1;             //enable ADC Start Of conversion

    ADC_MODUL1.ETSEL.bit.SOCBSEL = ET_CTR_PRD;   //sproži na periodo
    ADC_MODUL1.ETPS.bit.SOCBPRD = ET_1ST;        //ob vsakem prvem dogodku
    ADC_MODUL1.ETCLR.bit.SOCB = 1;               //clear possible flag
    ADC_MODUL1.ETSEL.bit.SOCBEN = 1;             //enable ADC Start Of conversion

    // SOC0 config
    AdcRegs.ADCSOC0CTL.bit.CHSEL = 8+4;         //set SOC0 channel select to ADCINB4
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 7;         //set SOC0 to start trigger on EPWM2A
    AdcRegs.ADCSOC0CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC0 S/H Window to 7 ADC Clock Cycles,

    // SOC1 config
    AdcRegs.ADCSOC1CTL.bit.CHSEL = 8+0;         //set SOC1 channel select to ADCINB0
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL = 7;         //set SOC1 to start trigger on EPWM2A
    AdcRegs.ADCSOC1CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC1 S/H Window to 7 ADC Clock Cycles,

    // SOC2 config
    AdcRegs.ADCSOC2CTL.bit.CHSEL = 8+6;         //set SOC2 channel select to ADCINB6
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL = 7;         //set SOC2 to start trigger on EPWM2A
    AdcRegs.ADCSOC2CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC2 S/H Window to 7 ADC Clock Cycles,

    // SOC3 config
    AdcRegs.ADCSOC3CTL.bit.CHSEL = 0;           //set SOC3 channel select to ADCINA0
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL = 7;         //set SOC3 to start trigger on EPWM2A
    AdcRegs.ADCSOC3CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC3 S/H Window to 7 ADC Clock Cycles,

    // SOC4 config
    AdcRegs.ADCSOC4CTL.bit.CHSEL = 8+2;         //set SOC4 channel select to ADCINB2
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL = 7;         //set SOC4 to start trigger on EPWM2A
    AdcRegs.ADCSOC4CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC4 S/H Window to 7 ADC Clock Cycles,

    // SOC5 config
    AdcRegs.ADCSOC5CTL.bit.CHSEL = 1;           //set SOC5 channel select to ADCINA1
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL = 7;         //set SOC5 to start trigger on EPWM2A
    AdcRegs.ADCSOC5CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC5 S/H Window to 7 ADC Clock Cycles,

    // SOC6 config
    AdcRegs.ADCSOC6CTL.bit.CHSEL = 6;           //set SOC6 channel select to ADCINA6
    AdcRegs.ADCSOC6CTL.bit.TRIGSEL = 7;         //set SOC6 to start trigger on EPWM2A
    AdcRegs.ADCSOC6CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC6 S/H Window to 7 ADC Clock Cycles,

    // SOC7 config
    AdcRegs.ADCSOC7CTL.bit.CHSEL = 2;           //set SOC7 channel select to ADCINA2
    AdcRegs.ADCSOC7CTL.bit.TRIGSEL = 7;         //set SOC7 to start trigger on EPWM2A
    AdcRegs.ADCSOC7CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC7 S/H Window to 7 ADC Clock Cycles,

    
    
    // SOC8 config
    AdcRegs.ADCSOC8CTL.bit.CHSEL = 8+4;         //set SOC8 channel select to ADCINA2
    AdcRegs.ADCSOC8CTL.bit.TRIGSEL = 8;         //set SOC8 to start trigger on EPWM2B
    AdcRegs.ADCSOC8CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC8 S/H Window to 7 ADC Clock Cycles,

    // SOC9 config
    AdcRegs.ADCSOC9CTL.bit.CHSEL = 8+0;         //set SOC9 channel select to ADCINA2
    AdcRegs.ADCSOC9CTL.bit.TRIGSEL = 8;         //set SOC9 to start trigger on EPWM2B
    AdcRegs.ADCSOC9CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC9 S/H Window to 7 ADC Clock Cycles,

    // SOC10 config
    AdcRegs.ADCSOC10CTL.bit.CHSEL = 8+6;        //set SOC10 channel select to ADCINA2
    AdcRegs.ADCSOC10CTL.bit.TRIGSEL = 8;        //set SOC10 to start trigger on EPWM2B
    AdcRegs.ADCSOC10CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC10 S/H Window to 7 ADC Clock Cycles,

    // SOC11 config
    AdcRegs.ADCSOC11CTL.bit.CHSEL = 0;          //set SOC11 channel select to ADCINA0
    AdcRegs.ADCSOC11CTL.bit.TRIGSEL = 8;        //set SOC11 to start trigger on EPWM2B
    AdcRegs.ADCSOC11CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC11 S/H Window to 7 ADC Clock Cycles,

    // SOC12 config
    AdcRegs.ADCSOC12CTL.bit.CHSEL = 8+2;        //set SOC12 channel select to ADCINB2
    AdcRegs.ADCSOC12CTL.bit.TRIGSEL = 8;        //set SOC12 to start trigger on EPWM2B
    AdcRegs.ADCSOC12CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC12 S/H Window to 7 ADC Clock Cycles,

    // SOC13 config
    AdcRegs.ADCSOC13CTL.bit.CHSEL = 1;          //set SOC13 channel select to ADCINA1
    AdcRegs.ADCSOC13CTL.bit.TRIGSEL = 8;        //set SOC13 to start trigger on EPWM2B
    AdcRegs.ADCSOC13CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC13 S/H Window to 7 ADC Clock Cycles,

    // SOC14 config
    AdcRegs.ADCSOC14CTL.bit.CHSEL = 6;          //set SOC14 channel select to ADCINA6
    AdcRegs.ADCSOC14CTL.bit.TRIGSEL = 8;        //set SOC14 to start trigger on EPWM2B
    AdcRegs.ADCSOC14CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC14 S/H Window to 7 ADC Clock Cycles,

    // SOC15 config
    AdcRegs.ADCSOC15CTL.bit.CHSEL = 2;          //set SOC15 channel select to ADCINA2
    AdcRegs.ADCSOC15CTL.bit.TRIGSEL = 8;        //set SOC15 to start trigger on EPWM2B
    AdcRegs.ADCSOC15CTL.bit.ACQPS = AQ_PRESCALAR;//set SOC15 S/H Window to 7 ADC Clock Cycles,

    //tu povemo naj se postavi interrupt flag, ko je zadnja pretvorba koncna
    //interrupt je se naprej onemogocen, flag ki se bo postavil pa nam bo
    //sluzil za detektiranje konca niza pretvorb
    AdcRegs.INTSEL1N2.bit.INT1SEL = 0x00;   //interrupt1 naj prozi signal EOC1, ker je to soc, ki se zadnji izvede
    AdcRegs.INTSEL1N2.bit.INT1E = 1;        //prekinitev ob interrpt dogodku je omogocena (da se lahko postavi flag)
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;   //pobrise se flag

    EDIS;

}   //end of AP_ADC_init

/**************************************************************
* Funkcija, ki pocaka da ADC konca s pretvorbo
* vzorcimo...
* return: void
**************************************************************/
#pragma CODE_SECTION(ADC_wait, "ramfuncs");
void ADC_wait(void)
{
    while (AdcRegs.ADCINTFLG.bit.ADCINT1 == 0)
    {
        /* DO NOTHING */
    }
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;               //pobrisem flag bit od ADC-ja
}
