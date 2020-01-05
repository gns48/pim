/**
 * 
 * Component:
 *  - Hashing algorithm module - SipHash
 *
 * Notes:
 *  - Current Algorithm:
 *     *) https://131002.net/siphash/
 *
 */

#ifndef __SIPHASH_H__
#define __SIPHASH_H__

typedef struct uint128_t {
    uint64_t p_low;
    uint64_t p_high;
} uint128_t;

uint64_t pim_siphash64(const uint8_t *in, const size_t inlen, const uint128_t *k, uint8_t *out);

#endif // #ifndef __SIPHASH_H__







