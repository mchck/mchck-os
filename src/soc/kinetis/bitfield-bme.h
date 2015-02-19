#define bf_mask_to_width(mask)                                  \
        ((mask) == 0b1 ? 1 :                                    \
                (mask) == 0b11 ? 2 :                            \
                (mask) == 0b111 ? 3 :                           \
                (mask) == 0b1111 ? 4 :                          \
                (mask) == 0b11111 ? 5 :                         \
                (mask) == 0b111111 ? 6 :                        \
                (mask) == 0b1111111 ? 7 :                       \
                (mask) == 0b11111111 ? 8 :                      \
                (mask) == 0b111111111 ? 9 :                     \
                (mask) == 0b1111111111 ? 10 :                   \
                (mask) == 0b11111111111 ? 11 :                  \
                (mask) == 0b111111111111 ? 12 :                 \
                (mask) == 0b1111111111111 ? 13 :                \
                (mask) == 0b11111111111111 ? 14 :               \
                (mask) == 0b111111111111111 ? 15 :              \
                (mask) == 0b1111111111111111 ? 16 : 0xffffffff)

#define bf_set_reg(loc, name, val)                                      \
        do {                                                            \
                uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                                                                        \
                bf_set(loc, _bitpos, bf_mask_to_width(_bitmask >> _bitpos), val); \
        } while (0)

#define bf_set(loc, pos, width, val)                                    \
        do {                                                            \
                if (__builtin_constant_p(val) &&                        \
                    __builtin_constant_p(pos) &&                        \
                    __builtin_constant_p(width) && width == 1 &&        \
                    bme_op_possible(loc)) {                             \
                        if (val)                                        \
                                bme_setbit(loc, pos);                   \
                        else                                            \
                                bme_clearbit(loc, pos);                 \
                } else if (__builtin_constant_p(val) &&                 \
                           bme_bf_possible(loc)) {                      \
                        bme_bf_set(loc, pos, width, val);               \
                } else {                                                \
                        typeof(&loc) locp = &loc;                       \
                        uint32_t _locval = *locp;                       \
                        *locp = bf_set1(_locval, pos, width, val);      \
                }                                                       \
        } while (0)

static inline int
bf_single_bit_p(int bitpos, uint32_t bitmask)
{
        return (__builtin_constant_p(bitmask) &&
                __builtin_constant_p(bitpos) &&
                bitmask == (1 << bitpos));
}

static inline uint32_t
bf_set1(uint32_t locval, int bitpos, uint32_t width, uint32_t val)
{
        uint32_t bitmask = ((1 << width) - 1) << bitpos;

        if (bf_single_bit_p(bitpos, bitmask)) {
                /* single bit op */
                if (val)
                        return (locval | bitmask);
                else
                        return (locval & ~bitmask);
        }
        return ((locval & ~bitmask) | (val << bitpos));
}


#define bf_get_reg(loc, name)                                           \
        ({                                                              \
                uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                                                                        \
                bf_get(loc, _bitpos, bf_mask_to_width(_bitmask >> _bitpos)); \
        })

#define bf_get(loc, pos, width)                                         \
        ({                                                              \
                uint32_t _res;                                          \
                                                                        \
                if (bme_bf_possible(loc))                               \
                        _res = bme_bf_get(loc, pos, width);             \
                else                                                    \
                        _res = ((loc) >> pos) & ((1 << width) - 1);     \
                _res;                                                   \
        })


#define bf_or(loc, val)                         \
        do {                                    \
                if (bme_op_possible(loc))       \
                        bme_or(loc, val);       \
                else                            \
                        loc |= val;             \
        } while (0)

#define bf_and(loc, val)                        \
        do {                                    \
                if (bme_op_possible(loc))       \
                        bme_and(loc, val);      \
                else                            \
                        loc &= val;             \
        } while (0)

#define bf_xor(loc, val)                        \
        do {                                    \
                if (bme_op_possible(loc))       \
                        bme_xor(loc, val);      \
                else                            \
                        loc ^= val;             \
        } while (0)
