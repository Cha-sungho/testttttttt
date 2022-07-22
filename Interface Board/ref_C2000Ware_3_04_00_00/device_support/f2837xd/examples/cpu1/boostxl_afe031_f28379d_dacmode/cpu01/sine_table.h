//#############################################################################
//
// FILE:   sine_table.h
//
// TITLE:  SINE Table
//
//###########################################################################
// $TI Release: F2837xD Support Library v3.12.00.00 $
// $Release Date: Fri Feb 12 19:03:23 IST 2021 $
// $Copyright:
// Copyright (C) 2013-2021 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

#ifndef SINE_TABLE_H_
#define SINE_TABLE_H_

#pragma DATA_SECTION(sineTable, "SINETABLE");

const Uint16 sineTable[] = {
0x200,0x200,0x201,0x202,0x203,0x203,0x204,0x205,0x206,0x207,0x207,0x208,0x209,0x20a,0x20a,0x20b,0x20c,0x20d,0x20e,0x20e,
0x20f,0x210,0x211,0x212,0x212,0x213,0x214,0x215,0x215,0x216,0x217,0x218,0x219,0x219,0x21a,0x21b,0x21c,0x21d,0x21d,0x21e,
0x21f,0x220,0x220,0x221,0x222,0x223,0x224,0x224,0x225,0x226,0x227,0x227,0x228,0x229,0x22a,0x22b,0x22b,0x22c,0x22d,0x22e,
0x22f,0x22f,0x230,0x231,0x232,0x232,0x233,0x234,0x235,0x236,0x236,0x237,0x238,0x239,0x239,0x23a,0x23b,0x23c,0x23d,0x23d,
0x23e,0x23f,0x240,0x240,0x241,0x242,0x243,0x244,0x244,0x245,0x246,0x247,0x247,0x248,0x249,0x24a,0x24b,0x24b,0x24c,0x24d,
0x24e,0x24e,0x24f,0x250,0x251,0x252,0x252,0x253,0x254,0x255,0x255,0x256,0x257,0x258,0x258,0x259,0x25a,0x25b,0x25c,0x25c,
0x25d,0x25e,0x25f,0x25f,0x260,0x261,0x262,0x263,0x263,0x264,0x265,0x266,0x266,0x267,0x268,0x269,0x269,0x26a,0x26b,0x26c,
0x26d,0x26d,0x26e,0x26f,0x270,0x270,0x271,0x272,0x273,0x273,0x274,0x275,0x276,0x276,0x277,0x278,0x279,0x279,0x27a,0x27b,
0x27c,0x27d,0x27d,0x27e,0x27f,0x280,0x280,0x281,0x282,0x283,0x283,0x284,0x285,0x286,0x286,0x287,0x288,0x289,0x289,0x28a,
0x28b,0x28c,0x28c,0x28d,0x28e,0x28f,0x28f,0x290,0x291,0x292,0x292,0x293,0x294,0x295,0x295,0x296,0x297,0x298,0x298,0x299,
0x29a,0x29b,0x29b,0x29c,0x29d,0x29e,0x29e,0x29f,0x2a0,0x2a1,0x2a1,0x2a2,0x2a3,0x2a4,0x2a4,0x2a5,0x2a6,0x2a7,0x2a7,0x2a8,
0x2a9,0x2aa,0x2aa,0x2ab,0x2ac,0x2ad,0x2ad,0x2ae,0x2af,0x2b0,0x2b0,0x2b1,0x2b2,0x2b2,0x2b3,0x2b4,0x2b5,0x2b5,0x2b6,0x2b7,
0x2b8,0x2b8,0x2b9,0x2ba,0x2bb,0x2bb,0x2bc,0x2bd,0x2bd,0x2be,0x2bf,0x2c0,0x2c0,0x2c1,0x2c2,0x2c3,0x2c3,0x2c4,0x2c5,0x2c5,
0x2c6,0x2c7,0x2c8,0x2c8,0x2c9,0x2ca,0x2ca,0x2cb,0x2cc,0x2cd,0x2cd,0x2ce,0x2cf,0x2cf,0x2d0,0x2d1,0x2d2,0x2d2,0x2d3,0x2d4,
0x2d5,0x2d5,0x2d6,0x2d7,0x2d7,0x2d8,0x2d9,0x2d9,0x2da,0x2db,0x2dc,0x2dc,0x2dd,0x2de,0x2de,0x2df,0x2e0,0x2e1,0x2e1,0x2e2,
0x2e3,0x2e3,0x2e4,0x2e5,0x2e5,0x2e6,0x2e7,0x2e8,0x2e8,0x2e9,0x2ea,0x2ea,0x2eb,0x2ec,0x2ec,0x2ed,0x2ee,0x2ef,0x2ef,0x2f0,
0x2f1,0x2f1,0x2f2,0x2f3,0x2f3,0x2f4,0x2f5,0x2f5,0x2f6,0x2f7,0x2f8,0x2f8,0x2f9,0x2fa,0x2fa,0x2fb,0x2fc,0x2fc,0x2fd,0x2fe,
0x2fe,0x2ff,0x300,0x300,0x301,0x302,0x302,0x303,0x304,0x304,0x305,0x306,0x306,0x307,0x308,0x308,0x309,0x30a,0x30a,0x30b,
0x30c,0x30c,0x30d,0x30e,0x30e,0x30f,0x310,0x310,0x311,0x312,0x312,0x313,0x314,0x314,0x315,0x316,0x316,0x317,0x318,0x318,
0x319,0x31a,0x31a,0x31b,0x31c,0x31c,0x31d,0x31e,0x31e,0x31f,0x320,0x320,0x321,0x322,0x322,0x323,0x323,0x324,0x325,0x325,
0x326,0x327,0x327,0x328,0x329,0x329,0x32a,0x32a,0x32b,0x32c,0x32c,0x32d,0x32e,0x32e,0x32f,0x330,0x330,0x331,0x331,0x332,
0x333,0x333,0x334,0x335,0x335,0x336,0x336,0x337,0x338,0x338,0x339,0x33a,0x33a,0x33b,0x33b,0x33c,0x33d,0x33d,0x33e,0x33f,
0x33f,0x340,0x340,0x341,0x342,0x342,0x343,0x343,0x344,0x345,0x345,0x346,0x346,0x347,0x348,0x348,0x349,0x349,0x34a,0x34b,
0x34b,0x34c,0x34c,0x34d,0x34e,0x34e,0x34f,0x34f,0x350,0x351,0x351,0x352,0x352,0x353,0x353,0x354,0x355,0x355,0x356,0x356,
0x357,0x358,0x358,0x359,0x359,0x35a,0x35a,0x35b,0x35c,0x35c,0x35d,0x35d,0x35e,0x35e,0x35f,0x360,0x360,0x361,0x361,0x362,
0x362,0x363,0x364,0x364,0x365,0x365,0x366,0x366,0x367,0x368,0x368,0x369,0x369,0x36a,0x36a,0x36b,0x36b,0x36c,0x36c,0x36d,
0x36e,0x36e,0x36f,0x36f,0x370,0x370,0x371,0x371,0x372,0x372,0x373,0x374,0x374,0x375,0x375,0x376,0x376,0x377,0x377,0x378,
0x378,0x379,0x379,0x37a,0x37a,0x37b,0x37c,0x37c,0x37d,0x37d,0x37e,0x37e,0x37f,0x37f,0x380,0x380,0x381,0x381,0x382,0x382,
0x383,0x383,0x384,0x384,0x385,0x385,0x386,0x386,0x387,0x387,0x388,0x388,0x389,0x389,0x38a,0x38a,0x38b,0x38b,0x38c,0x38c,
0x38d,0x38d,0x38e,0x38e,0x38f,0x38f,0x390,0x390,0x391,0x391,0x392,0x392,0x393,0x393,0x394,0x394,0x395,0x395,0x396,0x396,
0x397,0x397,0x398,0x398,0x398,0x399,0x399,0x39a,0x39a,0x39b,0x39b,0x39c,0x39c,0x39d,0x39d,0x39e,0x39e,0x39f,0x39f,0x39f,
0x3a0,0x3a0,0x3a1,0x3a1,0x3a2,0x3a2,0x3a3,0x3a3,0x3a3,0x3a4,0x3a4,0x3a5,0x3a5,0x3a6,0x3a6,0x3a7,0x3a7,0x3a7,0x3a8,0x3a8,
0x3a9,0x3a9,0x3aa,0x3aa,0x3ab,0x3ab,0x3ab,0x3ac,0x3ac,0x3ad,0x3ad,0x3ae,0x3ae,0x3ae,0x3af,0x3af,0x3b0,0x3b0,0x3b0,0x3b1,
0x3b1,0x3b2,0x3b2,0x3b3,0x3b3,0x3b3,0x3b4,0x3b4,0x3b5,0x3b5,0x3b5,0x3b6,0x3b6,0x3b7,0x3b7,0x3b7,0x3b8,0x3b8,0x3b9,0x3b9,
0x3b9,0x3ba,0x3ba,0x3bb,0x3bb,0x3bb,0x3bc,0x3bc,0x3bd,0x3bd,0x3bd,0x3be,0x3be,0x3be,0x3bf,0x3bf,0x3c0,0x3c0,0x3c0,0x3c1,
0x3c1,0x3c1,0x3c2,0x3c2,0x3c3,0x3c3,0x3c3,0x3c4,0x3c4,0x3c4,0x3c5,0x3c5,0x3c6,0x3c6,0x3c6,0x3c7,0x3c7,0x3c7,0x3c8,0x3c8,
0x3c8,0x3c9,0x3c9,0x3c9,0x3ca,0x3ca,0x3ca,0x3cb,0x3cb,0x3cc,0x3cc,0x3cc,0x3cd,0x3cd,0x3cd,0x3ce,0x3ce,0x3ce,0x3cf,0x3cf,
0x3cf,0x3d0,0x3d0,0x3d0,0x3d1,0x3d1,0x3d1,0x3d2,0x3d2,0x3d2,0x3d2,0x3d3,0x3d3,0x3d3,0x3d4,0x3d4,0x3d4,0x3d5,0x3d5,0x3d5,
0x3d6,0x3d6,0x3d6,0x3d7,0x3d7,0x3d7,0x3d7,0x3d8,0x3d8,0x3d8,0x3d9,0x3d9,0x3d9,0x3da,0x3da,0x3da,0x3da,0x3db,0x3db,0x3db,
0x3dc,0x3dc,0x3dc,0x3dc,0x3dd,0x3dd,0x3dd,0x3de,0x3de,0x3de,0x3de,0x3df,0x3df,0x3df,0x3df,0x3e0,0x3e0,0x3e0,0x3e1,0x3e1,
0x3e1,0x3e1,0x3e2,0x3e2,0x3e2,0x3e2,0x3e3,0x3e3,0x3e3,0x3e3,0x3e4,0x3e4,0x3e4,0x3e4,0x3e5,0x3e5,0x3e5,0x3e5,0x3e6,0x3e6,
0x3e6,0x3e6,0x3e7,0x3e7,0x3e7,0x3e7,0x3e8,0x3e8,0x3e8,0x3e8,0x3e9,0x3e9,0x3e9,0x3e9,0x3e9,0x3ea,0x3ea,0x3ea,0x3ea,0x3eb,
0x3eb,0x3eb,0x3eb,0x3eb,0x3ec,0x3ec,0x3ec,0x3ec,0x3ec,0x3ed,0x3ed,0x3ed,0x3ed,0x3ee,0x3ee,0x3ee,0x3ee,0x3ee,0x3ef,0x3ef,
0x3ef,0x3ef,0x3ef,0x3ef,0x3f0,0x3f0,0x3f0,0x3f0,0x3f0,0x3f1,0x3f1,0x3f1,0x3f1,0x3f1,0x3f2,0x3f2,0x3f2,0x3f2,0x3f2,0x3f2,
0x3f3,0x3f3,0x3f3,0x3f3,0x3f3,0x3f3,0x3f4,0x3f4,0x3f4,0x3f4,0x3f4,0x3f4,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f6,
0x3f6,0x3f6,0x3f6,0x3f6,0x3f6,0x3f7,0x3f7,0x3f7,0x3f7,0x3f7,0x3f7,0x3f7,0x3f7,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,
0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fb,0x3fb,
0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,
0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,
0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3ff,0x3ff,0x3ff,0x3ff,
0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,
0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,
0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3ff,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,
0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fe,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,
0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fd,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,0x3fc,
0x3fc,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fb,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,0x3fa,
0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f9,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,0x3f8,0x3f7,0x3f7,0x3f7,0x3f7,
0x3f7,0x3f7,0x3f7,0x3f7,0x3f6,0x3f6,0x3f6,0x3f6,0x3f6,0x3f6,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f5,0x3f4,0x3f4,0x3f4,
0x3f4,0x3f4,0x3f4,0x3f3,0x3f3,0x3f3,0x3f3,0x3f3,0x3f3,0x3f2,0x3f2,0x3f2,0x3f2,0x3f2,0x3f2,0x3f1,0x3f1,0x3f1,0x3f1,0x3f1,
0x3f0,0x3f0,0x3f0,0x3f0,0x3f0,0x3ef,0x3ef,0x3ef,0x3ef,0x3ef,0x3ef,0x3ee,0x3ee,0x3ee,0x3ee,0x3ee,0x3ed,0x3ed,0x3ed,0x3ed,
0x3ec,0x3ec,0x3ec,0x3ec,0x3ec,0x3eb,0x3eb,0x3eb,0x3eb,0x3eb,0x3ea,0x3ea,0x3ea,0x3ea,0x3e9,0x3e9,0x3e9,0x3e9,0x3e9,0x3e8,
0x3e8,0x3e8,0x3e8,0x3e7,0x3e7,0x3e7,0x3e7,0x3e6,0x3e6,0x3e6,0x3e6,0x3e5,0x3e5,0x3e5,0x3e5,0x3e4,0x3e4,0x3e4,0x3e4,0x3e3,
0x3e3,0x3e3,0x3e3,0x3e2,0x3e2,0x3e2,0x3e2,0x3e1,0x3e1,0x3e1,0x3e1,0x3e0,0x3e0,0x3e0,0x3df,0x3df,0x3df,0x3df,0x3de,0x3de,
0x3de,0x3de,0x3dd,0x3dd,0x3dd,0x3dc,0x3dc,0x3dc,0x3dc,0x3db,0x3db,0x3db,0x3da,0x3da,0x3da,0x3da,0x3d9,0x3d9,0x3d9,0x3d8,
0x3d8,0x3d8,0x3d7,0x3d7,0x3d7,0x3d7,0x3d6,0x3d6,0x3d6,0x3d5,0x3d5,0x3d5,0x3d4,0x3d4,0x3d4,0x3d3,0x3d3,0x3d3,0x3d2,0x3d2,
0x3d2,0x3d2,0x3d1,0x3d1,0x3d1,0x3d0,0x3d0,0x3d0,0x3cf,0x3cf,0x3cf,0x3ce,0x3ce,0x3ce,0x3cd,0x3cd,0x3cd,0x3cc,0x3cc,0x3cc,
0x3cb,0x3cb,0x3ca,0x3ca,0x3ca,0x3c9,0x3c9,0x3c9,0x3c8,0x3c8,0x3c8,0x3c7,0x3c7,0x3c7,0x3c6,0x3c6,0x3c6,0x3c5,0x3c5,0x3c4,
0x3c4,0x3c4,0x3c3,0x3c3,0x3c3,0x3c2,0x3c2,0x3c1,0x3c1,0x3c1,0x3c0,0x3c0,0x3c0,0x3bf,0x3bf,0x3be,0x3be,0x3be,0x3bd,0x3bd,
0x3bd,0x3bc,0x3bc,0x3bb,0x3bb,0x3bb,0x3ba,0x3ba,0x3b9,0x3b9,0x3b9,0x3b8,0x3b8,0x3b7,0x3b7,0x3b7,0x3b6,0x3b6,0x3b5,0x3b5,
0x3b5,0x3b4,0x3b4,0x3b3,0x3b3,0x3b3,0x3b2,0x3b2,0x3b1,0x3b1,0x3b0,0x3b0,0x3b0,0x3af,0x3af,0x3ae,0x3ae,0x3ae,0x3ad,0x3ad,
0x3ac,0x3ac,0x3ab,0x3ab,0x3ab,0x3aa,0x3aa,0x3a9,0x3a9,0x3a8,0x3a8,0x3a7,0x3a7,0x3a7,0x3a6,0x3a6,0x3a5,0x3a5,0x3a4,0x3a4,
0x3a3,0x3a3,0x3a3,0x3a2,0x3a2,0x3a1,0x3a1,0x3a0,0x3a0,0x39f,0x39f,0x39f,0x39e,0x39e,0x39d,0x39d,0x39c,0x39c,0x39b,0x39b,
0x39a,0x39a,0x399,0x399,0x398,0x398,0x398,0x397,0x397,0x396,0x396,0x395,0x395,0x394,0x394,0x393,0x393,0x392,0x392,0x391,
0x391,0x390,0x390,0x38f,0x38f,0x38e,0x38e,0x38d,0x38d,0x38c,0x38c,0x38b,0x38b,0x38a,0x38a,0x389,0x389,0x388,0x388,0x387,
0x387,0x386,0x386,0x385,0x385,0x384,0x384,0x383,0x383,0x382,0x382,0x381,0x381,0x380,0x380,0x37f,0x37f,0x37e,0x37e,0x37d,
0x37d,0x37c,0x37c,0x37b,0x37a,0x37a,0x379,0x379,0x378,0x378,0x377,0x377,0x376,0x376,0x375,0x375,0x374,0x374,0x373,0x372,
0x372,0x371,0x371,0x370,0x370,0x36f,0x36f,0x36e,0x36e,0x36d,0x36c,0x36c,0x36b,0x36b,0x36a,0x36a,0x369,0x369,0x368,0x368,
0x367,0x366,0x366,0x365,0x365,0x364,0x364,0x363,0x362,0x362,0x361,0x361,0x360,0x360,0x35f,0x35e,0x35e,0x35d,0x35d,0x35c,
0x35c,0x35b,0x35a,0x35a,0x359,0x359,0x358,0x358,0x357,0x356,0x356,0x355,0x355,0x354,0x353,0x353,0x352,0x352,0x351,0x351,
0x350,0x34f,0x34f,0x34e,0x34e,0x34d,0x34c,0x34c,0x34b,0x34b,0x34a,0x349,0x349,0x348,0x348,0x347,0x346,0x346,0x345,0x345,
0x344,0x343,0x343,0x342,0x342,0x341,0x340,0x340,0x33f,0x33f,0x33e,0x33d,0x33d,0x33c,0x33b,0x33b,0x33a,0x33a,0x339,0x338,
0x338,0x337,0x336,0x336,0x335,0x335,0x334,0x333,0x333,0x332,0x331,0x331,0x330,0x330,0x32f,0x32e,0x32e,0x32d,0x32c,0x32c,
0x32b,0x32a,0x32a,0x329,0x329,0x328,0x327,0x327,0x326,0x325,0x325,0x324,0x323,0x323,0x322,0x322,0x321,0x320,0x320,0x31f,
0x31e,0x31e,0x31d,0x31c,0x31c,0x31b,0x31a,0x31a,0x319,0x318,0x318,0x317,0x316,0x316,0x315,0x314,0x314,0x313,0x312,0x312,
0x311,0x310,0x310,0x30f,0x30e,0x30e,0x30d,0x30c,0x30c,0x30b,0x30a,0x30a,0x309,0x308,0x308,0x307,0x306,0x306,0x305,0x304,
0x304,0x303,0x302,0x302,0x301,0x300,0x300,0x2ff,0x2fe,0x2fe,0x2fd,0x2fc,0x2fc,0x2fb,0x2fa,0x2fa,0x2f9,0x2f8,0x2f8,0x2f7,
0x2f6,0x2f5,0x2f5,0x2f4,0x2f3,0x2f3,0x2f2,0x2f1,0x2f1,0x2f0,0x2ef,0x2ef,0x2ee,0x2ed,0x2ec,0x2ec,0x2eb,0x2ea,0x2ea,0x2e9,
0x2e8,0x2e8,0x2e7,0x2e6,0x2e5,0x2e5,0x2e4,0x2e3,0x2e3,0x2e2,0x2e1,0x2e1,0x2e0,0x2df,0x2de,0x2de,0x2dd,0x2dc,0x2dc,0x2db,
0x2da,0x2d9,0x2d9,0x2d8,0x2d7,0x2d7,0x2d6,0x2d5,0x2d5,0x2d4,0x2d3,0x2d2,0x2d2,0x2d1,0x2d0,0x2cf,0x2cf,0x2ce,0x2cd,0x2cd,
0x2cc,0x2cb,0x2ca,0x2ca,0x2c9,0x2c8,0x2c8,0x2c7,0x2c6,0x2c5,0x2c5,0x2c4,0x2c3,0x2c3,0x2c2,0x2c1,0x2c0,0x2c0,0x2bf,0x2be,
0x2bd,0x2bd,0x2bc,0x2bb,0x2bb,0x2ba,0x2b9,0x2b8,0x2b8,0x2b7,0x2b6,0x2b5,0x2b5,0x2b4,0x2b3,0x2b2,0x2b2,0x2b1,0x2b0,0x2b0,
0x2af,0x2ae,0x2ad,0x2ad,0x2ac,0x2ab,0x2aa,0x2aa,0x2a9,0x2a8,0x2a7,0x2a7,0x2a6,0x2a5,0x2a4,0x2a4,0x2a3,0x2a2,0x2a1,0x2a1,
0x2a0,0x29f,0x29e,0x29e,0x29d,0x29c,0x29b,0x29b,0x29a,0x299,0x298,0x298,0x297,0x296,0x295,0x295,0x294,0x293,0x292,0x292,
0x291,0x290,0x28f,0x28f,0x28e,0x28d,0x28c,0x28c,0x28b,0x28a,0x289,0x289,0x288,0x287,0x286,0x286,0x285,0x284,0x283,0x283,
0x282,0x281,0x280,0x280,0x27f,0x27e,0x27d,0x27d,0x27c,0x27b,0x27a,0x279,0x279,0x278,0x277,0x276,0x276,0x275,0x274,0x273,
0x273,0x272,0x271,0x270,0x270,0x26f,0x26e,0x26d,0x26d,0x26c,0x26b,0x26a,0x269,0x269,0x268,0x267,0x266,0x266,0x265,0x264,
0x263,0x263,0x262,0x261,0x260,0x25f,0x25f,0x25e,0x25d,0x25c,0x25c,0x25b,0x25a,0x259,0x258,0x258,0x257,0x256,0x255,0x255,
0x254,0x253,0x252,0x252,0x251,0x250,0x24f,0x24e,0x24e,0x24d,0x24c,0x24b,0x24b,0x24a,0x249,0x248,0x247,0x247,0x246,0x245,
0x244,0x244,0x243,0x242,0x241,0x240,0x240,0x23f,0x23e,0x23d,0x23d,0x23c,0x23b,0x23a,0x239,0x239,0x238,0x237,0x236,0x236,
0x235,0x234,0x233,0x232,0x232,0x231,0x230,0x22f,0x22f,0x22e,0x22d,0x22c,0x22b,0x22b,0x22a,0x229,0x228,0x227,0x227,0x226,
0x225,0x224,0x224,0x223,0x222,0x221,0x220,0x220,0x21f,0x21e,0x21d,0x21d,0x21c,0x21b,0x21a,0x219,0x219,0x218,0x217,0x216,
0x215,0x215,0x214,0x213,0x212,0x212,0x211,0x210,0x20f,0x20e,0x20e,0x20d,0x20c,0x20b,0x20a,0x20a,0x209,0x208,0x207,0x207,
0x206,0x205,0x204,0x203,0x203,0x202,0x201,0x200,0x200,0x1ff,0x1fe,0x1fd,0x1fc,0x1fc,0x1fb,0x1fa,0x1f9,0x1f8,0x1f8,0x1f7,
0x1f6,0x1f5,0x1f5,0x1f4,0x1f3,0x1f2,0x1f1,0x1f1,0x1f0,0x1ef,0x1ee,0x1ed,0x1ed,0x1ec,0x1eb,0x1ea,0x1ea,0x1e9,0x1e8,0x1e7,
0x1e6,0x1e6,0x1e5,0x1e4,0x1e3,0x1e2,0x1e2,0x1e1,0x1e0,0x1df,0x1df,0x1de,0x1dd,0x1dc,0x1db,0x1db,0x1da,0x1d9,0x1d8,0x1d8,
0x1d7,0x1d6,0x1d5,0x1d4,0x1d4,0x1d3,0x1d2,0x1d1,0x1d0,0x1d0,0x1cf,0x1ce,0x1cd,0x1cd,0x1cc,0x1cb,0x1ca,0x1c9,0x1c9,0x1c8,
0x1c7,0x1c6,0x1c6,0x1c5,0x1c4,0x1c3,0x1c2,0x1c2,0x1c1,0x1c0,0x1bf,0x1bf,0x1be,0x1bd,0x1bc,0x1bb,0x1bb,0x1ba,0x1b9,0x1b8,
0x1b8,0x1b7,0x1b6,0x1b5,0x1b4,0x1b4,0x1b3,0x1b2,0x1b1,0x1b1,0x1b0,0x1af,0x1ae,0x1ad,0x1ad,0x1ac,0x1ab,0x1aa,0x1aa,0x1a9,
0x1a8,0x1a7,0x1a7,0x1a6,0x1a5,0x1a4,0x1a3,0x1a3,0x1a2,0x1a1,0x1a0,0x1a0,0x19f,0x19e,0x19d,0x19c,0x19c,0x19b,0x19a,0x199,
0x199,0x198,0x197,0x196,0x196,0x195,0x194,0x193,0x192,0x192,0x191,0x190,0x18f,0x18f,0x18e,0x18d,0x18c,0x18c,0x18b,0x18a,
0x189,0x189,0x188,0x187,0x186,0x186,0x185,0x184,0x183,0x182,0x182,0x181,0x180,0x17f,0x17f,0x17e,0x17d,0x17c,0x17c,0x17b,
0x17a,0x179,0x179,0x178,0x177,0x176,0x176,0x175,0x174,0x173,0x173,0x172,0x171,0x170,0x170,0x16f,0x16e,0x16d,0x16d,0x16c,
0x16b,0x16a,0x16a,0x169,0x168,0x167,0x167,0x166,0x165,0x164,0x164,0x163,0x162,0x161,0x161,0x160,0x15f,0x15e,0x15e,0x15d,
0x15c,0x15b,0x15b,0x15a,0x159,0x158,0x158,0x157,0x156,0x155,0x155,0x154,0x153,0x152,0x152,0x151,0x150,0x14f,0x14f,0x14e,
0x14d,0x14d,0x14c,0x14b,0x14a,0x14a,0x149,0x148,0x147,0x147,0x146,0x145,0x144,0x144,0x143,0x142,0x142,0x141,0x140,0x13f,
0x13f,0x13e,0x13d,0x13c,0x13c,0x13b,0x13a,0x13a,0x139,0x138,0x137,0x137,0x136,0x135,0x135,0x134,0x133,0x132,0x132,0x131,
0x130,0x130,0x12f,0x12e,0x12d,0x12d,0x12c,0x12b,0x12a,0x12a,0x129,0x128,0x128,0x127,0x126,0x126,0x125,0x124,0x123,0x123,
0x122,0x121,0x121,0x120,0x11f,0x11e,0x11e,0x11d,0x11c,0x11c,0x11b,0x11a,0x11a,0x119,0x118,0x117,0x117,0x116,0x115,0x115,
0x114,0x113,0x113,0x112,0x111,0x110,0x110,0x10f,0x10e,0x10e,0x10d,0x10c,0x10c,0x10b,0x10a,0x10a,0x109,0x108,0x107,0x107,
0x106,0x105,0x105,0x104,0x103,0x103,0x102,0x101,0x101,0x100,0xff,0xff,0xfe,0xfd,0xfd,0xfc,0xfb,0xfb,0xfa,0xf9,
0xf9,0xf8,0xf7,0xf7,0xf6,0xf5,0xf5,0xf4,0xf3,0xf3,0xf2,0xf1,0xf1,0xf0,0xef,0xef,0xee,0xed,0xed,0xec,
0xeb,0xeb,0xea,0xe9,0xe9,0xe8,0xe7,0xe7,0xe6,0xe5,0xe5,0xe4,0xe3,0xe3,0xe2,0xe1,0xe1,0xe0,0xdf,0xdf,
0xde,0xdd,0xdd,0xdc,0xdc,0xdb,0xda,0xda,0xd9,0xd8,0xd8,0xd7,0xd6,0xd6,0xd5,0xd5,0xd4,0xd3,0xd3,0xd2,
0xd1,0xd1,0xd0,0xcf,0xcf,0xce,0xce,0xcd,0xcc,0xcc,0xcb,0xca,0xca,0xc9,0xc9,0xc8,0xc7,0xc7,0xc6,0xc5,
0xc5,0xc4,0xc4,0xc3,0xc2,0xc2,0xc1,0xc0,0xc0,0xbf,0xbf,0xbe,0xbd,0xbd,0xbc,0xbc,0xbb,0xba,0xba,0xb9,
0xb9,0xb8,0xb7,0xb7,0xb6,0xb6,0xb5,0xb4,0xb4,0xb3,0xb3,0xb2,0xb1,0xb1,0xb0,0xb0,0xaf,0xae,0xae,0xad,
0xad,0xac,0xac,0xab,0xaa,0xaa,0xa9,0xa9,0xa8,0xa7,0xa7,0xa6,0xa6,0xa5,0xa5,0xa4,0xa3,0xa3,0xa2,0xa2,
0xa1,0xa1,0xa0,0x9f,0x9f,0x9e,0x9e,0x9d,0x9d,0x9c,0x9b,0x9b,0x9a,0x9a,0x99,0x99,0x98,0x97,0x97,0x96,
0x96,0x95,0x95,0x94,0x94,0x93,0x93,0x92,0x91,0x91,0x90,0x90,0x8f,0x8f,0x8e,0x8e,0x8d,0x8d,0x8c,0x8b,
0x8b,0x8a,0x8a,0x89,0x89,0x88,0x88,0x87,0x87,0x86,0x86,0x85,0x85,0x84,0x83,0x83,0x82,0x82,0x81,0x81,
0x80,0x80,0x7f,0x7f,0x7e,0x7e,0x7d,0x7d,0x7c,0x7c,0x7b,0x7b,0x7a,0x7a,0x79,0x79,0x78,0x78,0x77,0x77,
0x76,0x76,0x75,0x75,0x74,0x74,0x73,0x73,0x72,0x72,0x71,0x71,0x70,0x70,0x6f,0x6f,0x6e,0x6e,0x6d,0x6d,
0x6c,0x6c,0x6b,0x6b,0x6a,0x6a,0x69,0x69,0x68,0x68,0x67,0x67,0x67,0x66,0x66,0x65,0x65,0x64,0x64,0x63,
0x63,0x62,0x62,0x61,0x61,0x60,0x60,0x60,0x5f,0x5f,0x5e,0x5e,0x5d,0x5d,0x5c,0x5c,0x5c,0x5b,0x5b,0x5a,
0x5a,0x59,0x59,0x58,0x58,0x58,0x57,0x57,0x56,0x56,0x55,0x55,0x54,0x54,0x54,0x53,0x53,0x52,0x52,0x51,
0x51,0x51,0x50,0x50,0x4f,0x4f,0x4f,0x4e,0x4e,0x4d,0x4d,0x4c,0x4c,0x4c,0x4b,0x4b,0x4a,0x4a,0x4a,0x49,
0x49,0x48,0x48,0x48,0x47,0x47,0x46,0x46,0x46,0x45,0x45,0x44,0x44,0x44,0x43,0x43,0x42,0x42,0x42,0x41,
0x41,0x41,0x40,0x40,0x3f,0x3f,0x3f,0x3e,0x3e,0x3e,0x3d,0x3d,0x3c,0x3c,0x3c,0x3b,0x3b,0x3b,0x3a,0x3a,
0x39,0x39,0x39,0x38,0x38,0x38,0x37,0x37,0x37,0x36,0x36,0x36,0x35,0x35,0x35,0x34,0x34,0x33,0x33,0x33,
0x32,0x32,0x32,0x31,0x31,0x31,0x30,0x30,0x30,0x2f,0x2f,0x2f,0x2e,0x2e,0x2e,0x2d,0x2d,0x2d,0x2d,0x2c,
0x2c,0x2c,0x2b,0x2b,0x2b,0x2a,0x2a,0x2a,0x29,0x29,0x29,0x28,0x28,0x28,0x28,0x27,0x27,0x27,0x26,0x26,
0x26,0x25,0x25,0x25,0x25,0x24,0x24,0x24,0x23,0x23,0x23,0x23,0x22,0x22,0x22,0x21,0x21,0x21,0x21,0x20,
0x20,0x20,0x20,0x1f,0x1f,0x1f,0x1e,0x1e,0x1e,0x1e,0x1d,0x1d,0x1d,0x1d,0x1c,0x1c,0x1c,0x1c,0x1b,0x1b,
0x1b,0x1b,0x1a,0x1a,0x1a,0x1a,0x19,0x19,0x19,0x19,0x18,0x18,0x18,0x18,0x17,0x17,0x17,0x17,0x16,0x16,
0x16,0x16,0x16,0x15,0x15,0x15,0x15,0x14,0x14,0x14,0x14,0x14,0x13,0x13,0x13,0x13,0x13,0x12,0x12,0x12,
0x12,0x11,0x11,0x11,0x11,0x11,0x10,0x10,0x10,0x10,0x10,0x10,0xf,0xf,0xf,0xf,0xf,0xe,0xe,0xe,
0xe,0xe,0xd,0xd,0xd,0xd,0xd,0xd,0xc,0xc,0xc,0xc,0xc,0xc,0xb,0xb,0xb,0xb,0xb,0xb,
0xa,0xa,0xa,0xa,0xa,0xa,0xa,0x9,0x9,0x9,0x9,0x9,0x9,0x8,0x8,0x8,0x8,0x8,0x8,0x8,
0x8,0x7,0x7,0x7,0x7,0x7,0x7,0x7,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x5,0x5,0x5,
0x5,0x5,0x5,0x5,0x5,0x5,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x3,0x3,0x3,0x3,
0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,
0x2,0x2,0x2,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
0x1,0x1,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3,
0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x5,
0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x6,0x7,0x7,0x7,
0x7,0x7,0x7,0x7,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x9,0x9,0x9,0x9,0x9,0x9,0xa,0xa,
0xa,0xa,0xa,0xa,0xa,0xb,0xb,0xb,0xb,0xb,0xb,0xc,0xc,0xc,0xc,0xc,0xc,0xd,0xd,0xd,
0xd,0xd,0xd,0xe,0xe,0xe,0xe,0xe,0xf,0xf,0xf,0xf,0xf,0x10,0x10,0x10,0x10,0x10,0x10,0x11,
0x11,0x11,0x11,0x11,0x12,0x12,0x12,0x12,0x13,0x13,0x13,0x13,0x13,0x14,0x14,0x14,0x14,0x14,0x15,0x15,
0x15,0x15,0x16,0x16,0x16,0x16,0x16,0x17,0x17,0x17,0x17,0x18,0x18,0x18,0x18,0x19,0x19,0x19,0x19,0x1a,
0x1a,0x1a,0x1a,0x1b,0x1b,0x1b,0x1b,0x1c,0x1c,0x1c,0x1c,0x1d,0x1d,0x1d,0x1d,0x1e,0x1e,0x1e,0x1e,0x1f,
0x1f,0x1f,0x20,0x20,0x20,0x20,0x21,0x21,0x21,0x21,0x22,0x22,0x22,0x23,0x23,0x23,0x23,0x24,0x24,0x24,
0x25,0x25,0x25,0x25,0x26,0x26,0x26,0x27,0x27,0x27,0x28,0x28,0x28,0x28,0x29,0x29,0x29,0x2a,0x2a,0x2a,
0x2b,0x2b,0x2b,0x2c,0x2c,0x2c,0x2d,0x2d,0x2d,0x2d,0x2e,0x2e,0x2e,0x2f,0x2f,0x2f,0x30,0x30,0x30,0x31,
0x31,0x31,0x32,0x32,0x32,0x33,0x33,0x33,0x34,0x34,0x35,0x35,0x35,0x36,0x36,0x36,0x37,0x37,0x37,0x38,
0x38,0x38,0x39,0x39,0x39,0x3a,0x3a,0x3b,0x3b,0x3b,0x3c,0x3c,0x3c,0x3d,0x3d,0x3e,0x3e,0x3e,0x3f,0x3f,
0x3f,0x40,0x40,0x41,0x41,0x41,0x42,0x42,0x42,0x43,0x43,0x44,0x44,0x44,0x45,0x45,0x46,0x46,0x46,0x47,
0x47,0x48,0x48,0x48,0x49,0x49,0x4a,0x4a,0x4a,0x4b,0x4b,0x4c,0x4c,0x4c,0x4d,0x4d,0x4e,0x4e,0x4f,0x4f,
0x4f,0x50,0x50,0x51,0x51,0x51,0x52,0x52,0x53,0x53,0x54,0x54,0x54,0x55,0x55,0x56,0x56,0x57,0x57,0x58,
0x58,0x58,0x59,0x59,0x5a,0x5a,0x5b,0x5b,0x5c,0x5c,0x5c,0x5d,0x5d,0x5e,0x5e,0x5f,0x5f,0x60,0x60,0x60,
0x61,0x61,0x62,0x62,0x63,0x63,0x64,0x64,0x65,0x65,0x66,0x66,0x67,0x67,0x67,0x68,0x68,0x69,0x69,0x6a,
0x6a,0x6b,0x6b,0x6c,0x6c,0x6d,0x6d,0x6e,0x6e,0x6f,0x6f,0x70,0x70,0x71,0x71,0x72,0x72,0x73,0x73,0x74,
0x74,0x75,0x75,0x76,0x76,0x77,0x77,0x78,0x78,0x79,0x79,0x7a,0x7a,0x7b,0x7b,0x7c,0x7c,0x7d,0x7d,0x7e,
0x7e,0x7f,0x7f,0x80,0x80,0x81,0x81,0x82,0x82,0x83,0x83,0x84,0x85,0x85,0x86,0x86,0x87,0x87,0x88,0x88,
0x89,0x89,0x8a,0x8a,0x8b,0x8b,0x8c,0x8d,0x8d,0x8e,0x8e,0x8f,0x8f,0x90,0x90,0x91,0x91,0x92,0x93,0x93,
0x94,0x94,0x95,0x95,0x96,0x96,0x97,0x97,0x98,0x99,0x99,0x9a,0x9a,0x9b,0x9b,0x9c,0x9d,0x9d,0x9e,0x9e,
0x9f,0x9f,0xa0,0xa1,0xa1,0xa2,0xa2,0xa3,0xa3,0xa4,0xa5,0xa5,0xa6,0xa6,0xa7,0xa7,0xa8,0xa9,0xa9,0xaa,
0xaa,0xab,0xac,0xac,0xad,0xad,0xae,0xae,0xaf,0xb0,0xb0,0xb1,0xb1,0xb2,0xb3,0xb3,0xb4,0xb4,0xb5,0xb6,
0xb6,0xb7,0xb7,0xb8,0xb9,0xb9,0xba,0xba,0xbb,0xbc,0xbc,0xbd,0xbd,0xbe,0xbf,0xbf,0xc0,0xc0,0xc1,0xc2,
0xc2,0xc3,0xc4,0xc4,0xc5,0xc5,0xc6,0xc7,0xc7,0xc8,0xc9,0xc9,0xca,0xca,0xcb,0xcc,0xcc,0xcd,0xce,0xce,
0xcf,0xcf,0xd0,0xd1,0xd1,0xd2,0xd3,0xd3,0xd4,0xd5,0xd5,0xd6,0xd6,0xd7,0xd8,0xd8,0xd9,0xda,0xda,0xdb,
0xdc,0xdc,0xdd,0xdd,0xde,0xdf,0xdf,0xe0,0xe1,0xe1,0xe2,0xe3,0xe3,0xe4,0xe5,0xe5,0xe6,0xe7,0xe7,0xe8,
0xe9,0xe9,0xea,0xeb,0xeb,0xec,0xed,0xed,0xee,0xef,0xef,0xf0,0xf1,0xf1,0xf2,0xf3,0xf3,0xf4,0xf5,0xf5,
0xf6,0xf7,0xf7,0xf8,0xf9,0xf9,0xfa,0xfb,0xfb,0xfc,0xfd,0xfd,0xfe,0xff,0xff,0x100,0x101,0x101,0x102,0x103,
0x103,0x104,0x105,0x105,0x106,0x107,0x107,0x108,0x109,0x10a,0x10a,0x10b,0x10c,0x10c,0x10d,0x10e,0x10e,0x10f,0x110,0x110,
0x111,0x112,0x113,0x113,0x114,0x115,0x115,0x116,0x117,0x117,0x118,0x119,0x11a,0x11a,0x11b,0x11c,0x11c,0x11d,0x11e,0x11e,
0x11f,0x120,0x121,0x121,0x122,0x123,0x123,0x124,0x125,0x126,0x126,0x127,0x128,0x128,0x129,0x12a,0x12a,0x12b,0x12c,0x12d,
0x12d,0x12e,0x12f,0x130,0x130,0x131,0x132,0x132,0x133,0x134,0x135,0x135,0x136,0x137,0x137,0x138,0x139,0x13a,0x13a,0x13b,
0x13c,0x13c,0x13d,0x13e,0x13f,0x13f,0x140,0x141,0x142,0x142,0x143,0x144,0x144,0x145,0x146,0x147,0x147,0x148,0x149,0x14a,
0x14a,0x14b,0x14c,0x14d,0x14d,0x14e,0x14f,0x14f,0x150,0x151,0x152,0x152,0x153,0x154,0x155,0x155,0x156,0x157,0x158,0x158,
0x159,0x15a,0x15b,0x15b,0x15c,0x15d,0x15e,0x15e,0x15f,0x160,0x161,0x161,0x162,0x163,0x164,0x164,0x165,0x166,0x167,0x167,
0x168,0x169,0x16a,0x16a,0x16b,0x16c,0x16d,0x16d,0x16e,0x16f,0x170,0x170,0x171,0x172,0x173,0x173,0x174,0x175,0x176,0x176,
0x177,0x178,0x179,0x179,0x17a,0x17b,0x17c,0x17c,0x17d,0x17e,0x17f,0x17f,0x180,0x181,0x182,0x182,0x183,0x184,0x185,0x186,
0x186,0x187,0x188,0x189,0x189,0x18a,0x18b,0x18c,0x18c,0x18d,0x18e,0x18f,0x18f,0x190,0x191,0x192,0x192,0x193,0x194,0x195,
0x196,0x196,0x197,0x198,0x199,0x199,0x19a,0x19b,0x19c,0x19c,0x19d,0x19e,0x19f,0x1a0,0x1a0,0x1a1,0x1a2,0x1a3,0x1a3,0x1a4,
0x1a5,0x1a6,0x1a7,0x1a7,0x1a8,0x1a9,0x1aa,0x1aa,0x1ab,0x1ac,0x1ad,0x1ad,0x1ae,0x1af,0x1b0,0x1b1,0x1b1,0x1b2,0x1b3,0x1b4,
0x1b4,0x1b5,0x1b6,0x1b7,0x1b8,0x1b8,0x1b9,0x1ba,0x1bb,0x1bb,0x1bc,0x1bd,0x1be,0x1bf,0x1bf,0x1c0,0x1c1,0x1c2,0x1c2,0x1c3,
0x1c4,0x1c5,0x1c6,0x1c6,0x1c7,0x1c8,0x1c9,0x1c9,0x1ca,0x1cb,0x1cc,0x1cd,0x1cd,0x1ce,0x1cf,0x1d0,0x1d0,0x1d1,0x1d2,0x1d3,
0x1d4,0x1d4,0x1d5,0x1d6,0x1d7,0x1d8,0x1d8,0x1d9,0x1da,0x1db,0x1db,0x1dc,0x1dd,0x1de,0x1df,0x1df,0x1e0,0x1e1,0x1e2,0x1e2,
0x1e3,0x1e4,0x1e5,0x1e6,0x1e6,0x1e7,0x1e8,0x1e9,0x1ea,0x1ea,0x1eb,0x1ec,0x1ed,0x1ed,0x1ee,0x1ef,0x1f0,0x1f1,0x1f1,0x1f2,
0x1f3,0x1f4,0x1f5,0x1f5,0x1f6,0x1f7,0x1f8,0x1f8,0x1f9,0x1fa,0x1fb,0x1fc,0x1fc,0x1fd,0x1fe,0x1ff};
#endif // SINE_TABLE_H_

//
// End of file
//
