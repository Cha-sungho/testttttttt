
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN           	: origin = 0x000000, length = 0x000002
   RAMM0           	: origin = 0x000080, length = 0x000380
   RAMD0           	: origin = 0x00B000, length = 0x000800
   RAMLS0          : origin = 0x008000, length = 0x000800
   RAMLS1          	: origin = 0x008800, length = 0x000800
   RAMLS2      		: origin = 0x009000, length = 0x000800
   RAMLS3      		: origin = 0x009800, length = 0x000800
   RAMLS4      		: origin = 0x00A000, length = 0x000800
   RESET           : origin = 0x3FFFC0, length = 0x000002
   RAMGS0      : origin = 0x00C000, length = 0x001000
   RAMGS1      : origin = 0x00D000, length = 0x001000
   RAMGS2      : origin = 0x00E000, length = 0x001000
   RAMGS3_code      : origin = 0x00F000, length = 0x000A00
   RAMGS4      : origin = 0x010000, length = 0x001000
   RAMGS5      : origin = 0x011000, length = 0x001000
   RAMGS6      : origin = 0x012000, length = 0x001000

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x00007E     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x0003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
   RAMD1           : origin = 0x00B800, length = 0x000800
   
   RAMLS5      : origin = 0x00A800, length = 0x000800

   RAMGS3_data	: origin = 0x00FA00, length = 0x500

   RAMGS7      : origin = 0x013000, length = 0x001000
   RAMGS8      : origin = 0x014000, length = 0x001000
   RAMGS9      : origin = 0x015000, length = 0x001000
   RAMGS10     : origin = 0x016000, length = 0x001000     

//   RAMGS11     : origin = 0x017000, length = 0x000FF8   /* Uncomment for F28374D, F28376D devices */

//   RAMGS11_RSVD : origin = 0x017FF8, length = 0x000008    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

   RAMGS11     : origin = 0x017000, length = 0x001000     /* Only Available on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */
   RAMGS12     : origin = 0x018000, length = 0x001000     /* Only Available on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */
   RAMGS13     : origin = 0x019000, length = 0x001000     /* Only Available on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */
   RAMGS14     : origin = 0x01A000, length = 0x001000     /* Only Available on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */
   RAMGS15     : origin = 0x01B000, length = 0x000FF8     /* Only Available on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */
   
//   RAMGS15_RSVD : origin = 0x01BFF8, length = 0x000008    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
                                                            /* Only on F28379D/_, F28377D/F28377S, F28375D/F28375S devices. Remove line on other devices. */

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
   
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : >> RAMGS2 | RAMGS3_code,   PAGE = 0
   .cinit           : > RAMM0,     PAGE = 0
   .pinit           : > RAMM0,     PAGE = 0
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   .stack           : > RAMM1,     PAGE = 1
   .ebss            : > RAMGS3_data,     PAGE = 1
   .econst          : > RAMGS3_data,     PAGE = 1
   .esysmem         : > RAMGS3_data,     PAGE = 1
   Filter_RegsFile  : > RAMGS8,	   PAGE = 1

   ramgs0           : > RAMGS3_data,    PAGE = 1
   ramgs1           : > RAMGS9,    PAGE = 1
   
#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000
       .TI.ramfunc      : > RAMD0      PAGE = 0
    #else
       ramfuncs         : > RAMD0      PAGE = 0
    #endif
#endif  
   
   /* The following section definitions are required when using the IPC API Drivers */ 
    GROUP : > CPU2TOCPU1RAM, PAGE = 1 
    {
        PUTBUFFER 
        PUTWRITEIDX 
        GETREADIDX 
    }
    
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }  


    /* The following section definition are for SDFM examples */
   Filter1_RegsFile : > RAMGS10,	PAGE = 1, fill=0x1111
   Filter2_RegsFile : > RAMGS11,	PAGE = 1, fill=0x2222
   Filter3_RegsFile : > RAMGS12,	PAGE = 1, fill=0x3333
   Filter4_RegsFile : > RAMGS13,	PAGE = 1, fill=0x4444
   Difference_RegsFile : >RAMGS14, 	PAGE = 1, fill=0x3333

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
