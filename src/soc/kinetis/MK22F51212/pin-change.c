#include "../pin-change.c"

extern const struct pin_change_handler pin_hooks_A, pin_hooks_A_end;
extern const struct pin_change_handler pin_hooks_B, pin_hooks_B_end;
extern const struct pin_change_handler pin_hooks_C, pin_hooks_C_end;
extern const struct pin_change_handler pin_hooks_D, pin_hooks_D_end;
extern const struct pin_change_handler pin_hooks_E, pin_hooks_E_end;

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
PORTC_Handler(void)
{
        pin_change_handler(PORTC_BASE_PTR, &pin_hooks_C, &pin_hooks_C_end);
}

void
PORTD_Handler(void)
{
        pin_change_handler(PORTD_BASE_PTR, &pin_hooks_D, &pin_hooks_D_end);
}

void
PORTE_Handler(void)
{
        pin_change_handler(PORTE_BASE_PTR, &pin_hooks_E, &pin_hooks_E_end);
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

        if (PIN_CHANGE_ENABLE(C)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTC, 1);
                PIN_CHANGE_SET_IRQC(C);
                int_enable(IRQ_PORTC);
        }

        if (PIN_CHANGE_ENABLE(D)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTD, 1);
                PIN_CHANGE_SET_IRQC(D);
                int_enable(IRQ_PORTD);
        }

        if (PIN_CHANGE_ENABLE(E)) {
                bf_set_reg(SIM_SCGC5, SIM_SCGC5_PORTE, 1);
                PIN_CHANGE_SET_IRQC(E);
                int_enable(IRQ_PORTE);
        }
}
