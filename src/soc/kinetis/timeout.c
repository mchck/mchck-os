#include <mchck.h>

static struct timeout_ctx *timeout_queue;

static union timeout_time timeout_lazy_now;
static struct timeout_ctx overflow;

static unsigned int timeout_ref_count = 0;

/* call with crit_active() */
static void
timeout_update_time(void)
{
        /* tell ctr to latch */
        LPTMR0_CNR = 0;
        uint16_t now = LPTMR0_CNR;
        if (timeout_lazy_now.count > now)
                timeout_lazy_now.epoch++;
        timeout_lazy_now.count = now;
}

union timeout_time
timeout_get_time()
{
        crit_enter();
        timeout_update_time();
        crit_exit();
        return timeout_lazy_now;
}

static void
timeout_empty(void *data)
{
}

/* call with crit_active() */
static void
timeout_schedule_wrap(void)
{
        /* we get triggered at the end of the period, so -1 */
        overflow.time.time = timeout_lazy_now.time - 1;
        overflow.time.epoch += 1;
        overflow.cb = timeout_empty;
        overflow.next = timeout_queue;
        timeout_queue = &overflow;
}

/* call with crit_active() */
static void
timeout_reschedule(void)
{
        if (timeout_queue == NULL) {
                if (timeout_ref_count > 0) {
                        /* the queue is empty but we still need to
                           keep the timebase running */
                        timeout_schedule_wrap();
                } else {
                        /* we can stop the timebase */
                        LPTMR0_CSR &= ~(LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK);
                }
                return;
        }

        /* will we have to wrap the epoch before the next timeout? */
        if (timeout_queue->time.count > timeout_lazy_now.count &&
            timeout_queue->time.epoch > timeout_lazy_now.epoch) {
                timeout_schedule_wrap();
        }
        LPTMR0_CMR = timeout_queue->time.count;
        LPTMR0_CSR |= LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TCF_MASK;
}

void
timeout_get_ref()
{
        crit_enter();
        timeout_ref_count++;
        timeout_reschedule();
        crit_exit();
}

void
timeout_put_ref()
{
        crit_enter();
        if (timeout_ref_count == 0)
                panic("timeout_put_ref");
        timeout_ref_count--;
        timeout_reschedule();
        crit_exit();
}

void
timeout_init(void)
{
        bf_set_reg(SIM_SCGC5, SIM_SCGC5_LPTMR, 1);
        LPTMR0_PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(LPTMR_PCS_LPO);

        LPTMR0_CSR = LPTMR_CSR_TCF_MASK | LPTMR_CSR_TFC_MASK;
        int_enable(IRQ_LPTMR0);
}

void
timeout_add(struct timeout_ctx *t, uint32_t ms, timeout_cb_t *cb, void *cbdata)
{
        crit_enter();
        timeout_update_time();

        *t = (struct timeout_ctx){
                .time.time = ms + timeout_lazy_now.time + 1,
                .cb = cb,
                .cbdata = cbdata,
        };

        /* XXX what if this traversal takes >= one timer tick? */
        struct timeout_ctx **p;
        for (p = &timeout_queue; *p != NULL; p = &(*p)->next) {
                if ((*p)->time.time > t->time.time)
                        break;
        }
        t->next = *p;
        *p = t;

        if (timeout_queue == t)
                timeout_reschedule();
        crit_exit();
}

int
timeout_cancel(struct timeout_ctx *t)
{
        crit_enter();
        struct timeout_ctx **p;
        for (p = &timeout_queue; *p != NULL; p = &(*p)->next) {
                if (*p == t)
                        break;
                if ((*p)->time.time > t->time.time)
                        return (-1);
        }
        *p = t->next;
        if (*p == timeout_queue)
                timeout_reschedule();
        crit_exit();
        return (0);
}

void
LPTMR0_Handler(void)
{
        crit_enter();
        timeout_update_time();
        if (timeout_queue == NULL) {
                // there are no tasks to run, schedule the next overflow
                timeout_reschedule();
                crit_exit();
                return;
        }

        // for each task whose time has past...
        while (timeout_queue->time.time <= timeout_lazy_now.time) {
                struct timeout_ctx *t = timeout_queue;
                timeout_queue = t->next;
                t->cb(t->cbdata);

                timeout_update_time();
                if (!timeout_queue)
                        break;
        }
        timeout_reschedule();
        crit_exit();
}
