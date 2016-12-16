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

void
Default_Reset_Handler(void)
{
        /* XXX this is permanent - move to application code? */
        watchdog_disable();

#ifdef EXTERNAL_XTAL
#error writeme
#else
        /* HIRC at 48MHz */
        MCG->C1 = 0;

        uint32_t clock = 48000000;
#endif

        memcpy(&_sdata, &_sidata, (uintptr_t)&_edata - (uintptr_t)&_sdata);
        memset(&_sbss, 0, (uintptr_t)&_ebss - (uintptr_t)&_sbss);
        core_clk = clock;

        main();
}
