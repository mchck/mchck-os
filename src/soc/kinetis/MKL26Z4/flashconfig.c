#include <mchck.h>

const static __attribute__((section(".flash_config"), used))
struct NV_MemMap flash_config = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        NV_FSEC_SEC(FTFA_FSEC_SEC_UNSECURE) |
        NV_FSEC_FSLACC(FTFA_FSEC_FSLACC_GRANT) |
        NV_FSEC_MEEN(FTFA_FSEC_MEEN_ENABLE) |
        NV_FSEC_KEYEN(FTFA_FSEC_KEYEN_ENABLE),
        0xff & ~NV_FOPT_NMI_DIS_MASK
};
