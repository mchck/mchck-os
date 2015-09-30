struct exception_frame;

struct thread_md {
        struct exception_frame *sp;
};

void md_yield(void);
