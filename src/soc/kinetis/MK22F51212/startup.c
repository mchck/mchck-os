#include <soc/cortex-m/startup.c>

#include <mchck.h>

uint32_t core_clk;

/**
 * The following variables are only used for their addresses;
 * their symbols are defined by the linker script.
 *
 * They are used to delimit various sections the process image:
 * _sidata marks where the flash copy of the .data section starts.
 * _sdata and _edata delimit the RAM addresses of the .data section.
 * _sbss and _ebss delimit the RAM BSS section in the same way.
 */

void main(void);

__attribute__((__externally_visible__))
void
Default_Reset_Handler(void)
{
        watchdog_disable();

#if EXTERNAL_XTAL
        /* 24MHz crystal -> 4MHz PLL input -> 120MHz system */

        MCG_C2 = MCG_C2_RANGE(MCG_RANGE_VERYHIGH) | MCG_C2_EREFS(MCG_EREF_OSC);
        MCG_C1 = MCG_C1_CLKS(MCG_CLKS_EXTERNAL);

        while (!bf_get_reg(MCG_S, MCG_S_OSCINIT0))
                /* NOTHING */;
        while (bf_get_reg(MCG_S, MCG_S_CLKST) != MCG_CLKST_EXTERNAL)
                /* NOTHING */;

        /* 4MHz PLL input */
        MCG_C5 = MCG_C5_PRDIV0(0b00101) | MCG_C5_PLLCLKEN0(1);
        /* 120MHz PLL output */
        MCG_C6 = MCG_C6_VDIV0(0b00110) | MCG_C6_PLLS(1);

        while (!bf_get_reg(MCG_S, MCG_S_PLLST))
                /* NOTHING */;
        while (!bf_get_reg(MCG_S, MCG_S_LOCK0))
                /* NOTHING */;

        /* system clock = 120MHz (max 120MHz) */
        /* bus clock = 60MHz (max 60MHz) */
        bf_set_reg(SIM_CLKDIV1, SIM_CLKDIV1_OUTDIV2, 1);
        /* flexbus clock = 30MHz (max 30MHz) */
        bf_set_reg(SIM_CLKDIV1, SIM_CLKDIV1_OUTDIV3, 3);
        /* flash clock = 24MHz (max 26.67MHz) */
        bf_set_reg(SIM_CLKDIV1, SIM_CLKDIV1_OUTDIV4, 4);

        MCG_C1 = MCG_C1_CLKS(MCG_CLKS_FLLPLL);

        while (bf_get_reg(MCG_S, MCG_S_CLKST) != MCG_CLKST_PLL)
                /* NOTHING */;

        uint32_t clock = 120000000;
#else
        /* FLL at 48MHz */
        MCG_C4 = MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK;
        bf_set_reg(SIM_SOPT2, SIM_SOPT2_PLLFLLSEL, SIM_PLLFLLSEL_FLL);

        uint32_t clock = 48000000;
#endif

        memcpy(&_sdata, &_sidata, (uintptr_t)&_edata - (uintptr_t)&_sdata);
        memset(&_sbss, 0, (uintptr_t)&_ebss - (uintptr_t)&_sbss);
        core_clk = clock;

        main();
}
