#include <mchck.h>

#include "temp-log.desc.h"

#include "xmodem.h"
#include "temp-log.h"


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


#define templog_min_interval 2
unsigned long templog_interval = templog_min_interval;

static struct cdc_ctx cdc;

static struct send_ctx cdc_send_ctx;
static struct xmodem_ctx xmodem;

static char input_line[100];


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

static void
cdc_sent(size_t remaining)
{
        fflush(stdout);
        if (cdc_send_ctx.state == SEND_RUNNING)
                send_cdc1(&cdc_send_ctx);
}

/* static size_t */
/* send_flash_csv(enum xmodem_result res, uint8_t *buf, size_t buflen, void *cbdata) */
/* { */
/*         size_t offs = (size_t)cbdata; */

/*         if (res != XMODEM_IN_PROGRESS) */
/*                 return (0); */

/*         struct templog_entry *e; */
/*         for (; offs < flash_addr; offs += sizeof(*s)) { */

/*         } */
/* } */

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
        } else if (strcmp(line, "?") == 0 ||
                   strcmp(line, "help") == 0) {
                printf("available commands: start, stop, interval, time, eraseall\r\n");
        } else if (strcmp(line, "stop") == 0) {
                templog_stop();
        } else if (strcmp(line, "start") == 0) {
                templog_start();
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
        } else if (strcmp(line, "eraseall") == 0) {
                if (!templog_stopped()) {
                        printf("please stop recording first.\r\n");
                        return;
                }
                flash_eraseall();
        } else if (strcmp(line, "download") == 0) {
                if (!templog_stopped()) {
                        printf("please stop recording first.\r\n");
                        return;
                }
                /* xmodem_start_send(&xmodem, &cdc_xfer, &cdc_send_ctx, send_flash_csv, (void *)0); */
        } else {
                printf("invalid command `%s'\r\n", line);
        }
}


static void
print_prompt(void)
{
        const char *statestr = "meh";

        if (flash_initializing_p())
                statestr = "initializing";
        else if (flash_full_p())
                statestr = "full";
        else if (templog_stopped())
                statestr = "stopped";
        else
                statestr = "recording";

        printf("%lu %s, %d°C, %d free, %lus > ",
               rtc_get_time(),
               statestr,
               templog_last_temp() >> 4,
               templog_free(),
               templog_interval);

        fflush(stdout);
}

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
        if (enable) {
                cdc_init(cdc_data, cdc_sent, &cdc);
                cdc_set_stdout(&cdc);
        }
}
