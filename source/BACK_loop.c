/****************************************************************
* FILENAME:     BACK_loop.c
* DESCRIPTION:  background code
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/

#include "BACK_loop.h"

// deklaracije lokalnih spremenljivk
bool on_tipka = FALSE;  // pulz, ko pritisnemo na tipko
bool mode_tipka = FALSE;  // pulz, ko pritisnemo na tipko

int sw_on_cnt_limit = 10;

int on_tipka_cnt = 0;
int mode_tipka_cnt = 0;

bool on_tipka_new;
bool mode_tipka_new;

int pulse_1000ms = 0;
int pulse_500ms = 0;
int pulse_100ms = 0;
int pulse_50ms = 0;
int pulse_10ms = 0;

// stevec, za pavzo po inicializaciji
int init_done_cnt = 0;
int fault_sensed_cnt = 0;

char str_to_send[] = "f=+10.0 Hz, M=+1.00 Nm";

// deklaracije zunanjih spremenljivk
extern int     interrupt_cnt;
extern float    komanda;
extern float    nap_dc;
extern float    iztek_trigger;
extern float    navor_mean;
extern float    speed_meh_abf;
// pototipi funkcij
void scan_keys(void);
void pulse_gen(void);

void startup_fcn(void);
void standby_fcn(void);
void work_fcn(void);
void fault_sensed_fcn(void);
void fault_fcn(void);

void hitrost_to_str(float hitrost, char *string);
void navor_to_str(float navor, char *string);

/**************************************************************
* Funkcija, ki se izvaja v ozadju med obratovanjem
**************************************************************/
void BACK_loop(void)
{

    // lokalne spremenljivke
    while (1)
    {

        // tukaj pride koda neskoncne zanke, ki tece v ozadju
        // generiranje pulzov in branje tipk
        pulse_gen();
        scan_keys();

        // vsake pol sekunde pošljem nov podatek
        // najprej pretvorim hitrost v string
        if (pulse_500ms == TRUE)
        {
            hitrost_to_str(speed_meh_abf, &str_to_send[2]);
            navor_to_str(navor_mean, &str_to_send[14]);
            //
            SCI_send_byte(&str_to_send, sizeof(str_to_send), 0);
        }

        /*****************/
        /* state machine */
        switch (state)
        {
        case Startup:
            startup_fcn();
            break;
        case Standby:
            standby_fcn();
            break;
        case Work:
            work_fcn();
            break;
        case Fault_sensed:
            fault_sensed_fcn();
            break;
        case Fault:
            fault_fcn();
            break;
        }

    }   // end of while(1)
}       // end of BACK_loop

void fault_fcn(void)
{
    // pobrišem napako, in grem v standby
    if (on_tipka == TRUE)
    {
        fault_flags.fault_registered = FALSE;
        fault_flags.cpu_overrun = FALSE;
        fault_flags.overcurrent = FALSE;
        fault_flags.HW_overcurent = FALSE;
        fault_flags.overspeed = FALSE;
        fault_flags.overtorque = FALSE;
        fault_flags.overvoltage = FALSE;
        fault_flags.undervoltage = FALSE;
        state = Standby;
    }
    // signalizacija
    PCB_fault_LED_on();
    PCB_on_LED_off();
}

void fault_sensed_fcn(void)
{

    // izklopim mostic
     SVM_disable();
     FLT_int_disable();

     if (fault_flags.fault_registered == FALSE)
     {
         fault_flags.fault_registered = TRUE;
     }
     // signalizacija
     PCB_fault_LED_on();
     PCB_on_LED_off();

     state = Fault;
}

void work_fcn(void)
{
    // normalno obratovanje
    // preverjam ce sem znotraj mejnih vrednost, ce je napetost DC-lika dovolj visoka
    // in ce uporabnik hoce izklop
    if ((on_tipka == TRUE))
    {
        state = Standby;
        SVM_disable();
        DELAY_US(100000);
    }

    // signaliziram da delujem
    PCB_on_LED_on();
    PCB_fault_LED_off();

    // signaliziram v katerem naèinu delovanja sem
    if (mode == Open_loop)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_toggle();
        }
    }
    if (mode == Torque)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_torque();
        }
    }
    if (mode == Speed)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_speed();
        }
    }
    if (mode == Position)
    {
        if (pulse_10ms == TRUE)
        {
            PCB_mode_LED_toggle();
        }
    }

}

void standby_fcn(void)
{

    // preklapljamo nacin delovanja
    // samo takrat ko pogledamo tipke
    if (mode_tipka == TRUE)
    {
        if (mode == Position)
        {
            DINT;
            mode = Open_loop;
            EINT;
        }
        else if (mode == Speed)
        {
            DINT;
            mode = Position;
            EINT;
        }
        else if (mode == Torque)
        {
            DINT;
            mode = Speed;
            EINT;
        }
        else if (mode == Open_loop)
        {
            DINT;
            mode = Torque;
            EINT;
        }
    }

    // sem pripravljen, sedaj samo cakam da uporabnik vklopi regulacijo
    // regulacija se vklopi samo in samo ce je zeljena vrednost pod 20%
    // vmes lahko preklapljamo nacin delovanja
    if ((on_tipka == TRUE) && (komanda < 0.2) && (komanda > -0.2))
    {
        DINT;
        state = Work;
        EINT;
        SVM_enable();
        DELAY_US(100000);
    }

    // signaliziram v katerem naèinu delovanja sem
    if (mode == Open_loop)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_toggle();
        }
    }
    if (mode == Torque)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_torque();
        }
    }
    if (mode == Speed)
    {
        if (pulse_100ms == TRUE)
        {
            PCB_mode_LED_speed();
        }
    }
    if (mode == Position)
    {
        if (pulse_10ms == TRUE)
        {
            PCB_mode_LED_toggle();
        }
    }

    // z hitrim utripanjem signaliziram da sem v pripravljenosti
    if (pulse_100ms == TRUE)
    {
        PCB_on_LED_toggle();
    }
    PCB_fault_LED_off();

}

void startup_fcn(void)
{
    // pri zagonu cakam, da se postavi DC-link napetost
    // in da uporabnik pritisne on tipko
    if ((nap_dc > U_DC_MIN) && (on_tipka == TRUE))
    {
        //state = Init;
        state = Standby;
    }

    // z hitrim utripanjem signaliziram da sem v zagonu
    if (pulse_100ms == TRUE)
    {
        PCB_on_LED_toggle();
        PCB_fault_LED_toggle();
    }
}

void navor_to_str(float navor, char *string)
{
    int enice;
    int desetinke;
    int stotinke;

    // predznak
    if (navor < 0)
    {
        *string = '-';
    }
    else
    {
        *string = '+';
    }

    navor = fabs(navor);

    // enice
    enice = (long)navor;
    *(string+1) = '0' + enice;

    *(string+2) = '.';

    // desetinke
    desetinke = (long)((navor - enice) * 10);
    *(string+3) = '0' + desetinke;

    // stotinke
    stotinke = (long)((((navor - enice) * 10) - desetinke) * 10);
    *(string+4) = '0' + stotinke;
}

void hitrost_to_str(float hitrost, char *string)
{
    int desetice;
    int enice;
    int desetinke;

    // predznak
    if (hitrost < 0)
    {
        *string = '-';
    }
    else
    {
        *string = '+';
    }

    hitrost = fabs(hitrost);

    // desetice
    desetice = (long)(hitrost / 10);
    *(string+1) = '0' + desetice;

    // enice
    enice = (long)(hitrost - desetice * 10);
    *(string+2) = '0' + enice;

    *(string+3) = '.';

    // desetinke
    desetinke = (long)(((hitrost - desetice * 10) - enice) * 10);
    *(string+4) = '0' + desetinke;
}


void pulse_gen(void)
{
    static long interrupt_cnt_old_500ms = 0;
    static long interrupt_cnt_old_100ms = 0;
    static long interrupt_cnt_old_50ms = 0;
    static long interrupt_cnt_old_10ms = 0;
    static int  pulse_1000ms_cnt = 0;

    long delta_cnt_500ms;
    long delta_cnt_100ms;
    long delta_cnt_50ms;
    long delta_cnt_10ms;

    if ( (interrupt_cnt - interrupt_cnt_old_500ms) < 0)
    {
        interrupt_cnt_old_500ms = interrupt_cnt_old_500ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_100ms) < 0)
    {
        interrupt_cnt_old_100ms = interrupt_cnt_old_100ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_50ms) < 0)
    {
        interrupt_cnt_old_50ms = interrupt_cnt_old_50ms - (SAMPLE_FREQ);
    }

    if ( (interrupt_cnt - interrupt_cnt_old_10ms) < 0)
    {
        interrupt_cnt_old_10ms = interrupt_cnt_old_10ms - (SAMPLE_FREQ);
    }

    delta_cnt_500ms = interrupt_cnt - interrupt_cnt_old_500ms;
    delta_cnt_100ms = interrupt_cnt - interrupt_cnt_old_100ms;
    delta_cnt_50ms = interrupt_cnt - interrupt_cnt_old_50ms;
    delta_cnt_10ms = interrupt_cnt - interrupt_cnt_old_10ms;

    // generiraj pulza vsakih 0.01
    if (delta_cnt_10ms > ((SAMPLE_FREQ)/100))
    {
        pulse_10ms = 1;
        interrupt_cnt_old_10ms = interrupt_cnt;
    }
    else
    {
        pulse_10ms = 0;
    }

    // generiraj pulze vsakih 0.05
    if (delta_cnt_50ms > ((SAMPLE_FREQ)/50))
    {
        pulse_50ms = 1;
        interrupt_cnt_old_50ms = interrupt_cnt;
    }
    else
    {
        pulse_50ms = 0;
    }


    // generiraj pulza vsakih 0.1
    if (delta_cnt_100ms > ((SAMPLE_FREQ)/10))
    {
        pulse_100ms = 1;
        interrupt_cnt_old_100ms = interrupt_cnt;
    }
    else
    {
        pulse_100ms = 0;
    }

    // generiraj pulza vsakih 0.5
    if (delta_cnt_500ms > ((SAMPLE_FREQ)/2))
    {
        pulse_500ms = 1;
        interrupt_cnt_old_500ms = interrupt_cnt;
    }
    else
    {
        pulse_500ms = 0;
    }

    // stejem pulze po 100ms
    if (pulse_100ms == 1)
    {
        pulse_1000ms_cnt = pulse_1000ms_cnt + 1;
        if (pulse_1000ms_cnt == 10)
        {
            pulse_1000ms = 1;
            pulse_1000ms_cnt = 0;
        }
        else
        {
            pulse_1000ms = 0;
        }
    }
    // da pulz traja samo in samo eno iteracijo
    if ((pulse_100ms != 1)&&(pulse_1000ms == 1))
    {
        pulse_1000ms = 0;
    }


}

void scan_keys(void)
{
    // lokalne spremenljivke

    // scan every cca 0.05s
    if (pulse_10ms == 1)
    {
        // preberem stanja tipk
        on_tipka_new = PCB_on_SW();
        mode_tipka_new = PCB_mode_SW();

        // ali smo pritisnili na tipko 1
        // ce je tipka pritisnjena, stopaj koliko casa je prisitsnjena
        if (on_tipka_new == TRUE)
        {
            on_tipka_cnt = on_tipka_cnt + 1;
        }
        // ce ni pritisnjena resetiraj stevec
        else
        {
            on_tipka_cnt = 0;
        }

        // ce je tipka pritisnjena dovolj casa, javi programu - samo enkrat
        if (on_tipka_cnt == sw_on_cnt_limit)
        {
            on_tipka = TRUE;
        }
        // sicer pa ne javi
        else
        {
            on_tipka = FALSE;
        }

        // ali smo pritisnili na tipko 2
        // ce je tipka pritisnjena, stopaj koliko casa je prisitsnjena
        if (mode_tipka_new == TRUE)
        {
            mode_tipka_cnt = mode_tipka_cnt + 1;
        }
        // ce ni pritisnjena resetiraj stevec
        else
        {
            mode_tipka_cnt = 0;
        }

        // ce je tipka pritisnjena dovolj casa, javi programu - samo enkrat
        if (mode_tipka_cnt == sw_on_cnt_limit)
        {
            mode_tipka = TRUE;
        }
        // sicer pa ne javi
        else
        {
            mode_tipka = FALSE;
        }
    }
    // da je pulz dolg res samo in samo eno iteracijo
    else
    {
        on_tipka = FALSE;
        mode_tipka = FALSE;
    }
}
