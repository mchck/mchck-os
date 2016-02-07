#include <mchck.h>

static FGPIO_Type *
fgpio_physgpio_from_pin(enum gpio_pin_id pin)
{
        switch (pin_port_from_pin(pin)) {
        case PIN_PORTA:
                return (FPTA);
        case PIN_PORTB:
                return (FPTB);
        case PIN_PORTC:
                return (FPTC);
        case PIN_PORTD:
                return (FPTD);
        case PIN_PORTE:
                return (FPTE);
        default:
                return (NULL);
        }
}

void
fgpio_set(enum gpio_pin_id pin)
{
        fgpio_physgpio_from_pin(pin)->PSOR = 1 << pin_physpin_from_pin(pin);
}

void
fgpio_clear(enum gpio_pin_id pin)
{
        fgpio_physgpio_from_pin(pin)->PCOR = 1 << pin_physpin_from_pin(pin);
}

void
fgpio_toggle(enum gpio_pin_id pin)
{
        fgpio_physgpio_from_pin(pin)->PTOR = 1 << pin_physpin_from_pin(pin);
}

enum gpio_pin_value
fgpio_read(enum gpio_pin_id pin)
{
        return ((fgpio_physgpio_from_pin(pin)->PDIR >> pin_physpin_from_pin(pin)) & 1);
}
