#include <mchck.h>

const static __attribute__((section(".flash_config"), used))
NV_Type flash_config = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        .FSEC = NV_FSEC_SEC(NV_FSEC_SEC_UNSECURE) |
        NV_FSEC_FSLACC(NV_FSEC_FSLACC_GRANT) |
        NV_FSEC_MEEN(NV_FSEC_MEEN_ENABLE) |
        NV_FSEC_KEYEN(NV_FSEC_KEYEN_ENABLE),
        .FOPT = (0xff &
                 ~(NV_FOPT_NMI_DIS_MASK |
                   NV_FOPT_BOOTSRC_SEL_MASK)) |
        NV_FOPT_BOOTSRC_SEL(NV_FOPT_BOOTSRC_FLASH)
};
