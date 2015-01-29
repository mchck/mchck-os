#define bme_wop(loc, val, op)                                   \
        do {                                                    \
                uintptr_t _addr = (uintptr_t)&(loc);            \
                _addr |= ((op) << 26);                          \
                volatile typeof((loc)) *locp = (void *)_addr;   \
                *locp = (val);                                  \
        } while (0)

#define bme_rop(loc, bitpos, op)                                \
        ({                                                      \
                uintptr_t _addr = (uintptr_t)&(loc);            \
                _addr |= ((op) << 26) | ((bitpos) << 21);       \
                volatile typeof((loc)) *locp = (void *)_addr;   \
                *locp;                                          \
        })

#define bme_op_possible(loc)                                            \
        (__builtin_constant_p(&loc) && !((uintptr_t)&loc & 0x9ff00000))

#define bme_and(loc, val) bme_wop(loc, val, 0b001)
#define bme_or(loc, val) bme_wop(loc, val, 0b010)
#define bme_xor(loc, val) bme_wop(loc, val, 0b011)
#define bme_clearbit(loc, bitpos) bme_rop(loc, bitpos, 0b010)
#define bme_setbit(loc, bitpos) bme_rop(loc, bitpos, 0b011)

#define bme_bf_set(loc, bitpos, width, val)                             \
        do {                                                            \
                uintptr_t _addr = (uintptr_t)&(loc);                    \
                _addr |= (1 << 28) | ((bitpos) << 23) | (((width) - 1) << 19); \
                volatile typeof((loc)) *locp = (void *)_addr;           \
                *locp = (val) << bitpos;                                \
        } while (0)

#define bme_bf_get(loc, bitpos, width)                                  \
        ({                                                              \
                uintptr_t _addr = (uintptr_t)&(loc);                    \
                _addr |= (1 << 28) | ((bitpos) << 23) | (((width) - 1) << 19); \
                volatile typeof((loc)) *locp = (void *)_addr;           \
                *locp;                                                  \
        })

#define bme_bf_possible(loc)                                            \
        (__builtin_constant_p(&loc) && !((uintptr_t)&loc & 0x9ff80000))
