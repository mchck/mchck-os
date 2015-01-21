#include <mchck.h>

const static __attribute__((section(".flash_config"), used))
struct NV_MemMap flash_config = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        NV_FSEC_SEC(FTFL_FSEC_SEC_UNSECURE) |
        NV_FSEC_FSLACC(FTFL_FSEC_FSLACC_GRANT) |
        NV_FSEC_MEEN(FTFL_FSEC_MEEN_ENABLE) |
        NV_FSEC_KEYEN(FTFL_FSEC_KEYEN_ENABLE),
        0xff & ~0x4 /* NV_FOPT_NMI_DIS_MASK is not defined in the header... */,
        0xff,
        0xff
};
