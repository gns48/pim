/**
 * @file   siphash.h
 * @author Gleb Semenov <gleb.semenov@gmail.com>
 * @date   Thu Jan  9 19:31:01 2020
 * 
 * @brief  Hashing algorithm module - SipHash, definitions
 * 
 * 
 */

#ifndef __SIPHASH_H__
#define __SIPHASH_H__

/**
 * SipGash key - 128 bit
 * a pair of uint64_t values
 */
typedef struct uint128_t {
    uint64_t p_low;
    uint64_t p_high;
} uint128_t;

/** 
 * pim_siphash64 - calculate SipHash for given block of data
 * 
 * @param in - ptr to data
 * @param inlen - data length
 * @param k - key
 * @param out - Hash value as array of bytes
 * 
 * @return  Hash value as 64 bit long unsigned integer//@}
 */
uint64_t pim_siphash64(const uint8_t *in, const size_t inlen, const uint128_t *k, uint8_t *out);

#endif // #ifndef __SIPHASH_H__







