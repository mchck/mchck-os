typedef enum IRQn_Type {
        SysTick_IRQn,
} IRQn_Type;

#define __CM4_REV 0
#define __FPU_PRESENT 0
#define __NVIC_PRIO_BITS 4
#define __Vendor_SysTickConfig 0

#include <soc/cortex-m/core_cm4.h>
#include <soc/cortex-m/soc.h>

void semihosting_exit(int fail) __attribute__((noreturn));
