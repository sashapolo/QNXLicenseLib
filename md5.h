/*
 * md5.h
 *
 *  Created on: 25.04.2013
 *      Author: Alexander
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

#ifndef MD5_H_
#define MD5_H_

typedef unsigned int uint4;  // 32bit

/* MD5 context. */
typedef struct {
    uint4 state[4];                                   /* state (ABCD) */
    uint4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                         /* input buffer */
} MD5Context;

void md5_init(MD5Context*);
void md5_update(MD5Context*, unsigned char*, unsigned int);
void md5_finalize(MD5Context*, unsigned char[16]);
void md5_print_hex(const unsigned char[16]);


#endif /* MD5_H_ */
