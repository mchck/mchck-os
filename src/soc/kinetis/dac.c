#include <mchck.h>

struct dac_ctx {
        DAC_Type * const dac;
};

struct dac_ctx dac0_ctx = {DAC0};
struct dac_ctx dac1_ctx = {DAC1};

void
dac_init(struct dac_ctx *ctx)
{
        switch ((uintptr_t)ctx->dac) {
        case DAC0_BASE:
                bf_set_reg(SIM_SCGC6, SIM_SCGC6_DAC0, 1);
                break;
        case DAC1_BASE:
                bf_set_reg(SIM_SCGC6, SIM_SCGC6_DAC1, 1);
                break;
        }
}

void
dac_enable(struct dac_ctx *ctx, int enable)
{
        bf_set_reg(DAC_C0_REG(ctx->dac), DAC_C0_DACEN, enable);
}

void
dac_output(struct dac_ctx *ctx, uint16_t val)
{
        DAC_DATL_REG(ctx->dac, 0) = val;
        DAC_DATH_REG(ctx->dac, 0) = val >> 8;
}
