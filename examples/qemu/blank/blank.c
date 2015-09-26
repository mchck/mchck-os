#include <mchck.h>

#include "queue.h"


struct exception_frame {
        /* saved by scheduler entry */
        uint32_t r4, r5, r6, r7, r8, r9, r10, r11;
        /* saved by CPU */
        uint32_t r0, r1, r2, r3, r12, lr, ret, xpsr;
};

enum thread_state {
        thread_state_invalid,
        thread_state_running,
        thread_state_runq,
        thread_state_blocked,
};

struct thread {
        struct exception_frame *sp;
        STAILQ_ENTRY(thread) queue;
        enum thread_state state;
        uintptr_t ident;
};


enum sys_op {
        sys_op_yield,
        sys_op_wait,
        sys_op_wakeup,
};


STAILQ_HEAD(runq, thread);

struct runq runq = STAILQ_HEAD_INITIALIZER(runq);
struct runq blockedq = STAILQ_HEAD_INITIALIZER(blockedq);

struct thread *curthread;

uint8_t supervisor_stack[512] __attribute__((aligned(8)));
struct thread initial;


__attribute__((naked))
uint32_t
syscall(enum sys_op op, ...)
{
        __asm__ volatile (
                "svc 0\n"
                "bx lr\n"
                );
}

void
yield(void)
{
        syscall(sys_op_yield);
}

void
wait(const void *ident)
{
        syscall(sys_op_wait, ident);
}

void
wakeup(const void *ident)
{
        syscall(sys_op_wakeup, ident);
}


struct thread *
thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg)
{
        struct thread *t = (void *)stackbase;
        struct exception_frame *f = (void *)((uintptr_t)stackbase + stacksize);

        f--;
        f->ret = (uintptr_t)fun;
        f->r0 = (uintptr_t)arg;
        f->xpsr = 1 << 24;
        t->sp = f;
        t->state = thread_state_runq;
        crit_enter();
        STAILQ_INSERT_TAIL(&runq, t, queue);
        crit_exit();
        return (t);
}

void
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

        __set_MSP((uintptr_t)supervisor_stack + sizeof(supervisor_stack));

        /* enter scheduler */
        curthread = &initial;
        curthread->state = thread_state_running;
        yield();
}

void
scheduler(void)
{
        crit_enter();
        if (curthread) {
                struct runq *q;

                if (curthread->state == thread_state_running)
                        curthread->state = thread_state_runq;

                if (curthread->state == thread_state_runq)
                        q = &runq;
                else
                        q = &blockedq;
                STAILQ_INSERT_TAIL(q, curthread, queue);
        }
        curthread = STAILQ_FIRST(&runq);
        if (curthread) {
                STAILQ_REMOVE_HEAD(&runq, queue);
                curthread->state = thread_state_running;
        }
        crit_exit();
}

void
sys_yield(void)
{
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void
sys_wait(uint32_t ident)
{
        curthread->ident = ident;
        curthread->state = thread_state_blocked;
        sys_yield();
}

int
sys_wakeup(uint32_t ident)
{
        int found = 0;

        crit_enter();
        struct thread *t, *tnext;
        STAILQ_FOREACH_SAFE(t, &blockedq, queue, tnext) {
                if (t->ident != ident)
                        continue;
                STAILQ_REMOVE(&blockedq, t, thread, queue);
                t->state = thread_state_runq;
                STAILQ_INSERT_TAIL(&runq, t, queue);
                found = 1;
        }
        crit_exit();
        return (found);
}

void
SVCall_Handler(enum sys_op op, uint32_t arg1, uint32_t arg2)
{
        int ret = 0;

        switch (op) {
        case sys_op_yield:
                sys_yield();
                break;
        case sys_op_wait:
                sys_wait(arg1);
                break;
        case sys_op_wakeup:
                ret = sys_wakeup(arg1);
                break;
        }

        curthread->sp->r0 = ret;
}

void __attribute__((naked))
PendSV_Handler(void)
{
        /* save remaining registers on task stack */
        __asm__ volatile (
                "push {lr}\n"
                "mrs %[savesp], PSP\n"
                "stmdb %[savesp]!, {r4-r11}\n"
                : [savesp] "=r" (curthread->sp)
                );

        SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
        scheduler();

        /* idle -> sleep */
        if (curthread)
                SCB->SCR &= SCB_SCR_SLEEPDEEP_Msk;
        else
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        /* XXX need idle thread to cope with spurious wakeups */

        /* restore remaining registers from task stack */
        __asm__ volatile (
                "ldmia %[savesp]!, {r4-r11}\n"
                "msr PSP, %[savesp]\n"
                "pop {lr}\n"
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
                for (int i = (int)arg; i >= 0; --i) {
                        printf("hi %u %d\n", (unsigned)arg, i);
                        yield();
                }
                wakeup(foo);
                wait(foo);
        }
}

void
main(void)
{
        thread_init(stack, sizeof(stack), foo, (void *)5);
        thread_init(stack2, sizeof(stack2), foo, (void *)8);
        enter_thread_mode();
        for (;;) {
                printf("main\n");
                yield();
        }
}
