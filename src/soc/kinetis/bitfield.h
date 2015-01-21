#define bf_set(loc, name, val)                                          \
        ({                                                              \
                typeof(&loc) locp = &loc;                               \
                uint32_t locval = *locp;                                \
                *locp = bf_set1(locval, name ## _SHIFT, name ## _MASK, val); \
        })

static inline uint32_t
bf_set1(uint32_t locval, int bitpos, uint32_t bitmask, uint32_t val)
{
        return ((locval & ~bitmask) | (val << bitpos));
}


#define bf_get(loc, name) \
        (((loc) & (name ## _MASK)) >> (name ## _SHIFT))
