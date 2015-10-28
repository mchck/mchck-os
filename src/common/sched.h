#include <common/queue.h>

enum thread_state {
        thread_state_invalid,
        thread_state_running,
        thread_state_runq,
        thread_state_blocked,
};

enum thread_prio {
        THREAD_PRIO_REALTIME,
        THREAD_PRIO_NORMAL,
        THREAD_PRIO_BESTEFFORT,
        THREAD_PRIO_MAX = THREAD_PRIO_BESTEFFORT
};

struct thread {
        struct thread_md md;
        STAILQ_ENTRY(thread) queue;
        enum thread_state state;
        uint32_t ident;
        uint32_t slice_remaining;
        enum thread_prio prio;
};

enum sys_op {
        sys_op_yield,
        sys_op_wait_cond,
        sys_op_wakeup,
        sys_op_setprio,
};

extern uint32_t scheduler_timeslice;
extern struct thread *curthread;

void sys_yield(void);
void sys_wait_cond(uint32_t ident, const volatile uint32_t *ptr, uint32_t val);
int sys_wakeup(uint32_t ident);
int sys_setprio(enum thread_prio prio);
void scheduler(void);

uint32_t syscall(enum sys_op op, ...);

void yield(void);
void wait(const void *ident);
void wait_cond(const void *ident, const volatile uint32_t *ptr, uint32_t val);
void wakeup(const void *ident);
int thread_setprio(enum thread_prio prio);
struct thread *thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg);
void mi_thread_init(struct thread *t);
void enter_thread_mode(void);
