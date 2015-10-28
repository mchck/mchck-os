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
#define DMA_NUM_CHAN 4

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

enum dmamux_channel {
  DMAMUX_DISABLED = 0,
  DMAMUX_UART0_Receive = 2,
  DMAMUX_UART0_Transmit = 3,
  DMAMUX_UART1_Receive = 4,
  DMAMUX_UART1_Transmit = 5,
  DMAMUX_UART2_Receive = 6,
  DMAMUX_UART2_Transmit = 7,
  DMAMUX_I2S0_Receive = 14,
  DMAMUX_I2S0_Transmit = 15,
  DMAMUX_SPI0_Receive = 16,
  DMAMUX_SPI0_Transmit = 17,
  DMAMUX_I2C0 = 22,
  DMAMUX_FTM0_Channel_0 = 24,
  DMAMUX_FTM0_Channel_1 = 25,
  DMAMUX_FTM0_Channel_2 = 26,
  DMAMUX_FTM0_Channel_3 = 27,
  DMAMUX_FTM0_Channel_4 = 28,
  DMAMUX_FTM0_Channel_5 = 29,
  DMAMUX_FTM0_Channel_6 = 30,
  DMAMUX_FTM0_Channel_7 = 31,
  DMAMUX_FTM1_Channel_0 = 32,
  DMAMUX_FTM1_Channel_1 = 33,
  DMAMUX_ADC0 = 40,
  DMAMUX_CMP0 = 42,
  DMAMUX_CMP1 = 43,
  DMAMUX_CMT = 47,
  DMAMUX_PDB = 48,
  DMAMUX_PORTA = 49,
  DMAMUX_PORTB = 50,
  DMAMUX_PORTC = 51,
  DMAMUX_PORTD = 52,
  DMAMUX_PORTE = 53,
  DMAMUX_ALWAYSON_0 = 54,
  DMAMUX_ALWAYSON_1 = 55,
  DMAMUX_ALWAYSON_2 = 56,
  DMAMUX_ALWAYSON_3 = 57,
  DMAMUX_ALWAYSON_4 = 58,
  DMAMUX_ALWAYSON_5 = 59,
  DMAMUX_ALWAYSON_6 = 60,
  DMAMUX_ALWAYSON_7 = 61,
  DMAMUX_ALWAYSON_8 = 62,
  DMAMUX_ALWAYSON_9 = 63,
};

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
#include <soc/kinetis/dma.h>
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
