#include <mchck.h>

uint8_t stack[512] __attribute__((aligned(8)));
uint8_t stack2[512] __attribute__((aligned(8)));

void main(void);

void
foo(void *arg)
{
        for (;;) {
                wait(foo);
                onboard_led(0);
                for (volatile int i = 1000000; i; --i)
                        /* NOTHING */;
                wakeup(main);
        }
}

void
main(void)
{
        thread_init(stack, sizeof(stack), foo, (void *)5);
        //thread_init(stack2, sizeof(stack2), foo, (void *)8);
        enter_thread_mode();
        for (;;) {
                onboard_led(1);
                for (volatile int i = 1000000; i; --i)
                        /* NOTHING */;
                wakeup(foo);
                wait(main);
        }
}
