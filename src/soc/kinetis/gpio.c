#include <mchck.h>

GPIO_Type *
gpio_physgpio_from_pin(enum gpio_pin_id pin)
{
        switch (pin_port_from_pin(pin)) {
        case PIN_PORTA:
                return (PTA);
        case PIN_PORTB:
                return (PTB);
        case PIN_PORTC:
                return (PTC);
        case PIN_PORTD:
                return (PTD);
        case PIN_PORTE:
                return (PTE);
        default:
                return (NULL);
        }
}

void
gpio_dir(enum gpio_pin_id pin, enum gpio_dir dir)
{
        int pinnum = pin_physpin_from_pin(pin);
        GPIO_Type *pinp = gpio_physgpio_from_pin(pin);

        switch (dir) {
        case GPIO_OUTPUT:
                bf_set(pinp->PDDR, pinnum, 1, 1);
                goto set_mux;
        case GPIO_INPUT:
                bf_set(pinp->PDDR, pinnum, 1, 0);
        set_mux:
                pin_mode(pin, PIN_MODE_MUX_ALT1);
                break;
        case GPIO_DISABLE:
                pin_mode(pin, PIN_MODE_MUX_ANALOG);
                break;
        }
}

void
gpio_write(enum gpio_pin_id pin, enum gpio_pin_value val)
{
        bf_set(gpio_physgpio_from_pin(pin)->PDOR, pin_physpin_from_pin(pin), 1, val);
}

void
gpio_toggle(enum gpio_pin_id pin)
{
        gpio_physgpio_from_pin(pin)->PTOR = 1 << pin_physpin_from_pin(pin);
}

enum gpio_pin_value
gpio_read(enum gpio_pin_id pin)
{
        return (bf_get(gpio_physgpio_from_pin(pin)->PDIR, pin_physpin_from_pin(pin), 1));
}
