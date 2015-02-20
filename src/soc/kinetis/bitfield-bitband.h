#define bf_set_reg(loc, name, val)                                      \
        do {                                                            \
        	uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                if (__builtin_constant_p(&(loc)) &&                     \
                    bitband_addr_valid_p((uintptr_t)&(loc)) &&          \
                    (1 << _bitpos) == _bitmask) {                       \
                        bitband_set((loc), _bitpos, (val));             \
                } else {                                                \
                        typeof(&(loc)) _locp = &(loc);                  \
                        uint32_t _locval = *_locp;                      \
                        *_locp = _bf_set1(_locval, _bitpos, _bitmask, (val)); \
                }                                                       \
        } while (0)

#define bf_get_reg(loc, name)                                           \
        ({                                                              \
        	uint32_t _bitpos = name ## _SHIFT;                      \
                uint32_t _bitmask = name ## _MASK;                      \
                uint32_t _res;                                          \
                                                                        \
                if (__builtin_constant_p(&(loc)) &&                     \
                    bitband_addr_valid_p((uintptr_t)&(loc)) &&          \
                    (1 << _bitpos) == _bitmask) {                       \
                        _res = bitband_get((loc), _bitpos);             \
                } else {                                                \
                        _res = (((loc) & (name ## _MASK)) >> (name ## _SHIFT)); \
                }                                                       \
                _res;                                                   \
        })
