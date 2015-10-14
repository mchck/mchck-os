#include <mchck.h>

static const enum gpio_pin_id led_pins[] = {GPIO_PTC8, GPIO_PTC9, GPIO_PTC10, GPIO_PTC11};

void
onboard_led(enum onboard_led_state state)
{
        enum gpio_pin_id led_pin = led_pins[(state & ONBOARD_LED_MASK) >> 8];

        if (state == ONBOARD_LED_FLOAT) {
                gpio_dir(led_pin, GPIO_DISABLE);
                return;
        }

        gpio_dir(led_pin, GPIO_OUTPUT);
        pin_mode(led_pin, PIN_MODE_DRIVE_HIGH);

        if (state == ONBOARD_LED_OFF || state == ONBOARD_LED_ON)
                gpio_write(led_pin, state);
        else
                gpio_toggle(led_pin);
}
