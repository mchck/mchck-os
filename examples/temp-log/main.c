#include <mchck.h>

#include "temp-log.desc.h"

#include "onewire.h"
#include "ds18b20.h"

#include "temp-log.h"


struct ow_ctx ow_ctx;
struct ds18b20_ctx ds;

void
main(void)
{
        timeout_init();
        rtc_init();

        pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        pin_mode(PIN_PTC3, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        ow_init(&ow_ctx, &uart1);
        ds_init(&ds, &ow_ctx);

        spiflash_pins_init();
        spi_init();

        flash_init();

        usb_init(&cdc_device);

        sys_yield_for_frogs();
}
