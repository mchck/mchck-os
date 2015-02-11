#include <soc/cortex-m/startup.c>

#include <mchck.h>

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

void
Default_Reset_Handler(void)
{
        watchdog_disable();

        /* Disable MPU */
        MPU_CESR = 0;

#ifdef EXTERNAL_XTAL
        OSC_CR = OSC_CR_SC16P_MASK;
        MCG.c2.raw = ((struct MCG_C2_t){
                        .range0 = MCG_RANGE_VERYHIGH,
                                .erefs0 = MCG_EREF_OSC
                                }).raw;
        MCG.c1.raw = ((struct MCG_C1_t){
                        .clks = MCG_CLKS_EXTERNAL,
                                .frdiv = 4, /* log2(EXTERNAL_XTAL) - 20 */
                                .irefs = 0
                                }).raw;

        while (!MCG.s.oscinit0)
                /* NOTHING */;
        while (MCG.s.clkst != MCG_CLKST_EXTERNAL)
                /* NOTHING */;

        MCG.c5.raw = ((struct MCG_C5_t){
                        .prdiv0 = ((EXTERNAL_XTAL / 2000000L) - 1),
                                .pllclken0 = 1
                                }).raw;
        MCG.c6.raw = ((struct MCG_C6_t){
                        .vdiv0 = 0,
                        .plls = 1
                                }).raw;

        while (!MCG.s.pllst)
                /* NOTHING */;
        while (!MCG.s.lock0)
                /* NOTHING */;

        MCG.c1.clks = MCG_CLKS_FLLPLL;

        while (MCG.s.clkst != MCG_CLKST_PLL)
                /* NOTHING */;

        SIM.sopt2.pllfllsel = SIM_PLLFLLSEL_PLL;
#else
        /* FLL at 48MHz */
        MCG_C4 = MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK;
        bf_set(SIM_SOPT2, SIM_SOPT2_PLLFLLSEL, SIM_PLLFLLSEL_FLL);
#endif

        memcpy(&_sdata, &_sidata, (uintptr_t)&_edata - (uintptr_t)&_sdata);
        memset(&_sbss, 0, (uintptr_t)&_ebss - (uintptr_t)&_sbss);

        main();
}
