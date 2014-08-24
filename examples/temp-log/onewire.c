#include <mchck.h>

#include "onewire.h"


static void ow_statemachine(struct ow_ctx *ctx);

static uint8_t
ow_get_data(struct ow_ctx *ctx)
{
        uint8_t val = 0;

        for (size_t i = 0; i < 8; ++i)
                val |= (ctx->read_data[i] == 0xff) << i;
        return (val);
}

static void
ow_set_data(struct ow_ctx *ctx, uint8_t val)
{
        for (int i = 0; i < 8; ++i)
                ctx->write_data[i] = (val & (1 << i)) == 0 ? 0 : 0xff;
}


static void
ow_next_state(const void *buf, size_t len, void *cbdata)
{
        ow_statemachine(cbdata);
}

static void
ow_queue_transact(struct ow_ctx *ctx, size_t len)
{
        uart_read(ctx->uart, &ctx->rx, ctx->read_data, len, ow_next_state, ctx);
        uart_write(ctx->uart, &ctx->tx, ctx->write_data, len, NULL, NULL);
}

static void
ow_statemachine(struct ow_ctx *ctx)
{
        size_t len = 0;

        switch (ctx->state) {
        case OW_RESET:
                uart_set_baudrate(ctx->uart, 115200);
                if (ctx->read_data[0] != 0xf0)
                        ctx->bus_state = OW_BUS_DEVICES;
                else
                        ctx->bus_state = OW_BUS_EMPTY;
                /* XXX check bus state? */
                ow_set_data(ctx, 0xcc); /* XXX select device */
                len = 8;
                ctx->state = OW_ROM;
                break;
        case OW_ROM:
                ow_set_data(ctx, ctx->funccmd);
                len = 8;
                ctx->state = OW_FUNC;
                break;
        case OW_FUNC:
                if (ctx->buf_len == 0)
                        goto transact_done;
                else
                        goto send_byte;
                /* NOTREACHED */
        case OW_PAYLOAD:
                if (ctx->dir == OW_RX)
                        ctx->buf[ctx->buf_pos++] = ow_get_data(ctx);
                if (ctx->buf_pos == ctx->buf_len) {
transact_done:
                        ctx->state = OW_IDLE;
                        if (ctx->data_cb)
                                ctx->data_cb(ctx, ctx->buf, ctx->buf_len, ctx->cbdata);
                        return;
                }
send_byte:
                switch (ctx->dir) {
                case OW_TX:
                        ow_set_data(ctx, ctx->buf[ctx->buf_pos++]);
                        break;
                case OW_RX:
                        ow_set_data(ctx, 0xff);
                        break;
                }
                ctx->state = OW_PAYLOAD;
                len = 8;
                break;

        default:
                /* XXX wat */
                break;
        }

        if (len > 0)
                ow_queue_transact(ctx, len);
}


void
ow_init(struct ow_ctx *ctx, struct uart_ctx *uart)
{
        ctx->uart = uart;
        uart_init(ctx->uart);
}

static int
ow_transact(struct ow_ctx *ctx, uint8_t funccmd, void *buf, size_t len, ow_data_cb_t cb, void *cbdata, enum ow_dir dir)
{
        if (ctx->state != OW_IDLE)
                return (-1);

        ctx->dir = dir;
        ctx->funccmd = funccmd;
        ctx->buf = buf;
        ctx->buf_pos = 0;
        ctx->buf_len = len;
        ctx->data_cb = cb;
        ctx->cbdata = cbdata;

        ctx->write_data[0] = 0xf0;
        uart_set_baudrate(ctx->uart, 9600);
        ctx->state = OW_RESET;
        ow_queue_transact(ctx, 1);
        return (0);
}

int
ow_read(struct ow_ctx *ctx, uint8_t funccmd, void *buf, size_t len, ow_data_cb_t cb, void *cbdata)
{
        return (ow_transact(ctx, funccmd, buf, len, cb, cbdata, OW_RX));
}

int
ow_write(struct ow_ctx *ctx, uint8_t funccmd, const void *buf, size_t len, ow_data_cb_t cb, void *cbdata)
{
        return (ow_transact(ctx, funccmd, (void *)buf, len, cb, cbdata, OW_TX));
}
