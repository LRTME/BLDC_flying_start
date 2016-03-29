/****************************************************************
 * FILENAME:     PER_int.c
 * DESCRIPTION:  periodic interrupt code
 * AUTHOR:       Mitja Nemec
 * DATE:         16.1.2009
 *
 ****************************************************************/
#include    "PER_int.h"

#include    "TIC_toc.h"

// definicije za uporabniski vmesnik - potenciometri
// širina mrtve cone
#define     POT_DEAD_BAND_WIDTH     0.1
//  širina nasièenja
#define     POT_SATURATION_WIDTH    0.02

// tokovna regulatorja
PID_float   reg_id = PID_FLOAT_DEFAULTS;
PID_float   reg_iq = PID_FLOAT_DEFAULTS;

// tokovna regulator za bldc
PID_float   reg_idc = PID_FLOAT_DEFAULTS;

// hitrostni regulator
PID_float   reg_speed = PID_FLOAT_DEFAULTS;

// regulator pozicije
PID_float   reg_position = PID_FLOAT_DEFAULTS;

// izracun hitrosti
RDIF_float  diff_speed = RDIF_FLOAT_DEFAULTS;

// transofrmacije
PARK_float      park_tok = PARK_FLOAT_DEFAULTS;
CLARKE_float    clarke_tok = CLARKE_FLOAT_DEFAULTS;
IPARK_float     ipark_napetost = IPARK_FLOAT_DEFAULTS;

// AB filter za izraèun hitrosti
ABF_float       ab_tracker = ABF_FLOAT_DEFAULTS;

// za mehansko moè (delovno in jalovo)
VAR_stat    navor_stat = VAR_STAT_DEFAULTS;
VAR_stat    hitrost_stat = VAR_STAT_DEFAULTS;
VAR_stat    pwr_mech_stat = VAR_STAT_DEFAULTS;
float       pwr_mech_imag = 0.0;
float       pwr_mech_real = 0.0;

float       navor_mean = 0;

// mehanika
long    kot_hex;
float   kot_raw = 0.0;
float   kot_offset = 0;
float   kot_meh = 0.0;      // per unit
float   kot_el = 0.0;       // radians
float   speed_meh_rdiff = 0.0;    // Hz
float   speed_meh_abf = 0.0;
float   speed_meh_ctrl = 0.0;    // Hz
bool    use_abf = FALSE;
float   sektor = 1;
float   kot_sektor = 0;
float   sektor_offset = 0.0;
float   position = 0;
float   revolutions = 0;

// tokovi
float   tok_i1 = 0.0;
float   tok_i2 = 0.0;
float   tok_i3 = 0.0;
float   tok_dc = 0.0;

float   tok_i1_offset = 3460.869;
float   tok_i2_offset = 3504.854;
float   tok_i3_offset = 3525.017;

float   tok_i1_fake_offset = 0.0;

float   tok_i1_offset_calib = 0.0;
float   tok_i2_offset_calib = 0.0;
float   tok_i3_offset_calib = 0.0;

float   tok_i1_gain = -(10.0/13.5) * (6.2/7.5)*((3.3 * 15.0)/(0.625 * 4096));
float   tok_i2_gain = -(10.0/13.5) * (6.2/7.5)*((3.3 * 15.0)/(0.625 * 4096));
float   tok_i3_gain = -(10.0/13.5) * (6.2/7.5)*((3.3 * 15.0)/(0.625 * 4096));

float   tok_d = 0.0;
float   tok_q = 0.0;

long    current_offset_counter = 0;

// napetosti
float   nap_u1 = 0.0;
float   nap_u2 = 0.0;
float   nap_u3 = 0.0;
float   nap_dc = 0.0;

float   nap_u1_offset = 0.0;
float   nap_u2_offset = 0.0;
float   nap_u3_offset = 0.0;
float   nap_dc_offset = 0.0;

float   nap_u1_gain = (0.5) * (3.3) * (18.0 + 1.0)/(1.0 * 4096.0);
float   nap_u2_gain = (0.5) * (3.3) * (18.0 + 1.0)/(1.0 * 4096.0);
float   nap_u3_gain = (0.5) * (3.3) * (18.0 + 1.0)/(1.0 * 4096.0);
float   nap_dc_gain = (0.5) * (3.3) * (18.0 + 1.0)/(1.0 * 4096.0);

// za generiranje testnih signalov
float   ref_freq = 0.1;
float   ref_kot = 0.0;
float   ref_duty = 0.4;
float   ref_amp = 0.0;
float   ref_offset = 0.0;
float   ref_out = 0.0;

// omejitev naklona pri spreminjanju konstante
SLEW_float  ref_step_konst = SLEW_FLOAT_DEFAULTS;
// omejitev naklona za stopnico zeljene vrednosti
SLEW_float  ref_step_slew = SLEW_FLOAT_DEFAULTS;
// omejitev naklona za slew zeljene vrednosti
SLEW_float  ref_slew = SLEW_FLOAT_DEFAULTS;

// izracunan navor
float   navor = 0.0;

// zeljene vrednosti
float   ref_open_loop = 0.0;
float   ref_torque = 0.0;
float   ref_speed = 0.0;
float   ref_position = 0.0;

// nastavitev
float   komanda = 0.0;

// vklopno razmerje
float   alpha = 0.0;
float   beta = 0.0;

// obremenjenost procesorja
float   cpu_load  = 0.0;

// spremenljikva s katero štejemo kolikokrat se je prekinitev predolgo izvajala
int     interrupt_overflow_counter = 0;
int     interrupt_cnt = 0;

// testiranje HW-ja
float   debug_angle = 0.0;
float   debug_duty = 0.1;
bool    debug = FALSE;
bool    debug_encoder = FALSE;

void check_limits(void);
void get_electrical(void);
void get_mechanical(void);
void motor_control(void);
void ref_gen(void);
void get_stat(void);
/**************************************************************
 * Prekinitev, ki v kateri se izvaja regulacija
 **************************************************************/
#pragma CODE_SECTION(PER_int, "ramfuncs");
void interrupt PER_int(void)
{
    /* lokalne spremenljivke */
    long interrupt_cycles;

    // najprej povem da sem se odzzval na prekinitev
    // Spustimo INT zastavico casovnika ePWM1
    EPwm2Regs.ETCLR.bit.INT = 1;
    // Spustimo INT zastavico v PIE enoti
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    // pozenem stoprico
    interrupt_cycles = TIC_time;
    TIC_start();

    // izracunam obremenjenost procesorja
    cpu_load = (float)interrupt_cycles / (CPU_FREQ/SWITCH_FREQ);

    // stevec prekinitev, ki se resetira vsako sekundo
    interrupt_cnt = interrupt_cnt + 1;
    if (interrupt_cnt > SWITCH_FREQ)
    {
        interrupt_cnt = 0;
    }

    get_mechanical();

    get_electrical();

    // generator željene vrednosti
    ref_amp = komanda;

    ref_gen();
    ref_open_loop = ref_out;
    ref_torque = ref_out * TORQUE_MAX;
    ref_speed = ref_out * SPEED_MAX;
    ref_position = ref_out;

    // izracunam navor
    navor = (3.0 * POLE_PAIRS / 2.0) * (FLUX * tok_q + (L_D - L_Q) * tok_d * tok_q);

    check_limits();

    // ce ne delujem, potem lahko kalibriram tokovne sonde
    if (state == Standby)
    {
        //tok_i1 = tok_i1_gain * (TOK_1_Z + TOK_1_P - tok_i1_offset);
        tok_i1_offset = tok_i1_offset + 0.001 * (tok_i1/tok_i1_gain);
        tok_i2_offset = tok_i2_offset + 0.001 * (tok_i2/tok_i2_gain);
        tok_i3_offset = tok_i3_offset + 0.001 * (tok_i3/tok_i3_gain);
    }

    motor_control();

    // statistika
    get_stat();

    // osvežim data loger
    DLOG_GEN_update();

    /* preverim, èe me sluèajno èaka nova prekinitev.
       èe je temu tako, potem je nekaj hudo narobe
       saj je èas izvajanja prekinitve predolg
       vse skupaj se mora zgoditi najmanj 10krat,
       da reèemo da je to res problem
     */
    if (EPwm2Regs.ETFLG.bit.INT == TRUE)
    {
        // povecam stevec, ki steje take dogodke
        interrupt_overflow_counter = interrupt_overflow_counter + 1;

        // in ce se je vse skupaj zgodilo 10 krat se ustavim
        // v kolikor uC krmili kakšen resen HW, potem moèno
        // proporoèam lepše "hendlanje" takega dogodka
        // beri:ugasni moènostno stopnjo, ...
        if (interrupt_overflow_counter >= 10)
        {
            fault_flags.cpu_overrun = TRUE;
        }
    }

    // ustavim štoparico
    TIC_stop();

}   // end of PWM_int

/**************************************************************
 * Funckija, ki pripravi vse potrebno za izvajanje
 * prekinitvene rutine
 **************************************************************/
void PER_int_setup(void)
{
    // Proženje prekinitve
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    //sproži prekinitev na periodo
    EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;         //ob vsakem prvem dogodku
    EPwm2Regs.ETCLR.bit.INT = 1;                //clear possible flag
    EPwm2Regs.ETSEL.bit.INTEN = 1;              //enable interrupt

    // inicializacija tekocega odvoda
    RDIF_FLOAT_INIT(diff_speed);

    // inicializacija statistike
    navor_stat.Mean = 0;
    navor_stat.Sum_square = 0;
    VAR_STAT_MACRO_INIT(navor_stat);

    hitrost_stat.Mean = 0;
    hitrost_stat.Sum_square = 0;
    VAR_STAT_MACRO_INIT(hitrost_stat);

    pwr_mech_stat.Mean = 0;
    pwr_mech_stat.Sum_square = 0;
    VAR_STAT_MACRO_INIT(pwr_mech_stat);

    // inicializacija omejitev naklona
    ref_step_konst.Slope_down = 2.0;
    ref_step_konst.Slope_up = 2.0;
    ref_step_konst.Sampling_period = SAMPLE_TIME;
    ref_step_slew.Slope_down = 20.0;
    ref_step_slew.Slope_up = 20.0;
    ref_step_slew.Sampling_period = SAMPLE_TIME;
    ref_slew.Slope_down = 2.0;
    ref_slew.Slope_up = 2.0;
    ref_slew.Sampling_period = SAMPLE_TIME;


    // inicializacija AB filtra
    ab_tracker.Alpha = 0.1;
    ab_tracker.Beta = 0.001;
    ab_tracker.Sampling_period = SAMPLE_TIME;

    // inicializiram data logger
    dlog.iptr1 = &reg_iq.Ref;
    dlog.iptr2 = &reg_iq.Fdb;
    dlog.iptr3 = &tok_i2;
    dlog.iptr4 = &tok_i3;

    dlog.trig = &ref_kot;
    dlog.trig_value = 0.5;
    dlog.slope = Negative;

    dlog.prescalar = 1;
    dlog.mode = Normal;


    // inicializiram tokovna regulatorja
    reg_id.Kp = 0.05;
    reg_id.Ki = 1000.0;
    reg_id.Kd = 0.0;
    reg_id.OutMax = +0.95;
    reg_id.OutMin = -0.95;
    reg_id.Sampling_period = SAMPLE_TIME;

    reg_iq.Kp = reg_id.Kp;
    reg_iq.Ki = reg_id.Ki;
    reg_iq.Kd = reg_id.Kd;
    reg_iq.OutMax = reg_id.OutMax;
    reg_iq.OutMin = reg_id.OutMin;
    reg_iq.Sampling_period = reg_id.Sampling_period;

    reg_idc.Kp = 0.05;
    reg_idc.Ki = 1000.0;
    reg_idc.Kd = 0.0;
    reg_idc.OutMax = +0.95;
    reg_idc.OutMin = -0.95;
    reg_idc.Sampling_period = SAMPLE_TIME;

    // inicializiram hitrostni regulator
    reg_speed.Kp = 0.5;
    reg_speed.Ki = 10.0;
    reg_speed.Kd = 0.0;
    reg_speed.OutMax = +TORQUE_MAX;
    reg_speed.OutMin = -TORQUE_MAX;
    reg_speed.Sampling_period = SAMPLE_TIME;
    // inicializiram pozicijski regulator
    reg_position.Kp = 30;
    reg_position.Ki = 0.000;
    reg_position.Kd = 1.0;
    reg_position.OutMax = +SPEED_MAX;
    reg_position.OutMin = -SPEED_MAX;
    reg_position.Sampling_period = SAMPLE_TIME;

    // inicializiram štoparico
    TIC_init();

    // registriram prekinitveno rutino
    EALLOW;
    PieVectTable.EPWM2_INT = &PER_int;
    EDIS;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    IER |= M_INT3;
    // da mi prekinitev teèe  tudi v real time naèinu
    // (za razhoršèevanje main zanke in BACK_loop zanke)
    SetDBGIER(M_INT3);
}

#pragma CODE_SECTION(get_stat, "ramfuncs");
void get_stat(void)
{
    // pomerim navor
    navor_stat.In = navor;
    navor_stat.Index_value = kot_meh;
    VAR_STAT_MACRO(navor_stat);

    navor_mean = navor_stat.Mean;

    // pomerim hitrost
    hitrost_stat.In = speed_meh_ctrl;
    hitrost_stat.Index_value = kot_meh;
    VAR_STAT_MACRO(hitrost_stat);

    // pomerim moc
    pwr_mech_stat.In = navor * speed_meh_ctrl * 2 * PI;
    pwr_mech_stat.Index_value = kot_meh;
    VAR_STAT_MACRO(pwr_mech_stat);
    pwr_mech_real = pwr_mech_stat.Mean;

    // izracunam "jalovo" mehansko moc
    pwr_mech_imag = sqrt(hitrost_stat.Rms * hitrost_stat.Rms * navor_stat.Rms * navor_stat.Rms * 2 * PI * 2 * PI - pwr_mech_real * pwr_mech_real);

}

#pragma CODE_SECTION(motor_control, "ramfuncs");
void motor_control(void)
{
    // ce delam, potem reguliram
    if (state == Work)
    {
        // BLDC
        if (type == BLDC)
        {
            if (mode == Open_loop)
            {
                // samo za test BLDC-delovanja
                SVM_update_bldc(ref_open_loop, sektor);
            }
            if (mode == Torque)
            {
                // samo dokler smo pod nazivno hitrostjo
                reg_idc.Ref = 2 * ref_torque / (FLUX * POLE_PAIRS * 3);
                if (speed_meh_ctrl > (+SPEED_MAX))
                {
                    reg_idc.Ref = 0.0;
                }
                if (speed_meh_ctrl < (-SPEED_MAX))
                {
                    reg_idc.Ref = 0.0;
                }

                reg_idc.Fdb = tok_dc;
                PID_FLOAT_CALC(reg_idc);


                // samo za test BLDC-delovanja
                SVM_update_bldc(reg_idc.Out, sektor);
            }
            if (mode == Speed)
            {
                reg_speed.Ref = ref_speed;
                reg_speed.Fdb = speed_meh_ctrl;
                PID_FLOAT_CALC(reg_speed);

                reg_idc.Ref = 2 * reg_speed.Out / (3 * POLE_PAIRS * FLUX);
                reg_idc.Fdb = tok_dc;
                PID_FLOAT_CALC(reg_idc);

                // samo za test BLDC-delovanja
                SVM_update_bldc(reg_idc.Out, sektor);
            }
            if (mode == Position)
            {
                reg_position.Ref = ref_position;
                reg_position.Fdb = position;
                PID_FLOAT_CALC(reg_position);

                // hitrostna regulacija
                reg_speed.Ref = reg_position.Out;
                reg_speed.Fdb = speed_meh_ctrl;
                PID_FLOAT_CALC(reg_speed);

                reg_idc.Ref = 2 * reg_speed.Out / (3 * POLE_PAIRS * FLUX);
                reg_idc.Fdb = tok_dc;
                PID_FLOAT_CALC(reg_idc);

                // samo za test BLDC-delovanja
                SVM_update_bldc(reg_idc.Out, sektor);
            }
        }

        // FOC
        if (type == PMSM)
        {
            if (mode == Open_loop)
            {
                // inverzni park
                ipark_napetost.Angle = kot_el * 2 * PI;
                ipark_napetost.Ds = 0;
                ipark_napetost.Qs = ref_open_loop;
                IPARK_FLOAT_CALC(ipark_napetost);

                ipark_napetost.Alpha = ipark_napetost.Alpha;
                ipark_napetost.Beta = ipark_napetost.Beta;

                SVM_update(ipark_napetost.Alpha, ipark_napetost.Beta);
            }

            // navorna regulacija
            if (mode == Torque)
            {
                // samo dokler smo pod nazivno hitrostjo
                reg_iq.Ref = 2 * ref_torque / (3 * POLE_PAIRS * FLUX);
                if (speed_meh_ctrl > (+SPEED_MAX))
                {
                    reg_iq.Ref = 0.0;
                }
                if (speed_meh_ctrl < (-SPEED_MAX))
                {
                    reg_iq.Ref = 0.0;
                }

                // tokovna regulacija
                reg_id.Ref = 0.0;
                reg_id.Fdb = tok_d;
                PID_FLOAT_CALC(reg_id);

                reg_iq.Fdb = tok_q;
                PID_FLOAT_CALC(reg_iq);

                // inverzni park
                ipark_napetost.Angle = kot_el * 2 * PI;
                ipark_napetost.Ds = reg_id.Out;
                ipark_napetost.Qs = reg_iq.Out;
                IPARK_FLOAT_CALC(ipark_napetost);

                ipark_napetost.Alpha = ipark_napetost.Alpha;
                ipark_napetost.Beta = ipark_napetost.Beta;

                SVM_update(ipark_napetost.Alpha, ipark_napetost.Beta);
            }

            // hitrostna regulacija
            if (mode == Speed)
            {
                // generator zeljene vrednosti - za testiranje regulatorjev

                reg_speed.Ref = ref_speed;
                reg_speed.Fdb = speed_meh_ctrl;
                PID_FLOAT_CALC(reg_speed);

                // tokovna regulacija
                reg_id.Ref = 0.0;
                reg_id.Fdb = tok_d;
                PID_FLOAT_CALC(reg_id);

                reg_iq.Ref = 2 * reg_speed.Out / (FLUX * POLE_PAIRS * 3);
                reg_iq.Fdb = tok_q;
                PID_FLOAT_CALC(reg_iq);

                // inverzni park
                ipark_napetost.Angle = kot_el * 2 * PI;
                ipark_napetost.Ds = reg_id.Out;
                ipark_napetost.Qs = reg_iq.Out;
                IPARK_FLOAT_CALC(ipark_napetost);

                if (debug_encoder == TRUE)
                {
                    debug_duty = komanda;
                    SVM_update(debug_duty, 0.0);
                }

                else
                {
                    ipark_napetost.Alpha = ipark_napetost.Alpha;
                    ipark_napetost.Beta = ipark_napetost.Beta;

                    SVM_update(ipark_napetost.Alpha, ipark_napetost.Beta);
                }

            }
            if (mode == Position)
            {
                reg_position.Ref = ref_position;
                reg_position.Fdb = position;
                PID_FLOAT_CALC(reg_position);

                reg_speed.Ref = reg_position.Out;
                reg_speed.Fdb = speed_meh_ctrl;
                PID_FLOAT_CALC(reg_speed);

                // tokovna regulacija
                reg_id.Ref = 0.0;
                reg_id.Fdb = tok_d;
                PID_FLOAT_CALC(reg_id);

                reg_iq.Ref = 2 * reg_speed.Out / (FLUX * POLE_PAIRS * 3);
                reg_iq.Fdb = tok_q;
                PID_FLOAT_CALC(reg_iq);

                // inverzni park
                ipark_napetost.Angle = kot_el * 2 * PI;
                ipark_napetost.Ds = reg_id.Out;
                ipark_napetost.Qs = reg_iq.Out;
                IPARK_FLOAT_CALC(ipark_napetost);

                SVM_update(ipark_napetost.Alpha, ipark_napetost.Beta);
            }
        }

    }
    if (state == Standby)
    {
        reg_position.Ui = 0.0;
        reg_speed.Ui = 0.0;
        reg_id.Ui = 0.0;
        reg_iq.Ui = 0.0;

        reg_idc.Ui = 0.0;

        // da nam regulator pozicije ne znori, èe smo predhodno
        // z hitrostno regulacijo šli nekam v tri krasne
        revolutions = 0;
    }
}

#pragma CODE_SECTION(get_electrical, "ramfuncs");
void get_electrical(void)
{
    static float    komanda_old = 0.0;

    // pocakam da ADC konca s pretvorbo
    ADC_wait();

    // poberem vrednosti iz AD pretvornika
    // kalibracija preostalega toka
    if (current_offset_calibrated == FALSE)
    {
        // akumuliram offset
        tok_i1_offset_calib = tok_i1_offset_calib + (TOK_1_Z + TOK_1_P);
        tok_i2_offset_calib = tok_i2_offset_calib + (TOK_2_Z + TOK_2_P);
        tok_i3_offset_calib = tok_i3_offset_calib + (TOK_3_Z + TOK_3_P);

        // ko potece dovolj casa, sporocim da lahko grem naprej
        // in izracunam povprecni offset
        current_offset_counter = current_offset_counter + 1;
        if (current_offset_counter == SWITCH_FREQ)
        {
            current_offset_calibrated = TRUE;
            tok_i1_offset = tok_i1_offset_calib / SWITCH_FREQ;
            tok_i2_offset = tok_i2_offset_calib / SWITCH_FREQ;
            tok_i3_offset = tok_i3_offset_calib / SWITCH_FREQ;
        }
        tok_i1 = 0.0;
        tok_i2 = 0.0;
        tok_i3 = 0.0;
    }
    else
    {
        tok_i1 = tok_i1_gain * (TOK_1_Z + TOK_1_P - tok_i1_offset);
        tok_i2 = tok_i2_gain * (TOK_2_Z + TOK_2_P - tok_i2_offset);
        tok_i3 = tok_i3_gain * (TOK_3_Z + TOK_3_P - tok_i3_offset);

        tok_i1 = tok_i1 + tok_i1_fake_offset;
    }

    // okoli nicle vrzem zeleno vrednost na cisto niclo
    // še komande izpeljem uni in bipolarno
    // okoli nicle vrzem zeleno vrednost na cisto niclo
    komanda = 1.0 - ((KOMANDA_Z + KOMANDA_P) / 4096.0);
    komanda = komanda * 1.0 / (1.0 - (POT_DEAD_BAND_WIDTH * 2 + POT_SATURATION_WIDTH * 2));
    if (komanda > 0.0)
    {
        if (komanda < POT_DEAD_BAND_WIDTH)
        {
            komanda = 0;
        }
        else
        {
            komanda = komanda - POT_DEAD_BAND_WIDTH;
        }
    }
    if (komanda < 0.0)
    {
        if (komanda > -POT_DEAD_BAND_WIDTH)
        {
            komanda = 0;
        }
        else
        {
            komanda = komanda + POT_DEAD_BAND_WIDTH;
        }
    }
    if (komanda > 1.0)
    {
        komanda = 1.0;
    }
    if (komanda < -1.0)
    {
        komanda = -1.0;
    }

    // na vse komande dam se histerezo 0.005 - da imam manj suma
    // najprej zaokrozim na +-0.005
    komanda = (long)(komanda * 200);

    // dodam histerezo
    if (fabs(komanda_old - komanda) < 2)
    {
        komanda = komanda_old;
    }
    // si zapomnim za naslednjiè
    komanda_old = 2*(long)(komanda / 2);
    // in spravim dol na 0-1
    komanda = komanda / 200;

    // se napetosti
    nap_u1 = nap_u1_gain * (NAP_1_Z + NAP_1_P - nap_u1_offset);
    nap_u2 = nap_u2_gain * (NAP_1_Z + NAP_1_P - nap_u2_offset);
    nap_u3 = nap_u3_gain * (NAP_1_Z + NAP_1_P - nap_u3_offset);
    nap_dc = nap_dc_gain * (U_DC_Z + U_DC_P - nap_dc_offset);

    // clarke
    clarke_tok.As = tok_i1;
    clarke_tok.Bs = tok_i2;
    CLARKE_FLOAT_CALC(clarke_tok);

    // parke
    park_tok.Alpha = clarke_tok.Alpha;
    park_tok.Beta = clarke_tok.Beta;
    park_tok.Angle = kot_el * (2 * PI);
    PARK_FLOAT_CALC(park_tok);
    tok_d = park_tok.Ds;
    tok_q = park_tok.Qs;

    // tok dc
    switch((int)sektor)
    {
    case 1:
        tok_dc = tok_i3;
        break;
    case 2:
        tok_dc = tok_i1;
        break;
    case 3:
        tok_dc = tok_i1;
        break;
    case 4:
        tok_dc = tok_i2;
        break;
    case 5:
        tok_dc = tok_i2;
        break;
    case 6:
        tok_dc = tok_i3;
        break;
    default:
        tok_dc = 0.0;
        break;
    }
    tok_dc = -tok_dc;
}

#pragma CODE_SECTION(get_mechanical, "ramfuncs");
void get_mechanical(void)
{
    float delta = 0;
    static float kot_meh_prev = 0;


    // preberem kot rotorja
    kot_raw = SPI_getkot();
    kot_hex = kot_raw;

    // izracunam kot rotorja
    kot_meh = (kot_raw - kot_offset) * (1.0/ENC_RES);
    if (kot_meh < 0.0)
    {
        kot_meh = kot_meh + 1.0;
    }
    kot_meh = kot_meh;

    // štejem obrate
    delta = kot_meh - kot_meh_prev;
    if (delta > +0.5)
    {
        revolutions = revolutions - 1;
    }
    if (delta < -0.5)
    {
        revolutions = revolutions + 1;
    }
    kot_meh_prev = kot_meh;

    // naraèunam pozivicijo
    position = kot_meh + revolutions;

    // izracunam hitrost rotorja
    diff_speed.in = kot_meh;
    RDIF_FLOAT_CALC(diff_speed);
    // popravim v primeru preliva
    if (diff_speed.out > 0.5)
    {
        diff_speed.out = diff_speed.out - 1;
    }
    if (diff_speed.out < -0.5)
    {
        diff_speed.out = diff_speed.out + 1;
    }
    speed_meh_rdiff = diff_speed.out * (SAMPLE_FREQ / RDIF_BUFF_SIZE);

    // se z Alfa beta filtrom
    ab_tracker.ThetaInK = kot_meh;
    ABF_float_calc(&ab_tracker);
    speed_meh_abf = ab_tracker.ThetaDotOutK;

    if (use_abf == TRUE)
    {
        speed_meh_ctrl = speed_meh_abf;
    }
    else
    {
        speed_meh_ctrl = speed_meh_rdiff;
    }

    // popravim kot zaradi zamika pri prenosu
    kot_meh = kot_meh + speed_meh_rdiff/SWITCH_FREQ;

    // izracunam elektricni kot rotorja
    kot_el = (kot_meh * POLE_PAIRS) - ((long)(kot_meh * POLE_PAIRS));

    // izracunam sektor za BLDC
    kot_sektor = kot_el + sektor_offset;
    if (kot_sektor > 1.0)
    {
        kot_sektor = kot_sektor - 1.0;
    }
    if (kot_sektor < 0.0)
    {
        kot_sektor = kot_sektor + 1.0;
    }
    sektor = (long)(6 * (kot_sektor + (30.0/360)));
    sektor = sektor + 1;
    if (sektor > 6)
    {
        sektor = sektor - 6;
    }

}

#pragma CODE_SECTION(check_limits, "ramfuncs");
void check_limits(void)
{
    // preverim ali sem znotraj dovoljenega podroèja delovanja
    if ((navor > +TORQUE_LIM) || (navor < -TORQUE_LIM))
    {
        fault_flags.overtorque = TRUE;
        state = Fault_sensed;
    }

    if ((speed_meh_ctrl > +SPEED_LIM) || (speed_meh_ctrl < -SPEED_LIM))
    {
        fault_flags.overspeed = TRUE;
        state = Fault_sensed;
    }

    if (nap_dc > U_DC_LIM)
    {
        fault_flags.overvoltage = TRUE;
        state = Fault_sensed;
    }
    if (nap_dc < U_DC_MIN)
    {
        fault_flags.undervoltage = TRUE;
        state = Fault_sensed;
    }
    if ((tok_i1 > +CURRENT_LIM) || (tok_i1 < -CURRENT_LIM))
    {
        fault_flags.overcurrent = TRUE;
        state = Fault_sensed;
    }
    if ((tok_i2 > +CURRENT_LIM) || (tok_i2 < -CURRENT_LIM))
    {
        fault_flags.overcurrent = TRUE;
        state = Fault_sensed;
    }
    if ((tok_i3 > +CURRENT_LIM) || (tok_i3 < -CURRENT_LIM))
    {
        fault_flags.overcurrent = TRUE;
        state = Fault_sensed;
    }
}

#pragma CODE_SECTION(ref_gen, "ramfuncs");
void ref_gen(void)
{
    // vrednost, ki jo pošljem ven
    float   ref_internal = 0.0;

    // najprej generiram kot
    ref_kot = ref_kot + ref_freq * SAMPLE_TIME;
    if (ref_kot > 1.0)
    {
        ref_kot = ref_kot - 1.0;
    }
    if (ref_kot < 0.0)
    {
        ref_kot = ref_kot + 1.0;
    }

    // potem pa generiram referenèni signal glede na izbrano obliko
    switch(ref_type)
    {
    case Step:
        // generiram signal zaradi zašèite elektronike dodam omejitev naklona
        // najprej generiram step
        if (ref_kot < ref_duty)
        {
            ref_internal = ref_amp + ref_offset;
        }
        else
        {
            ref_internal = ref_offset;
        }
        // omejim naklon
        // tudi pri èisti stopnici omejim naklon zaradi strojne opreme
        ref_step_slew.In = ref_internal;
        SLEW_FLOAT_CALC(ref_step_slew);
        ref_out = ref_step_slew.Out;

        break;
    case Slew:
        // generiram signal
        if (ref_kot < ref_duty)
        {
            ref_internal = ref_amp + ref_offset;
        }
        else
        {
            ref_internal = ref_offset;
        }
        // omejim naklon
        ref_slew.In = ref_internal;
        SLEW_FLOAT_CALC(ref_slew);
        ref_out = ref_slew.Out;

        break;

    case Konst:
        // generiram signal
        ref_step_konst.In = ref_amp;
        SLEW_FLOAT_CALC(ref_step_konst);
        ref_out = ref_step_konst.Out;

        break;

    case Sine:
        // generiram signal
        ref_out = ref_offset + ref_amp * sin(2*PI*ref_kot);
        break;

    default:
        ref_out = 0.0;
        break;

    }
}
