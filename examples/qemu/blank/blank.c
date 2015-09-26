#include <mchck.h>

#include "queue.h"


struct thread {
        uint32_t sp;
        STAILQ_ENTRY(thread) runq;
};

struct exception_frame {
        /* saved by scheduler entry */
        uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
        /* saved by CPU */
        uint32_t r0, r1, r2, r3, r12, lr, ret, xpsr;
};


STAILQ_HEAD(, thread) runq = STAILQ_HEAD_INITIALIZER(runq);

struct thread *curthread;


struct thread *
thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg)
{
        struct thread *t = (void *)stackbase;
        struct exception_frame *f = (void *)((uintptr_t)stackbase + stacksize);

        f--;
        f->ret = (uintptr_t)fun;
        f->r0 = (uintptr_t)arg;
        f->xpsr = 1 << 24;
        t->sp = (uintptr_t)f;
        STAILQ_INSERT_TAIL(&runq, t, runq);
        return (t);
}

void __attribute__((noreturn))
enter_thread_mode(void)
{
        /**
         * Idle (first) thread setup.
         */
        /* set up PSP == current (MSP) sp */
        __set_PSP(__get_MSP());

        /* switch to PSP */
        CONTROL_Type ctrl = {.w = __get_CONTROL()};
        ctrl.b.SPSEL = 1;
        __set_CONTROL(ctrl.w);
        __ISB();

        /* enter scheduler */
        __asm__ volatile ("svc 0");

        /**
         * We should never get here (because of SLEEPONEXIT), but ARM
         * says that there might be spurious returns.
         */
        for (;;)
                __WFI();

        __builtin_unreachable();
}

void
scheduler(void)
{
        if (curthread)
                STAILQ_INSERT_TAIL(&runq, curthread, runq);
        curthread = STAILQ_FIRST(&runq);
        if (curthread)
                STAILQ_REMOVE_HEAD(&runq, runq);
}

void
SVCall_Handler(enum syscall op, ...)
{
        switch (op) {
        case sys_op_yield:
                SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
                break;
        case sys_op_sleep:
                break;
        }
}

void __attribute__((naked))
PendSV_Handler(void)
{
        /* save remaining registers on task stack */
        __asm__ volatile (
                "mrs %[savesp], PSP\n"
                "stmdb %[savesp]!, {r4-r11}\n"
                : [savesp] "=r" (curthread->sp)
                );

        SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
        scheduler();

        /*  */
        if (curthread)
                SCB->SCR &= SCB_SCR_SLEEPDEEP_Msk;
        else
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

        /* restore remaining registers from task stack */
        __asm__ volatile (
                "ldmia %[savesp]!, {r4-r11}\n"
                "msr PSP, %[savesp]\n"
                "bx lr\n"
                :: [savesp] "r" (curthread->sp) : "0"
                );
}


uint8_t stack[512] __attribute__((aligned(8)));
uint8_t stack2[512] __attribute__((aligned(8)));

void
foo(void *arg)
{
        for (;;) {
                printf("hi %u\n", (unsigned)arg);
                __asm__ volatile ("svc 0");
        }
}

void
main(void)
{
        thread_init(stack, sizeof(stack), foo, (void *)5);
        thread_init(stack2, sizeof(stack2), foo, (void *)8);
        enter_thread_mode();
}
