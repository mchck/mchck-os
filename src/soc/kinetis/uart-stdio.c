#include <mchck.h>

static uint8_t wr_buf[32];
static struct uart_trans_ctx wr_ctx;

static size_t
uart_stdio_write(const uint8_t *buf, size_t len, void *data)
{
        if (wr_ctx.remaining)
                return (0);
        if (len > sizeof(wr_buf))
                len = sizeof(wr_buf);
        memcpy(wr_buf, buf, len);
        if (uart_write(data, &wr_ctx, wr_buf, len, NULL, NULL) < 0)
                return (0);
        else
                return (len);
}

static const struct _stdio_file_ops uart_ops = {
        .write = uart_stdio_write,
};

void
uart_set_stdout(struct uart_ctx *ctx)
{
        stdout->ops_data = ctx;
        stdout->ops = &uart_ops;
}
