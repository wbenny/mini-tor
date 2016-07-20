/*
 * Copyright (c) 2007, Cameron Rich
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BIGINT_HEADER
#define BIGINT_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maintain a number of precomputed variables when doing reduction */
#define BIGINT_M_OFFSET     0    /**< Normal modulo offset. */
#define BIGINT_NUM_MODS     1

/* Architecture specific functions for big ints */
#define COMP_RADIX          4294967296ULL         /**< Max component + 1 */
#define COMP_MAX            0xFFFFFFFFFFFFFFFFULL/**< (Max dbl comp -1) */
#define COMP_BIT_SIZE       32  /**< Number of bits in a component. */
#define COMP_BYTE_SIZE      4   /**< Number of bytes in a component. */
#define COMP_NUM_NIBBLES    8   /**< Used For diagnostics only. */
typedef uint32_t comp;	        /**< A single precision component. */
typedef uint64_t long_comp;     /**< A double precision component. */
typedef int64_t slong_comp;     /**< A signed double precision component. */

/**
 * @struct  _bigint
 * @brief A big integer basic object
 */
struct _bigint
{
    struct _bigint* next;       /**< The next bigint in the cache. */
    short size;                 /**< The number of components in this bigint. */
    short max_comps;            /**< The heapsize allocated for this bigint */
    int refs;                   /**< An internal reference count. */
    comp* comps;                /**< A ptr to the actual component data */
};

typedef struct _bigint bigint;  /**< An alias for _bigint */

/**
 * Maintains the state of the cache, and a number of variables used in
 * reduction.
 */
typedef struct /**< A big integer "session" context. */
{
    bigint *active_list;                    /**< Bigints currently used. */
    bigint *free_list;                      /**< Bigints not used. */
    bigint *bi_radix;                       /**< The radix used. */
    bigint *bi_mod[BIGINT_NUM_MODS];        /**< modulus */

    bigint *bi_normalised_mod[BIGINT_NUM_MODS]; /**< Normalised mod storage. */
    bigint **g;                 /**< Used by sliding-window. */
    int window;                 /**< The size of the sliding window */
    int active_count;           /**< Number of active bigints. */
    int free_count;             /**< Number of free bigints. */

    uint8_t mod_offset;         /**< The mod offset we are using */
} BI_CTX;

#define PERMANENT           0x7FFF55AA  /**< A magic number for permanents. */

BI_CTX *bi_initialize(void);
void bi_terminate(BI_CTX *ctx);
void bi_permanent(bigint *bi);
void bi_depermanent(bigint *bi);
void bi_clear_cache(BI_CTX *ctx);
void bi_free(BI_CTX *ctx, bigint *bi);
bigint *bi_copy(bigint *bi);
bigint *bi_clone(BI_CTX *ctx, const bigint *bi);
void bi_export(BI_CTX *ctx, bigint *bi, uint8_t *data, int size);
bigint *bi_import(BI_CTX *ctx, const uint8_t *data, int len);
bigint *int_to_bi(BI_CTX *ctx, comp i);

/* the functions that actually do something interesting */
bigint *bi_add(BI_CTX *ctx, bigint *bia, bigint *bib);
bigint *bi_subtract(BI_CTX *ctx, bigint *bia,
        bigint *bib, int *is_negative);
bigint *bi_divide(BI_CTX *ctx, bigint *bia, bigint *bim, int is_mod);
bigint *bi_multiply(BI_CTX *ctx, bigint *bia, bigint *bib);
bigint *bi_mod_power(BI_CTX *ctx, bigint *bi, bigint *biexp);
bigint *bi_mod_power2(BI_CTX *ctx, bigint *bi, bigint *bim, bigint *biexp);
int bi_compare(bigint *bia, bigint *bib);
void bi_set_mod(BI_CTX *ctx, bigint *bim, int mod_offset);
void bi_free_mod(BI_CTX *ctx, int mod_offset);

/**
 * @def bi_mod
 * Find the residue of B. bi_set_mod() must be called before hand.
 */
#define bi_mod(A, B)      bi_divide(A, B, ctx->bi_mod[ctx->mod_offset], 1)

/**
 * bi_residue() is technically the same as bi_mod(), but it uses the
 * appropriate reduction technique (which is bi_mod() when doing classical
 * reduction).
 */
#define bi_residue(A, B)         bi_mod(A, B)

#define bi_square(A, B)     bi_multiply(A, bi_copy(B), B)

#ifdef __cplusplus
}
#endif

#endif
