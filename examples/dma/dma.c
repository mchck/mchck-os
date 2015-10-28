#include <mchck.h>

static uint16_t dstbuf[500];

struct sema adc_start_sema;

static void
dma_done(struct dma_ctx *ctx, enum dma_status status, void *cbdata)
{
        onboard_led(status == DMA_STATUS_HALF);
}

void
adc_calibration_done(void)
{
        sema_wake(&adc_start_sema);
}

void
main(void)
{
        adc_init();
        dma_init();
        enter_thread_mode();

        sema_wait(&adc_start_sema);

        struct dma_ctx *ctx;
        ctx = dma_setup(DMAMUX_ADC0, &ADC_R_REG(ADC0, 0), dstbuf, 2, sizeof(dstbuf)/2, DMA_SRC_STICKY | DMA_DOUBLEBUF | DMA_LOOP, dma_done, NULL);

        adc_sample_prepare(ADC_MODE_CONTINUOUS);
        bf_set_reg(ADC_SC2_REG(ADC0), ADC_SC2_DMAEN, 1);
        ADC_SC1_REG(ADC0, 0) = ADC_SC1_ADCH(0) | ADC_SC1_DIFF_MASK;

        wait(main);
}
