#include <MKL26Z4.h>

#define FLASH_SECTOR_SIZE 1024

#include <soc/cortex-m/soc.h>

#include <soc/kinetis/bitfield.h>

#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfa.h>
#include <soc/kinetis/periph/usbotg.h>
#include <soc/kinetis/periph/sim.h>

#include <soc/kinetis/adc.h>
#include <soc/kinetis/crc.h>
#include <soc/kinetis/cop.h>
#include <soc/kinetis/flash.h>
#include <soc/kinetis/i2c.h>
#include <soc/kinetis/pin.h>
#include <soc/kinetis/pin_change.h>
#include <soc/kinetis/gpio.h>
#include <soc/kinetis/pit.h>
#include <soc/kinetis/rtc.h>
#include <soc/kinetis/spi.h>
#include <soc/kinetis/stdio.h>
#include <soc/kinetis/timeout.h>
#include <soc/kinetis/uart.h>
