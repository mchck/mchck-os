#include "../pin-change.c"

extern const struct pin_change_handler pin_hooks_A, pin_hooks_A_end;
extern const struct pin_change_handler pin_hooks_C, pin_hooks_C_end;
extern const struct pin_change_handler pin_hooks_D, pin_hooks_D_end;

void
PORTA_Handler(void)
{
        pin_change_handler(PORTA_BASE_PTR, &pin_hooks_A, &pin_hooks_A_end);
}

void
PORTD_Handler(void)
{
        pin_change_handler(PORTC_BASE_PTR, &pin_hooks_C, &pin_hooks_C_end);
        pin_change_handler(PORTD_BASE_PTR, &pin_hooks_D, &pin_hooks_D_end);
}

void
pin_change_init(void)
{
        if (PIN_CHANGE_ENABLE(A)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTA, 1);
                PIN_CHANGE_SET_IRQC(A);
                int_enable(IRQ_PORTA);
        }

        if (PIN_CHANGE_ENABLE(C)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTC, 1);
                PIN_CHANGE_SET_IRQC(C);
        }
        if (PIN_CHANGE_ENABLE(D)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTD, 1);
                PIN_CHANGE_SET_IRQC(D);
        }
        if (PIN_CHANGE_ENABLE(C) || PIN_CHANGE_ENABLE(D)) {
                int_enable(IRQ_PORTD);
        }
}
