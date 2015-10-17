#include <soc/cortex-m/intnums.h>

#include <MK22F51212.h>

#define FlashConfig_BASE_PTR FTFA_FlashConfig_BASE_PTR

#define FLASH_SECTOR_SIZE 2048
#define FLASH_ELEM_SIZE 4

#define HAVE_PIT_SEPARATE_IRQ
#define USB_FMC_MASTER 4

#include <soc/cortex-m/soc.h>
#include <soc/cortex-m/sched-m3.h>
#include <soc/cortex-m/bitband.h>
#include <soc/cortex-m/bitfield-bitband.h>

#include <soc/kinetis/bitfield-bitband.h>

#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfa.h>
#include <soc/kinetis/periph/flashconfig.h>
#include <soc/kinetis/periph/fmc.h>
#include <soc/kinetis/periph/usbotg.h>
#include <soc/kinetis/periph/mcg.h>
#include <soc/kinetis/periph/sim.h>
#include <soc/kinetis/periph/dspi.h>
#include <soc/kinetis/periph/ftm.h>
#include <soc/kinetis/periph/adc.h>

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
#include <soc/kinetis/dspi.h>
#include <soc/kinetis/timeout.h>
#include <soc/kinetis/uart.h>
#include <soc/kinetis/uart-fifo.h>
#include <soc/kinetis/usb.h>
#include <soc/kinetis/wdog.h>
#include <soc/kinetis/sys-register.h>
