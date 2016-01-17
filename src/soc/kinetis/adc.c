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
struct adc_ctx adc1_ctx = {ADC1};

static void
adc_calibrate_cb(struct adc_ctx *ctx, uint16_t val, int error, void *cbdata)
{
        /* base ADC calibration */
        if (error || bf_get_reg(ADC_SC3_REG(ctx->adc), ADC_SC3_CALF))
                return;

        uint32_t calib = 0;
        calib += ADC_CLPD_REG(ctx->adc);
        calib += ADC_CLPS_REG(ctx->adc);
        calib += ADC_CLP4_REG(ctx->adc);
        calib += ADC_CLP3_REG(ctx->adc);
        calib += ADC_CLP2_REG(ctx->adc);
        calib += ADC_CLP1_REG(ctx->adc);
        calib += ADC_CLP0_REG(ctx->adc);
        ADC_PG_REG(ctx->adc) = (calib >> 1) | 0x8000;

        calib = 0;
        calib += ADC_CLMD_REG(ctx->adc);
        calib += ADC_CLMS_REG(ctx->adc);
        calib += ADC_CLM4_REG(ctx->adc);
        calib += ADC_CLM3_REG(ctx->adc);
        calib += ADC_CLM2_REG(ctx->adc);
        calib += ADC_CLM1_REG(ctx->adc);
        calib += ADC_CLM0_REG(ctx->adc);
        ADC_MG_REG(ctx->adc) = (calib >> 1) | 0x8000;
}

static void
adc_calibrate_voltage_cb(struct adc_ctx *ctx, uint16_t val, int error, void *cbdata)
{
        /* disable band-gap buffer */
        bf_set_reg(PMC_REGSC, PMC_REGSC_BGBE, 1);

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
        bf_set_reg(PMC_REGSC, PMC_REGSC_BGBE, 1);

        switch ((uintptr_t)ctx->adc) {
        case ADC0_BASE:
                /* enable clock */
                bf_set_reg(SIM_SCGC6, SIM_SCGC6_ADC0, 1);
                /* enable interrupt handler */
                int_enable(IRQ_ADC0);
                break;
        case ADC1_BASE:
                /* enable clock */
                bf_set_reg(SIM_SCGC6, SIM_SCGC6_ADC1, 1);
                /* enable interrupt handler */
                int_enable(IRQ_ADC1);
                break;
        }

        /* setup ADC calibration */
        adc_sample_prepare(ctx, ADC_MODE_SAMPLE_LONG | ADC_AVG_32);
        ctx->stat_a.cb = adc_calibrate_cb;
        ctx->stat_a.active = 1;

        /* enable interrupt */
        ADC_SC1_REG(ctx->adc,0) =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(ADC_ADCH_DISABLED); /* do not start ADC */

        /* start calibration */
        bf_set_reg(ADC_SC3_REG(ctx->adc), ADC_SC3_CAL, 1);
}

void
adc_sample_prepare(struct adc_ctx *ctx, enum adc_mode mode)
{
        ADC_CFG1_REG(ctx->adc) =
                ((mode & ADC_MODE_POWER_LOW) ? ADC_CFG1_ADLPC_MASK : 0) |
                ADC_CFG1_ADIV(ADC_DIV_1) |
                ((mode & ADC_MODE_SAMPLE_LONG) ? ADC_CFG1_ADLSMP_MASK : 0) |
                ADC_CFG1_MODE(ADC_BIT_16) |
                ADC_CFG1_ADICLK(ADC_CLK_ADACK);
        ADC_CFG2_REG(ctx->adc) =
                ADC_CFG2_MUXSEL_MASK | /* we only have b channels on the K20 */
                ((mode & ADC_MODE_KEEP_CLOCK) ? ADC_CFG2_ADACKEN_MASK : 0) |
                ((mode & ADC_MODE_SPEED_HIGH) ? ADC_CFG2_ADHSC_MASK : 0);
        ADC_SC2_REG(ctx->adc) = ADC_SC2_REFSEL(ADC_REF_DEFAULT);
        ADC_SC3_REG(ctx->adc) =
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
        ADC_SC1_REG(ctx->adc, 0) =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(channel);
        /* XXX diff */
        return (0);
}

int
adc_sample_abort(struct adc_ctx *ctx)
{
        crit_enter();
        ADC_SC1_REG(ctx->adc, 0) = ADC_SC1_ADCH(ADC_ADCH_DISABLED);
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
        if (bf_get_reg(ADC_SC1_REG(ctx->adc, 0), ADC_SC1_COCO)) {
                adc_result_cb_t *cb;
                void *cbdata;
                uint16_t val;

                crit_enter();
                cb = ctx->stat_a.cb;
                cbdata = ctx->stat_a.cbdata;
                if (!bf_get_reg(ADC_SC3_REG(ctx->adc), ADC_SC3_ADCO))
                        ctx->stat_a.active = 0;
                val = ADC_R_REG(ctx->adc, 0);  /* clears interrupt */
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

void
ADC1_Handler(void)
{
        adc_interrupt_handler(&adc1_ctx);
}
