#include <mchck.h>

bool
sema_try_wait_count(struct sema *sema, int count)
{
        uint32_t oldcount = sema->count;
        if (oldcount < count)
                return (false);
        return (__sync_bool_compare_and_swap(&sema->count, oldcount, oldcount - count));
}

void
sema_wait_count(struct sema *sema, int count)
{
        for (;;) {
                uint32_t oldcount = sema->count;
                if (oldcount >= count) {
                        if (__sync_bool_compare_and_swap(&sema->count, oldcount, oldcount - count))
                                return;
                } else {
                        wait_cond(sema, &sema->count, oldcount);
                }
        }
}

void
sema_wait(struct sema *sema)
{
        sema_wait_count(sema, 1);
}

void
sema_wake_count(struct sema *sema, int count)
{
        __sync_add_and_fetch(&sema->count, count);
        wakeup(sema);
}

void
sema_wake(struct sema *sema)
{
        sema_wake_count(sema, 1);
}
