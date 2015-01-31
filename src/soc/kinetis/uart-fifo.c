#include <mchck.h>

/*
 * Kinetis UART with FIFO and bells and whistles.
 */

#define MIN(a,b) (((a) < (b)) ? (a) : (b))


static void
init(struct uart_ctx *ctx)
{
        UART_MemMapPtr uart = ctx->uart;

        switch ((uintptr_t)uart) {
        case (uintptr_t)UART0_BASE_PTR:
                bf_set(SIM_SCGC4, SIM_SCGC4_UART0, 1);
                int_enable(IRQ_UART0_status);
                break;
        case (uintptr_t)UART1_BASE_PTR:
                bf_set(SIM_SCGC4, SIM_SCGC4_UART1, 1);
                int_enable(IRQ_UART1_status);
                break;
        case (uintptr_t)UART2_BASE_PTR:
                bf_set(SIM_SCGC4, SIM_SCGC4_UART2, 1);
                int_enable(IRQ_UART2_status);
                break;
        }

        // Enable FIFOs
        bf_set(UART_PFIFO_REG(uart), UART_PFIFO_RXFE, 1);
        bf_set(UART_PFIFO_REG(uart), UART_PFIFO_TXFE, 1);

        // XXX arbitrary FIFO watermarks
        bf_set(UART_TWFIFO_REG(uart), UART_TWFIFO_TXWATER, 8); /* XXX other fifos have only 1 */
        bf_set(UART_RWFIFO_REG(uart), UART_RWFIFO_RXWATER, 1); // FIXME: See comment at end of uart_start_rx

        bf_set(UART_C2_REG(uart), UART_C2_RE, 1);
        bf_set(UART_C2_REG(uart), UART_C2_TE, 1);
}

static void
set_baudrate(struct uart_ctx *ctx, unsigned int baudrate)
{
        UART_MemMapPtr uart = ctx->uart;
        unsigned int clockrate = 48000000; /* XXX use real clock rate */
        unsigned int sbr = clockrate / 16 / baudrate;
        unsigned int brfa = (2 * clockrate / baudrate) % 32;

        bf_set(UART_BDH_REG(uart), UART_BDH_SBR, sbr >> 8);
        bf_set(UART_BDL_REG(uart), UART_BDL_SBR, sbr & 0xff);
        bf_set(UART_C4_REG(uart), UART_C4_BRFA, brfa);
}

static void
start_tx(struct uart_ctx *ctx)
{
        UART_MemMapPtr uart = ctx->uart;

        unsigned int depth = 1 << (bf_get(UART_PFIFO_REG(uart), UART_PFIFO_TXFIFOSIZE) + 1);
        if (depth == 2)
                depth = 1;

        while (bf_get(UART_TCFIFO_REG(uart), UART_TCFIFO_TXCOUNT) < depth) {
                struct uart_trans_ctx *tctx = ctx->tx_queue;
                if (!tctx)
                        return;
                UART_D_REG(uart) = *tctx->pos;
                tctx->pos++;
                tctx->remaining--;
                if (tctx->remaining == 0) {
                        ctx->tx_queue = tctx->next;
                        if (tctx->cb)
                                tctx->cb(tctx->buf, tctx->pos - tctx->buf, tctx->cbdata);
                }
        }
}

static void
start_rx(struct uart_ctx *ctx)
{
        UART_MemMapPtr uart = ctx->uart;
        int remaining;

        while ((remaining = bf_get(UART_RCFIFO_REG(uart), UART_RCFIFO_RXCOUNT)) != 0) {
                struct uart_trans_ctx *rctx = ctx->rx_queue;
                if (!rctx)
                        return;
                /* clear flags */
                if (remaining == 1)
                        (void)UART_S1_REG(uart);
                *rctx->pos = UART_D_REG(uart);
                rctx->pos++;
                rctx->remaining--;

                bool stop = false;
                stop |= rctx->flags.stop_on_terminator
                        && *(rctx->pos - 1) == rctx->terminator;
                stop |= rctx->remaining == 0;
                if (stop) {
                        ctx->rx_queue = rctx->next;
                        rctx->remaining = 0;
                        if (rctx->cb)
                                rctx->cb(rctx->buf, rctx->pos - rctx->buf, rctx->cbdata);
                }
        }

        /*
         * ensure watermark is low enough.
         * otherwise we may not be notified when the receive completes.
         */
        #if 0
        /* FIXME: This seems to lock up the peripheral */
        unsigned int depth = 1 << (uart->uart->pfifo.rxfifosize + 1);
        uart->uart->c2.re = 0;
        uart->uart->rwfifo = MIN(uart->uart->pfifo.rxfifosize - 1,
                                 uart->rx_queue->remaining);
        uart->uart->c2.re = 1;
        #endif
}

static void
start(struct uart_ctx *ctx)
{
        UART_MemMapPtr uart = ctx->uart;

        if (ctx->rx_queue)
                bf_set(UART_C2_REG(uart), UART_C2_RIE, 1);
        if (ctx->tx_queue)
                bf_set(UART_C2_REG(uart), UART_C2_TIE, 1);
}

static void
irq_handler(struct uart_ctx *ctx)
{
        UART_MemMapPtr uart = ctx->uart;

        uint8_t s1 = UART_S1_REG(uart);

        if (bf_get(s1, UART_S1_TDRE)) {
                if (ctx->tx_queue != NULL)
                        start_tx(ctx);
                else
                        bf_set(UART_C2_REG(uart), UART_C2_TIE, 0);
        }
        if (bf_get(UART_RCFIFO_REG(uart), UART_RCFIFO_RXCOUNT) > 0) {
                if (ctx->rx_queue != NULL)
                        start_rx(ctx);
                else
                        bf_set(UART_C2_REG(uart), UART_C2_RIE, 0);
        }
        if (bf_get(s1, UART_S1_FE)) {
                /* simply clear flag and hope for the best */
                (void)UART_D_REG(uart);
        }

        /* final clear of watermark flags */
        (void)UART_S1_REG(uart);
}

const struct uart_methods uart_fifo_methods = {
        .init = init,
        .set_baudrate = set_baudrate,
        .start = start,
        .irq_handler = irq_handler,
};
