#include <mchck.h>

STAILQ_HEAD(runq, thread);

static struct runq runq = STAILQ_HEAD_INITIALIZER(runq);
static struct runq blockedq = STAILQ_HEAD_INITIALIZER(blockedq);

struct thread *curthread;


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

void
mi_thread_init(struct thread *t)
{
        t->state = thread_state_runq;
        crit_enter();
        STAILQ_INSERT_TAIL(&runq, t, queue);
        crit_exit();
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
                if (!curthread->slice_remaining)
                        curthread->slice_remaining = scheduler_timeslice;
        }
        crit_exit();
}

void
sched_update_timeslice(void)
{
        curthread->slice_remaining = SysTick->VAL;
        /* overflow or very little left */
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) ||
            curthread->slice_remaining < scheduler_timeslice / 256)
                curthread->slice_remaining = 0;
}

void
sys_yield(void)
{
        if (curthread->state == thread_state_running)
                curthread->slice_remaining = 0;
        else
                sched_update_timeslice();
        md_yield();
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
                if (t->slice_remaining)
                        STAILQ_INSERT_HEAD(&runq, t, queue);
                else
                        STAILQ_INSERT_TAIL(&runq, t, queue);
                found = 1;
        }
        crit_exit();
        return (found);
}
