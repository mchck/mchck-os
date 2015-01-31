#include <mchck.h>

const uint8_t sys_reset_to_loader_magic[] = "\xff\x00\x7fRESET TO LOADER\x7f\x00\xff";


void
sys_reset(void)
{
        SCB_AIRCR =
                SCB_AIRCR_VECTKEY(SCB_AIRCR_VECTKEY) |
                SCB_AIRCR_SYSRESETREQ_MASK;
        for (;;);
}

void __attribute__((noreturn))
sys_yield_for_frogs(void)
{
        bf_set(SCB_SCR, SCB_SCR_SLEEPONEXIT, 1);
        for (;;)
                __asm__("wfi");
}

static int crit_nest;

void
crit_enter(void)
{
        __asm__("cpsid i");
        crit_nest++;
}

void
crit_exit(void)
{
        if (--crit_nest == 0)
                __asm__("cpsie i");
}

int
crit_active(void)
{
        return (crit_nest != 0);
}

static volatile const char *panic_reason;

void __attribute__((noreturn))
panic(const char *reason)
{
        crit_enter();
        panic_reason = reason;

        for (;;)
                /* infinite loop */;
}

void
int_enable(size_t intno)
{
        (&NVIC_BASE_PTR->ISER)[intno / 32] = 1 << (intno % 32);
}

void
int_disable(size_t intno)
{
        (&NVIC_BASE_PTR->ICER)[intno / 32] = 1 << (intno % 32);
}
