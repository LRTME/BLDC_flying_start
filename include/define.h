/****************************************************************
* FILENAME:     define.h           
* DESCRIPTION:  file with global define macros
* AUTHOR:       Mitja Nemec
* START DATE:   16.1.2009
* VERSION:      1.0
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       16.1.2009   Mitja Nemec Initial version
*
****************************************************************/
#ifndef     __DEFINE_H__
#define     __DEFINE_H__

// frekvenca PWM-ja
#define     SWITCH_FREQ     20000L

// veèkratnik preklopne frekvence
#define     SAMP_PRESCALE   1

// Vzorèna frekvenca
#define     SAMPLE_FREQ     (SWITCH_FREQ/SAMP_PRESCALE)

// vzorèna perioda
#define     SAMPLE_TIME     (1.0/SAMPLE_FREQ)

// frekvenca procesorja v Hz
#define     CPU_FREQ        80000000

// parametri stroja
#define     POLE_PAIRS      3
#define     L_D             0.000068
#define     L_Q             0.000086
#define     R_S             0.0567
#define     FLUX            0.0093407
#define     J_MOT           0.0001682

// podatki dajalnika
#define     ENC_BITS        12
#define     ENC_RES         (1<<12)

// obratovalni parametri
#define     TORQUE_MAX      1.0
#define     TORQUE_LIM      1.5
#define     SPEED_MAX       15.0
#define     SPEED_LIM       18.0
#define     POSITION_MAX    (0.2)
#define     CURRENT_MAX     25.0 // 1Nm -> 23 A
#define     CURRENT_LIM     36.0 // 1.5Nm -> 36 A
#define     U_DC_MIN        10.0
#define     U_DC_MAX        36.0
#define     U_DC_LIM        50.0

// definicije matematiènih konstant
#define     SQRT3           1.7320508075688772935274463415059
#define     SQRT2           1.4142135623730950488016887242097
#define     PI              3.1415926535897932384626433832795

// deklaracije za logicne operacije
typedef enum {FALSE = 0, TRUE} bool;

#endif // end of __DEFINE_H__ definition
