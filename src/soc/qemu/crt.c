#include <mchck.h>

extern void main(void);

static uint8_t stack[2048] __attribute__((aligned(16), used));

__attribute__((__externally_visible__))
void
_start(void)
{
        memcpy(&_sdata, &_sidata, (uintptr_t)&_edata - (uintptr_t)&_sdata);
        memset(&_sbss, 0, (uintptr_t)&_ebss - (uintptr_t)&_sbss);
        main();
        semihosting_exit(1);
}

void SVCall_Handler(void) __attribute__((weak));
void PendSV_Handler(void) __attribute__((weak));
void SysTick_Handler(void) __attribute__((weak));

uint32_t isr_vectors[] __attribute__((section(".isr_vectors"), externally_visible)) = {
        [0] = (uintptr_t)&stack[sizeof(stack)],
        [1] = (uintptr_t)_start,
        [11] = (uintptr_t)SVCall_Handler,
        [14] = (uintptr_t)PendSV_Handler,
        [15] = (uintptr_t)SysTick_Handler,
};
