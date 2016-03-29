/****************************************************************
* FILENAME:     2802x_lnk_RAM.cmd             
* DESCRIPTION:  device linker command file 
* AUTHOR:       Mitja Nemec
* START DATE:   5.3.2011
* VERSION:      1.0
*
* NOTES: based upon DSP28069 Header Files V1.00
*
* CHANGES : 
* VERSION   DATE        WHO         DETAIL 
* 1.0       5.3.2011     Mitja Nemec Initial version
*
*
****************************************************************/


MEMORY
{
PAGE 0 :
    RESET_M     : origin = 0x000000,    length = 0x000002

    P_M0        : origin = 0x000050,    length = 0x0003B0     /* on-chip RAM block M0 */
    P_M1        : origin = 0x000400,    length = 0x000400     /* on-chip RAM block M1 */

    P_L78       : origin = 0x010000,    length = 0x004000     /* on-chip RAM block L1 */
   
    P_OTP       : origin = 0x3D7800,    length = 0x0003FA     /* on-chip OTP */

    P_FH        : origin = 0x3D8000,    length = 0x004000     /* on-chip FLASH */
    P_FG        : origin = 0x3DC000,    length = 0x004000     /* on-chip FLASH */
    P_FF        : origin = 0x3E0000,    length = 0x004000     /* on-chip FLASH */
    P_FE        : origin = 0x3E4000,    length = 0x004000     /* on-chip FLASH */   
    P_FD        : origin = 0x3E8000,    length = 0x004000     /* on-chip FLASH */
    P_FC        : origin = 0x3EC000,    length = 0x004000     /* on-chip FLASH */
    P_FB        : origin = 0x3F0000,    length = 0x004000     /* on-chip FLASH */
    P_FA        : origin = 0x3F4000,    length = 0x003F80     /* on-chip FLASH */
   
    CSM_RSVD    : origin = 0x3F7F80,    length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */
    RESET_F     : origin = 0x3F7FF6,    length = 0x000002     /* Part of FLASHA.  Used for "boot to Flash" bootloader mode. */
    PSWD        : origin = 0x3F7FF8,    length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */

    FPUTABLES   : origin = 0x3FD860,    length = 0x0006A0	  /* FPU Tables in Boot ROM */
    IQTABLES    : origin = 0x3FDF00,    length = 0x000B50     /* IQ Math Tables in Boot ROM */
    IQTABLES2   : origin = 0x3FEA50,    length = 0x00008C     /* IQ Math Tables in Boot ROM */
    IQTABLES3   : origin = 0x3FEADC,    length = 0x0000AA	  /* IQ Math Tables in Boot ROM */

    BOOTROM     : origin = 0x3FF3B0,    length = 0x000C10     /* Boot ROM */
    RESET       : origin = 0x3FFFC0,    length = 0x000002     /* part of boot ROM  */
    VECTORS     : origin = 0x3FFFC2,    length = 0x00003E     /* part of boot ROM  */

PAGE 1 :
    D_M0_S      : origin = 0x000002,    length = 0x00004E     /* Part of M0, BOOT rom will use this for stack */
    D_M0        : origin = 0x000050,    length = 0x0003B0     /* on-chip RAM block M0 */
    D_M1        : origin = 0x000400,    length = 0x000400     /* on-chip RAM block M1 */

    D_L06       : origin = 0x008000,    length = 0x008000     /* on-chip RAM block L0 */

    D_OTP       : origin = 0x3D7800,    length = 0x0003FA     /* on-chip OTP */

    D_FH        : origin = 0x3D8000,    length = 0x004000     /* on-chip FLASH */
    D_FG        : origin = 0x3DC000,    length = 0x004000     /* on-chip FLASH */
    D_FF        : origin = 0x3E0000,    length = 0x004000     /* on-chip FLASH */
    D_FE        : origin = 0x3E4000,    length = 0x004000     /* on-chip FLASH */   
    D_FD        : origin = 0x3E8000,    length = 0x004000     /* on-chip FLASH */
    D_FC        : origin = 0x3EC000,    length = 0x004000     /* on-chip FLASH */
    D_FB        : origin = 0x3F0000,    length = 0x004000     /* on-chip FLASH */
    D_FA        : origin = 0x3F4000,    length = 0x003F80     /* on-chip FLASH */
}

SECTIONS
{
/* VARIABLES */
    .bss: >         D_L06,           PAGE = 1
    .ebss: >        D_L06,           PAGE = 1

/* CODE */
    .text: >        P_L78,           PAGE = 0

    ramfuncs:       LOAD = P_L78,
                    LOAD_START(_RamfuncsLoadStart),
                    LOAD_END(_RamfuncsLoadEnd),
                    RUN_START(_RamfuncsRunStart),
                    PAGE = 0

/* IQ MATH FUNCTIONS */
    IQmath:         LOAD = P_L78,
                    LOAD_START(_IQfuncsLoadStart),
                    LOAD_END(_IQfuncsLoadEnd),
                    RUN_START(_IQfuncsRunStart),
                    PAGE = 0

/* IQ MATH TABLES */
    IQmathTables: > IQTABLES,       PAGE = 0, type = NOLOAD
  
    IQmathTables2: >IQTABLES2,      PAGE = 0, TYPE = NOLOAD
    IQmathTables3: >IQTABLES3,      PAGE = 0, TYPE = NOLOAD
    FPUmathTables: >FPUTABLES,      PAGE = 0, TYPE = NOLOAD

/* STACK */
    .stack: >       D_M0,
                    RUN_START(_stack_start),
                    RUN_END(_stack_end),       
                    PAGE = 1
                    
/* HEAP */  
    .sysmem: >      D_L06,           PAGE = 1
    .esysmem: >     D_L06,           PAGE = 1

/* CONSTANTS */
    .econst: >      D_L06,           PAGE = 1
    .const: >       D_L06,           PAGE = 1

/* STARTUP CODE */
    codestart >     RESET_M         PAGE = 0
    .reset: >       P_L78,           PAGE = 0
    .cinit: >       P_L78,           PAGE = 0

/* COMPILER GENERATED CODE */
    .pinit: >       P_L78,           PAGE = 0
    .switch: >      P_L78,           PAGE = 0

/* PASSWORDS */     
    csmpasswds: >   PSWD,           PAGE = 0, type = NOLOAD
    csm_rsvd: >     CSM_RSVD,       PAGE = 0, type = NOLOAD

/* BOOT ROM INTERRUPT VECTORS */
    vectors: >      VECTORS,        PAGE = 0, TYPE = DSECT
}
