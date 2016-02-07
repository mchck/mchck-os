#include <mchck.h>

static struct pit_ctx_t {
	pit_callback *cb;
} ctx[4];

void
pit_init(void)
{
	bf_set_reg(SIM->SCGC6, SIM_SCGC6_PIT, 1);
	bf_set_reg(PIT->MCR, PIT_MCR_MDIS, 0);
	bf_set_reg(PIT->MCR, PIT_MCR_FRZ, 0);

#if defined(HAVE_PIT_SEPARATE_IRQ)
	int_enable(IRQ_PIT0);
	int_enable(IRQ_PIT1);
	int_enable(IRQ_PIT2);
	int_enable(IRQ_PIT3);
#else
	int_enable(IRQ_PIT);
#endif
}

void
pit_start(enum pit_id id, uint32_t cycles, pit_callback *cb)
{
	ctx[id].cb = cb;
	PIT->CHANNEL[id].LDVAL = cycles;
	bf_set_reg(PIT->CHANNEL[id].TFLG, PIT_TFLG_TIF, 1);
	bf_set_reg(PIT->CHANNEL[id].TCTRL, PIT_TCTRL_TIE, cb != NULL);
	bf_set_reg(PIT->CHANNEL[id].TCTRL, PIT_TCTRL_TEN, 1);
}

void
pit_stop(enum pit_id id)
{
	bf_set_reg(PIT->CHANNEL[id].TCTRL, PIT_TCTRL_TEN, 0);
}

uint32_t
pit_cycle(enum pit_id id)
{
	return PIT->CHANNEL[id].CVAL;
}

static void
common_handler(enum pit_id id)
{
	bf_set_reg(PIT->CHANNEL[id].TFLG, PIT_TFLG_TIF, 1);
	ctx[id].cb(id);
}


#if defined(HAVE_PIT_SEPARATE_IRQ)

void
PIT0_Handler(void)
{
	common_handler(PIT_0);
}

void
PIT1_Handler(void)
{
	common_handler(PIT_1);
}

void
PIT2_Handler(void)
{
	common_handler(PIT_2);
}

void
PIT3_Handler(void)
{
	common_handler(PIT_3);
}

#else

void
PIT_Handler(void)
{
	if (bf_get_reg(PIT->CHANNEL[0].TFLG, PIT_TFLG_TIF))
		common_handler(PIT_0);
	if (bf_get_reg(PIT->CHANNEL[1].TFLG, PIT_TFLG_TIF))
		common_handler(PIT_1);
}

#endif
