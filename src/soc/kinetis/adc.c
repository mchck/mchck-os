#include <mchck.h>

struct adc_ctx {
        ADC_Type * const adc;
        volatile struct {
                adc_result_cb_t *cb;
                void *cbdata;
                int active;
        } stat_a, stat_b;
        unsigned long fract scale;
};

struct adc_ctx adc0_ctx = {ADC0};
#ifdef ADC1
struct adc_ctx adc1_ctx = {ADC1};
#endif

static void
adc_calibrate_cb(struct adc_ctx *ctx, uint16_t val, int error, void *cbdata)
{
        /* base ADC calibration */
        if (error || bf_get_reg(ctx->adc->SC3, ADC_SC3_CALF))
                return;

        uint32_t calib = 0;
        calib += ctx->adc->CLPD;
        calib += ctx->adc->CLPS;
        calib += ctx->adc->CLP4;
        calib += ctx->adc->CLP3;
        calib += ctx->adc->CLP2;
        calib += ctx->adc->CLP1;
        calib += ctx->adc->CLP0;
        ctx->adc->PG = (calib >> 1) | 0x8000;

#if defined(ADC_CLMD_REG)
        calib = 0;
        calib += ctx->adc->CLMD;
        calib += ctx->adc->CLMS;
        calib += ctx->adc->CLM4;
        calib += ctx->adc->CLM3;
        calib += ctx->adc->CLM2;
        calib += ctx->adc->CLM1;
        calib += ctx->adc->CLM0;
        ctx->adc->MG = (calib >> 1) | 0x8000;
#endif

        adc_calibration_done(ctx);
}

static void
adc_calibrate_voltage_cb(struct adc_ctx *ctx, uint16_t val, int error, void *cbdata)
{
        /* disable band-gap buffer */
        bf_set_reg(PMC->REGSC, PMC_REGSC_BGBE, 1);

        if (error)
                return;

        /**
         * The value we read is V_BG = 1V.  This allows us to
         * calibrate our voltage scale, even if we're running
         * unregulated from battery.  Do not mistake the V_BG bandgap
         * voltage with VREF's bandgap voltage, which is 1.2V.  Also,
         * V_REFH is not VREF's output voltage, but the ADC reference
         * voltage, by default tied to AVDD.
         *
         * (1) bg = V_BG / V_REFH
         * (2) x = V_x / V_REFH
         *     V_x = x * V_REFH
         *     V_x = x * V_BG / bg       (with (1))
         *     V_x = x / bg              (with V_BG = 1V)
         * (3) scale = 1 / bg
         * (4) V_x = x * scale           (with (3))
         *
         * We just store scale and use it in subsequent conversions (4).
         */

        ctx->scale = 1.0lR / val;

        adc_calibration_done(ctx);
}

static void
calibration_done(struct adc_ctx *ctx)
{
}

void adc_calibration_done(void *) __attribute__((weak, alias("calibration_done")));

void
adc_calibrate_voltage(struct adc_ctx *ctx, unsigned accum reference)
{
        if (reference != 0) {
                ctx->scale = (unsigned long accum)reference >> 16;
        } else {
                adc_sample_start(ctx, ADC_BANDGAP, adc_calibrate_voltage_cb, NULL);
        }
}

unsigned accum
adc_as_voltage(struct adc_ctx *ctx, uint16_t val)
{
        return (val * ctx->scale);
}

void
adc_init(struct adc_ctx *ctx)
{
        /**
         * Enable bandgap buffer.  We need this later to calibrate our
         * reference scale.  However, we start it now, so that it will
         * have time to stabilize. */
        bf_set_reg(PMC->REGSC, PMC_REGSC_BGBE, 1);

        switch ((uintptr_t)ctx->adc) {
        case ADC0_BASE:
                /* enable clock */
                bf_set_reg(SIM->SCGC6, SIM_SCGC6_ADC0, 1);
                /* enable interrupt handler */
                int_enable(IRQ_ADC0);
                break;
#ifdef ADC1
        case ADC1_BASE:
                /* enable clock */
                bf_set_reg(SIM->SCGC6, SIM_SCGC6_ADC1, 1);
                /* enable interrupt handler */
                int_enable(IRQ_ADC1);
                break;
#endif
        }

        /* setup ADC calibration */
        adc_sample_prepare(ctx, ADC_MODE_SAMPLE_LONG | ADC_AVG_32);
        ctx->stat_a.cb = adc_calibrate_cb;
        ctx->stat_a.active = 1;

        /* enable interrupt */
        ctx->adc->SC1[0] =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(ADC_ADCH_DISABLED); /* do not start ADC */

        /* start calibration */
        bf_set_reg(ctx->adc->SC3, ADC_SC3_CAL, 1);
}

void
adc_sample_prepare(struct adc_ctx *ctx, enum adc_mode mode)
{
        /* XXX hack: if we have negative range, we have 16 bits? */
#if defined(ADC_CLMD_REG)
        const uint32_t bits = ADC_BIT_16;
#else
        const uint32_t bits = ADC_BIT_12;
#endif

        ctx->adc->CFG1 =
                ((mode & ADC_MODE_POWER_LOW) ? ADC_CFG1_ADLPC_MASK : 0) |
                ADC_CFG1_ADIV(ADC_DIV_1) |
                ((mode & ADC_MODE_SAMPLE_LONG) ? ADC_CFG1_ADLSMP_MASK : 0) |
                ADC_CFG1_MODE(bits) |
                ADC_CFG1_ADICLK(ADC_CLK_ADACK);
        ctx->adc->CFG2 =
                ADC_CFG2_MUXSEL_MASK | /* we only have b channels on the K20 */
                ((mode & ADC_MODE_KEEP_CLOCK) ? ADC_CFG2_ADACKEN_MASK : 0) |
                ((mode & ADC_MODE_SPEED_HIGH) ? ADC_CFG2_ADHSC_MASK : 0);
        ctx->adc->SC2 = ADC_SC2_REFSEL(ADC_REF_DEFAULT);
        ctx->adc->SC3 =
                ((mode & ADC_MODE_CONTINUOUS) ? ADC_SC3_ADCO_MASK : 0) |
                ((mode & ADC_MODE__AVG) ? ADC_SC3_AVGE_MASK : 0 ) |
                ADC_SC3_AVGS((mode & ADC_MODE__AVG_MASK) / ADC_MODE__AVG / 2); /* XXX ugly */
}

int
adc_sample_start(struct adc_ctx *ctx, enum adc_channel channel, adc_result_cb_t *cb, void *cbdata)
{
        if (ctx->stat_a.active)
                return (-1);

        /* XXX check for previous conversion running */
        ctx->stat_a.cb = cb;
        ctx->stat_a.cbdata = cbdata;

        /* trigger conversion */
        ctx->adc->SC1[0] =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(channel);
        /* XXX diff */
        return (0);
}

int
adc_sample_abort(struct adc_ctx *ctx)
{
        crit_enter();
        ctx->adc->SC1[0] = ADC_SC1_ADCH(ADC_ADCH_DISABLED);
        if (ctx->stat_a.active) {
                adc_result_cb_t *cb = ctx->stat_a.cb;
                void *cbdata = ctx->stat_a.cbdata;

                ctx->stat_a.active = 0;
                crit_exit();
                cb(ctx, 0, 1, cbdata);
                return (0);
        } else {
                crit_exit();
                return (-1);
        }
}

void
adc_interrupt_handler(struct adc_ctx *ctx)
{
        if (bf_get_reg(ctx->adc->SC1[0], ADC_SC1_COCO)) {
                adc_result_cb_t *cb;
                void *cbdata;
                uint16_t val;

                crit_enter();
                cb = ctx->stat_a.cb;
                cbdata = ctx->stat_a.cbdata;
                if (!bf_get_reg(ctx->adc->SC3, ADC_SC3_ADCO))
                        ctx->stat_a.active = 0;
                val = ctx->adc->R[0];  /* clears interrupt */
                crit_exit();

                cb(ctx, val, 0, cbdata);
        }
        /* XXX repeat for sb1 */
}

void
ADC0_Handler(void)
{
        adc_interrupt_handler(&adc0_ctx);
}

#ifdef ADC1
void
ADC1_Handler(void)
{
        adc_interrupt_handler(&adc1_ctx);
}
#endif
