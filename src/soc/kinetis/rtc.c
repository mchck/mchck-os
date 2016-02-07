#include <mchck.h>

static struct rtc_alarm_ctx *alarm_head = NULL;

void
rtc_init(void)
{
        bf_set_reg(SIM->SCGC6, SIM_SCGC6_RTC, 1);
        bf_set_reg(RTC->CR, RTC_CR_OSCE, 1);
}

int
rtc_start_counter(void)
{
        if (bf_get_reg(RTC->SR, RTC_SR_TIF))
                return 1;
        bf_set_reg(RTC->SR, RTC_SR_TCE, 1);
        return 0;
}

uint32_t
rtc_get_time(void)
{
        return (RTC->TSR);
}

void
rtc_set_time(uint32_t seconds)
{
        int started = bf_get_reg(RTC->SR, RTC_SR_TCE);

        bf_set_reg(RTC->SR, RTC_SR_TCE, 0);
        RTC->TSR = seconds;
        if (started)
                bf_set_reg(RTC->SR, RTC_SR_TCE, 1);
}

static void
rtc_alarm_update(void)
{
        if (rtc_get_time() == RTC_INVALID_TIME)
                return;

        if (alarm_head) {
                RTC->TAR = alarm_head->time;
                int_enable(IRQ_RTC);
        } else {
                int_disable(IRQ_RTC);
        }
}

void
rtc_alarm_add(struct rtc_alarm_ctx *ctx, uint32_t time,
              rtc_alarm_cb *cb, void *cbdata)
{
        ctx->time = time;
        ctx->cb = cb;
        ctx->cbdata = cbdata;

        crit_enter();
        if (alarm_head) {
                struct rtc_alarm_ctx **last_next = &alarm_head;
                struct rtc_alarm_ctx *tail = alarm_head;
                while (tail && time > tail->time)
                        tail = tail->next;
                ctx->next = tail;
                *last_next = ctx;
        } else {
                ctx->next = NULL;
                alarm_head = ctx;
        }
        rtc_alarm_update();
        crit_exit();
}

void
rtc_alarm_cancel(struct rtc_alarm_ctx *ctx)
{
        crit_enter();
        struct rtc_alarm_ctx **next = &alarm_head;
        while (*next) {
                if (*next == ctx) {
                        *next = ctx->next;
                        break;
                }
                next = &(*next)->next;
        }
        crit_exit();
}

void
RTC_alarm_Handler(void)
{
        uint32_t time = rtc_get_time();
        while (alarm_head) {
                if (alarm_head->time <= time) {
                        struct rtc_alarm_ctx *ctx = alarm_head;
                        alarm_head = alarm_head->next;
                        rtc_alarm_update();
                        ctx->cb(ctx->cbdata);
                } else {
                        break;
                }
        }
}
