#include <soc/cortex-m/intnums.h>

#include <MK22F51212.h>

#define FlashConfig_BASE_PTR FTFA_FlashConfig_BASE_PTR

#define FLASH_SECTOR_SIZE 2048
#define FLASH_ELEM_SIZE 4

#define HAVE_PIT_SEPARATE_IRQ
#define USB_FMC_MASTER 4
#define DMA_NUM_CHAN 16

enum dmamux_channel {
        DMAMUX_DISABLED = 0,
        DMAMUX_UART0_Receive = 2,
        DMAMUX_UART0_Transmit = 3,
        DMAMUX_UART1_Receive = 4,
        DMAMUX_UART1_Transmit = 5,
        DMAMUX_UART2_Receive = 6,
        DMAMUX_UART2_Transmit = 7,
        DMAMUX_I2S0_Receive = 12,
        DMAMUX_I2S0_Transmit = 13,
        DMAMUX_SPI0_Receive = 14,
        DMAMUX_SPI0_Transmit = 15,
        DMAMUX_SPI1 = 16,
        DMAMUX_I2C0 = 18,
        DMAMUX_I2C1 = 19,
        DMAMUX_FTM0_Channel_0 = 20,
        DMAMUX_FTM0_Channel_1 = 21,
        DMAMUX_FTM0_Channel_2 = 22,
        DMAMUX_FTM0_Channel_3 = 23,
        DMAMUX_FTM0_Channel_4 = 24,
        DMAMUX_FTM0_Channel_5 = 25,
        DMAMUX_FTM0_Channel_6 = 26,
        DMAMUX_FTM0_Channel_7 = 27,
        DMAMUX_FTM1_Channel_0 = 28,
        DMAMUX_FTM1_Channel_1 = 29,
        DMAMUX_FTM2_Channel_0 = 30,
        DMAMUX_FTM2_Channel_1 = 31,
        DMAMUX_FTM3_Channel_0 = 32,
        DMAMUX_FTM3_Channel_1 = 33,
        DMAMUX_FTM3_Channel_2 = 34,
        DMAMUX_FTM3_Channel_3 = 35,
        DMAMUX_FTM3_Channel_4 = 36,
        DMAMUX_FTM3_Channel_5 = 37,
        DMAMUX_FTM3_Channel_6 = 38,
        DMAMUX_FTM3_Channel_7 = 39,
        DMAMUX_ADC0 = 40,
        DMAMUX_ADC1 = 41,
        DMAMUX_CMP0 = 42,
        DMAMUX_CMP1 = 43,
        DMAMUX_DAC0 = 45,
        DMAMUX_DAC1 = 46,
        DMAMUX_PDB = 48,
        DMAMUX_PORTA = 49,
        DMAMUX_PORTB = 50,
        DMAMUX_PORTC = 51,
        DMAMUX_PORTD = 52,
        DMAMUX_PORTE = 53,
        DMAMUX_LPUART0_Receive = 58,
        DMAMUX_LPUART0_Transmit = 59,
        DMAMUX_ALWAYSON_0 = 60,
        DMAMUX_ALWAYSON_1 = 61,
        DMAMUX_ALWAYSON_2 = 62,
        DMAMUX_ALWAYSON_3 = 63,
};

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
