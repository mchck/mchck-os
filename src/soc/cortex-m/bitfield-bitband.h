#define bf_set(loc, pos, width, val)                                    \
        do {                                                            \
                if (__builtin_constant_p(&(loc)) &&                     \
                    bitband_addr_valid_p((uintptr_t)&(loc)) &&          \
                    (width) == 1) {                                     \
                        bitband_set((loc), (pos), (val));               \
                } else {                                                \
                        typeof(&(loc)) _locp = &(loc);                  \
                        uint32_t _locval = *_locp;                      \
                        *_locp = _bf_set1(_locval, (pos), ((1 << (width)) - 1) << (pos), (val)); \
                }                                                       \
        } while (0)

static inline uint32_t
_bf_set1(uint32_t locval, int bitpos, uint32_t bitmask, uint32_t val)
{
        if (__builtin_constant_p(bitpos) &&
            __builtin_constant_p(bitmask) &&
            (bitmask >> bitpos) == 1) {
                /* single bit op */
                if (val)
                        return (locval | bitmask);
                else
                        return (locval & ~bitmask);
        }
        return ((locval & ~bitmask) | (val << bitpos));
}

#define bf_get(loc, pos, width)                                         \
        ({                                                              \
                uint32_t _res;                                          \
                                                                        \
                if (__builtin_constant_p(&(loc)) &&                     \
                    bitband_addr_valid_p((uintptr_t)&(loc)) &&          \
                    (width) == 1) {                                     \
                        _res = bitband_get((loc), (pos));               \
                } else {                                                \
                        _res = ((loc) >> (pos)) & ((1 << (width)) - 1); \
                }                                                       \
                _res;                                                   \
        })
