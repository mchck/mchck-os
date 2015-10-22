#include <mchck.h>

STAILQ_HEAD(runq, thread);

static struct runq runq[THREAD_PRIO_MAX + 1] = {
        STAILQ_HEAD_INITIALIZER(runq[0]),
        STAILQ_HEAD_INITIALIZER(runq[1]),
        STAILQ_HEAD_INITIALIZER(runq[2]),
};
static struct runq blockedq = STAILQ_HEAD_INITIALIZER(blockedq);

uint32_t scheduler_timeslice;

static struct thread initial;
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

int
thread_setprio(enum thread_prio prio)
{
        return (syscall(sys_op_setprio, prio));
}

struct runq *
thread2runq(struct thread *t)
{
        return (&runq[t->prio]);
}

void
mi_thread_init(struct thread *t)
{
        t->state = thread_state_runq;
        t->prio = THREAD_PRIO_NORMAL;
        curthread->slice_remaining = 0;
        crit_enter();
        STAILQ_INSERT_TAIL(thread2runq(t), t, queue);
        crit_exit();
}

void
enter_thread_mode(void)
{
        md_enter_thread_mode();

        /* enter scheduler */
        curthread = &initial;
        curthread->state = thread_state_running;
        curthread->prio = THREAD_PRIO_NORMAL;
        curthread->slice_remaining = 0;
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
                        q = thread2runq(curthread);
                else
                        q = &blockedq;
                STAILQ_INSERT_TAIL(q, curthread, queue);
        }
        for (int i = 0; i <= THREAD_PRIO_MAX; ++i) {
                curthread = STAILQ_FIRST(&runq[i]);
                if (curthread)
                        break;
        }
        if (curthread) {
                STAILQ_REMOVE_HEAD(thread2runq(curthread), queue);
                curthread->state = thread_state_running;
                if (!curthread->slice_remaining)
                        curthread->slice_remaining = scheduler_timeslice;
        }
        crit_exit();
}

void
sys_yield(void)
{
        if (curthread->state == thread_state_running)
                curthread->slice_remaining = 0;
        else
                md_sched_update_timeslice();
        md_need_reschedule();
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
                struct runq *q = thread2runq(t);
                if (t->slice_remaining)
                        STAILQ_INSERT_HEAD(q, t, queue);
                else
                        STAILQ_INSERT_TAIL(q, t, queue);
                found = 1;

                if (t->prio < curthread->prio)
                        md_need_reschedule();
        }
        crit_exit();
        return (found);
}

int
sys_setprio(enum thread_prio prio)
{
        if (prio < 0 || prio > THREAD_PRIO_MAX)
                return (-1);

        if (curthread->prio == prio)
                return (0);

        crit_enter();
        curthread->prio = prio;
        crit_exit();
        md_need_reschedule();

        return (0);
}
