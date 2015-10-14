#include <mchck.h>

const __attribute__((section(".flash_config"), used, externally_visible, aligned(16)))
NV_Type flash_config = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        .FSEC =
        NV_FSEC_SEC(NV_FSEC_SEC_UNSECURE) |
        NV_FSEC_FSLACC(NV_FSEC_FSLACC_GRANT) |
        NV_FSEC_MEEN(NV_FSEC_MEEN_ENABLE) |
        NV_FSEC_KEYEN(NV_FSEC_KEYEN_ENABLE),
        .FOPT = 0xff & ~NV_FOPT_NMI_DIS(1),
};
