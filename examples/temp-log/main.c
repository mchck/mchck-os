#include <mchck.h>

#include "onewire.h"
#include "ds18b20.h"
#include "xmodem.h"


#include "temp-log.desc.h"


enum {
        SPIFLASH_PAGE_SIZE = 256,
        SPIFLASH_SECTOR_SIZE = 4096,
};

enum flash_state {
        FLASH_INIT,
        FLASH_FIND_EMPTY_SECTOR,
        FLASH_READY,
        FLASH_PAGE_FILLED,
        FLASH_WRITING,
        FLASH_ERASING,
        FLASH_FULL,
};

enum templog_state {
        TEMPLOG_IDLE,
        TEMPLOG_READING,
        TEMPLOG_SLEEPING,
        TEMPLOG_STOPPING,
        TEMPLOG_STOPPED
};

struct templog_entry {
        uint32_t time;
        uint32_t temp;
};


static void got_temp_data(struct ds18b20_ctx *ctx, int16_t temp, void *cbdata);
static void flash_flush_data(void);
static void flash_state_machine(void);
static void templog_statemachine(void);


static struct cdc_ctx cdc;
static struct ow_ctx ow_ctx;
static struct ds18b20_ctx ds;

static struct spiflash_transaction flash_trans;
static uint8_t flash_page[SPIFLASH_PAGE_SIZE];
static size_t flash_total_size = (1<<20); /* XXX read dynamically */

static enum flash_state flash_state = FLASH_INIT;
static size_t flash_addr;
static size_t flash_pagepos;

static enum templog_state templog_state = TEMPLOG_STOPPED;
static struct timeout_ctx templog_timeout;
static int16_t last_temp;

#define templog_min_interval 2
static unsigned long templog_interval = templog_min_interval;


static int
page_space_available(void)
{
        if (flash_pagepos + sizeof(struct templog_entry) <= SPIFLASH_PAGE_SIZE)
                return (1);
        else
                return (0);
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
                flash_flush_data();
        }

        /* printf("pagepos %d, pos %d, free %d, temp %d %d/16\r\n", */
        /*        flash_pagepos, */
        /*        flash_addr, */
        /*        flash_total_size - flash_addr, */
        /*        temp >> 4, temp & 0xf); */

        onboard_led(ONBOARD_LED_ON);

        templog_statemachine();
}

static void
read_temp_again(void *cbdata)
{
        templog_statemachine();
}

static void
templog_statemachine(void)
{
        switch (templog_state) {
        case TEMPLOG_IDLE:
        case TEMPLOG_SLEEPING:
                timeout_add(&templog_timeout, templog_interval * 1000, read_temp_again, NULL);
                onboard_led(ONBOARD_LED_OFF);
                ds_read(&ds, got_temp_data, NULL);
                templog_state = TEMPLOG_READING;
                break;
        case TEMPLOG_READING:
                /* timeout was triggered before */
                templog_state = TEMPLOG_SLEEPING;
                break;
        case TEMPLOG_STOPPING:
        case TEMPLOG_STOPPED:
                templog_state = TEMPLOG_STOPPED;
                break;
        }
}


static int
flash_completely_full(void)
{
        if (flash_addr >= flash_total_size)
                return 1;
        else
                return 0;
}

static void
flush_flash_done(void *cbdata)
{
        flash_state_machine();
}

static void
flash_flush_data(void)
{
        if (flash_state != FLASH_READY)
                return;

        flash_state = FLASH_PAGE_FILLED;
        flash_state_machine();
}

static void
flash_find_empty_sector(void *cbdata)
{
        int empty = 1;
        for (size_t i = 0; i < sizeof(flash_page); ++i) {
                if (flash_page[i] != 0xff) {
                        empty = 0;
                        break;
                }
        }
        if (empty) {
                flash_state = FLASH_READY;
        } else {
                flash_addr += SPIFLASH_SECTOR_SIZE;
                if (flash_completely_full())
                        flash_state = FLASH_FULL;
        }
        flash_state_machine();
}

static void
flash_state_machine(void)
{
again:
        switch (flash_state) {
        case FLASH_INIT:
                /* XXX read size */
                flash_state = FLASH_FIND_EMPTY_SECTOR;
                goto again;
        case FLASH_FIND_EMPTY_SECTOR:
                spiflash_read_page(&onboard_flash, &flash_trans,
                                   flash_page, flash_addr, sizeof(flash_page),
                                   flash_find_empty_sector, NULL);
                break;
        case FLASH_READY:
                break;
        case FLASH_PAGE_FILLED:
                spiflash_program_page(&onboard_flash, &flash_trans,
                                      flash_addr, flash_page, flash_pagepos,
                                      flush_flash_done, NULL);
                flash_state = FLASH_WRITING;
                flash_pagepos = 0;
                break;
        case FLASH_WRITING:
                flash_addr += SPIFLASH_PAGE_SIZE;
                if (flash_completely_full()) {
                        flash_state = FLASH_FULL;
                } else {
                        if ((flash_addr & (SPIFLASH_SECTOR_SIZE - 1)) == 0) {
                                spiflash_erase_sector(&onboard_flash, &flash_trans,
                                                      flash_addr,
                                                      flush_flash_done, NULL);
                                flash_state = FLASH_ERASING;
                        } else {
                                flash_state = FLASH_READY;
                        }
                }
                break;
        case FLASH_ERASING:
                flash_state = FLASH_READY;
                break;
        case FLASH_FULL:
                break;
        }
}


struct send_ctx {
        enum {
                SEND_IDLE,
                SEND_RUNNING,
                SEND_DONE
        } state;
        void *buf;
        size_t len;
        size_t pos;
        xmodem_xfer_cb_t cb;
        void *cbdata;
};

static void
send_cdc1(struct send_ctx *ctx)
{
        size_t space = cdc_write_space(&cdc);
        if (ctx->len - ctx->pos < space)
                space = ctx->len - ctx->pos;
        cdc_write((void *)((uintptr_t)ctx->buf + ctx->pos), space, &cdc);
        ctx->pos += space;
        if (ctx->pos == ctx->len) {
                ctx->state = SEND_DONE;
                ctx->cb(ctx->cbdata);
        }
}

static void
send_xmodem_cdc(void *xfer_ctx, const void *buf, size_t len, xmodem_xfer_cb_t cb, void *cbdata)
{
        struct send_ctx *ctx = xfer_ctx;

        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->buf = (void *)buf;
        ctx->len = len;
        send_cdc1(ctx);
}

static void
recv_xmodem_cdc(void *xfer_ctx, void *buf, size_t len, xmodem_xfer_cb_t cb, void *cbdata)
{
        struct send_ctx *ctx = xfer_ctx;

        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->buf = buf;
        ctx->len = len;
}

static const struct xmodem_xfer_funcs cdc_xfer = {
        .send = send_xmodem_cdc,
	.recv = recv_xmodem_cdc,
};

static struct send_ctx cdc_send_ctx;

static void
cdc_sent(size_t remaining)
{
        fflush(stdout);
        if (cdc_send_ctx.state == SEND_RUNNING)
                send_cdc1(&cdc_send_ctx);
}

static int
read_num(const char *line, const char *cmd, unsigned long *out)
{
        const char *num = line + strlen(cmd);

        while (*num == ' ')
                ++num;

        if (*num == 0)
                return (0);

        char *ep = NULL;
        unsigned long i = strtoul(num, &ep, 0);

        while (*ep == ' ')
                ++ep;
        if (*ep != 0) {
                printf("invalid number `%s'\r\n", ep);
                return (0);
        }

        *out = i;

        return (1);
}


static void
handle_command(char *line)
{
        if (strcmp(line, "") == 0) {
                return;
        } else if (strcmp(line, "stop") == 0) {
                crit_enter();
                templog_state = TEMPLOG_STOPPING;
                crit_exit();
        } else if (strcmp(line, "start") == 0) {
                crit_enter();
                if (templog_state == TEMPLOG_STOPPED) {
                        templog_state = TEMPLOG_IDLE;
                        templog_statemachine();
                }
                crit_exit();
        } else if (strncmp(line, "interval", 8) == 0) {
                unsigned long n;

                if (!read_num(line, "interval", &n)) {
                        printf("syntax: interval  <seconds>\r\n");
                        return;
                }

                if (n < templog_min_interval) {
                        printf("interval too small: minimum is %d\r\n", templog_min_interval);
                        return;
                }
                templog_interval = n;
        } else if (strncmp(line, "time", 4) == 0) {
                unsigned long n;

                if (!read_num(line, "time", &n)) {
                        printf("syntax: time <seconds-since-epoch>\r\n");
                        return;
                }

                rtc_set_time(n);
                rtc_start_counter();
        } else {
                printf("invalid command `%s'\r\n", line);
        }
}


static void
print_prompt(void)
{
        const char *statestr;

        if (flash_state < FLASH_READY)
                statestr = "initializing";
        else if (flash_state == FLASH_FULL)
                statestr = "full";
        else if (templog_state >= TEMPLOG_STOPPING)
                statestr = "stopped";
        else
                statestr = "recording";

        printf("%lu %s, %d°C, %d free, %lus > ",
               rtc_get_time(),
               statestr,
               last_temp >> 4,
               (flash_total_size - flash_addr - flash_pagepos) / sizeof(struct templog_entry),
               templog_interval);

        fflush(stdout);
}

static char input_line[100];

static void
handle_input(uint8_t *buf, size_t len)
{
        size_t curlen = strlen(input_line);

#define CTRL(x) (x - 'a' + 1)
        for (; len > 0; ++buf, --len) {
                switch (*buf) {
                case CTRL('h'):
                case 0x7f:
                        if (curlen > 0) {
                                input_line[curlen - 1] = 0;
                                printf("\b \b");
                        }
                        break;
                case CTRL('u'):
                        input_line[0] = 0;
                        fputc('\r', stdout);
                        for (; curlen > 0; --curlen)
                                fputc(' ', stdout);
                        fputc('\r', stdout);
                        break;
                case '\r':
                case '\n':
                        printf("\r\n");
                        handle_command(input_line);
                        input_line[0] = 0;
                        print_prompt();
                        break;
                case ' ':
                case 'A'...'Z':
                case 'a'...'z':
                case '0'...'9':
                        if (curlen + 2 < sizeof(input_line)) {
                                input_line[curlen++] = *buf;
                                fputc(*buf, stdout);
                        }
                        input_line[curlen] = 0;
                        break;
                }
        }
        fflush(stdout);
}


static void
cdc_data(uint8_t *buf, size_t len)
{
        handle_input(buf, len);
        cdc_read_more(&cdc);
}


void
init_usb(int enable)
{
        onboard_led(ONBOARD_LED_ON);
        if (enable) {
                cdc_init(cdc_data, cdc_sent, &cdc);
                cdc_set_stdout(&cdc);
        }
}

void
main(void)
{
        timeout_init();
        rtc_init();

        pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        pin_mode(PIN_PTC3, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        ow_init(&ow_ctx, &uart1);
        ds_init(&ds, &ow_ctx);

        spiflash_pins_init();
        spi_init();

        flash_state_machine();

        usb_init(&cdc_device);

        sys_yield_for_frogs();
}
