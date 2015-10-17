#include <mchck.h>

struct adc_ctx {
        volatile struct {
                adc_result_cb_t *cb;
                void *cbdata;
                int active;
        } stat_a, stat_b;
        unsigned long fract scale;
};

static struct adc_ctx adc_ctx;

static void
adc_calibrate_cb(uint16_t val, int error, void *cbdata)
{
        /* base ADC calibration */
        if (error || bf_get_reg(ADC0_SC3, ADC_SC3_CALF))
                return;

        uint32_t calib = 0;
        calib += ADC0_CLPD;
        calib += ADC0_CLPS;
        calib += ADC0_CLP4;
        calib += ADC0_CLP3;
        calib += ADC0_CLP2;
        calib += ADC0_CLP1;
        calib += ADC0_CLP0;
        ADC0_PG = (calib >> 1) | 0x8000;

        calib = 0;
        calib += ADC0_CLMD;
        calib += ADC0_CLMS;
        calib += ADC0_CLM4;
        calib += ADC0_CLM3;
        calib += ADC0_CLM2;
        calib += ADC0_CLM1;
        calib += ADC0_CLM0;
        ADC0_MG = (calib >> 1) | 0x8000;

        adc_calibrate_voltage(0);
}

static void
adc_calibrate_voltage_cb(uint16_t val, int error, void *cbdata)
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

        adc_ctx.scale = 1.0lR / val;

        adc_calibration_done();
}

static void
calibration_done(void)
{
}

void adc_calibration_done(void) __attribute__((weak, alias("calibration_done")));

void
adc_calibrate_voltage(unsigned accum reference)
{
        if (reference != 0) {
                adc_ctx.scale = (unsigned long accum)reference >> 16;
        } else {
                adc_sample_start(ADC_BANDGAP, adc_calibrate_voltage_cb, NULL);
        }
}

unsigned accum
adc_as_voltage(uint16_t val)
{
        return (val * adc_ctx.scale);
}

void
adc_init(void)
{
        /**
         * Enable bandgap buffer.  We need this later to calibrate our
         * reference scale.  However, we start it now, so that it will
         * have time to stabilize. */
        bf_set_reg(PMC_REGSC, PMC_REGSC_BGBE, 1);

        /* enable clock */
        bf_set_reg(SIM_SCGC6, SIM_SCGC6_ADC0, 1);

        /* enable interrupt handler */
        int_enable(IRQ_ADC0);

        /* setup ADC calibration */
        adc_sample_prepare(ADC_MODE_SAMPLE_LONG | ADC_AVG_32);
        adc_ctx.stat_a.cb = adc_calibrate_cb;
        adc_ctx.stat_a.active = 1;

        /* enable interrupt */
        ADC0_SC1A =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(ADC_ADCH_DISABLED); /* do not start ADC */

        /* start calibration */
        bf_set_reg(ADC0_SC3, ADC_SC3_CAL, 1);
}

void
adc_sample_prepare(enum adc_mode mode)
{
        ADC0_CFG1 =
                ((mode & ADC_MODE_POWER_LOW) ? ADC_CFG1_ADLPC_MASK : 0) |
                ADC_CFG1_ADIV(ADC_DIV_1) |
                ((mode & ADC_MODE_SAMPLE_LONG) ? ADC_CFG1_ADLSMP_MASK : 0) |
                ADC_CFG1_MODE(ADC_BIT_16) |
                ADC_CFG1_ADICLK(ADC_CLK_ADACK);
        ADC0_CFG2 =
                ADC_CFG2_MUXSEL_MASK | /* we only have b channels on the K20 */
                ((mode & ADC_MODE_KEEP_CLOCK) ? ADC_CFG2_ADACKEN_MASK : 0) |
                ((mode & ADC_MODE_SPEED_HIGH) ? ADC_CFG2_ADHSC_MASK : 0);
        ADC0_SC2 = ADC_SC2_REFSEL(ADC_REF_DEFAULT);
        ADC0_SC3 =
                ((mode & ADC_MODE_CONTINUOUS) ? ADC_SC3_ADCO_MASK : 0) |
                ((mode & ADC_MODE__AVG) ? ADC_SC3_AVGE_MASK : 0 ) |
                ADC_SC3_AVGS((mode & ADC_MODE__AVG_MASK) / ADC_MODE__AVG / 2); /* XXX ugly */
}

int
adc_sample_start(enum adc_channel channel, adc_result_cb_t *cb, void *cbdata)
{
        if (adc_ctx.stat_a.active)
                return (-1);

        /* XXX check for previous conversion running */
        adc_ctx.stat_a.cb = cb;
        adc_ctx.stat_a.cbdata = cbdata;

        /* trigger conversion */
        ADC0_SC1A =
                ADC_SC1_AIEN_MASK |
                ADC_SC1_ADCH(channel);
        /* XXX diff */
        return (0);
}

int
adc_sample_abort(void)
{
        crit_enter();
        ADC0_SC1A = ADC_SC1_ADCH(ADC_ADCH_DISABLED);
        if (adc_ctx.stat_a.active) {
                adc_result_cb_t *cb = adc_ctx.stat_a.cb;
                void *cbdata = adc_ctx.stat_a.cbdata;

                adc_ctx.stat_a.active = 0;
                crit_exit();
                cb(0, 1, cbdata);
                return (0);
        } else {
                crit_exit();
                return (-1);
        }
}

void
ADC0_Handler(void)
{
        if (bf_get_reg(ADC0_SC1A, ADC_SC1_COCO)) {
                adc_result_cb_t *cb;
                void *cbdata;
                uint16_t val;

                crit_enter();
                cb = adc_ctx.stat_a.cb;
                cbdata = adc_ctx.stat_a.cbdata;
                if (!bf_get_reg(ADC0_SC3, ADC_SC3_ADCO))
                        adc_ctx.stat_a.active = 0;
                val = ADC0_RA;  /* clears interrupt */
                crit_exit();

                cb(val, 0, cbdata);
        }
        /* XXX repeat for sb1 */
}
