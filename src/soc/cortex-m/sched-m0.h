struct extended_exception_frame;

struct thread_md {
        struct extended_exception_frame *sp;
};

void md_need_reschedule(void);
void md_sched_update_timeslice(void);
void md_enter_thread_mode(void);
