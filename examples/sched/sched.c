#include <mchck.h>

uint8_t stack[512] __attribute__((aligned(8)));

void main(void);

void
foo(void *arg)
{
        for (;;) {
                //wait(foo);
                for (volatile int i = 1000000; i; --i)
                        onboard_led(0);
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
                for (volatile int i = 1000000; i; --i)
                        onboard_led(1);
                wakeup(foo);
                //wait(main);
        }
}
