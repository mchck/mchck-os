#include <soc/cortex-m/intnums.h>

#include <MKL27Z4.h>

#define FlashConfig_BASE_PTR FTFA_FlashConfig

#define FLASH_SECTOR_SIZE 1024

typedef GPIO_Type FGPIO_Type;
#define FPTA_BASE (PTA_BASE + 0xf8000000 - PTA_BASE)
#define FPTA ((FGPIO_Type *)FPTA_BASE)
#define FPTB_BASE (PTB_BASE + 0xf8000000 - PTA_BASE)
#define FPTB ((FGPIO_Type *)FPTB_BASE)
#define FPTC_BASE (PTC_BASE + 0xf8000000 - PTA_BASE)
#define FPTC ((FGPIO_Type *)FPTC_BASE)
#define FPTD_BASE (PTD_BASE + 0xf8000000 - PTA_BASE)
#define FPTD ((FGPIO_Type *)FPTD_BASE)
#define FPTE_BASE (PTE_BASE + 0xf8000000 - PTA_BASE)
#define FPTE ((FGPIO_Type *)FPTE_BASE)

#include <soc/cortex-m/soc.h>
#include <soc/cortex-m/sched-m0.h>

#include <soc/kinetis/bme.h>
#include <soc/kinetis/bitfield-bme.h>

#include <soc/kinetis/periph/adc.h>
#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfa.h>
#include <soc/kinetis/periph/flashconfig.h>
#include <soc/kinetis/periph/usbotg.h>
#include <soc/kinetis/periph/sim.h>

#include <soc/kinetis/adc.h>
#include <soc/kinetis/cop.h>
#include <soc/kinetis/flash.h>
#include <soc/kinetis/i2c.h>
#include <soc/kinetis/pin.h>
#include <soc/kinetis/pin_change.h>
#include <soc/kinetis/gpio.h>
#include <soc/kinetis/fgpio.h>
#include <soc/kinetis/pit.h>
#include <soc/kinetis/rtc.h>
#include <soc/kinetis/timeout.h>
#include <soc/kinetis/uart.h>
#include <soc/kinetis/uart-stdio.h>
#include <soc/kinetis/lpuart.h>
#include <soc/kinetis/usb.h>
#include <soc/kinetis/sys-register.h>
