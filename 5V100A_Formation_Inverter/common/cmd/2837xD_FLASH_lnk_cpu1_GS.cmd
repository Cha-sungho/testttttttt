
MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN           	: origin = 0x080000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAMD0           	: origin = 0x00B000, length = 0x000800
   RAMLS012          	: origin = 0x008000, length = 0x001800
//   RAMLS1          	: origin = 0x008800, length = 0x000800
//   RAMLS2      		: origin = 0x009000, length = 0x000800
   RAMLS3      		: origin = 0x009800, length = 0x000800
   RAMLS4      		: origin = 0x00A000, length = 0x000800
//   RAMGS8      : origin = 0x014000, length = 0x001000
//   RAMGS14          : origin = 0x01A000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS15          : origin = 0x01B000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RESET           	: origin = 0x3FFFC0, length = 0x000002

   /* Flash sectors */
   FLASHA           : origin = 0x080002, length = 0x001FFE	/* on-chip Flash */
   FLASHB           : origin = 0x082000, length = 0x002000	/* on-chip Flash */
   FLASHC           : origin = 0x084000, length = 0x002000	/* on-chip Flash */
   FLASHD           : origin = 0x086000, length = 0x002000	/* on-chip Flash */
   FLASHE           : origin = 0x088000, length = 0x008000	/* on-chip Flash */
   FLASHF           : origin = 0x090000, length = 0x008000	/* on-chip Flash */
   FLASHG           : origin = 0x098000, length = 0x008000	/* on-chip Flash */
   FLASHH           : origin = 0x0A0000, length = 0x008000	/* on-chip Flash */
   FLASHI           : origin = 0x0A8000, length = 0x008000	/* on-chip Flash */
   FLASHJ           : origin = 0x0B0000, length = 0x008000	/* on-chip Flash */
   FLASHK           : origin = 0x0B8000, length = 0x002000	/* on-chip Flash */
   FLASHL           : origin = 0x0BA000, length = 0x002000	/* on-chip Flash */
   FLASHM           : origin = 0x0BC000, length = 0x002000	/* on-chip Flash */
   FLASHN           : origin = 0x0BE000, length = 0x002000	/* on-chip Flash */

PAGE 1 : /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS5      : origin = 0x00A800, length = 0x000800

   RAMGS0_5      : origin = 0x00C000, length = 0x06000

 //  RAMGS0      : origin = 0x00C000, length = 0x001000
 //  RAMGS1      : origin = 0x00D000, length = 0x001000
//   RAMGS2      : origin = 0x00E000, length = 0x001000
//   RAMGS3      : origin = 0x00F000, length = 0x001000
//   RAMGS4      : origin = 0x010000, length = 0x001000
//   RAMGS5      : origin = 0x011000, length = 0x001000

  //   RAMGS5     : origin = 0x011000, length = 0x00FFF

   RAMGS6_8          : origin = 0x012000,   length = 0x002FF0
   RAMGS_ref_DIS	 : origin = 0x014FFF	length = 0x000001

//   RAMGS6      : origin = 0x012000, length = 0x001000		//ramgs 11
//   RAMGS7      : origin = 0x013000, length = 0x001000
//   RAMGS8      : origin = 0x014000, length = 0x001000

//   RAMGS9_14      : origin = 0x015000, length = 0x006000

  // RMMGS_GS5     : origin = 0x11000, length = 0x0FFF

   RMMGS_STEP	 :	origin = 0x15000,	length = 0x3500
   RMMGS_Flag2	 :	origin = 0x18500,	length = 0x10
   RAMGS_RXBUF	 :  origin = 0x18510,	length = 0x1000
   RAMGS_TXBUF	 :  origin = 0x19510,	length = 0x1000
   RAMGS_EN_FLAG :	origin = 0x1A510,	length = 0x1

   RAMGS_pause_flag    : origin = 0x1A511 ,length = 0x1
   RAMGS_can_data : origin = 0x1A512, length = 0x8

   RAMGS_dbg_data  : origin = 0x1A51A, length = 0x1E
   RAMGS_step_index : origin = 0x1A538, length = 0x1


//   RAMGS9      : origin = 0x015000, length = 0x001000
//   RAMGS10     : origin = 0x016000, length = 0x001000
//   RAMGS11     : origin = 0x017000, length = 0x001000
//   RAMGS12     : origin = 0x018000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
//   RAMGS13     : origin = 0x019000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
//   RAMGS14     : origin = 0x01A000, length = 0x001000
//   														     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
//   RAMGS15_LOG_EN	 : origin = 0x01BFFF,   length = 0x01C000

  //  RAMGS15      : origin = 0x01B000, length = 0xFFF

     RAMGS15         : origin = 0x01B000,   length = 0x1
     RAMGS15_2       : origin = 0x01B001,   length = 0x1
     RAMGS15_3       : origin = 0x01B002,   length = 0x1
     RAMGS15_4       : origin = 0x01B003,   length = 0x1
     RAMGS15_5       : origin = 0x01B004,   length = 0x1
     RAMGS15_6       : origin = 0x01B005,   length = 0x1
     RAMGS15_7       : origin = 0x01B006,   length = 0x1
     RAMGS15_8       : origin = 0x01B007,   length = 0x1
     RAMGS15_9       : origin = 0x01B008,   length = 0x1
     RAMGS15_10      : origin = 0x01B009,   length = 0x1
	 RAMGS15_11      : origin = 0x01B00A,   length = 0x1
	 RAMGS15_12      : origin = 0x01B00B,   length = 0x1
 RAMGS15_13      : origin = 0x01B00C,   length = 0x1
    RAMGS15_14   : origin = 0x01B00D,   length = 0x1
    RAMGS15_15   : origin = 0x01B00E,   length = 0x1
   RAMGS15_16      : origin = 0x01B00F,   length = 0x4
   RAMGS15_17      : origin = 0x01B013,   length = 0x8
   
    


   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
}

SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > FLASHB,      PAGE = 0, ALIGN(4)
   .pinit              : > FLASHB,      PAGE = 0, ALIGN(4)
   .text               : >> FLASHE | FLASHF,      PAGE = 0, ALIGN(4)
   codestart           : > BEGIN,       PAGE = 0, ALIGN(4)

   /* Allocate uninitalized data sections: */

   .stack              : > RAMM1,        PAGE = 1
   .ebss               : > RAMGS0_5,      PAGE = 1
   .esysmem            : > RAMLS5,       PAGE = 1

   /* Initalized sections go in Flash */
   .econst             : > FLASHC,      PAGE = 0, ALIGN(4)
   .switch             : > FLASHC,      PAGE = 0, ALIGN(4)

   .reset              : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

//   Filter_RegsFile     : > RAMGS0,	   PAGE = 1

//   SHARERAMGS0		: > RAMGS0,		PAGE = 1
//   SHARERAMGS1		: > RAMGS1,		PAGE = 1

//   SHARERAMGS12		: > RAMGS12,		PAGE = 1
//   SHARERAMGS13		: > RAMGS13,		PAGE = 1
//   SHARERAMGS14		: > RAMGS14,		PAGE = 0
//   SHARERAMGS15		: > RAMGS15,		PAGE = 0

   SHARERAMGS6	   : >  RAMGS6_8, 				    		PAGE = 1

   RAMGS_ref_DIS	   : > 	RAMGS_ref_DIS						PAGE = 1
//	 SHARERAMGS9	   : > 	RAMGS9_14							PAGE = 1


   // SHARER_GS5_RAM  :>   RMMGS_GS5 , PAGE = 1

	//SHARER_GS15_RAM   :> RAMGS15     ,PAGE = 1 

		SHARER_GS15_RAM   :> RAMGS15 ,          PAGE = 1
		SHARER_GS15_RAM_2   :> RAMGS15_2 ,          PAGE = 1
		SHARER_GS15_RAM_3   :> RAMGS15_3 ,          PAGE = 1
		SHARER_GS15_RAM_4   :> RAMGS15_4 ,          PAGE = 1
		SHARER_GS15_RAM_5   :> RAMGS15_5 ,          PAGE = 1
		SHARER_GS15_RAM_6   :> RAMGS15_6 ,          PAGE = 1
		SHARER_GS15_RAM_7   :> RAMGS15_7 ,          PAGE = 1
		SHARER_GS15_RAM_8   :> RAMGS15_8 ,          PAGE = 1
		SHARER_GS15_RAM_9   :> RAMGS15_9 ,          PAGE = 1
		SHARER_GS15_RAM_10  :> RAMGS15_10,          PAGE = 1
		SHARER_GS15_RAM_11  :> RAMGS15_11,          PAGE = 1
		SHARER_GS15_RAM_12  :> RAMGS15_12,          PAGE = 1
		SHARER_GS15_RAM_13  :> RAMGS15_13,          PAGE = 1
		SHARER_GS15_RAM_14  :> RAMGS15_14,          PAGE = 1
           
		SHARER_GS15_RAM_15 :>  RAMGS15_15,          PAGE = 1

		SHARER_GS15_RAM_16  :> RAMGS15_16,          PAGE = 1
		SHARER_GS15_RAM_17  :> RAMGS15_17,          PAGE = 1






	SHARER_STEP		:>   RMMGS_STEP,							PAGE = 1
	SHARER_FLAG2	:>   RMMGS_Flag2,	 						PAGE = 1
	SHARER_RXBUF	:>   RAMGS_RXBUF,	 						PAGE = 1
	SHARER_TXBUF	:>   RAMGS_TXBUF,							PAGE = 1
	SHARER_ENFLAG	:>   RAMGS_EN_FLAG ,						PAGE = 1

	SHARER_PAUSE_FLAG:> RAMGS_pause_flag PAGE = 1
	SHARER_CAN_DATA :> RAMGS_can_data,PAGE = 1
	SHARER_STEP_INDEX   :> RAMGS_step_index, PAGE = 1

            SHARER_DBG_DATA :>  RAMGS_dbg_data, PAGE = 1

//     SHARERAMGS15	   : >  RAMGS15,		 				    PAGE = 1
//   SHARERAMGS15_LOG_EN : > RAMGS15_LOG_EN,					PAGE = 1

//   ramgs0           : > RAMGS0,     PAGE = 1
//   ramgs1           : > RAMGS1,     PAGE = 1

#ifdef __TI_COMPILER_VERSION__
	#if __TI_COMPILER_VERSION__ >= 15009000
	.TI.ramfunc : {} LOAD = FLASHD,
						 RUN = RAMLS012 | RAMLS3 | RAMLS4,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
						 PAGE = 0, ALIGN(4)
	#else
   ramfuncs            : LOAD = FLASHD,
                         RUN = RAMLS012 |RAMLS3 | RAMLS4,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)
    #endif
#endif

   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }

   /* The following section definition are for SDFM examples */
//   Filter1_RegsFile : > RAMGS1,	PAGE = 1, fill=0x1111
//   Filter2_RegsFile : > RAMGS2,	PAGE = 1, fill=0x2222
//   Filter3_RegsFile : > RAMGS3,	PAGE = 1, fill=0x3333
//   Filter4_RegsFile : > RAMGS4,	PAGE = 1, fill=0x4444
//   Difference_RegsFile : >RAMGS5, 	PAGE = 1, fill=0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
