#include <soc/cortex-m/intnums.h>

#include <MK20D5.h>

#undef DCRSR
#undef DCRDR
#undef DEMCR
#define __NVIC_PRIO_BITS 4
typedef enum {
  NMI_IRQn                      = 2,
  Hard_Fault_IRQn               = 3,
  Mem_Manage_Fault_IRQn         = 4,
  Bus_Fault_IRQn                = 5,
  Usage_Fault_IRQn              = 6,
  Reserved7_IRQn                = 7,
  Reserved8_IRQn                = 8,
  Reserved9_IRQn                = 9,
  Reserved10_IRQn               = 10,
  SVCall_IRQn                   = 11,
  DebugMonitor_IRQn             = 12,
  Reserved13_IRQn               = 13,
  PendableSrvReq_IRQn           = 14,
  SysTick_IRQn                  = 15,
  DMA0_IRQn                     = 16,
  DMA1_IRQn                     = 17,
  DMA2_IRQn                     = 18,
  DMA3_IRQn                     = 19,
  DMA_Error_IRQn                = 20,
  Reserved21_IRQn               = 21,
  FTFL_IRQn                     = 22,
  Read_Collision_IRQn           = 23,
  LVD_LVW_IRQn                  = 24,
  LLW_IRQn                      = 25,
  Watchdog_IRQn                 = 26,
  I2C0_IRQn                     = 27,
  SPI0_IRQn                     = 28,
  I2S0_Tx_IRQn                  = 29,
  I2S0_Rx_IRQn                  = 30,
  UART0_LON_IRQn                = 31,
  UART0_RX_TX_IRQn              = 32,
  UART0_ERR_IRQn                = 33,
  UART1_RX_TX_IRQn              = 34,
  UART1_ERR_IRQn                = 35,
  UART2_RX_TX_IRQn              = 36,
  UART2_ERR_IRQn                = 37,
  ADC0_IRQn                     = 38,
  CMP0_IRQn                     = 39,
  CMP1_IRQn                     = 40,
  FTM0_IRQn                     = 41,
  FTM1_IRQn                     = 42,
  CMT_IRQn                      = 43,
  RTC_IRQn                      = 44,
  RTC_Seconds_IRQn              = 45,
  PIT0_IRQn                     = 46,
  PIT1_IRQn                     = 47,
  PIT2_IRQn                     = 48,
  PIT3_IRQn                     = 49,
  PDB0_IRQn                     = 50,
  USB0_IRQn                     = 51,
  USBDCD_IRQn                   = 52,
  TSI0_IRQn                     = 53,
  MCG_IRQn                      = 54,
  LPTimer_IRQn                  = 55,
  PORTA_IRQn                    = 56,
  PORTB_IRQn                    = 57,
  PORTC_IRQn                    = 58,
  PORTD_IRQn                    = 59,
  PORTE_IRQn                    = 60,
  SWI_IRQn                      = 61
} IRQn_Type;
#include <core_cm4.h>

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
typedef volatile struct MCM_MemMap MCM_Type;

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


typedef volatile struct ADC_MemMap ADC_Type;
typedef volatile struct CMP_MemMap CMP_Type;
typedef volatile struct CMT_MemMap CMT_Type;
typedef volatile struct CRC_MemMap CRC_Type;
typedef volatile struct DMA_MemMap DMA_Type;
typedef volatile struct DMAMUX_MemMap DMAMUX_Type;
typedef volatile struct ETF_MemMap ETF_Type;
typedef volatile struct EWM_MemMap EWM_Type;
typedef volatile struct FMC_MemMap FMC_Type;
typedef volatile struct FPB_MemMap FPB_Type;
typedef volatile struct FTFL_MemMap FTFL_Type;
typedef volatile struct FTM_MemMap FTM_Type;
typedef volatile struct GPIO_MemMap GPIO_Type;
typedef volatile struct I2C_MemMap I2C_Type;
typedef volatile struct I2S_MemMap I2S_Type;
typedef volatile struct LLWU_MemMap LLWU_Type;
typedef volatile struct LPTMR_MemMap LPTMR_Type;
typedef volatile struct MCG_MemMap MCG_Type;
typedef volatile struct NV_MemMap NV_Type;
typedef volatile struct OSC_MemMap OSC_Type;
typedef volatile struct PDB_MemMap PDB_Type;
typedef volatile struct PIT_MemMap PIT_Type;
typedef volatile struct PMC_MemMap PMC_Type;
typedef volatile struct PORT_MemMap PORT_Type;
typedef volatile struct RCM_MemMap RCM_Type;
typedef volatile struct RFSYS_MemMap RFSYS_Type;
typedef volatile struct RFVBAT_MemMap RFVBAT_Type;
typedef volatile struct RTC_MemMap RTC_Type;
typedef volatile struct SIM_MemMap SIM_Type;
typedef volatile struct SMC_MemMap SMC_Type;
typedef volatile struct SPI_MemMap SPI_Type;
typedef volatile struct TPIU_MemMap TPIU_Type;
typedef volatile struct TSI_MemMap TSI_Type;
typedef volatile struct UART_MemMap UART_Type;
typedef volatile struct USB_MemMap USB_Type;
typedef volatile struct USBDCD_MemMap USBDCD_Type;
typedef volatile struct VREF_MemMap VREF_Type;
typedef volatile struct WDOG_MemMap WDOG_Type;

#define ADC0 ADC0_BASE_PTR
#define CMP0 CMP0_BASE_PTR
#define CMP1 CMP1_BASE_PTR
#define CMT CMT_BASE_PTR
#define DMA DMA_BASE_PTR
#define DMAMUX DMAMUX_BASE_PTR
#define ETF ETF_BASE_PTR
#define EWM EWM_BASE_PTR
#define FMC FMC_BASE_PTR
#define FPB FPB_BASE_PTR
#define FTFL FTFL_BASE_PTR
#define FTM0 FTM0_BASE_PTR
#define FTM1 FTM1_BASE_PTR
#define PTA PTA_BASE_PTR
#define PTB PTB_BASE_PTR
#define PTC PTC_BASE_PTR
#define PTD PTD_BASE_PTR
#define PTE PTE_BASE_PTR
#define I2C0 I2C0_BASE_PTR
#define I2S0 I2S0_BASE_PTR
#define LLWU LLWU_BASE_PTR
#define LPTMR0 LPTMR0_BASE_PTR
#define MCG MCG_BASE_PTR
#define MCM MCM_BASE_PTR
#define FTFL_FlashConfig FTFL_FlashConfig_BASE_PTR
#define OSC0 OSC0_BASE_PTR
#define PDB0 PDB0_BASE_PTR
#define PIT PIT_BASE_PTR
#define PMC PMC_BASE_PTR
#define PORTA PORTA_BASE_PTR
#define PORTB PORTB_BASE_PTR
#define PORTC PORTC_BASE_PTR
#define PORTD PORTD_BASE_PTR
#define PORTE PORTE_BASE_PTR
#define RCM RCM_BASE_PTR
#define RFSYS RFSYS_BASE_PTR
#define RFVBAT RFVBAT_BASE_PTR
#define RTC RTC_BASE_PTR
#define SIM SIM_BASE_PTR
#define SMC SMC_BASE_PTR
#define SPI0 SPI0_BASE_PTR
#define TPIU TPIU_BASE_PTR
#define TSI0 TSI0_BASE_PTR
#define UART0 UART0_BASE_PTR
#define UART1 UART1_BASE_PTR
#define UART2 UART2_BASE_PTR
#define USB0 USB0_BASE_PTR
#define USBDCD USBDCD_BASE_PTR
#define VREF VREF_BASE_PTR
#define WDOG WDOG_BASE_PTR

#define ADC0_BASE ((uintptr_t)ADC0_BASE_PTR)
#define CMP0_BASE ((uintptr_t)CMP0_BASE_PTR)
#define CMP1_BASE ((uintptr_t)CMP1_BASE_PTR)
#define CMT_BASE ((uintptr_t)CMT_BASE_PTR)
#define CRC_BASE ((uintptr_t)CRC_BASE_PTR)
#define DMA_BASE ((uintptr_t)DMA_BASE_PTR)
#define DMAMUX_BASE ((uintptr_t)DMAMUX_BASE_PTR)
#define ETF_BASE ((uintptr_t)ETF_BASE_PTR)
#define EWM_BASE ((uintptr_t)EWM_BASE_PTR)
#define FMC_BASE ((uintptr_t)FMC_BASE_PTR)
#define FPB_BASE ((uintptr_t)FPB_BASE_PTR)
#define FTFL_BASE ((uintptr_t)FTFL_BASE_PTR)
#define FTM0_BASE ((uintptr_t)FTM0_BASE_PTR)
#define FTM1_BASE ((uintptr_t)FTM1_BASE_PTR)
#define PTA_BASE ((uintptr_t)PTA_BASE_PTR)
#define PTB_BASE ((uintptr_t)PTB_BASE_PTR)
#define PTC_BASE ((uintptr_t)PTC_BASE_PTR)
#define PTD_BASE ((uintptr_t)PTD_BASE_PTR)
#define PTE_BASE ((uintptr_t)PTE_BASE_PTR)
#define I2C0_BASE ((uintptr_t)I2C0_BASE_PTR)
#define I2S0_BASE ((uintptr_t)I2S0_BASE_PTR)
#define LLWU_BASE ((uintptr_t)LLWU_BASE_PTR)
#define LPTMR0_BASE ((uintptr_t)LPTMR0_BASE_PTR)
#define MCG_BASE ((uintptr_t)MCG_BASE_PTR)
#define MCM_BASE ((uintptr_t)MCM_BASE_PTR)
#define FTFL_FlashConfig_BASE ((uintptr_t)FTFL_FlashConfig_BASE_PTR)
#define OSC0_BASE ((uintptr_t)OSC0_BASE_PTR)
#define PDB0_BASE ((uintptr_t)PDB0_BASE_PTR)
#define PIT_BASE ((uintptr_t)PIT_BASE_PTR)
#define PMC_BASE ((uintptr_t)PMC_BASE_PTR)
#define PORTA_BASE ((uintptr_t)PORTA_BASE_PTR)
#define PORTB_BASE ((uintptr_t)PORTB_BASE_PTR)
#define PORTC_BASE ((uintptr_t)PORTC_BASE_PTR)
#define PORTD_BASE ((uintptr_t)PORTD_BASE_PTR)
#define PORTE_BASE ((uintptr_t)PORTE_BASE_PTR)
#define RCM_BASE ((uintptr_t)RCM_BASE_PTR)
#define RFSYS_BASE ((uintptr_t)RFSYS_BASE_PTR)
#define RFVBAT_BASE ((uintptr_t)RFVBAT_BASE_PTR)
#define RTC_BASE ((uintptr_t)RTC_BASE_PTR)
#define SIM_BASE ((uintptr_t)SIM_BASE_PTR)
#define SMC_BASE ((uintptr_t)SMC_BASE_PTR)
#define SPI0_BASE ((uintptr_t)SPI0_BASE_PTR)
#define TPIU_BASE ((uintptr_t)TPIU_BASE_PTR)
#define TSI0_BASE ((uintptr_t)TSI0_BASE_PTR)
#define UART0_BASE ((uintptr_t)UART0_BASE_PTR)
#define UART1_BASE ((uintptr_t)UART1_BASE_PTR)
#define UART2_BASE ((uintptr_t)UART2_BASE_PTR)
#define USB0_BASE ((uintptr_t)USB0_BASE_PTR)
#define USBDCD_BASE ((uintptr_t)USBDCD_BASE_PTR)
#define VREF_BASE ((uintptr_t)VREF_BASE_PTR)
#define WDOG_BASE ((uintptr_t)WDOG_BASE_PTR)


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
