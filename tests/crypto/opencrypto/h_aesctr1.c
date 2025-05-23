/* $NetBSD$ */

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/time.h>

#include <crypto/cryptodev.h>

/*
 * Test vectors from RFC 3686
 *
 * Test vectors 3, 6, and 9 are disabled because we don't support
 * 36-byte (ie, unpadded) operations.
 */

const struct {
	size_t len;
	size_t key_len;
	unsigned char key[36];		/* Includes 32-bit nonce */
	unsigned char iv[8];
	unsigned char plaintx[36];
	unsigned char ciphertx[36];
} tests[] = {
	/* Test Vector #1: Encrypting 16 octets using AES-CTR w/ 128-bit key*/
	{ 16, 20, 
	  { 0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC,
	    0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E,
	    0x00, 0x00, 0x00, 0x30 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
	    0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67 },
	  { 0xE4, 0x09, 0x5D, 0x4F, 0xB7, 0xA7, 0xB3, 0x79,
	    0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8 }
	},

	/* Test Vector #2: Encrypting 32 octets using AES-CTR w/ 128-bit key */
	{ 32, 20, 
	  { 0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7,
	    0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63,
	    0x00, 0x6C, 0xB6, 0xDB },
	  { 0xC0, 0x54, 0x3B, 0x59, 0xDA, 0x48, 0xD9, 0x0B },
	  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },
	  { 0x51, 0x04, 0xA1, 0x06, 0x16, 0x8A, 0x72, 0xD9,
	    0x79, 0x0D, 0x41, 0xEE, 0x8E, 0xDA, 0xD3, 0x88,
	    0xEB, 0x2E, 0x1E, 0xFC, 0x46, 0xDA, 0x57, 0xC8,
	    0xFC, 0xE6, 0x30, 0xDF, 0x91, 0x41, 0xBE, 0x28 }
	},

	/* Test Vector #3: Encrypting 36 octets using AES-CTR w/ 128-bit key */
/*	{ 36, 20,
	  { 0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
	    0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC,
	    0x00, 0xE0, 0x01, 0x7B },
	  { 0x27, 0x77, 0x7F, 0x3F, 0x4A, 0x17, 0x86, 0xF0 },
	  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	    0x20, 0x21, 0x22, 0x23 },
	  { 0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9,
	    0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
	    0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36,
	    0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
	    0x25, 0xB2, 0x07, 0x2F }
	},
*/
	/* Test Vector #4: Encrypting 16 octets using AES-CTR w/ 192-bit key */
	{ 16, 28,
	  { 0x16, 0xAF, 0x5B, 0x14, 0x5F, 0xC9, 0xF5, 0x79,
	    0xC1, 0x75, 0xF9, 0x3E, 0x3B, 0xFB, 0x0E, 0xED,
	    0x86, 0x3D, 0x06, 0xCC, 0xFD, 0xB7, 0x85, 0x15,
	    0x00, 0x00, 0x00, 0x48 },
	  { 0x36, 0x73, 0x3C, 0x14, 0x7D, 0x6D, 0x93, 0xCB },
	  { 0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
	    0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67 },
	  { 0x4B, 0x55, 0x38, 0x4F, 0xE2, 0x59, 0xC9, 0xC8,
	    0x4E, 0x79, 0x35, 0xA0, 0x03, 0xCB, 0xE9, 0x28 }
	},

	/* Test Vector #5: Encrypting 32 octets using AES-CTR w/ 192-bit key */
	{ 32, 28,
	  { 0x7C, 0x5C, 0xB2, 0x40, 0x1B, 0x3D, 0xC3, 0x3C,
	    0x19, 0xE7, 0x34, 0x08, 0x19, 0xE0, 0xF6, 0x9C,
	    0x67, 0x8C, 0x3D, 0xB8, 0xE6, 0xF6, 0xA9, 0x1A,
	    0x00, 0x96, 0xB0, 0x3B },
	  { 0x02, 0x0C, 0x6E, 0xAD, 0xC2, 0xCB, 0x50, 0x0D },
	  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },
	  { 0x45, 0x32, 0x43, 0xFC, 0x60, 0x9B, 0x23, 0x32,
	    0x7E, 0xDF, 0xAA, 0xFA, 0x71, 0x31, 0xCD, 0x9F,
	    0x84, 0x90, 0x70, 0x1C, 0x5A, 0xD4, 0xA7, 0x9C,
	    0xFC, 0x1F, 0xE0, 0xFF, 0x42, 0xF4, 0xFB, 0x00 }
	},

	/* Test Vector #6: Encrypting 36 octets using AES-CTR w/ 192-bit key */
/*
	{ 36, 28,
	  { 0x02, 0xBF, 0x39, 0x1E, 0xE8, 0xEC, 0xB1, 0x59,
	    0xB9, 0x59, 0x61, 0x7B, 0x09, 0x65, 0x27, 0x9B,
	    0xF5, 0x9B, 0x60, 0xA7, 0x86, 0xD3, 0xE0, 0xFE,
	    0x00, 0x07, 0xBD, 0xFD },
	  { 0x5C, 0xBD, 0x60, 0x27, 0x8D, 0xCC, 0x09, 0x12 },
	  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	    0x20, 0x21, 0x22, 0x23 },
	  { 0x96, 0x89, 0x3F, 0xC5, 0x5E, 0x5C, 0x72, 0x2F,
	    0x54, 0x0B, 0x7D, 0xD1, 0xDD, 0xF7, 0xE7, 0x58,
	    0xD2, 0x88, 0xBC, 0x95, 0xC6, 0x91, 0x65, 0x88,
	    0x45, 0x36, 0xC8, 0x11, 0x66, 0x2F, 0x21, 0x88,
	    0xAB, 0xEE, 0x09, 0x35 },
	},
*/
	/* Test Vector #7: Encrypting 16 octets using AES-CTR w/ 256-bit key */
	{ 16, 36,
	  { 0x77, 0x6B, 0xEF, 0xF2, 0x85, 0x1D, 0xB0, 0x6F,
	    0x4C, 0x8A, 0x05, 0x42, 0xC8, 0x69, 0x6F, 0x6C,
	    0x6A, 0x81, 0xAF, 0x1E, 0xEC, 0x96, 0xB4, 0xD3,
	    0x7F, 0xC1, 0xD6, 0x89, 0xE6, 0xC1, 0xC1, 0x04,
	    0x00, 0x00, 0x00, 0x60 },
	  { 0xDB, 0x56, 0x72, 0xC9, 0x7A, 0xA8, 0xF0, 0xB2 },
	  { 0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
	    0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67 },
	  { 0x14, 0x5A, 0xD0, 0x1D, 0xBF, 0x82, 0x4E, 0xC7,
	    0x56, 0x08, 0x63, 0xDC, 0x71, 0xE3, 0xE0, 0xC0 },
	},

	/* Test Vector #8: Encrypting 32 octets using AES-CTR w/ 256-bit key */
	{ 32, 36,
	  { 0xF6, 0xD6, 0x6D, 0x6B, 0xD5, 0x2D, 0x59, 0xBB,
	    0x07, 0x96, 0x36, 0x58, 0x79, 0xEF, 0xF8, 0x86,
	    0xC6, 0x6D, 0xD5, 0x1A, 0x5B, 0x6A, 0x99, 0x74,
	    0x4B, 0x50, 0x59, 0x0C, 0x87, 0xA2, 0x38, 0x84,
	    0x00, 0xFA, 0xAC, 0x24 },
	  { 0xC1, 0x58, 0x5E, 0xF1, 0x5A, 0x43, 0xD8, 0x75 },
	  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F },
	  { 0xF0, 0x5E, 0x23, 0x1B, 0x38, 0x94, 0x61, 0x2C,
	    0x49, 0xEE, 0x00, 0x0B, 0x80, 0x4E, 0xB2, 0xA9,
	    0xB8, 0x30, 0x6B, 0x50, 0x8F, 0x83, 0x9D, 0x6A,
	    0x55, 0x30, 0x83, 0x1D, 0x93, 0x44, 0xAF, 0x1C },
	},

	/* Test Vector #9: Encrypting 36 octets using AES-CTR w/ 256-bit key */
/*
	{ 36, 36,
	  { 0xFF 0x7A 0x61 0x7C 0xE6 0x91 0x48 0xE4,
	    0xF1 0x72 0x6E 0x2F 0x43 0x58 0x1D 0xE2,
	    0xAA 0x62 0xD9 0xF8 0x05 0x53 0x2E 0xDF,
	    0xF1 0xEE 0xD6 0x87 0xFB 0x54 0x15 0x3D,
	    0x00 0x1C 0xC5 0xB7 },
	  { 0x51 0xA5 0x1D 0x70 0xA1 0xC1 0x11 0x48 },
	  { 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07,
	    0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F,
	    0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17,
	    0x18 0x19 0x1A 0x1B 0x1C 0x1D 0x1E 0x1F,
	    0x20 0x21 0x22 0x23 },
	  { 0xEB 0x6C 0x52 0x82 0x1D 0x0B 0xBB 0xF7,
	    0xCE 0x75 0x94 0x46 0x2A 0xCA 0x4F 0xAA,
	    0xB4 0x07 0xDF 0x86 0x65 0x69 0xFD 0x07,
	    0xF4 0x8C 0xC0 0xB5 0x83 0xD6 0x07 0x1F,
	    0x1E 0xC0 0xE6 0xB8 },
	},
*/
};

int
main(void)
{
	int fd, res;
	size_t i;
	struct session_op cs;
	struct crypt_op co;
	unsigned char buf[36];

	for (i = 0; i < __arraycount(tests); i++) {
		fd = open("/dev/crypto", O_RDWR, 0);
		if (fd < 0)
			err(1, "open %zu", i);
		memset(&cs, 0, sizeof(cs));
		cs.cipher = CRYPTO_AES_CTR;
		cs.keylen = tests[i].key_len;
		cs.key = __UNCONST(&tests[i].key);
		res = ioctl(fd, CIOCGSESSION, &cs);
		if (res < 0)
			err(1, "CIOCGSESSION %zu", i);

		memset(&co, 0, sizeof(co));
		co.ses = cs.ses;
		co.op = COP_ENCRYPT;
		co.len = tests[i].len;
		co.src = __UNCONST(&tests[i].plaintx);
		co.dst = buf;
		co.dst_len = sizeof(buf);
		co.iv = __UNCONST(&tests[i].iv);
		res = ioctl(fd, CIOCCRYPT, &co);
		if (res < 0)
			err(1, "CIOCCRYPT %zu", i);

		if (memcmp(co.dst, tests[i].ciphertx, tests[i].len)) {
			size_t j;
			printf(" Loc  Actual  Golden\n");
			for (j = 0; j < tests[i].len; j++)
				printf("0x%2zu:  0x%2x    0x%2x\n", j,
					buf[j], tests[i].ciphertx[j]);
			warnx("verification failed %zu", i);
		}
		close(fd);
	}
	return 0;
}
