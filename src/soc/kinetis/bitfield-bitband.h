#define bf_set(loc, name, val)                                          \
        {                                                               \
        	uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                if (__builtin_constant_p(&loc) &&                       \
                    bitband_addr_valid_p((uintptr_t)&loc) &&            \
                    (1 << _bitpos) == _bitmask) {                       \
                        bitband_set(loc, _bitpos, val);                 \
                } else {                                                \
                        typeof(&loc) locp = &loc;                       \
                        uint32_t _locval = *locp;                       \
                        *locp = bf_set1(_locval, _bitpos, _bitmask, val); \
                }                                                       \
        }

static inline int
bf_single_bit_p(int bitpos, uint32_t bitmask)
{
        return (__builtin_constant_p(bitmask) &&
                __builtin_constant_p(bitpos) &&
                bitmask == (1 << bitpos));
}

static inline uint32_t
bf_set1(uint32_t locval, int bitpos, uint32_t bitmask, uint32_t val)
{
        if (bf_single_bit_p(bitpos, bitmask)) {
                /* single bit op */
                if (val)
                        return (locval | bitmask);
                else
                        return (locval & ~bitmask);
        }
        return ((locval & ~bitmask) | (val << bitpos));
}


#define bf_get(loc, name)                                               \
        ({                                                              \
        	uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                uint32_t _res;                                          \
                                                                        \
                if (__builtin_constant_p(&loc) &&                       \
                    bitband_addr_valid_p((uintptr_t)&loc) &&            \
                    (1 << _bitpos) == _bitmask) {                       \
                        _res = bitband_get(loc, _bitpos);               \
                } else {                                                \
                        _res = (((loc) & (name ## _MASK)) >> (name ## _SHIFT)); \
                }                                                       \
                _res;                                                   \
        })
