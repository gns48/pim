/**
 * pi3's Linux kernel Runtime Guard
 *
 * Component:
 *  - Hashing algorithm module - SipHash
 *
 * Notes:
 *  - Current Algorithm:
 *     https://131002.net/siphash/
 *
 */

#include <linux/types.h>
#include "siphash.h"

/* default: SipHash-2-4 */
#define cROUNDS 2
#define dROUNDS 4

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define U32TO8_LE(p, v)                                                 \
    (p)[0] = (uint8_t)((v));                                            \
    (p)[1] = (uint8_t)((v) >> 8);                                       \
    (p)[2] = (uint8_t)((v) >> 16);                                      \
    (p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v)                                                 \
    U32TO8_LE((p), (uint32_t)((v)));                                    \
    U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define U8TO64_LE(p)                                                    \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |                 \
     ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) |          \
     ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) |          \
     ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define SIPROUND                                                        \
    do {                                                                \
        v0 += v1;                                                       \
        v1 = ROTL(v1, 13);                                              \
        v1 ^= v0;                                                       \
        v0 = ROTL(v0, 32);                                              \
        v2 += v3;                                                       \
        v3 = ROTL(v3, 16);                                              \
        v3 ^= v2;                                                       \
        v0 += v3;                                                       \
        v3 = ROTL(v3, 21);                                              \
        v3 ^= v0;                                                       \
        v2 += v1;                                                       \
        v1 = ROTL(v1, 17);                                              \
        v1 ^= v2;                                                       \
        v2 = ROTL(v2, 32);                                              \
    } while (0)

static const uint64_t V0 = 0x736f6d6570736575ULL;
static const uint64_t V1 = 0x646f72616e646f6dULL;   
static const uint64_t V2 = 0x6c7967656e657261ULL;
static const uint64_t V3 = 0x7465646279746573ULL;
    
// Siphash, 64 bit version
uint64_t pim_siphash64(const uint8_t *in, const size_t inlen, const uint128_t *k, uint8_t *out) {
    uint64_t v0 = V0;
    uint64_t v1 = V1;
    uint64_t v2 = V2;
    uint64_t v3 = V3;
    uint64_t k0 = k->p_low;
    uint64_t k1 = k->p_high;
    uint64_t m;
    int i;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    const int left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    for (; in != end; in += 8) {
        m = U8TO64_LE(in);
        v3 ^= m;

        for (i = 0; i < cROUNDS; ++i)
            SIPROUND;
      
        v0 ^= m;
    }

    switch (left) {
    case 7:
        b |= ((uint64_t)in[6]) << 48;
        /* FALLTHROUGH */
    case 6:
        b |= ((uint64_t)in[5]) << 40;
        /* FALLTHROUGH */
    case 5:
        b |= ((uint64_t)in[4]) << 32;
        /* FALLTHROUGH */
    case 4:
        b |= ((uint64_t)in[3]) << 24;
        /* FALLTHROUGH */
    case 3:
        b |= ((uint64_t)in[2]) << 16;
        /* FALLTHROUGH */
    case 2:
        b |= ((uint64_t)in[1]) << 8;
        /* FALLTHROUGH */
    case 1:
        b |= ((uint64_t)in[0]);
        break;
    case 0:
        break;
    }

    v3 ^= b;

    for (i = 0; i < cROUNDS; ++i)
        SIPROUND;

    v0 ^= b;
    v2 ^= 0xff;

    for (i = 0; i < dROUNDS; ++i)
        SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
    U64TO8_LE(out, b);

    return *(uint64_t*)out;
    
}







