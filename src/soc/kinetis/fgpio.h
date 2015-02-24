void fgpio_set(enum gpio_pin_id pin, enum gpio_pin_value val);
void fgpio_clear(enum gpio_pin_id pin, enum gpio_pin_value val);
void fgpio_toggle(enum gpio_pin_id pin);
enum gpio_pin_value fgpio_read(enum gpio_pin_id pin);
