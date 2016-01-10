#include <mchck.h>

static uint32_t dma_used_chan;
static struct dma_ctx dma_ctx[DMA_NUM_CHAN];

void
dma_init(void)
{
        bf_set_reg(SIM_SCGC6, SIM_SCGC6_DMAMUX, 1);
        DMA_CR = DMA_CR_EMLM_MASK |
                DMA_CR_HOE_MASK |
                DMA_CR_ERCA_MASK |
                DMA_CR_EDBG_MASK;
        DMA_CERR = DMA_CERR_CAEI_MASK;
        int_enable(IRQ_DMA_Error);
}

static int
alloc_free_channel(void)
{
        int i;

        crit_enter();
        for (i = DMA_NUM_CHAN - 1; i >= 0; --i) {
                if ((dma_used_chan & (1 << i)) == 0)
                        break;
        }
        if (i >= 0)
                dma_used_chan |= 1 << i;
        crit_exit();

        return (i);
}

int
dma_ctx2idx(struct dma_ctx *ctx)
{
        if (ctx == NULL)
                return (-1);
        return (ctx - dma_ctx);
}

struct dma_ctx *
dma_setup(enum dmamux_channel dmach, const volatile void *src, void *dst, size_t elemsz, size_t nelem, enum dma_flags flags, dma_cb_t cb, void *cbdata)
{
        int idx = alloc_free_channel();
        if (idx < 0)
                return (NULL);

        struct dma_ctx *ctx = &dma_ctx[idx];

        ctx->cb = cb;
        ctx->cbdata = cbdata;

        uint32_t xfer_size, xfer_size_bit;
        if (((uintptr_t)src & 3) == 0 &&
            ((uintptr_t)dst & 3) == 0 &&
            (elemsz & 3) == 0) {
                xfer_size = 4;
                xfer_size_bit = 0b010;
        } else if (((uintptr_t)src & 1) == 0 &&
                   ((uintptr_t)dst & 1) == 0 &&
                   (elemsz & 1) == 0) {
                xfer_size = 2;
                xfer_size_bit = 0b001;
        } else {
                xfer_size = 1;
                xfer_size_bit = 0;
        }

        DMA_SADDR(idx) = (uintptr_t)src;
        DMA_SOFF(idx) = xfer_size;
        DMA_ATTR(idx) = DMA_ATTR_SSIZE(xfer_size_bit) | DMA_ATTR_DSIZE(xfer_size_bit);
        DMA_NBYTES_MLOFFYES(idx) =
                DMA_NBYTES_MLOFFYES_MLOFF(-elemsz) |
                DMA_NBYTES_MLOFFYES_NBYTES(elemsz) |
                (flags & DMA_SRC_STICKY ? DMA_NBYTES_MLOFFYES_SMLOE_MASK : 0) |
                (flags & DMA_DST_STICKY ? DMA_NBYTES_MLOFFYES_DMLOE_MASK : 0);
        DMA_SLAST(idx) = -elemsz * (flags & DMA_SRC_STICKY ? 1 : nelem);
        DMA_DADDR(idx) = (uintptr_t)dst;
        DMA_DOFF(idx) = elemsz;
        DMA_BITER_ELINKNO(idx) =
                DMA_CITER_ELINKNO(idx) =
                DMA_CITER_ELINKNO_CITER(nelem);
        DMA_DLAST_SGA(idx) = -elemsz * (flags & DMA_DST_STICKY ? 1 : nelem);
        DMA_CSR(idx) =
                (flags & DMA_LOOP ? 0 : DMA_CSR_DREQ_MASK) |
                (flags & DMA_DOUBLEBUF ? DMA_CSR_INTHALF_MASK : 0) |
                DMA_CSR_INTMAJOR_MASK;

        DMA_SEEI = DMA_SEEI_SEEI(idx);
        DMA_CINT = DMA_CINT_CINT(idx);
        int_enable(IRQ_DMA0 + idx);
        DMAMUX_CHCFG(idx) = DMAMUX_CHCFG_SOURCE(dmach) | DMAMUX_CHCFG_ENBL_MASK;
        DMA_SERQ = DMA_SERQ_SERQ(idx);

        return (ctx);
}

static void
DMA_Handler(int idx)
{
        struct dma_ctx *ctx = &dma_ctx[idx];
        enum dma_status status;

        DMA_CINT = DMA_CINT_CINT(idx);
        if (DMA_CSR(idx) & DMA_CSR_DONE_MASK)
                status = DMA_STATUS_FULL;
        else
                status = DMA_STATUS_HALF;
        if (ctx->cb)
                ctx->cb(ctx, status, ctx->cbdata);
}

void DMA0_Handler(void) {DMA_Handler(0);}
void DMA1_Handler(void) {DMA_Handler(1);}
void DMA2_Handler(void) {DMA_Handler(2);}
void DMA3_Handler(void) {DMA_Handler(3);}

#if DMA_NUM_CHAN > 4
void DMA4_Handler(void) {DMA_Handler(4);}
void DMA5_Handler(void) {DMA_Handler(5);}
void DMA6_Handler(void) {DMA_Handler(6);}
void DMA7_Handler(void) {DMA_Handler(7);}
void DMA8_Handler(void) {DMA_Handler(8);}
void DMA9_Handler(void) {DMA_Handler(9);}
void DMA10_Handler(void) {DMA_Handler(10);}
void DMA11_Handler(void) {DMA_Handler(11);}
void DMA12_Handler(void) {DMA_Handler(12);}
void DMA13_Handler(void) {DMA_Handler(13);}
void DMA14_Handler(void) {DMA_Handler(14);}
void DMA15_Handler(void) {DMA_Handler(15);}
#endif

void
DMA_error_Handler(void)
{
        for (int idx = 0; idx < DMA_NUM_CHAN; ++idx) {
                if (DMA_ERR & (1 << idx)) {
                        struct dma_ctx *ctx = &dma_ctx[idx];

                        DMA_CERR = DMA_CERR_CERR(idx);
                        if (ctx->cb)
                                ctx->cb(ctx, DMA_STATUS_ERROR, ctx->cbdata);
                }
        }
}
