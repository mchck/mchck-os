#include <mchck.h>

uint8_t stack[512] __attribute__((aligned(8)));
uint8_t stack2[512] __attribute__((aligned(8)));

void
t1(void *a)
{
        thread_setprio(THREAD_PRIO_REALTIME);
        wait(t1);
        for (int rep = 100; rep; --rep) {
                printf("hi t1\n");
                for (volatile int delay = 100000; delay; --delay)
                        /* NOTHING */;
                if (rep == 85) {
                        printf("t1: going to sleep\n");
                        wait(t1);
                }
                if (rep == 70) {
                        printf("t1: setting normal priority\n");
                        thread_setprio(THREAD_PRIO_NORMAL);
                }
        }
        wait(NULL);
}

void
t2(void *a)
{
        for (int rep = 100; rep; --rep) {
                printf("hi t2\n");
                for (volatile int delay = 100000; delay; --delay)
                        /* NOTHING */;
                if (rep == 80) {
                        printf("t2: waking up t1\n");
                        wakeup(t1);
                }
        }
        wait(NULL);
}

void
main(void)
{
        thread_init(stack, sizeof(stack), t1, NULL);
        thread_init(stack2, sizeof(stack2), t2, NULL);
        enter_thread_mode();
        thread_setprio(THREAD_PRIO_REALTIME);
        wakeup(t1);
        for (int rep = 100; rep; --rep) {
                printf("main\n");
                for (volatile int delay = 100000; delay; --delay)
                        /* NOTHING */;
                if (rep == 70) {
                        printf("main: setting bulk priority\n");
                        thread_setprio(THREAD_PRIO_BESTEFFORT);
                }
        }
        printf("end\n");
        wait(NULL);
}
