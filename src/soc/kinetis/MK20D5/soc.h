#include <soc/cortex-m/intnums.h>

#include <MK20D5.h>

/* fix up inconsistent header naming */
#define SIM_SCGC5_LPTMR_SHIFT SIM_SCGC5_LPTIMER_SHIFT
#define SIM_SCGC5_LPTMR_MASK SIM_SCGC5_LPTIMER_MASK

#define CRC_DATA CRC_CRC
#define CRC_DATALL CRC_CRCLL

#define FlashConfig_BASE_PTR FTFL_FlashConfig_BASE_PTR
#define FlexRAM_BASE_PTR ((void *)0x14000000)

#define FLASH_SECTOR_SIZE 1024
#define FLASH_SECTION_SIZE 1024
#define FLASH_ELEM_SIZE 4

#define HAVE_PIT_SEPARATE_IRQ
#define USB_FMC_MASTER 3

/* Missing from header */
typedef struct MCM_MemMap {
  uint8_t RESERVED_0[8];
  uint16_t PLASC;
  uint16_t PLAMC;
  uint32_t PLACR;
} volatile *MCM_MemMapPtr;

#define MCM_PLASC_REG(base)                      ((base)->PLASC)
#define MCM_PLAMC_REG(base)                      ((base)->PLAMC)
#define MCM_PLACR_REG(base)                      ((base)->PLACR)

#define MCM_PLASC_ASC_MASK                       0xFFu
#define MCM_PLASC_ASC_SHIFT                      0
#define MCM_PLASC_ASC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLASC_ASC_SHIFT))&MCM_PLASC_ASC_MASK)
#define MCM_PLAMC_AMC_MASK                       0xFFu
#define MCM_PLAMC_AMC_SHIFT                      0
#define MCM_PLAMC_AMC(x)                         (((uint16_t)(((uint16_t)(x))<<MCM_PLAMC_AMC_SHIFT))&MCM_PLAMC_AMC_MASK)
#define MCM_PLACR_ARB_MASK                       0x200u
#define MCM_PLACR_ARB_SHIFT                      9

#define MCM_BASE_PTR                             ((MCM_MemMapPtr)0xE0080000u)
#define MCM_BASE_PTRS                            { MCM_BASE_PTR }

#define MCM_PLASC                                MCM_PLASC_REG(MCM_BASE_PTR)
#define MCM_PLAMC                                MCM_PLAMC_REG(MCM_BASE_PTR)
#define MCM_PLACR                                MCM_PLACR_REG(MCM_BASE_PTR)

#define ADC0                                     ADC0_BASE_PTR


#include <soc/cortex-m/soc.h>
#include <soc/cortex-m/sched-m3.h>
#include <soc/cortex-m/bitband.h>
#include <soc/cortex-m/bitfield-bitband.h>

#include <soc/kinetis/bitfield-bitband.h>

#include <soc/kinetis/periph/lptmr.h>
#include <soc/kinetis/periph/ftfl.h>
#include <soc/kinetis/periph/flashconfig.h>
#include <soc/kinetis/periph/fmc.h>
#include <soc/kinetis/periph/usbotg.h>
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
