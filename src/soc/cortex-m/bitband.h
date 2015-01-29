static inline int
bitband_addr_valid_p(uintptr_t addr)
{
        switch (addr & 0xf0000000) {
        case 0x20000000:
        case 0x40000000:
                return 1;
        default:
                return 0;
        }
}

static inline uintptr_t
bitband_addr(uintptr_t addr, uint32_t bitpos)
{
        uintptr_t offset = addr & 0xfffff;
        uintptr_t addr_section = addr & 0xf0000000;
        return (addr_section | 0x02000000 | (offset * 32) | (bitpos * 4));
}

#define bitband_set(loc, bitpos, value)                                 \
        do {                                                            \
                volatile typeof(loc) *p = (void *)bitband_addr((uintptr_t)&loc, bitpos); \
                *p = value;                                             \
        } while (0)

#define bitband_get(loc, bitpos)                                        \
        ({                                                              \
                volatile typeof(loc) *p = (void *)bitband_addr((uintptr_t)&loc, bitpos); \
                *p;                                                     \
        })
