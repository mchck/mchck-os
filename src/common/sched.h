#include <common/queue.h>

enum thread_state {
        thread_state_invalid,
        thread_state_running,
        thread_state_runq,
        thread_state_blocked,
};

struct thread {
        struct thread_md md;
        STAILQ_ENTRY(thread) queue;
        enum thread_state state;
        uint32_t ident;
        uint32_t slice_remaining;
};

enum sys_op {
        sys_op_yield,
        sys_op_wait,
        sys_op_wakeup,
};

extern uint32_t scheduler_timeslice;
extern struct thread *curthread;

void sys_yield(void);
void sys_wait(uint32_t ident);
int sys_wakeup(uint32_t ident);
void scheduler(void);

uint32_t syscall(enum sys_op op, ...);

void yield(void);
void wait(const void *ident);
void wakeup(const void *ident);
struct thread *thread_init(void *stackbase, size_t stacksize, void (*fun)(void *), void *arg);
void mi_thread_init(struct thread *t);
void enter_thread_mode(void);
