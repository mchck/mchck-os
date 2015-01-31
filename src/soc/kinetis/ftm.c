#include <mchck.h>

void
ftm_init(void)
{
        bf_set(SIM_SCGC6, SIM_SCGC6_FTM0, 1);     // turn on clock to ftm0
	FTM0_MOD = 0xffff;      // set counter modulo value to maximum
	FTM0_CNTIN = 0;

        for (int i = 0; i < FTM_NUM_CH; i++) {
                bf_set(FTM0_CnSC(i), FTM_CnSC_MSB, 1);
                bf_set(FTM0_CnSC(i), FTM_CnSC_ELSB, 1);
                FTM0_CnV(i) = 0x8000;
        }

        bf_set(FTM0_SC, FTM_SC_CLKS, FTM_CLKS_SYSTEM);       // select clock for counter
}

void
ftm_set_raw(enum FTM_CH_t channels, uint16_t duty)
{
        for (int i = 0; i < FTM_NUM_CH; i++) {
                if(channels & (1 << i))
                        FTM0_CnV(i) = duty;
        }
}
