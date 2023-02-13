/**
 * Adaptado de https://github.com/rshuston/FFT-C
 */


#ifndef _fft_h_
#define _fft_h_


#ifdef __cplusplus
extern "C" {
#endif


#include "complex.h"

/*
 * Find the log base 2 of an N-bit integer in O(lg(N)) operations using
 * multiply and lookup method
 */

unsigned log2_u(unsigned v);

typedef enum fft_dir {
    FFT_FORWARD,    /* kernel uses "-1" sign */
    FFT_INVERSE     /* kernel uses "+1" sign */
} fft_dir;


/* In-place FFT */
void ffti_f(complex_f data[], unsigned log2_N, fft_dir direction);

/* ... In-place FFT stage functions ... */
void ffti_copy_shuffle_f(complex_f src[], complex_f dst[], unsigned log2_N);
void ffti_shuffle_f(complex_f data[], unsigned log2_N);
void ffti_evaluate_f(complex_f data[], unsigned log2_N, fft_dir direction);

/* Recursive FFT */
void fftr_f(complex_f data[], unsigned log2_N, fft_dir direction);

/* Recursive FFT, user-supplied scratchpad buffer */
void fftrb_f(complex_f data[], unsigned log2_N, fft_dir direction, complex_f scratch[]);


#ifdef __cplusplus
}
#endif


#endif
