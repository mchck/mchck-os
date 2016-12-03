#include <mchck.h>

static LPUART_Type *
c2l(struct uart_ctx *ctx)
{
        return ((LPUART_Type *)ctx->uart);
}

static void
init(struct uart_ctx *ctx)
{
        switch((uintptr_t)ctx->uart) {
        case (uintptr_t)LPUART0_BASE:
                bf_set_reg(SIM->SOPT2, SIM_SOPT2_LPUART0SRC, 0b01);
                bf_set_reg(SIM->SCGC5, SIM_SCGC5_LPUART0, 1);
                int_enable(IRQ_LPUART0);
                break;
#ifdef LPUART1_BASE
        case (uintptr_t)LPUART1_BASE:
                bf_set_reg(SIM->SOPT2, SIM_SOPT2_LPUART1SRC, 0b01);
                bf_set_reg(SIM->SCGC5, SIM_SCGC5_LPUART1, 1);
                int_enable(IRQ_LPUART1);
                break;
#endif
        default:
                return;
        }

        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_RE, 1);
        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_TE, 1);
}

static void
set_baudrate(struct uart_ctx *ctx, unsigned int baudrate)
{
        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_RE, 0);
        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_TE, 0);

        uint32_t oversample = bf_get_reg(c2l(ctx)->BAUD, LPUART_BAUD_OSR);
        uint32_t factor = 48000000 / (oversample + 1) / baudrate;
        bf_set_reg(c2l(ctx)->BAUD, LPUART_BAUD_SBR, factor);

        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_RE, 1);
        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_TE, 1);
}

static void
start(struct uart_ctx *ctx)
{
        if (ctx->rx_queue)
                bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_RIE, 1);
        if (ctx->tx_queue)
                bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_TIE, 1);
}

static void
start_tx(struct uart_ctx *ctx)
{
        while (bf_get_reg(c2l(ctx)->STAT, LPUART_STAT_TDRE)) {
                struct uart_trans_ctx *tctx = ctx->tx_queue;
                if (!tctx)
                        return;
                c2l(ctx)->DATA = *tctx->pos;
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
        while (bf_get_reg(c2l(ctx)->STAT, LPUART_STAT_RDRF)) {
                struct uart_trans_ctx *rctx = ctx->rx_queue;
                if (!rctx)
                        return;
                uint32_t data = c2l(ctx)->DATA;
                if (bf_get_reg(data, LPUART_DATA_PARITYE) || bf_get_reg(data, LPUART_DATA_FRETSC)) {
                        /* drop data byte, had a bit error */
                        continue;
                }
                *rctx->pos = data & 0xff;
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
}

static void
irq_handler(struct uart_ctx *ctx)
{
        uint32_t stat = c2l(ctx)->STAT;

        if (bf_get_reg(stat, LPUART_STAT_TDRE)) {
                if (ctx->tx_queue != NULL)
                        start_tx(ctx);
                else
                        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_TIE, 0);
        }
        if (bf_get_reg(stat, LPUART_STAT_RDRF)) {
                if (ctx->rx_queue != NULL)
                        start_rx(ctx);
                else
                        bf_set_reg(c2l(ctx)->CTRL, LPUART_CTRL_RIE, 0);
        }
}

const struct uart_methods lpuart_fifo_methods = {
        .init = init,
        .set_baudrate = set_baudrate,
        .start = start,
        .irq_handler = irq_handler,
};


struct uart_ctx lpuart0 = {&lpuart_fifo_methods, LPUART0};

void
LPUART0_Handler(void)
{
        lpuart0.methods->irq_handler(&lpuart0);
}

#ifdef LPUART1_BASE
struct uart_ctx lpuart1 = {&lpuart_fifo_methods, LPUART1};

void
LPUART1_Handler(void)
{
	lpuart0.methods->irq_handler(&lpuart0);
}
#endif
