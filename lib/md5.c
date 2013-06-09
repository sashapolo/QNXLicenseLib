/*!
 * @file
 * @author  Александр Половцев (sasha_polo@mail.ru)
 * @date    25.04.2013
 * @brief   Реализация функций из md5.h
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

#include <stdio.h>
#include <string.h>

#include "md5.h"

/*!
 * @name Константы для функции md5_transform
 */
/**@{*/
const uint4 S11 = 7;
const uint4 S12 = 12;
const uint4 S13 = 17;
const uint4 S14 = 22;
const uint4 S21 = 5;
const uint4 S22 = 9;
const uint4 S23 = 14;
const uint4 S24 = 20;
const uint4 S31 = 4;
const uint4 S32 = 11;
const uint4 S33 = 16;
const uint4 S34 = 23;
const uint4 S41 = 6;
const uint4 S42 = 10;
const uint4 S43 = 15;
const uint4 S44 = 21;
/**@}*/

static void md5_transform(uint4[4], const unsigned char[64]);
static void md5_encode(unsigned char*, const uint4*, size_t);
static void md5_decode(uint4*, const unsigned char*, size_t);

unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*!
 * @name Вспомогательные функции для трансформации
 */
/**@{*/
uint4 md5_f(uint4 x, uint4 y, uint4 z) {
    return (x & y) | (~x & z);
}

uint4 md5_g(uint4 x, uint4 y, uint4 z) {
    return (x & z) | (y & ~z);
}

uint4 md5_h(uint4 x, uint4 y, uint4 z) {
    return x ^ y ^ z;
}

uint4 md5_i(uint4 x, uint4 y, uint4 z) {
    return y ^ (x | ~z);
}

uint4 md5_rotate_left(uint4 x, uint4 n) {
    return (x << n) | (x >> (32 - n));
}

uint4 md5_ff(uint4 a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
    return md5_rotate_left(a + md5_f(b, c, d) + x + ac, s) + b;
}

uint4 md5_gg(uint4 a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
    return md5_rotate_left(a + md5_g(b, c, d) + x + ac, s) + b;
}

uint4 md5_hh(uint4 a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
    return md5_rotate_left(a + md5_h(b, c, d) + x + ac, s) + b;
}

uint4 md5_ii(uint4 a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
    return md5_rotate_left(a + md5_i(b, c, d) + x + ac, s) + b;
}
/**@}*/

void md5_init(md5_context_t* context) {
    context->count[0] = 0;
    context->count[1] = 0;

    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void md5_update(md5_context_t* context, const unsigned char* input, unsigned int len) {
    unsigned int i, index, partLen;

    // Compute number of bytes mod 64
    index = (context->count[0] >> 3) & 0x3F;

    /* Update number of bits */
    context->count[0] += (len << 3);
    if (context->count[0] < (len << 3)) {
      context->count[1]++;
    }
    context->count[1] += (len >> 29);
    partLen = 64 - index;

    // Transform as many times as possible.
    if (len >= partLen) {
        memcpy(&context->buffer[index], input, partLen);
        md5_transform(context->state, context->buffer);

        for (i = partLen; i + 63 < len; i += 64) {
            md5_transform(context->state, &input[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    // Buffer remaining input
    memcpy(&context->buffer[index], &input[i], len - i);
}

void md5_finalize(md5_context_t* context, unsigned char digest[16]) {
    unsigned char bits[8];
    unsigned int index, padLen;

    // Save number of bits
    md5_encode(bits, context->count, 8);

    // Pad out to 56 mod 64.
    index = (context->count[0] >> 3) & 0x3f;
    padLen = (index < 56) ? (56 - index) : (120 - index);
    md5_update(context, PADDING, padLen);

    // Append length (before padding)
    md5_update(context, bits, 8);
    // Store state in digest
    md5_encode(digest, context->state, 16);

    // Zeroize sensitive information.
    bzero(context, sizeof(*context));
}

//! Основная md5-трансформация.
/*!
 * Функция преоразует состояние в зависимости от блока сообщения.
 *
 * @param [out] state текущее состояние
 * @param [in] block блок сообщения
 */
static void md5_transform(uint4 state[4], const unsigned char block[64]) {
    uint4 a = state[0];
    uint4 b = state[1];
    uint4 c = state[2];
    uint4 d = state[3];
    uint4 x[16];

    md5_decode(x, block, 64);

    /* Round 1 */
    a = md5_ff(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    d = md5_ff(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    c = md5_ff(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    b = md5_ff(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    a = md5_ff(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    d = md5_ff(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    c = md5_ff(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    b = md5_ff(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    a = md5_ff(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    d = md5_ff(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    c = md5_ff(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    b = md5_ff(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    a = md5_ff(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    d = md5_ff(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    c = md5_ff(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    b = md5_ff(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    a = md5_gg(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    d = md5_gg(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    c = md5_gg(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    b = md5_gg(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    a = md5_gg(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    d = md5_gg(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    c = md5_gg(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    b = md5_gg(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    a = md5_gg(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    d = md5_gg(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    c = md5_gg(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    b = md5_gg(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    a = md5_gg(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    d = md5_gg(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    c = md5_gg(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    b = md5_gg(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    a = md5_hh(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    d = md5_hh(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    c = md5_hh(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    b = md5_hh(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    a = md5_hh(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    d = md5_hh(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    c = md5_hh(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    b = md5_hh(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    a = md5_hh(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    d = md5_hh(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    c = md5_hh(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    b = md5_hh(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    a = md5_hh(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    d = md5_hh(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    c = md5_hh(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    b = md5_hh(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    a = md5_ii(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    d = md5_ii(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    c = md5_ii(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    b = md5_ii(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    a = md5_ii(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    d = md5_ii(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    c = md5_ii(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    b = md5_ii(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    a = md5_ii(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    d = md5_ii(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    c = md5_ii(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    b = md5_ii(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    a = md5_ii(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    d = md5_ii(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    c = md5_ii(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    b = md5_ii(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    // Zeroize sensitive information.
    bzero(x, sizeof(x));
}

//! Кодирование @p input (uint4) в @p ouput (unsigned char).
/*!
 * @param [out] output
 * @param [in] input
 * @param [in] len размер @p output и @p input. Должен быть кратен 4
 */
static void md5_encode(unsigned char* output, const uint4* input, size_t len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = input[i] & 0xff;
        output[j+1] = (input[i] >> 8) & 0xff;
        output[j+2] = (input[i] >> 16) & 0xff;
        output[j+3] = (input[i] >> 24) & 0xff;
    }
}

//! Декодирование @p input (unsigned char) в @p ouput (unit4).
/*!
 * @param [out] output
 * @param [in] input
 * @param [in] len размер @p output и @p input. Должен быть кратен 4
 */
static void md5_decode(uint4* output, const unsigned char* input, size_t len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = input[j] | (input[j+1] << 8) | (input[j+2] << 16) | (input[j+3] << 24);
    }
}
