#define EXTERNAL_XTAL 10000000

enum onboard_led_state {
        ONBOARD_LED_OFF = 0,
        ONBOARD_LED_ON = 1,
        ONBOARD_LED_MASK = 0xf00,
        ONBOARD_LED_ERROR = 0x000,
        ONBOARD_LED_CALIBRATE = 0x100,
        ONBOARD_LED_CAPTURE = 0x200,
        ONBOARD_LED_POWER = 0x300,
        ONBOARD_LED_TOGGLE = -1,
        ONBOARD_LED_FLOAT = -2
};

void onboard_led(enum onboard_led_state);
