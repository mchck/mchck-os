struct extended_exception_frame;

struct thread_md {
        struct extended_exception_frame *sp;
};

void md_yield(void);
void md_sched_update_timeslice(void);
