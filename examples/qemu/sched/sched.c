#include <mchck.h>

uint8_t stack[512] __attribute__((aligned(8)));
uint8_t stack2[512] __attribute__((aligned(8)));

void
foo(void *arg)
{
        for (int rep = 100; rep;) {
                for (int i = (int)arg; i >= 0 && rep; --i, --rep) {
                        printf("hi %u %d\n", (unsigned)arg, i);
                        for (volatile int delay = 100000; delay; --delay)
                                /* NOTHING */;
                }
        }
        wait(NULL);
}

void
main(void)
{
        thread_init(stack, sizeof(stack), foo, (void *)5);
        thread_init(stack2, sizeof(stack2), foo, (void *)8);
        enter_thread_mode();
        for (int rep = 200; rep; --rep) {
                printf("main\n");
                for (volatile int delay = 100000; delay; --delay)
                        /* NOTHING */;
        }
        printf("end\n");
        wait(NULL);
}
