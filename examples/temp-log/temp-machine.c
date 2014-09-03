#include <mchck.h>

#include "onewire.h"
#include "ds18b20.h"

#include "temp-log.h"


enum templog_state {
        TEMPLOG_IDLE,
        TEMPLOG_READING,
        TEMPLOG_SLEEPING,
        TEMPLOG_STOPPING,
        TEMPLOG_STOP_FLUSHING,
        TEMPLOG_STOPPED
};


static enum templog_state templog_state = TEMPLOG_STOPPED;
static struct timeout_ctx templog_timeout;

static size_t flash_pagepos;
static int16_t last_temp;


static void got_temp_data(struct ds18b20_ctx *ctx, int16_t temp, void *cbdata);
static void templog_statemachine(void);


static int
page_space_available(void)
{
        if (flash_pagepos + sizeof(struct templog_entry) <= SPIFLASH_PAGE_SIZE)
                return (1);
        else
                return (0);
}

static void
flush_done(void *cbdata)
{
        flash_pagepos = 0;
}

static void
got_temp_data(struct ds18b20_ctx *ctx, int16_t temp, void *cbdata)
{
        struct templog_entry e;

        last_temp = temp;
        if (page_space_available()) {
                e.time = rtc_get_time();
                e.temp = temp;
                memcpy(&flash_page[flash_pagepos], &e, sizeof(e));
                flash_pagepos += sizeof(e);
        } else {
                flash_flush_data(flash_pagepos, flush_done, NULL);
        }

        /* printf("pagepos %d, pos %d, free %d, temp %d %d/16\r\n", */
        /*        flash_pagepos, */
        /*        flash_addr, */
        /*        flash_total_size - flash_addr, */
        /*        temp >> 4, temp & 0xf); */

        templog_statemachine();
}

static void
read_temp_again(void *cbdata)
{
        templog_statemachine();
}

static void
stop_flush_done(void *cbdata)
{
        templog_statemachine();
}

static void
templog_statemachine(void)
{
        switch (templog_state) {
        case TEMPLOG_IDLE:
        case TEMPLOG_SLEEPING:
                onboard_led(ONBOARD_LED_ON);
                timeout_add(&templog_timeout, templog_interval * 1000, read_temp_again, NULL);
                ds_read(&ds, got_temp_data, NULL);
                templog_state = TEMPLOG_READING;
                onboard_led(ONBOARD_LED_OFF);
                break;
        case TEMPLOG_READING:
                /* timeout was triggered before */
                templog_state = TEMPLOG_SLEEPING;
                break;
        case TEMPLOG_STOPPING:
                flash_flush_data(flash_pagepos, stop_flush_done, NULL);
                flash_pagepos = 0;
                templog_state = TEMPLOG_STOP_FLUSHING;
                break;
        case TEMPLOG_STOP_FLUSHING:
                templog_state = TEMPLOG_STOPPED;
                break;
        case TEMPLOG_STOPPED:
                break;
        }
}

void
templog_start(void)
{
        if (templog_state == TEMPLOG_STOPPED) {
                templog_state = TEMPLOG_IDLE;
                templog_statemachine();
        } else {
                templog_state = TEMPLOG_IDLE;
        }
}

void
templog_stop(void)
{
        templog_state = TEMPLOG_STOPPING;
}

bool
templog_stopped(void)
{
        return (templog_state == TEMPLOG_STOPPED);
}

size_t
templog_free(void)
{
        return ((flash_free() - flash_pagepos) / sizeof(struct templog_entry));
}

int16_t
templog_last_temp(void)
{
        return (last_temp);
}
