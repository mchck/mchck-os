#include <mchck.h>

struct exception_frame {
        /* saved by CPU */
        uint32_t r0, r1, r2, r3, r12, lr, ret, xpsr;
};

struct extended_exception_frame {
        /* saved by scheduler entry */
        uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
        struct exception_frame;
};

static uint8_t supervisor_stack[512] __attribute__((aligned(8)));

static uint8_t idle_stack[16*4] __attribute__((aligned(8)));
static struct thread *idle;


void
md_need_reschedule(void)
{
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

static struct thread *
md_thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg)
{
        struct thread *t = (void *)stackbase;
        struct extended_exception_frame *f = (void *)((uintptr_t)stackbase + stacksize);

        f--;
        f->ret = (uintptr_t)fun;
        f->r0 = (uintptr_t)arg;
        f->xpsr = 1 << 24;
        t->md.sp = f;
        return (t);
}

struct thread *
thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg)
{
        struct thread *t;

        t = md_thread_init(stackbase, stacksize, fun, arg);
        mi_thread_init(t);
        return (t);
}

static void
idle_thread(void *arg)
{
        for (;;)
                __WFI();
}

void
md_enter_thread_mode(void)
{
        /* 100Hz time slice */
        scheduler_timeslice = core_clk / 100;

        /**
         * Idle thread setup.  Only use md_thread_init to prevent the
         * thread from being placed on the runq.
         */
        idle = md_thread_init(idle_stack, sizeof(idle_stack), idle_thread, NULL);

        /**
         * Initial thread setup.
         */
        /* set up PSP == current (MSP) sp */
        __set_PSP(__get_MSP());

        /* switch to PSP */
        CONTROL_Type ctrl = {.w = __get_CONTROL()};
        ctrl.b.SPSEL = 1;
        __set_CONTROL(ctrl.w);
        __ISB();

        __set_MSP((uintptr_t)supervisor_stack + sizeof(supervisor_stack));
}

void
SysTick_Handler(void)
{
        sys_yield();
}

void
SVCall_Handler(enum sys_op op, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
        struct exception_frame *ex;
        __asm__ volatile (
                "mrs %[savesp], PSP\n"
                : [savesp] "=r" (ex)
                );

        int ret = 0;

        switch (op) {
        case sys_op_yield:
                sys_yield();
                break;
        case sys_op_wait_cond:
                sys_wait_cond(arg1, (void *)arg2, arg3);
                break;
        case sys_op_wakeup:
                ret = sys_wakeup(arg1);
                break;
        case sys_op_setprio:
                ret = sys_setprio(arg1);
                break;
        }

        ex->r0 = ret;
}

static int
md_handler_syscall(enum sys_op op, uint32_t arg1)
{
        switch (op) {
        case sys_op_wakeup:
                return (sys_wakeup(arg1));
        default:
                panic("invalid syscall from handler mode");
        }
}

__attribute__((naked))
uint32_t
syscall(enum sys_op op, ...)
{
        __asm__ volatile (
                "mov r12, r0\n"
                "mrs r0, IPSR\n"
                "cmp r0, 0\n"
                "mov r0, r12\n"
                "bne md_handler_syscall\n"
                "svc 0\n"
                "bx lr\n"
                :: "i" (md_handler_syscall)
                );
}


void __attribute__((naked))
PendSV_Handler(void)
{
        uint32_t savesp;

        /* save remaining registers on task stack */
        __asm__ volatile (
                "push {lr}\n"
                "mrs %[savesp], PSP\n"
                "stmdb %[savesp]!, {r4-r11}\n"
                : [savesp] "=r" (savesp)
                );

        if (curthread)
                curthread->md.sp = (void *)savesp;
        else
                idle->md.sp = (void *)savesp;

        SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
        scheduler();

        struct thread *returnthread = curthread;

        /* idle -> sleep */
        if (returnthread) {
                SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
                SysTick->LOAD = curthread->slice_remaining;

                if (curthread->prio == THREAD_PRIO_REALTIME) {
                        /* realtime threads do not get interrupted */
                        SysTick->CTRL = 0;
                } else {
                        SysTick->CTRL =
                                SysTick_CTRL_CLKSOURCE_Msk |
                                SysTick_CTRL_TICKINT_Msk |
                                SysTick_CTRL_ENABLE_Msk;
                }
        } else {
                returnthread = idle;
                SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
                SysTick->CTRL = 0;
        }
        SysTick->VAL = 0;       /* trigger reload */

        /* restore remaining registers from task stack */
        __asm__ volatile (
                "ldmia %[savesp]!, {r4-r11}\n"
                "msr PSP, %[savesp]\n"
                "pop {pc}\n"
                :: [savesp] "r" (returnthread->md.sp) : "0"
                );
}

void
md_sched_update_timeslice(void)
{
        curthread->slice_remaining = SysTick->VAL;
        /* overflow or very little left */
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) ||
            curthread->slice_remaining < scheduler_timeslice / 256)
                curthread->slice_remaining = 0;
}
