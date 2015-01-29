#include <MK20D5.h>

#define FLASH_SECTOR_SIZE 1024
#define FLASH_SECTION_SIZE 1024
#define FLASH_ELEM_SIZE 4

#include <soc/cortex-m/soc.h>
#include <soc/cortex-m/bitband.h>

#include <soc/kinetis/bitfield-bitband.h>

#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfl.h>
#include <soc/kinetis/periph/usbotg.h>
#include <soc/kinetis/periph/sim.h>

#include <soc/kinetis/adc.h>
#include <soc/kinetis/crc.h>
#include <soc/kinetis/flash.h>
#include <soc/kinetis/ftm.h>
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
#include <soc/kinetis/wdog.h>
