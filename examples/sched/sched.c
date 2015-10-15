#include <mchck.h>

uint8_t stack[512] __attribute__((aligned(8)));

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
        enter_thread_mode();
        thread_init(stack, sizeof(stack), foo, (void *)5);

        /**
         * yield here to give foo a chance to reach its wait().
         */
        yield();
        for (;;) {
                onboard_led(1);
                for (volatile int i = 1000000; i; --i)
                        /* NOTHING */;
                wakeup(foo);
                wait(main);
        }
}
