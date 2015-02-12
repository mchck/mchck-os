#include <mchck.h>

void
uart_init(struct uart_ctx *ctx)
{
        ctx->methods->init(ctx);
}

void
uart_set_baudrate(struct uart_ctx *ctx, unsigned int baudrate)
{
        ctx->methods->set_baudrate(ctx, baudrate);
}

static void
uart_queue_transfer(struct uart_ctx *uart, struct uart_trans_ctx *ctx)
{
        crit_enter();
        ctx->next = NULL;
        for (struct uart_trans_ctx **c = ctx->queue; ; c = &(*c)->next) {
                if (*c == NULL) {
                        *c = ctx;
                        /* we're at the head, so start transfer */
                        if (c == ctx->queue)
                                uart->methods->start(uart);
                        break;
                }
        }
        crit_exit();
}

int
uart_write(struct uart_ctx *uart, struct uart_trans_ctx *ctx,
           const void *buf, size_t len,
           uart_cb cb, void *cbdata)
{
        if (ctx->remaining)
                return -1;
        ctx->pos = ctx->buf = (void *)buf;
        ctx->remaining = len;
        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->queue = &uart->tx_queue;
        uart_queue_transfer(uart, ctx);
        return 0;
}

void
uart_read(struct uart_ctx *uart, struct uart_trans_ctx *ctx,
          void *buf, size_t len,
          uart_cb cb, void *cbdata)
{
        if (ctx->remaining)
                return;
        ctx->flags.stop_on_terminator = false;
        ctx->pos = ctx->buf = buf;
        ctx->remaining = len;
        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->queue = &uart->rx_queue;
        uart_queue_transfer(uart, ctx);
}

void
uart_read_until(struct uart_ctx *uart, struct uart_trans_ctx *ctx,
                void *buf, size_t len, char until,
                uart_cb cb, void *cbdata)
{
        if (ctx->remaining)
                return;
        ctx->flags.stop_on_terminator = true;
        ctx->terminator = until;
        ctx->pos = ctx->buf = buf;
        ctx->remaining = len;
        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->queue = &uart->rx_queue;
        uart_queue_transfer(uart, ctx);
}

int
uart_abort(struct uart_ctx *uart, struct uart_trans_ctx *ctx)
{
        int success = 0;

        crit_enter();
        for (struct uart_trans_ctx **c = ctx->queue; *c != NULL; c = &(*c)->next) {
                if (*c == ctx) {
                        *c = ctx->next;
                        success = 1;
                        ctx->remaining = 0;
                        if (ctx->pos != ctx->buf && ctx->cb)
                                ctx->cb(ctx->buf, ctx->pos - ctx->buf, ctx->cbdata);
                        break;
                }
        }
        crit_exit();

        return (success);
}
