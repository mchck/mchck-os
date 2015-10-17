#include <mchck.h>

enum {
        SIM_RAMSIZE_8KB = 1,
        SIM_RAMSIZE_16KB = 3
};

enum {
        SIM_OSC32KSEL_SYSTEM = 0,
        SIM_OSC32KSEL_RTC = 2,
        SIM_OSC32KSEL_LPO = 3
};

enum {
        SIM_RTCCLKOUTSEL_1HZ = 0,
        SIM_RTCCLKOUTSEL_32KHZ = 1
};

enum {
        SIM_CLKOUTSEL_FLASH = 2,
        SIM_CLKOUTSEL_LPO = 3,
        SIM_CLKOUTSEL_MCG = 4,
        SIM_CLKOUTSEL_RTC = 5,
        SIM_CLKOUTSEL_OSC = 6
};

enum {
        SIM_PTD7PAD_SINGLE = 0,
        SIM_PTD7PAD_DOUBLE = 1
};

enum {
        SIM_TRACECLKSEL_MCG = 0,
        SIM_TRACECLKSEL_CORE = 1
};

enum {
        SIM_PLLFLLSEL_FLL = 0b00,
        SIM_PLLFLLSEL_PLL = 0b01,
        SIM_PLLFLLSEL_IRC48M = 0b11,
};

enum {
        SIM_USBSRC_EXTERNAL = 0,
        SIM_USBSRC_MCG = 1
};

enum {
        SIM_FTMFLT_FTM = 0,
        SIM_FTMFLT_CMP = 1
};

enum {
        SIM_FTMCHSRC_FTM = 0,
        SIM_FTMCHSRC_CMP0 = 1,
        SIM_FTMCHSRC_CMP1 = 2,
        SIM_FTMCHSRC_USBSOF = 3
};

enum {
        SIM_FTMCLKSEL_CLK0 = 0,
        SIM_FTMCLKSEL_CLK1 = 1
};

enum {
        SIM_FTMTRGSRC_HSCMP0 = 0,
        SIM_FTMTRGSRC_FTM1 = 1
};

enum {
        SIM_UARTTXSRC_UART = 0,
        SIM_UARTTXSRC_FTM = 1
};

enum {
        SIM_UARTRXSRC_UART = 0,
        SIM_UARTRXSRC_CMP0 = 1,
        SIM_UARTRXSRC_CMP1 = 2
};

enum {
        SIM_ADCTRGSEL_PDB = 0,
        SIM_ADCTRGSEL_HSCMP0 = 1,
        SIM_ADCTRGSEL_HSCMP1 = 2,
        SIM_ADCTRGSEL_PIT0 = 4,
        SIM_ADCTRGSEL_PIT1 = 5,
        SIM_ADCTRGSEL_PIT2 = 6,
        SIM_ADCTRGSEL_PIT3 = 7,
        SIM_ADCTRGSEL_FTM0 = 8,
        SIM_ADCTRGSEL_FTM1 = 9,
        SIM_ADCTRGSEL_RTCALARM = 12,
        SIM_ADCTRGSEL_RTCSECS = 13,
        SIM_ADCTRGSEL_LPTIMER = 14
};

enum {
        SIM_ADCPRETRGSEL_A = 0,
        SIM_ADCPRETRGSEL_B = 1
};

enum {
        SIM_ADCALTTRGEN_PDB = 0,
        SIM_ADCALTTRGEN_ALT = 1
};

enum {
        SIM_PINID_32 = 2,
        SIM_PINID_48 = 4,
        SIM_PINID_64 = 5
};

enum {
        SIM_FAMID_K10 = 0,
        SIM_FAMID_K20 = 1
};

enum {
        SIM_FLEXNVM_DATA_32_EEPROM_0  = 0x0,
        SIM_FLEXNVM_DATA_24_EEPROM_8  = 0x1,
        SIM_FLEXNVM_DATA_16_EEPROM_16 = 0x2,
        SIM_FLEXNVM_DATA_8_EEPROM_24  = 0x9,
        SIM_FLEXNVM_DATA_0_EEPROM_32  = 0x3
};

enum {
        SIM_EESIZE_2KB = 3,
        SIM_EESIZE_1KB = 4,
        SIM_EESIZE_512B = 5,
        SIM_EESIZE_256B = 6,
        SIM_EESIZE_128B = 7,
        SIM_EESIZE_64B = 8,
        SIM_EESIZE_32B = 9,
        SIM_EESIZE_0B = 15
};

enum {
        SIM_PFSIZE_32KB = 3,
        SIM_PFSIZE_64KB = 5,
        SIM_PFSIZE_128KB = 7
};

enum {
        SIM_NVMSIZE_0KB = 0,
        SIM_NVMSIZE_32KB = 3
};

enum {
        SIM_PFLSH_FLEXNVM = 0,
        SIM_PFLSH_PROGRAM = 1
};
