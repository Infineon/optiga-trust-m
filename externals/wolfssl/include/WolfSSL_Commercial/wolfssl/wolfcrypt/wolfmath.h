/* wolfmath.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * http://www.wolfssl.com
 */


#ifndef __WOLFMATH_H__
#define __WOLFMATH_H__


/* common math functions */
WOLFSSL_LOCAL int get_digit_count(mp_int* a);
WOLFSSL_LOCAL mp_digit get_digit(mp_int* a, int n);
WOLFSSL_LOCAL int get_rand_digit(WC_RNG* rng, mp_digit* d);
WOLFSSL_LOCAL int mp_rand(mp_int* a, int digits, WC_RNG* rng);


#endif /* __WOLFMATH_H__ */
