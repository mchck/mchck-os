enum pin_port_id {
        PIN_PORTA = 1 << 16,
        PIN_PORTB = 2 << 16,
        PIN_PORTC = 3 << 16,
        PIN_PORTD = 4 << 16,
        PIN_PORTE = 5 << 16,
};

enum pin_id {
        PIN_PTA0 = PIN_PORTA | 0, PIN_PTA1 = PIN_PORTA | 1, PIN_PTA2 = PIN_PORTA | 2, PIN_PTA3 = PIN_PORTA | 3,
        PIN_PTA4 = PIN_PORTA | 4, PIN_PTA5 = PIN_PORTA | 5, PIN_PTA6 = PIN_PORTA | 6, PIN_PTA7 = PIN_PORTA | 7,
        PIN_PTA8 = PIN_PORTA | 8, PIN_PTA9 = PIN_PORTA | 9, PIN_PTA10 = PIN_PORTA | 10, PIN_PTA11 = PIN_PORTA | 11,
        PIN_PTA12 = PIN_PORTA | 12, PIN_PTA13 = PIN_PORTA | 13, PIN_PTA14 = PIN_PORTA | 14, PIN_PTA15 = PIN_PORTA | 15,
        PIN_PTA16 = PIN_PORTA | 16, PIN_PTA17 = PIN_PORTA | 17, PIN_PTA18 = PIN_PORTA | 18, PIN_PTA19 = PIN_PORTA | 19,
        PIN_PTA20 = PIN_PORTA | 20, PIN_PTA21 = PIN_PORTA | 21, PIN_PTA22 = PIN_PORTA | 22, PIN_PTA23 = PIN_PORTA | 23,
        PIN_PTA24 = PIN_PORTA | 24, PIN_PTA25 = PIN_PORTA | 25, PIN_PTA26 = PIN_PORTA | 26, PIN_PTA27 = PIN_PORTA | 27,
        PIN_PTA28 = PIN_PORTA | 28, PIN_PTA29 = PIN_PORTA | 29, PIN_PTA30 = PIN_PORTA | 30, PIN_PTA31 = PIN_PORTA | 31,

        PIN_PTB0 = PIN_PORTB | 0, PIN_PTB1 = PIN_PORTB | 1, PIN_PTB2 = PIN_PORTB | 2, PIN_PTB3 = PIN_PORTB | 3,
        PIN_PTB4 = PIN_PORTB | 4, PIN_PTB5 = PIN_PORTB | 5, PIN_PTB6 = PIN_PORTB | 6, PIN_PTB7 = PIN_PORTB | 7,
        PIN_PTB8 = PIN_PORTB | 8, PIN_PTB9 = PIN_PORTB | 9, PIN_PTB10 = PIN_PORTB | 10, PIN_PTB11 = PIN_PORTB | 11,
        PIN_PTB12 = PIN_PORTB | 12, PIN_PTB13 = PIN_PORTB | 13, PIN_PTB14 = PIN_PORTB | 14, PIN_PTB15 = PIN_PORTB | 15,
        PIN_PTB16 = PIN_PORTB | 16, PIN_PTB17 = PIN_PORTB | 17, PIN_PTB18 = PIN_PORTB | 18, PIN_PTB19 = PIN_PORTB | 19,
        PIN_PTB20 = PIN_PORTB | 20, PIN_PTB21 = PIN_PORTB | 21, PIN_PTB22 = PIN_PORTB | 22, PIN_PTB23 = PIN_PORTB | 23,
        PIN_PTB24 = PIN_PORTB | 24, PIN_PTB25 = PIN_PORTB | 25, PIN_PTB26 = PIN_PORTB | 26, PIN_PTB27 = PIN_PORTB | 27,
        PIN_PTB28 = PIN_PORTB | 28, PIN_PTB29 = PIN_PORTB | 29, PIN_PTB30 = PIN_PORTB | 30, PIN_PTB31 = PIN_PORTB | 31,

        PIN_PTC0 = PIN_PORTC | 0, PIN_PTC1 = PIN_PORTC | 1, PIN_PTC2 = PIN_PORTC | 2, PIN_PTC3 = PIN_PORTC | 3,
        PIN_PTC4 = PIN_PORTC | 4, PIN_PTC5 = PIN_PORTC | 5, PIN_PTC6 = PIN_PORTC | 6, PIN_PTC7 = PIN_PORTC | 7,
        PIN_PTC8 = PIN_PORTC | 8, PIN_PTC9 = PIN_PORTC | 9, PIN_PTC10 = PIN_PORTC | 10, PIN_PTC11 = PIN_PORTC | 11,
        PIN_PTC12 = PIN_PORTC | 12, PIN_PTC13 = PIN_PORTC | 13, PIN_PTC14 = PIN_PORTC | 14, PIN_PTC15 = PIN_PORTC | 15,
        PIN_PTC16 = PIN_PORTC | 16, PIN_PTC17 = PIN_PORTC | 17, PIN_PTC18 = PIN_PORTC | 18, PIN_PTC19 = PIN_PORTC | 19,
        PIN_PTC20 = PIN_PORTC | 20, PIN_PTC21 = PIN_PORTC | 21, PIN_PTC22 = PIN_PORTC | 22, PIN_PTC23 = PIN_PORTC | 23,
        PIN_PTC24 = PIN_PORTC | 24, PIN_PTC25 = PIN_PORTC | 25, PIN_PTC26 = PIN_PORTC | 26, PIN_PTC27 = PIN_PORTC | 27,
        PIN_PTC28 = PIN_PORTC | 28, PIN_PTC29 = PIN_PORTC | 29, PIN_PTC30 = PIN_PORTC | 30, PIN_PTC31 = PIN_PORTC | 31,

        PIN_PTD0 = PIN_PORTD | 0, PIN_PTD1 = PIN_PORTD | 1, PIN_PTD2 = PIN_PORTD | 2, PIN_PTD3 = PIN_PORTD | 3,
        PIN_PTD4 = PIN_PORTD | 4, PIN_PTD5 = PIN_PORTD | 5, PIN_PTD6 = PIN_PORTD | 6, PIN_PTD7 = PIN_PORTD | 7,
        PIN_PTD8 = PIN_PORTD | 8, PIN_PTD9 = PIN_PORTD | 9, PIN_PTD10 = PIN_PORTD | 10, PIN_PTD11 = PIN_PORTD | 11,
        PIN_PTD12 = PIN_PORTD | 12, PIN_PTD13 = PIN_PORTD | 13, PIN_PTD14 = PIN_PORTD | 14, PIN_PTD15 = PIN_PORTD | 15,
        PIN_PTD16 = PIN_PORTD | 16, PIN_PTD17 = PIN_PORTD | 17, PIN_PTD18 = PIN_PORTD | 18, PIN_PTD19 = PIN_PORTD | 19,
        PIN_PTD20 = PIN_PORTD | 20, PIN_PTD21 = PIN_PORTD | 21, PIN_PTD22 = PIN_PORTD | 22, PIN_PTD23 = PIN_PORTD | 23,
        PIN_PTD24 = PIN_PORTD | 24, PIN_PTD25 = PIN_PORTD | 25, PIN_PTD26 = PIN_PORTD | 26, PIN_PTD27 = PIN_PORTD | 27,
        PIN_PTD28 = PIN_PORTD | 28, PIN_PTD29 = PIN_PORTD | 29, PIN_PTD30 = PIN_PORTD | 30, PIN_PTD31 = PIN_PORTD | 31,

        PIN_PTE0 = PIN_PORTE | 0, PIN_PTE1 = PIN_PORTE | 1, PIN_PTE2 = PIN_PORTE | 2, PIN_PTE3 = PIN_PORTE | 3,
        PIN_PTE4 = PIN_PORTE | 4, PIN_PTE5 = PIN_PORTE | 5, PIN_PTE6 = PIN_PORTE | 6, PIN_PTE7 = PIN_PORTE | 7,
        PIN_PTE8 = PIN_PORTE | 8, PIN_PTE9 = PIN_PORTE | 9, PIN_PTE10 = PIN_PORTE | 10, PIN_PTE11 = PIN_PORTE | 11,
        PIN_PTE12 = PIN_PORTE | 12, PIN_PTE13 = PIN_PORTE | 13, PIN_PTE14 = PIN_PORTE | 14, PIN_PTE15 = PIN_PORTE | 15,
        PIN_PTE16 = PIN_PORTE | 16, PIN_PTE17 = PIN_PORTE | 17, PIN_PTE18 = PIN_PORTE | 18, PIN_PTE19 = PIN_PORTE | 19,
        PIN_PTE20 = PIN_PORTE | 20, PIN_PTE21 = PIN_PORTE | 21, PIN_PTE22 = PIN_PORTE | 22, PIN_PTE23 = PIN_PORTE | 23,
        PIN_PTE24 = PIN_PORTE | 24, PIN_PTE25 = PIN_PORTE | 25, PIN_PTE26 = PIN_PORTE | 26, PIN_PTE27 = PIN_PORTE | 27,
        PIN_PTE28 = PIN_PORTE | 28, PIN_PTE29 = PIN_PORTE | 29, PIN_PTE30 = PIN_PORTE | 30, PIN_PTE31 = PIN_PORTE | 31,
};

enum pin_mode {
        PIN_MODE__SLEW            =                         2 << 2,
        PIN_MODE_SLEW_FAST        = PIN_MODE__SLEW       | (0 << 2),
        PIN_MODE_SLEW_SLOW        = PIN_MODE__SLEW       | (1 << 2),
        PIN_MODE__SLEW_MASK       = PIN_MODE__SLEW       | (1 << 2),

        PIN_MODE__PULL            =                         4 << 4,
        PIN_MODE_PULL_OFF         = PIN_MODE__PULL       | (0 << 4),
        PIN_MODE_PULLUP           = PIN_MODE__PULL       | (1 << 4),
        PIN_MODE_PULLDOWN         = PIN_MODE__PULL       | (2 << 4),
        PIN_MODE__PULL_MASK       = PIN_MODE__PULL       | (3 << 4),

        PIN_MODE__DRIVE           =                         2 << 7,
        PIN_MODE_DRIVE_LOW        = PIN_MODE__DRIVE      | (0 << 7),
        PIN_MODE_DRIVE_HIGH       = PIN_MODE__DRIVE      | (1 << 7),
        PIN_MODE__DRIVE_MASK      = PIN_MODE__DRIVE      | (1 << 7),

        PIN_MODE__FILTER          =                         2 << 9,
        PIN_MODE_FILTER_OFF       = PIN_MODE__FILTER     | (0 << 9),
        PIN_MODE_FILTER_ON        = PIN_MODE__FILTER     | (1 << 9),
        PIN_MODE__FILTER_MASK     = PIN_MODE__FILTER     | (1 << 9),

        PIN_MODE__OPEN_DRAIN      =                         2 << 11,
        PIN_MODE_OPEN_DRAIN_OFF   = PIN_MODE__OPEN_DRAIN | (0 << 11),
        PIN_MODE_OPEN_DRAIN_ON    = PIN_MODE__OPEN_DRAIN | (1 << 11),
        PIN_MODE__OPEN_DRAIN_MASK = PIN_MODE__OPEN_DRAIN | (1 << 11),

        PIN_MODE__MUX             =                         8 << 13,
        PIN_MODE_MUX_ALT0         = PIN_MODE__MUX        | (0 << 13),
        PIN_MODE_MUX_ANALOG       = PIN_MODE__MUX        | (0 << 13),
        PIN_MODE_MUX_ALT1         = PIN_MODE__MUX        | (1 << 13),
        PIN_MODE_MUX_GPIO         = PIN_MODE__MUX        | (1 << 13),
        PIN_MODE_MUX_ALT2         = PIN_MODE__MUX        | (2 << 13),
        PIN_MODE_MUX_ALT3         = PIN_MODE__MUX        | (3 << 13),
        PIN_MODE_MUX_ALT4         = PIN_MODE__MUX        | (4 << 13),
        PIN_MODE_MUX_ALT5         = PIN_MODE__MUX        | (5 << 13),
        PIN_MODE_MUX_ALT6         = PIN_MODE__MUX        | (6 << 13),
        PIN_MODE_MUX_ALT7         = PIN_MODE__MUX        | (7 << 13),
        PIN_MODE__MUX_MASK        = PIN_MODE__MUX        | (7 << 13),

        PIN_MODE_RESET            =                         1 << 31
};


static inline enum pin_port_id
pin_port_from_pin(enum pin_id pin)
{
        return (pin & 0xf0000);
}

static inline int
pin_portnum_from_pin(enum pin_id pin)
{
        return (pin_port_from_pin(pin) >> 16);
}

static inline int
pin_physpin_from_pin(enum pin_id pin)
{
        return (pin & 0xffff);
}

PORT_Type *pin_physport_from_pin(enum pin_id pin);
void pin_mode(enum pin_id pin, enum pin_mode mode);
