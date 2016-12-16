#include <soc/cortex-m/intnums.h>

#include <MKL03Z4.h>

#define FlashConfig_BASE_PTR FTFA_FlashConfig_BASE_PTR

#define FLASH_SECTOR_SIZE 1024

#include <soc/cortex-m/soc.h>
#include <soc/cortex-m/sched-m0.h>

#include <soc/kinetis/bme.h>
#include <soc/kinetis/bitfield-bme.h>

#include <soc/kinetis/periph/adc.h>
#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfa.h>
#include <soc/kinetis/periph/flashconfig.h>
#include <soc/kinetis/periph/sim.h>

#include <soc/kinetis/adc.h>
#include <soc/kinetis/cop.h>
#include <soc/kinetis/flash.h>
#include <soc/kinetis/i2c.h>
#include <soc/kinetis/uart.h>
#include <soc/kinetis/uart-stdio.h>
#include <soc/kinetis/lpuart.h>
#include <soc/kinetis/pin.h>
#include <soc/kinetis/pin_change.h>
#include <soc/kinetis/gpio.h>
#include <soc/kinetis/fgpio.h>
#include <soc/kinetis/rtc.h>
#include <soc/kinetis/spi.h>
#include <soc/kinetis/timeout.h>
#include <soc/kinetis/sys-register.h>
