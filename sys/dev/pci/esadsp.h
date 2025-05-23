/* $NetBSD$ */

/*
 * Copyright (c) 2002 Lennart Augustsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ESS Allegro-1 / Maestro3 Audio Driver
 *
 * Lots of magic based on the FreeBSD maestro3 driver and
 * reverse engineering.
 * Original driver by Don Kim.
 *
 */

uint16_t esa_minisrc_lpf_image[] = {
	0x0743, 0x1104, 0x0a4c, 0xf88d, 0x242c,
	0x1023, 0x1aa9, 0x0b60, 0xefdd, 0x186f
};

#define ESA_ARB_VOLUME 0x6800

static struct play_vals {
	uint16_t addr, val;
} esa_playvals[] = {
	{ ESA_CDATA_LEFT_VOLUME, ESA_ARB_VOLUME},
	{ ESA_CDATA_RIGHT_VOLUME, ESA_ARB_VOLUME},
	{ ESA_SRC3_DIRECTION_OFFSET, 0} ,
	{ ESA_SRC3_DIRECTION_OFFSET + 3, 0x0000},
	{ ESA_SRC3_DIRECTION_OFFSET + 4, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 5, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 6, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 7, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 8, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 9, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 10, 0x8000},
	{ ESA_SRC3_DIRECTION_OFFSET + 11, 0xFF00},
	{ ESA_SRC3_DIRECTION_OFFSET + 13, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 14, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 15, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 16, 8},
	{ ESA_SRC3_DIRECTION_OFFSET + 17, 50*2},
	{ ESA_SRC3_DIRECTION_OFFSET + 18, ESA_MINISRC_BIQUAD_STAGE - 1},
	{ ESA_SRC3_DIRECTION_OFFSET + 20, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 21, 0}
};

static struct rec_vals {
	uint16_t addr, val;
} esa_recvals[] = {
	{ ESA_CDATA_LEFT_VOLUME, ESA_ARB_VOLUME},
	{ ESA_CDATA_RIGHT_VOLUME, ESA_ARB_VOLUME},
	{ ESA_SRC3_DIRECTION_OFFSET, 1},
	{ ESA_SRC3_DIRECTION_OFFSET + 3, 0x0000},
	{ ESA_SRC3_DIRECTION_OFFSET + 4, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 5, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 6, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 7, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 8, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 9, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 10, 0x8000},
	{ ESA_SRC3_DIRECTION_OFFSET + 11, 0xFF00},
	{ ESA_SRC3_DIRECTION_OFFSET + 13, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 14, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 15, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 16, 50},
	{ ESA_SRC3_DIRECTION_OFFSET + 17, 8},
	{ ESA_SRC3_DIRECTION_OFFSET + 18, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 19, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 20, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 21, 0},
	{ ESA_SRC3_DIRECTION_OFFSET + 22, 0xff}
};

static uint16_t esa_assp_kernel_image[] = {
 0x7980, 0x0030, 0x7980, 0x03b4, 0x7980, 0x03b4, 0x7980, 0x00fb,
 0x7980, 0x00dd, 0x7980, 0x03b4, 0x7980, 0x0332, 0x7980, 0x0287,
 0x7980, 0x03b4, 0x7980, 0x03b4, 0x7980, 0x03b4, 0x7980, 0x03b4,
 0x7980, 0x031a, 0x7980, 0x03b4, 0x7980, 0x022f, 0x7980, 0x03b4,
 0x7980, 0x03b4, 0x7980, 0x03b4, 0x7980, 0x03b4, 0x7980, 0x03b4,
 0x7980, 0x0063, 0x7980, 0x006b, 0x7980, 0x03b4, 0x7980, 0x03b4,
 0xbf80, 0x2c7c, 0x8806, 0x8804, 0xbe40, 0xbc20, 0xae09, 0x1000,
 0xae0a, 0x0001, 0x6938, 0xeb08, 0x0053, 0x695a, 0xeb08, 0x00d6,
 0x0009, 0x8b88, 0x6980, 0xe388, 0x0036, 0xbe30, 0xbc20, 0x6909,
 0xb801, 0x9009, 0xbe41, 0xbe41, 0x6928, 0xeb88, 0x0078, 0xbe41,
 0xbe40, 0x7980, 0x0038, 0xbe41, 0xbe41, 0x903a, 0x6938, 0xe308,
 0x0056, 0x903a, 0xbe41, 0xbe40, 0xef00, 0x903a, 0x6939, 0xe308,
 0x005e, 0x903a, 0xef00, 0x690b, 0x660c, 0xef8c, 0x690a, 0x660c,
 0x620b, 0x6609, 0xef00, 0x6910, 0x660f, 0xef04, 0xe388, 0x0075,
 0x690e, 0x660f, 0x6210, 0x660d, 0xef00, 0x690e, 0x660d, 0xef00,
 0xae70, 0x0001, 0xbc20, 0xae27, 0x0001, 0x6939, 0xeb08, 0x005d,
 0x6926, 0xb801, 0x9026, 0x0026, 0x8b88, 0x6980, 0xe388, 0x00cb,
 0x9028, 0x0d28, 0x4211, 0xe100, 0x007a, 0x4711, 0xe100, 0x00a0,
 0x7a80, 0x0063, 0xb811, 0x660a, 0x6209, 0xe304, 0x007a, 0x0c0b,
 0x4005, 0x100a, 0xba01, 0x9012, 0x0c12, 0x4002, 0x7980, 0x00af,
 0x7a80, 0x006b, 0xbe02, 0x620e, 0x660d, 0xba10, 0xe344, 0x007a,
 0x0c10, 0x4005, 0x100e, 0xba01, 0x9012, 0x0c12, 0x4002, 0x1003,
 0xba02, 0x9012, 0x0c12, 0x4000, 0x1003, 0xe388, 0x00ba, 0x1004,
 0x7980, 0x00bc, 0x1004, 0xba01, 0x9012, 0x0c12, 0x4001, 0x0c05,
 0x4003, 0x0c06, 0x4004, 0x1011, 0xbfb0, 0x01ff, 0x9012, 0x0c12,
 0x4006, 0xbc20, 0xef00, 0xae26, 0x1028, 0x6970, 0xbfd0, 0x0001,
 0x9070, 0xe388, 0x007a, 0xae28, 0x0000, 0xef00, 0xae70, 0x0300,
 0x0c70, 0xb00c, 0xae5a, 0x0000, 0xef00, 0x7a80, 0x038a, 0x697f,
 0xb801, 0x907f, 0x0056, 0x8b88, 0x0ca0, 0xb008, 0xaf71, 0xb000,
 0x4e71, 0xe200, 0x00f3, 0xae56, 0x1057, 0x0056, 0x0ca0, 0xb008,
 0x8056, 0x7980, 0x03a1, 0x0810, 0xbfa0, 0x1059, 0xe304, 0x03a1,
 0x8056, 0x7980, 0x03a1, 0x7a80, 0x038a, 0xbf01, 0xbe43, 0xbe59,
 0x907c, 0x6937, 0xe388, 0x010d, 0xba01, 0xe308, 0x010c, 0xae71,
 0x0004, 0x0c71, 0x5000, 0x6936, 0x9037, 0xbf0a, 0x109e, 0x8b8a,
 0xaf80, 0x8014, 0x4c80, 0xbf0a, 0x0560, 0xf500, 0xbf0a, 0x0520,
 0xb900, 0xbb17, 0x90a0, 0x6917, 0xe388, 0x0148, 0x0d17, 0xe100,
 0x0127, 0xbf0c, 0x0578, 0xbf0d, 0x057c, 0x7980, 0x012b, 0xbf0c,
 0x0538, 0xbf0d, 0x053c, 0x6900, 0xe308, 0x0135, 0x8b8c, 0xbe59,
 0xbb07, 0x90a0, 0xbc20, 0x7980, 0x0157, 0x030c, 0x8b8b, 0xb903,
 0x8809, 0xbec6, 0x013e, 0x69ac, 0x90ab, 0x69ad, 0x90ab, 0x0813,
 0x660a, 0xe344, 0x0144, 0x0309, 0x830c, 0xbc20, 0x7980, 0x0157,
 0x6955, 0xe388, 0x0157, 0x7c38, 0xbf0b, 0x0578, 0xf500, 0xbf0b,
 0x0538, 0xb907, 0x8809, 0xbec6, 0x0156, 0x10ab, 0x90aa, 0x6974,
 0xe388, 0x0163, 0xae72, 0x0540, 0xf500, 0xae72, 0x0500, 0xae61,
 0x103b, 0x7a80, 0x02f6, 0x6978, 0xe388, 0x0182, 0x8b8c, 0xbf0c,
 0x0560, 0xe500, 0x7c40, 0x0814, 0xba20, 0x8812, 0x733d, 0x7a80,
 0x0380, 0x733e, 0x7a80, 0x0380, 0x8b8c, 0xbf0c, 0x056c, 0xe500,
 0x7c40, 0x0814, 0xba2c, 0x8812, 0x733f, 0x7a80, 0x0380, 0x7340,
 0x7a80, 0x0380, 0x6975, 0xe388, 0x018e, 0xae72, 0x0548, 0xf500,
 0xae72, 0x0508, 0xae61, 0x1041, 0x7a80, 0x02f6, 0x6979, 0xe388,
 0x01ad, 0x8b8c, 0xbf0c, 0x0560, 0xe500, 0x7c40, 0x0814, 0xba18,
 0x8812, 0x7343, 0x7a80, 0x0380, 0x7344, 0x7a80, 0x0380, 0x8b8c,
 0xbf0c, 0x056c, 0xe500, 0x7c40, 0x0814, 0xba24, 0x8812, 0x7345,
 0x7a80, 0x0380, 0x7346, 0x7a80, 0x0380, 0x6976, 0xe388, 0x01b9,
 0xae72, 0x0558, 0xf500, 0xae72, 0x0518, 0xae61, 0x1047, 0x7a80,
 0x02f6, 0x697a, 0xe388, 0x01d8, 0x8b8c, 0xbf0c, 0x0560, 0xe500,
 0x7c40, 0x0814, 0xba08, 0x8812, 0x7349, 0x7a80, 0x0380, 0x734a,
 0x7a80, 0x0380, 0x8b8c, 0xbf0c, 0x056c, 0xe500, 0x7c40, 0x0814,
 0xba14, 0x8812, 0x734b, 0x7a80, 0x0380, 0x734c, 0x7a80, 0x0380,
 0xbc21, 0xae1c, 0x1090, 0x8b8a, 0xbf0a, 0x0560, 0xe500, 0x7c40,
 0x0812, 0xb804, 0x8813, 0x8b8d, 0xbf0d, 0x056c, 0xe500, 0x7c40,
 0x0815, 0xb804, 0x8811, 0x7a80, 0x034a, 0x8b8a, 0xbf0a, 0x0560,
 0xe500, 0x7c40, 0x731f, 0xb903, 0x8809, 0xbec6, 0x01f9, 0x548a,
 0xbe03, 0x98a0, 0x7320, 0xb903, 0x8809, 0xbec6, 0x0201, 0x548a,
 0xbe03, 0x98a0, 0x1f20, 0x2f1f, 0x9826, 0xbc20, 0x6935, 0xe388,
 0x03a1, 0x6933, 0xb801, 0x9033, 0xbfa0, 0x02ee, 0xe308, 0x03a1,
 0x9033, 0xbf00, 0x6951, 0xe388, 0x021f, 0x7334, 0xbe80, 0x5760,
 0xbe03, 0x9f7e, 0xbe59, 0x9034, 0x697e, 0x0d51, 0x9013, 0xbc20,
 0x695c, 0xe388, 0x03a1, 0x735e, 0xbe80, 0x5760, 0xbe03, 0x9f7e,
 0xbe59, 0x905e, 0x697e, 0x0d5c, 0x9013, 0x7980, 0x03a1, 0x7a80,
 0x038a, 0xbf01, 0xbe43, 0x6977, 0xe388, 0x024e, 0xae61, 0x104d,
 0x0061, 0x8b88, 0x6980, 0xe388, 0x024e, 0x9071, 0x0d71, 0x000b,
 0xafa0, 0x8010, 0xafa0, 0x8010, 0x0810, 0x660a, 0xe308, 0x0249,
 0x0009, 0x0810, 0x660c, 0xe388, 0x024e, 0x800b, 0xbc20, 0x697b,
 0xe388, 0x03a1, 0xbf0a, 0x109e, 0x8b8a, 0xaf80, 0x8014, 0x4c80,
 0xe100, 0x0266, 0x697c, 0xbf90, 0x0560, 0x9072, 0x0372, 0x697c,
 0xbf90, 0x0564, 0x9073, 0x0473, 0x7980, 0x0270, 0x697c, 0xbf90,
 0x0520, 0x9072, 0x0372, 0x697c, 0xbf90, 0x0524, 0x9073, 0x0473,
 0x697c, 0xb801, 0x907c, 0xbf0a, 0x10fd, 0x8b8a, 0xaf80, 0x8010,
 0x734f, 0x548a, 0xbe03, 0x9880, 0xbc21, 0x7326, 0x548b, 0xbe03,
 0x618b, 0x988c, 0xbe03, 0x6180, 0x9880, 0x7980, 0x03a1, 0x7a80,
 0x038a, 0x0d28, 0x4711, 0xe100, 0x02be, 0xaf12, 0x4006, 0x6912,
 0xbfb0, 0x0c00, 0xe388, 0x02b6, 0xbfa0, 0x0800, 0xe388, 0x02b2,
 0x6912, 0xbfb0, 0x0c00, 0xbfa0, 0x0400, 0xe388, 0x02a3, 0x6909,
 0x900b, 0x7980, 0x02a5, 0xaf0b, 0x4005, 0x6901, 0x9005, 0x6902,
 0x9006, 0x4311, 0xe100, 0x02ed, 0x6911, 0xbfc0, 0x2000, 0x9011,
 0x7980, 0x02ed, 0x6909, 0x900b, 0x7980, 0x02b8, 0xaf0b, 0x4005,
 0xaf05, 0x4003, 0xaf06, 0x4004, 0x7980, 0x02ed, 0xaf12, 0x4006,
 0x6912, 0xbfb0, 0x0c00, 0xe388, 0x02e7, 0xbfa0, 0x0800, 0xe388,
 0x02e3, 0x6912, 0xbfb0, 0x0c00, 0xbfa0, 0x0400, 0xe388, 0x02d4,
 0x690d, 0x9010, 0x7980, 0x02d6, 0xaf10, 0x4005, 0x6901, 0x9005,
 0x6902, 0x9006, 0x4311, 0xe100, 0x02ed, 0x6911, 0xbfc0, 0x2000,
 0x9011, 0x7980, 0x02ed, 0x690d, 0x9010, 0x7980, 0x02e9, 0xaf10,
 0x4005, 0xaf05, 0x4003, 0xaf06, 0x4004, 0xbc20, 0x6970, 0x9071,
 0x7a80, 0x0078, 0x6971, 0x9070, 0x7980, 0x03a1, 0xbc20, 0x0361,
 0x8b8b, 0x6980, 0xef88, 0x0272, 0x0372, 0x7804, 0x9071, 0x0d71,
 0x8b8a, 0x000b, 0xb903, 0x8809, 0xbec6, 0x0309, 0x69a8, 0x90ab,
 0x69a8, 0x90aa, 0x0810, 0x660a, 0xe344, 0x030f, 0x0009, 0x0810,
 0x660c, 0xe388, 0x0314, 0x800b, 0xbc20, 0x6961, 0xb801, 0x9061,
 0x7980, 0x02f7, 0x7a80, 0x038a, 0x5d35, 0x0001, 0x6934, 0xb801,
 0x9034, 0xbf0a, 0x109e, 0x8b8a, 0xaf80, 0x8014, 0x4880, 0xae72,
 0x0550, 0xf500, 0xae72, 0x0510, 0xae61, 0x1051, 0x7a80, 0x02f6,
 0x7980, 0x03a1, 0x7a80, 0x038a, 0x5d35, 0x0002, 0x695e, 0xb801,
 0x905e, 0xbf0a, 0x109e, 0x8b8a, 0xaf80, 0x8014, 0x4780, 0xae72,
 0x0558, 0xf500, 0xae72, 0x0518, 0xae61, 0x105c, 0x7a80, 0x02f6,
 0x7980, 0x03a1, 0x001c, 0x8b88, 0x6980, 0xef88, 0x901d, 0x0d1d,
 0x100f, 0x6610, 0xe38c, 0x0358, 0x690e, 0x6610, 0x620f, 0x660d,
 0xba0f, 0xe301, 0x037a, 0x0410, 0x8b8a, 0xb903, 0x8809, 0xbec6,
 0x036c, 0x6a8c, 0x61aa, 0x98ab, 0x6a8c, 0x61ab, 0x98ad, 0x6a8c,
 0x61ad, 0x98a9, 0x6a8c, 0x61a9, 0x98aa, 0x7c04, 0x8b8b, 0x7c04,
 0x8b8d, 0x7c04, 0x8b89, 0x7c04, 0x0814, 0x660e, 0xe308, 0x0379,
 0x040d, 0x8410, 0xbc21, 0x691c, 0xb801, 0x901c, 0x7980, 0x034a,
 0xb903, 0x8809, 0x8b8a, 0xbec6, 0x0388, 0x54ac, 0xbe03, 0x618c,
 0x98aa, 0xef00, 0xbc20, 0xbe46, 0x0809, 0x906b, 0x080a, 0x906c,
 0x080b, 0x906d, 0x081a, 0x9062, 0x081b, 0x9063, 0x081e, 0x9064,
 0xbe59, 0x881e, 0x8065, 0x8166, 0x8267, 0x8368, 0x8469, 0x856a,
 0xef00, 0xbc20, 0x696b, 0x8809, 0x696c, 0x880a, 0x696d, 0x880b,
 0x6962, 0x881a, 0x6963, 0x881b, 0x6964, 0x881e, 0x0065, 0x0166,
 0x0267, 0x0368, 0x0469, 0x056a, 0xbe3a,
};

static uint16_t esa_assp_minisrc_image[] = {
                                         0xbf80, 0x101e, 0x906e,
 0x006e, 0x8b88, 0x6980, 0xef88, 0x906f, 0x0d6f, 0x6900, 0xeb08,
 0x0412, 0xbc20, 0x696e, 0xb801, 0x906e, 0x7980, 0x0403, 0xb90e,
 0x8807, 0xbe43, 0xbf01, 0xbe47, 0xbe41, 0x7a80, 0x002a, 0xbe40,
 0x3029, 0xefcc, 0xbe41, 0x7a80, 0x0028, 0xbe40, 0x3028, 0xefcc,
 0x6907, 0xe308, 0x042a, 0x6909, 0x902c, 0x7980, 0x042c, 0x690d,
 0x902c, 0x1009, 0x881a, 0x100a, 0xba01, 0x881b, 0x100d, 0x881c,
 0x100e, 0xba01, 0x881d, 0xbf80, 0x00ed, 0x881e, 0x050c, 0x0124,
 0xb904, 0x9027, 0x6918, 0xe308, 0x04b3, 0x902d, 0x6913, 0xbfa0,
 0x7598, 0xf704, 0xae2d, 0x00ff, 0x8b8d, 0x6919, 0xe308, 0x0463,
 0x691a, 0xe308, 0x0456, 0xb907, 0x8809, 0xbec6, 0x0453, 0x10a9,
 0x90ad, 0x7980, 0x047c, 0xb903, 0x8809, 0xbec6, 0x0460, 0x1889,
 0x6c22, 0x90ad, 0x10a9, 0x6e23, 0x6c22, 0x90ad, 0x7980, 0x047c,
 0x101a, 0xe308, 0x046f, 0xb903, 0x8809, 0xbec6, 0x046c, 0x10a9,
 0x90a0, 0x90ad, 0x7980, 0x047c, 0xb901, 0x8809, 0xbec6, 0x047b,
 0x1889, 0x6c22, 0x90a0, 0x90ad, 0x10a9, 0x6e23, 0x6c22, 0x90a0,
 0x90ad, 0x692d, 0xe308, 0x049c, 0x0124, 0xb703, 0xb902, 0x8818,
 0x8b89, 0x022c, 0x108a, 0x7c04, 0x90a0, 0x692b, 0x881f, 0x7e80,
 0x055b, 0x692a, 0x8809, 0x8b89, 0x99a0, 0x108a, 0x90a0, 0x692b,
 0x881f, 0x7e80, 0x055b, 0x692a, 0x8809, 0x8b89, 0x99af, 0x7b99,
 0x0484, 0x0124, 0x060f, 0x101b, 0x2013, 0x901b, 0xbfa0, 0x7fff,
 0xe344, 0x04ac, 0x901b, 0x8b89, 0x7a80, 0x051a, 0x6927, 0xba01,
 0x9027, 0x7a80, 0x0523, 0x6927, 0xe308, 0x049e, 0x7980, 0x050f,
 0x0624, 0x1026, 0x2013, 0x9026, 0xbfa0, 0x7fff, 0xe304, 0x04c0,
 0x8b8d, 0x7a80, 0x051a, 0x7980, 0x04b4, 0x9026, 0x1013, 0x3026,
 0x901b, 0x8b8d, 0x7a80, 0x051a, 0x7a80, 0x0523, 0x1027, 0xba01,
 0x9027, 0xe308, 0x04b4, 0x0124, 0x060f, 0x8b89, 0x691a, 0xe308,
 0x04ea, 0x6919, 0xe388, 0x04e0, 0xb903, 0x8809, 0xbec6, 0x04dd,
 0x1fa0, 0x2fae, 0x98a9, 0x7980, 0x050f, 0xb901, 0x8818, 0xb907,
 0x8809, 0xbec6, 0x04e7, 0x10ee, 0x90a9, 0x7980, 0x050f, 0x6919,
 0xe308, 0x04fe, 0xb903, 0x8809, 0xbe46, 0xbec6, 0x04fa, 0x17a0,
 0xbe1e, 0x1fae, 0xbfbf, 0xff00, 0xbe13, 0xbfdf, 0x8080, 0x99a9,
 0xbe47, 0x7980, 0x050f, 0xb901, 0x8809, 0xbec6, 0x050e, 0x16a0,
 0x26a0, 0xbfb7, 0xff00, 0xbe1e, 0x1ea0, 0x2eae, 0xbfbf, 0xff00,
 0xbe13, 0xbfdf, 0x8080, 0x99a9, 0x850c, 0x860f, 0x6907, 0xe388,
 0x0516, 0x0d07, 0x8510, 0xbe59, 0x881e, 0xbe4a, 0xef00, 0x101e,
 0x901c, 0x101f, 0x901d, 0x10a0, 0x901e, 0x10a0, 0x901f, 0xef00,
 0x101e, 0x301c, 0x9020, 0x731b, 0x5420, 0xbe03, 0x9825, 0x1025,
 0x201c, 0x9025, 0x7325, 0x5414, 0xbe03, 0x8b8e, 0x9880, 0x692f,
 0xe388, 0x0539, 0xbe59, 0xbb07, 0x6180, 0x9880, 0x8ba0, 0x101f,
 0x301d, 0x9021, 0x731b, 0x5421, 0xbe03, 0x982e, 0x102e, 0x201d,
 0x902e, 0x732e, 0x5415, 0xbe03, 0x9880, 0x692f, 0xe388, 0x054f,
 0xbe59, 0xbb07, 0x6180, 0x9880, 0x8ba0, 0x6918, 0xef08, 0x7325,
 0x5416, 0xbe03, 0x98a0, 0x732e, 0x5417, 0xbe03, 0x98a0, 0xef00,
 0x8ba0, 0xbec6, 0x056b, 0xbe59, 0xbb04, 0xaa90, 0xbe04, 0xbe1e,
 0x99e0, 0x8be0, 0x69a0, 0x90d0, 0x69a0, 0x90d0, 0x081f, 0xb805,
 0x881f, 0x8b90, 0x69a0, 0x90d0, 0x69a0, 0x9090, 0x8bd0, 0x8bd8,
 0xbe1f, 0xef00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};
