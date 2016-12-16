#include "../pin-change.c"

extern const struct pin_change_handler pin_hooks_A, pin_hooks_A_end;
extern const struct pin_change_handler pin_hooks_B, pin_hooks_B_end;

void
PORTA_Handler(void)
{
        pin_change_handler(PORTA_BASE_PTR, &pin_hooks_A, &pin_hooks_A_end);
}

void
PORTB_Handler(void)
{
        pin_change_handler(PORTB_BASE_PTR, &pin_hooks_B, &pin_hooks_B_end);
}

void
pin_change_init(void)
{
        if (PIN_CHANGE_ENABLE(A)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTA, 1);
                PIN_CHANGE_SET_IRQC(A);
                int_enable(IRQ_PORTA);
        }

        if (PIN_CHANGE_ENABLE(B)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTB, 1);
                PIN_CHANGE_SET_IRQC(B);
                int_enable(IRQ_PORTB);
        }
}
