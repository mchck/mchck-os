#include <mchck.h>

PORT_Type *
pin_physport_from_pin(enum pin_id pin)
{
        switch (pin_port_from_pin(pin)) {
        case PIN_PORTA:
                return (PORTA);
        case PIN_PORTB:
                return (PORTB);
        case PIN_PORTC:
                return (PORTC);
        case PIN_PORTD:
                return (PORTD);
        case PIN_PORTE:
                return (PORTE);
        default:
                return (NULL);
        }
}

void
pin_mode(enum pin_id pin, enum pin_mode mode)
{
        int pinnum = pin_physpin_from_pin(pin);

        /* enable port clock */
        bf_set(SIM->SCGC5, pin_portnum_from_pin(pin) + 8, 1, 1);

        uint32_t pcr = pin_physport_from_pin(pin)->PCR[pinnum];

        if (mode & PIN_MODE_RESET) {
                pcr &= ~0xff;
                bf_set_reg(pcr, PORT_PCR_MUX, 0);
        }

        if (mode & PIN_MODE__SLEW) {
                switch (mode & PIN_MODE__SLEW_MASK) {
                case PIN_MODE_SLEW_FAST:
                        bf_set_reg(pcr, PORT_PCR_SRE, 0);
                        break;
                case PIN_MODE_SLEW_SLOW:
                        bf_set_reg(pcr, PORT_PCR_SRE, 1);
                        break;
                }
        }

        if (mode & PIN_MODE__PULL) {
                switch (mode & PIN_MODE__PULL_MASK) {
                case PIN_MODE_PULL_OFF:
                        bf_set_reg(pcr, PORT_PCR_PE, 0);
                        break;
                case PIN_MODE_PULLDOWN:
                        bf_set_reg(pcr, PORT_PCR_PE, 1);
                        bf_set_reg(pcr, PORT_PCR_PS, 0);
                        break;
                case PIN_MODE_PULLUP:
                        bf_set_reg(pcr, PORT_PCR_PE, 1);
                        bf_set_reg(pcr, PORT_PCR_PS, 1);
                        break;
                }
        }

        if (mode & PIN_MODE__DRIVE) {
                switch (mode & PIN_MODE__DRIVE_MASK) {
                case PIN_MODE_DRIVE_LOW:
                        bf_set_reg(pcr, PORT_PCR_DSE, 0);
                        break;
                case PIN_MODE_DRIVE_HIGH:
                        bf_set_reg(pcr, PORT_PCR_DSE, 1);
                        break;
                }
        }

        if (mode & PIN_MODE__FILTER) {
                switch (mode & PIN_MODE__FILTER_MASK) {
                case PIN_MODE_FILTER_OFF:
                        bf_set_reg(pcr, PORT_PCR_PFE, 0);
                        break;
                case PIN_MODE_FILTER_ON:
                        bf_set_reg(pcr, PORT_PCR_PFE, 1);
                        break;
                }
        }

        if (mode & PIN_MODE__OPEN_DRAIN) {
#ifdef PORT_PCR_ODE_MASK
                switch (mode & PIN_MODE__OPEN_DRAIN_MASK) {
                case PIN_MODE_OPEN_DRAIN_OFF:
                        bf_set_reg(pcr, PORT_PCR_ODE, 0);
                        break;
                case PIN_MODE_OPEN_DRAIN_ON:
                        bf_set_reg(pcr, PORT_PCR_ODE, 1);
                        break;
                }
#else
                panic("no open drain mode available");
#endif
        }

        if (mode & PIN_MODE__MUX) {
                bf_set_reg(pcr, PORT_PCR_MUX, (mode & PIN_MODE__MUX_MASK) >> 13);
        }

        pin_physport_from_pin(pin)->PCR[pinnum] = pcr;
}
