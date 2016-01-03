#include <mchck.h>

#include "usb-serial-loopback.desc.h"

void
new_data(uint8_t *data, size_t len)
{
        onboard_led(-1);
        cdc_write(data, len, &loopback);
        cdc_read_more(&loopback);
}

void
loopback_init(int enable)
{
        if (enable) {
                new_data(NULL, 0);
        }
}

void
main(void)
{
        usb_init(&cdc_device);
        sys_yield_for_frogs();
}
