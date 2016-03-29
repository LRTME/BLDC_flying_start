/****************************************************************
* FILENAME:     SVM_drv.c             
* DESCRIPTION:  SVM HW driver
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.1
****************************************************************/
#include "SVM_drv.h"

enum SVM_STATE svm_status = DISABLE;

/**************************************************************
* nastavi mrtvi cas v nanosekundah
**************************************************************/
void SVM_dead_time(float dead_time)
{
    float cpu_cycle_time = (1.0/CPU_FREQ);
    
    long cycle_number;
    
    // naracunam koliko ciklov to znese
    cycle_number = (dead_time / 1000000000) / cpu_cycle_time;

    SVM_MODUL1.DBFED = cycle_number;
    SVM_MODUL1.DBRED = cycle_number;
    SVM_MODUL2.DBFED = cycle_number;
    SVM_MODUL2.DBRED = cycle_number;
    SVM_MODUL3.DBFED = cycle_number;
    SVM_MODUL3.DBRED = cycle_number;
}

/**************************************************************
* Funkcija, ki na podlagi vklopnega razmerja
* vklopi doloèene tranzistorje
* return: void
* arg1: vklopno razmerje od -1.0 do +1.0 (format IQ)
**************************************************************/
#pragma CODE_SECTION(SVM_update_DC, "ramfuncs");
void SVM_update_DC(float duty)
{
    unsigned int compare1;
    unsigned int compare2;
    long delta;

    unsigned int perioda;



    // delam samo v primeru ce je mostic omogocen
    if (svm_status == ENABLE)
    {
        // zašèita za duty cycle
        //(zašèita za sektor je narejena v default switch case stavku)
        if (duty < -1.0) duty = -1.0;
        if (duty > +1.0) duty = +1.0;

        perioda = SVM_MODUL1.TBPRD;
        // koda da naracunam vrednost, ki bo sla v CMPR register
        delta = (perioda * duty)/2;

        compare1 = perioda/2 - delta;
        compare2 = perioda/2 + delta;

        // tretjo vejo za ziher izklopim
        SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
        SVM_MODUL3.AQCSFRC.bit.CSFB = 2;

        // vpisem v register
        SVM_MODUL1.CMPA.half.CMPA = compare1;
        SVM_MODUL2.CMPA.half.CMPA = compare2;
    }
}  //end of FB_update

/**************************************************************
* Na vrne vklopno razmerje veje
* returns:
**************************************************************/
#pragma CODE_SECTION(SVM_get_duty_leg1, "ramfuncs");
float SVM_get_duty_leg1(void)
{
    return ((float)SVM_MODUL1.CMPA.half.CMPA / (float)SVM_MODUL1.TBPRD);
}
#pragma CODE_SECTION(SVM_get_duty_leg2, "ramfuncs");
float SVM_get_duty_leg2(void)
{
    return ((float)SVM_MODUL2.CMPA.half.CMPA / (float)SVM_MODUL1.TBPRD);
}
#pragma CODE_SECTION(SVM_get_duty_leg3, "ramfuncs");
float SVM_get_duty_leg3(void)
{
    return ((float)SVM_MODUL3.CMPA.half.CMPA / (float)SVM_MODUL1.TBPRD);
}


/**************************************************************
* Na podlagi sektor in vklopnega razmerja nastavi PWM module
* returns:  
* arg2:     vklopno razmerje
* arg2:     sektor [1-6]
**************************************************************/
#pragma CODE_SECTION(SVM_update_bldc, "ramfuncs");
void SVM_update_bldc(float svm_duty, int svm_sektor)
{       
    unsigned int compare;
    long delta;
    unsigned int perioda;

    // delam samo ce je mostic omogocen
    if (svm_status == ENABLE)
    {
        svm_duty = - svm_duty;
        // preverim predznak
        if (svm_duty < 0)
        {
            svm_sektor = svm_sektor + 3;
            if (svm_sektor > 6)
            {
                svm_sektor = svm_sektor - 6;
            }
        }
    
        // zasšita
        svm_duty = fabs(svm_duty);
        if (svm_duty > 1.0) svm_duty = 1.0;

        perioda = SVM_MODUL1.TBPRD;
        // koda da naracunam vrednost, ki bo sla v CMPR register
        delta = perioda * svm_duty;

        compare = perioda - delta;

        SVM_MODUL1.CMPA.half.CMPA = compare;
        SVM_MODUL2.CMPA.half.CMPA = compare;
        SVM_MODUL3.CMPA.half.CMPA = compare;

        //
        switch(svm_sektor)
        {
            case 1:
                    // Z
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 2;

                    // L
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;

                    // H
                    SVM_MODUL3.CMPA.half.CMPA = compare;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 0;
                    break;
            case 2:
                    // H
                    SVM_MODUL1.CMPA.half.CMPA = compare;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 0;

                    // L
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;

                    // Z
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 2;
                    break;
            case 3:
                    // H
                    SVM_MODUL1.CMPA.half.CMPA = compare;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 0;

                    // Z
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 2;

                    // L
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
            case 4:
                    // Z
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 2;

                    // H
                    SVM_MODUL2.CMPA.half.CMPA = compare;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 0;

                    // L
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
            case 5:
                    // L
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;

                    // H
                    SVM_MODUL2.CMPA.half.CMPA = compare;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 0;

                    // Z
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 2;
                    break;
            case 6:
                    // L
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;

                    // Z
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 2;

                    // H
                    SVM_MODUL3.CMPA.half.CMPA = compare;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 0;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 0;
                    break;
            default:
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 2;

                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 2;

                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 2;
                    break;
        }
    }
}


/**************************************************************
* Na podlagi sektor in vklopnega razmerja nastavi PWM module
* returns:  
* arg1:     sektor [1-6]
* arg2:     vklopno razmerje
**************************************************************/
#pragma CODE_SECTION(SVM_update_six, "ramfuncs");
void SVM_update_six(float duty, int sektor)
{       
    unsigned int compare;
    long delta;
    unsigned int perioda;
    int aq_bit = 0;

    // delam samo ce je mostic omogocen
    if (svm_status == ENABLE)
    {
        // zasšita
        if (duty < 0.0) duty = 0.0;
        if (duty > 1.0) duty = 1.0;

        perioda = SVM_MODUL1.TBPRD;
        // koda da naracunam vrednost, ki bo sla v CMPR register
        delta = perioda * duty;

        compare = perioda - delta;

        // ko je compare tak da se eden od tranzistorjev ne vklopi, daj ven 100 duty cycle
        // if compare velu is close to zero, turn on during whole interval
        if (compare < (unsigned int)(SVM_MODUL1.DBFED/2))
        {
            compare = 0;
            aq_bit = 2;
        } 
        // if compare value is close to PERIOD, switch off during whole interval
        if (compare > (perioda - SVM_MODUL1.DBFED/2))
        {
            compare = perioda + 1;
            aq_bit = 1;
        }

        SVM_MODUL1.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL1.AQCSFRC.bit.CSFB = 0;
        SVM_MODUL2.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL2.AQCSFRC.bit.CSFB = 0;
        SVM_MODUL3.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL3.AQCSFRC.bit.CSFB = 0;

        //
        switch(sektor)
        {
            case 1:
                    SVM_MODUL1.CMPA.half.CMPA = compare;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL2.CMPA.half.CMPA = perioda;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL3.CMPA.half.CMPA = perioda;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
            case 2:
                    SVM_MODUL1.CMPA.half.CMPA = compare;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL2.CMPA.half.CMPA = compare;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL3.CMPA.half.CMPA = perioda;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
            case 3:
                    SVM_MODUL1.CMPA.half.CMPA = perioda;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL2.CMPA.half.CMPA = compare;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL3.CMPA.half.CMPA = perioda;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
            case 4:
                    SVM_MODUL1.CMPA.half.CMPA = perioda;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL2.CMPA.half.CMPA = compare;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL3.CMPA.half.CMPA = compare;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = aq_bit;
                    break;
            case 5:
                    SVM_MODUL1.CMPA.half.CMPA = perioda;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL2.CMPA.half.CMPA = perioda;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL3.CMPA.half.CMPA = compare;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = aq_bit;
                    break;
            case 6:
                    SVM_MODUL1.CMPA.half.CMPA = compare;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = aq_bit;
                    SVM_MODUL2.CMPA.half.CMPA = perioda;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL3.CMPA.half.CMPA = compare;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = aq_bit;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = aq_bit;
                    break;
            default:
                    SVM_MODUL1.CMPA.half.CMPA = perioda;
                    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL2.CMPA.half.CMPA = perioda;
                    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;
                    SVM_MODUL3.CMPA.half.CMPA = perioda;
                    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
                    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
                    break;
        }
    }
}

/**************************************************************
* Inicializacija PWM modula za SVM
* returns:  
**************************************************************/
void SVM_init(void)
{
// period, compare & phase values
    // Init Timer-Base Period Register for EPWM1-EPWM3
    SVM_MODUL1.TBPRD = SVM_PERIOD/2;
    SVM_MODUL2.TBPRD = SVM_PERIOD/2;
    SVM_MODUL3.TBPRD = SVM_PERIOD/2;

    // setup timer base
    SVM_MODUL1.TBCTL.bit.PHSDIR = 1;     // count up after sync
    SVM_MODUL1.TBCTL.bit.CLKDIV = 0;
    SVM_MODUL1.TBCTL.bit.HSPCLKDIV = 0;
    SVM_MODUL1.TBCTL.bit.SYNCOSEL = 1;   // sync out on zero
    SVM_MODUL1.TBCTL.bit.PRDLD = 0;      // shadowed period reload
    SVM_MODUL1.TBCTL.bit.PHSEN = 0;      // master timer does not sync

    SVM_MODUL2.TBCTL.bit.PHSDIR = 1;     // count up after sync
    SVM_MODUL2.TBCTL.bit.CLKDIV = 0;
    SVM_MODUL2.TBCTL.bit.HSPCLKDIV = 0;
    SVM_MODUL2.TBCTL.bit.SYNCOSEL = 0;   // sync out is sync in
    SVM_MODUL2.TBCTL.bit.PRDLD = 0;      // shadowed period reload
    SVM_MODUL2.TBCTL.bit.PHSEN = 0;      // slave timer does sync

    SVM_MODUL3.TBCTL.bit.PHSDIR = 1;     // count up after sync
    SVM_MODUL3.TBCTL.bit.CLKDIV = 0;
    SVM_MODUL3.TBCTL.bit.HSPCLKDIV = 0;
    SVM_MODUL3.TBCTL.bit.SYNCOSEL = 3;   // sync out is disabled
    SVM_MODUL3.TBCTL.bit.PRDLD = 0;      // shadowed period reload
    SVM_MODUL3.TBCTL.bit.PHSEN = 0;      // slave timer does sync

    // debug mode behafiour
    #if SVM_DEBUG_MODE == 0
    SVM_MODUL1.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    SVM_MODUL2.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    SVM_MODUL3.TBCTL.bit.FREE_SOFT = 1;  // stop after current cycle
    #endif
    #if SVM_DEBUG_MODE == 1
    SVM_MODUL1.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    SVM_MODUL2.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    SVM_MODUL3.TBCTL.bit.FREE_SOFT = 0;  // stop after current cycle
    #endif
    #if SVM_DEBUG_MODE == 2
    SVM_MODUL1.TBCTL.bit.FREE_SOFT = 3;  // run free
    SVM_MODUL2.TBCTL.bit.FREE_SOFT = 3;  // run free
    SVM_MODUL3.TBCTL.bit.FREE_SOFT = 3;  // stop after current cycle
    #endif

    // Init Timer-Base Phase Register for EPWM1-EPWM3
    SVM_MODUL1.TBPHS.half.TBPHS = 0;
    SVM_MODUL2.TBPHS.half.TBPHS = 0;
    SVM_MODUL3.TBPHS.half.TBPHS = 0;

    // compare setup
    SVM_MODUL1.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    SVM_MODUL1.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    SVM_MODUL2.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    SVM_MODUL2.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    SVM_MODUL3.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // re-load on zero
    SVM_MODUL3.CMPCTL.bit.SHDWAMODE = CC_SHADOW; // shadowed compare reload

    SVM_MODUL1.CMPA.half.CMPA = SVM_PERIOD/4; //50% duty cycle
    SVM_MODUL2.CMPA.half.CMPA = SVM_PERIOD/4; //50% duty cycle
    SVM_MODUL3.CMPA.half.CMPA = SVM_PERIOD/4; //50% duty cycle

    // action qualifier setup
    SVM_MODUL1.AQSFRC.bit.RLDCSF = 0;
    SVM_MODUL2.AQSFRC.bit.RLDCSF = 0;
    SVM_MODUL3.AQSFRC.bit.RLDCSF = 0;

    SVM_MODUL1.AQCTLA.bit.CAU = AQ_SET;     // set output on CMPA_UP
    SVM_MODUL1.AQCTLA.bit.CAD = AQ_CLEAR;   // clear output on CMPA_DOWN
    SVM_MODUL1.AQCTLB.bit.CAU = AQ_SET;     // set output on CMPA_UP
    SVM_MODUL1.AQCTLB.bit.CAD = AQ_CLEAR;   // clear output on CMPA_DOWN

    SVM_MODUL2.AQCTLA.bit.CAU = AQ_SET;     // clear output on CMPA_UP
    SVM_MODUL2.AQCTLA.bit.CAD = AQ_CLEAR;   // set output on CMPA_DOWN
    SVM_MODUL2.AQCTLB.bit.CAU = AQ_SET;     // set output on CMPA_UP
    SVM_MODUL2.AQCTLB.bit.CAD = AQ_CLEAR;   // clear output on CMPA_DOWN

    SVM_MODUL3.AQCTLA.bit.CAU = AQ_SET;     // clear output on CMPA_UP
    SVM_MODUL3.AQCTLA.bit.CAD = AQ_CLEAR;   // set output on CMPA_DOWN
    SVM_MODUL3.AQCTLB.bit.CAU = AQ_SET;     // set output on CMPA_UP
    SVM_MODUL3.AQCTLB.bit.CAD = AQ_CLEAR;   // clear output on CMPA_DOWN

    // Init Dead-Band Generator Control Register for EPWM1-EPWM3
    SVM_MODUL1.DBCTL.bit.IN_MODE = 2;    //
    SVM_MODUL1.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    SVM_MODUL1.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    SVM_MODUL2.DBCTL.bit.IN_MODE = 2;    //
    SVM_MODUL2.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    SVM_MODUL2.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    SVM_MODUL3.DBCTL.bit.IN_MODE = 2;    //
    SVM_MODUL3.DBCTL.bit.POLSEL = 2;     // active high complementary mode
    SVM_MODUL3.DBCTL.bit.OUT_MODE = 3;   // dead band on both outputs

    // Init Dead-Band Generator Falling/Rising Edge Delay Register for EPWM1-EPWM3
    SVM_dead_time(SVM_DEAD_TIME);

    // Do NOT use chopper module
    SVM_MODUL1.PCCTL.bit.CHPEN = 0;
    SVM_MODUL2.PCCTL.bit.CHPEN = 0;
    SVM_MODUL3.PCCTL.bit.CHPEN = 0;

    EALLOW;
    // trip zone functionality
/*  SVM_MODUL1.TZSEL.bit.OSHT1 = 0;      // TZ1 triggers tripzone
    SVM_MODUL1.TZCTL.bit.TZA = 2;        // force low
    SVM_MODUL1.TZCTL.bit.TZB = 2;        // force low
    SVM_MODUL1.TZCLR.bit.OST = 1;        // clear any pending flags

    SVM_MODUL2.TZSEL.bit.OSHT1 = 0;      // TZ1 triggers tripzone
    SVM_MODUL2.TZCTL.bit.TZA = 2;        // force low
    SVM_MODUL2.TZCTL.bit.TZB = 2;        // force low
    SVM_MODUL2.TZCLR.bit.OST = 1;        // clear any pending flags

    SVM_MODUL3.TZSEL.bit.OSHT1 = 0;      // TZ1 triggers tripzone
    SVM_MODUL3.TZCTL.bit.TZA = 2;        // force low
    SVM_MODUL3.TZCTL.bit.TZB = 2;        // force low
    SVM_MODUL3.TZCLR.bit.OST = 1;        // clear any pending flags
*/
// event trigger module
    EDIS;
    // output pin setup
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // EPWM2A pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // EPWM2B pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // EPWM3A pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // EPWM3B pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   // EPWM4A pin
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   // EPWM5B pin


    // input pin setup
    //GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0; // enable pull up on TZ1
    //GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;
    EDIS;                         // Disable EALLOW

    // privzeto je modul onemogoèen
    SVM_disable();
}

/**************************************************************
* Pozene casovnike za SVM modul
* returns:  
**************************************************************/
void SVM_start(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    SVM_MODUL1.TBCTL.bit.CTRMODE = 2;
    SVM_MODUL2.TBCTL.bit.CTRMODE = 2;
    SVM_MODUL3.TBCTL.bit.CTRMODE = 2;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

/**************************************************************
* Izklopi vse tranzistorje - urgentno
* returns:
**************************************************************/
void SVM_trip(void)
{
    SVM_MODUL1.TZFRC.bit.OST = 1;
    SVM_MODUL2.TZFRC.bit.OST = 1;
    SVM_MODUL3.TZFRC.bit.OST = 1;

    svm_status = TRIP;
}

/**************************************************************
* Izklopi vse izhode
* returns:  
**************************************************************/
#pragma CODE_SECTION(SVM_disable, "ramfuncs");
void SVM_disable(void)
{
    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL1.AQCSFRC.bit.CSFB = 2;

    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL2.AQCSFRC.bit.CSFB = 2;

    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL3.AQCSFRC.bit.CSFB = 2;

    svm_status = DISABLE;
}

/**************************************************************
* vklopi vse izhode
* returns:  
**************************************************************/
#pragma CODE_SECTION(SVM_enable, "ramfuncs");
void SVM_enable(void)
{
    SVM_MODUL1.AQCSFRC.bit.CSFA = 0;
    SVM_MODUL1.AQCSFRC.bit.CSFB = 0;

    SVM_MODUL2.AQCSFRC.bit.CSFA = 0;
    SVM_MODUL2.AQCSFRC.bit.CSFB = 0;

    SVM_MODUL3.AQCSFRC.bit.CSFA = 0;
    SVM_MODUL3.AQCSFRC.bit.CSFB = 0;

    EALLOW;
    SVM_MODUL1.TZCLR.bit.OST = 1;
    SVM_MODUL2.TZCLR.bit.OST = 1;
    SVM_MODUL3.TZCLR.bit.OST = 1;
    EDIS;

    svm_status = ENABLE;
}

/**************************************************************
* vrne status (delnam/ne delam)
* returns:  
**************************************************************/
int SVM_status(void)
{
    return(svm_status);
}

/**************************************************************
* Vklopi spodnje tranzistorje
* returns:  
**************************************************************/
void SVM_bootstrap(void)
{
    SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL1.AQCSFRC.bit.CSFB = 1;

    SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL2.AQCSFRC.bit.CSFB = 1;

    SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
    SVM_MODUL3.AQCSFRC.bit.CSFB = 1;

    svm_status = BOOTSTRAP;
}

/**************************************************************
* Vklopi zgornje tranzistorje
* returns:
**************************************************************/
#pragma CODE_SECTION(SVM_high, "ramfuncs");
void SVM_high(void)
{
    SVM_MODUL1.AQCSFRC.bit.CSFA = 2;
    SVM_MODUL1.AQCSFRC.bit.CSFB = 2;

    SVM_MODUL2.AQCSFRC.bit.CSFA = 2;
    SVM_MODUL2.AQCSFRC.bit.CSFB = 2;

    SVM_MODUL3.AQCSFRC.bit.CSFA = 2;
    SVM_MODUL3.AQCSFRC.bit.CSFB = 2;

    svm_status = BOOTSTRAP;
}

/**************************************************************
* Na podlagi Ualpha in Ubeta nastavi PWM module
* returns:  
* arg1:     napetost Ualpha [-1,+1]
* arg2:     napetost Ubeta [-1,+1]
**************************************************************/
#pragma CODE_SECTION(SVM_update, "ramfuncs");
void SVM_update(float Ualpha, float Ubeta) 
{
    #define SVM_SQRT3   0.86602540378443864676372317075294

    /* lokalne spremenljivke */
    float Va;
    float Vb;
    float Vc;
    float t1;
    float t2;
    float Ta;
    float Tb;
    float Tc;
    int Sector = 0;


    unsigned int compare;
    unsigned int perioda;
    long delta;

    // izvedem samo ce je mostic omogocen
    if (svm_status == ENABLE)
    {
        // tlake na zacetku bi blo mogoce dobr prevert limite na vhodih
        if (Ualpha > (+1.0))
        {
            Ualpha = (+1.0);
        }
        if (Ualpha < (-1.0))
        {
            Ualpha = (-1.0);
        }
        if (Ubeta > (+1.0))
        {
            Ubeta = (+1.0);
        }
        if (Ubeta < (-1.0))
        {
            Ubeta = (-1.0);
        }
        
        // Inverse clarke transformation
        Va = -Ubeta;
        Vb = -0.5 * Ubeta - SVM_SQRT3 * Ualpha;  // 0.8660254 = sqrt(3)/2
        Vc = -0.5 * Ubeta + SVM_SQRT3 * Ualpha;  // 0.8660254 = sqrt(3)/2
        
        // 60 degree Sector determination
        if (Va > 0.0)
           Sector = 1; 
        if (Vc < 0.0)
           Sector = Sector + 2;
        if (Vb < 0.0)
           Sector = Sector + 4;
           
        // X,Y,Z (Va,Vb,Vc) calculations
        if (Sector == 0)  // Sector 0: this is special case for (Ualpha,Ubeta) = (0,0)
        {
           Ta = 0.5;
           Tb = 0.5;
           Tc = 0.5;
        }
        if (Sector == 1)
        {
           t1 = Vc;
           t2 = Vb;
           Tb = (1.0 - t1 - t2) / 2;
           Ta = Tb + t1;
           Tc = Ta + t2;
        }
        else if (Sector == 2)
        {
           t1 = Vb;
           t2 = -Va;
           Ta = (1.0 - t1 - t2) / 2;
           Tc = Ta + t1;
           Tb = Tc + t2;
        }      
        else if (Sector == 3)
        {
           t1 = -Vc;
           t2 = Va;
           Ta = (1.0 - t1 - t2) / 2;
           Tb = Ta + t1;
           Tc = Tb + t2;
        }   
        else if (Sector == 4)
        {
           t1 = -Va;
           t2 = Vc;
           Tc = (1.0 - t1 - t2) / 2;
           Tb = Tc + t1;
           Ta = Tb + t2;
        }   
        else if (Sector == 5)
        {
           t1 = Va;
           t2 = -Vb;
           Tb = (1.0 - t1 - t2) / 2;
           Tc = Tb + t1;
           Ta = Tc + t2;
        }   
        else if (Sector == 6)
        {
           t1 = -Vb;
           t2 = -Vc;
           Tc = (1.0 - t1 - t2) / 2;
           Ta = Tc + t1;
           Tb = Ta + t2;
        }

        // saturacija
        if (Ta > 1.0) Ta = 1.0;
        if (Tb > 1.0) Tb = 1.0;
        if (Tc > 1.0) Tc = 1.0;
        if (Ta < 0.0) Ta = 0.0;
        if (Tb < 0.0) Tb = 0.0;
        if (Tc < 0.0) Tc = 0.0;

        // koda da naracunam vrednost, ki bo sla v CMPR register
        /* first leg */
        perioda = SVM_MODUL1.TBPRD;

        delta = perioda * Ta;

        compare = perioda - delta;

        SVM_MODUL1.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL1.AQCSFRC.bit.CSFB = 0;

        // if compare value is close to zero, turn on during whole interval
        if (compare < (SVM_MODUL1.DBFED/2))
        {
            compare = 0;
            SVM_MODUL1.AQCSFRC.bit.CSFA = 2;
            SVM_MODUL1.AQCSFRC.bit.CSFB = 2;
        } 
        // if compare value is close to PERIOD, switch off during whole interval
        if (compare > (perioda - SVM_MODUL1.DBFED))
        {
            compare = perioda + 1;
            SVM_MODUL1.AQCSFRC.bit.CSFA = 1;
            SVM_MODUL1.AQCSFRC.bit.CSFB = 1;
        }

        SVM_MODUL1.CMPA.half.CMPA = compare;

        /* second leg */
        perioda = SVM_MODUL2.TBPRD;

        delta = perioda * Tb;

        compare = perioda - delta;

        SVM_MODUL2.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL2.AQCSFRC.bit.CSFB = 0;

        // if compare value is close to zero, turn on during whole interval
        if (compare < (SVM_MODUL1.DBFED/2))
        {
            compare = 0;
            SVM_MODUL2.AQCSFRC.bit.CSFA = 2;
            SVM_MODUL2.AQCSFRC.bit.CSFB = 2;
        } 
        // if compare value is close to PERIOD, switch off during whole interval
        if (compare > (perioda - SVM_MODUL1.DBFED/2))
        {
            compare = perioda + 1;
            SVM_MODUL2.AQCSFRC.bit.CSFA = 1;
            SVM_MODUL2.AQCSFRC.bit.CSFB = 1;
        }

        SVM_MODUL2.CMPA.half.CMPA = compare;

        /* third leg */
        perioda = SVM_MODUL3.TBPRD;

        delta = perioda * Tc;

        compare = perioda - delta;

        SVM_MODUL3.AQCSFRC.bit.CSFA = 0;
        SVM_MODUL3.AQCSFRC.bit.CSFB = 0;

        // if compare value is close to zero, turn on during whole interval
        if (compare < (SVM_MODUL1.DBFED/2))
        {
            compare = 0;
            SVM_MODUL3.AQCSFRC.bit.CSFA = 2;
            SVM_MODUL3.AQCSFRC.bit.CSFB = 2;
        } 
        // if compare value is close to PERIOD, switch off during whole interval
        if (compare > (perioda - SVM_MODUL1.DBFED/2))
        {
            compare = perioda + 1;
            SVM_MODUL3.AQCSFRC.bit.CSFA = 1;
            SVM_MODUL3.AQCSFRC.bit.CSFB = 1;
        }

        SVM_MODUL3.CMPA.half.CMPA = compare;
    }    
}

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene periode
* in je natancna na cikel natancno
* return: void
* arg1: zelena perioda
**************************************************************/
#pragma CODE_SECTION(SVM_period, "ramfuncs");
void SVM_period(float perioda)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ * perioda)
    temp_tbper = perioda * CPU_FREQ/2;
    
    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    
    // nastavim TBPER
    SVM_MODUL1.TBPRD = celi_del;
    SVM_MODUL2.TBPRD = celi_del;
    SVM_MODUL3.TBPRD = celi_del;
}   //end of FB_period

/**************************************************************
* Funkcija, ki nastavi periodo, za doseganje zeljene frekvence
* in je natancna na cikel natancno
* return: void
* arg1: zelena frekvenca
**************************************************************/
#pragma CODE_SECTION(SVM_frequency, "ramfuncs");
void SVM_frequency(float frekvenca)
{
    // spremenljivke
    float   temp_tbper;
    static float ostanek = 0;
    long celi_del;

    // naracunam TBPER (CPU_FREQ / SAMPLING_FREQ) - 1
    temp_tbper = (CPU_FREQ/2)/frekvenca;

    // izlocim celi del in ostanek
    celi_del = (long)temp_tbper;
    ostanek = temp_tbper - celi_del;
    // povecam celi del, ce je ostanek veji od 1
    if (ostanek > 1.0)
    {
        ostanek = ostanek - 1.0;
        celi_del = celi_del + 1;
    }
    
    // nastavim TBPER
    SVM_MODUL1.TBPRD = celi_del - 1;
    SVM_MODUL2.TBPRD = celi_del - 1;
    SVM_MODUL3.TBPRD = celi_del - 1;
}   //end of FB_frequency
