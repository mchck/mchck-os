#include <mchck.h>

#include "xmodem.h"


static void xmodem_send_block(struct xmodem_ctx *ctx);


static void
xmodem_next_block(struct xmodem_ctx *ctx)
{
        size_t blksize = ctx->cb(XMODEM_IN_PROGRESS, ctx->data.block, sizeof(ctx->data.block), ctx->cbdata);
        if (blksize < sizeof(ctx->data.block)) {
                memset(&ctx->data.block[blksize], XMODEM_EOF, sizeof(ctx->data.block) - blksize);
                ctx->state = XMODEM_FINISHING;
        }
}

static void
xmodem_received_reply(void *cbdata)
{
        struct xmodem_ctx *ctx = cbdata;

        if (ctx->state == XMODEM_STOP)
                return;

        switch (ctx->data.op) {
        case XMODEM_ACK:
                /* move on */
                timeout_cancel(&ctx->timeout);
                if (ctx->state == XMODEM_SENDING) {
                        xmodem_next_block(ctx);
                } else {
                        ctx->state = XMODEM_STOP;
                        ctx->cb(XMODEM_DONE, NULL, 0, cbdata);
                }
                break;
        case XMODEM_NAK:
                /* resend */
                timeout_cancel(&ctx->timeout);
                xmodem_send_block(ctx);
                break;
        case XMODEM_CAN:
                timeout_cancel(&ctx->timeout);
                ctx->state = XMODEM_STOP;
                ctx->cb(XMODEM_CANCELED, NULL, 0, cbdata);
                break;
        default:
                ctx->xfer_funcs->recv(ctx->xfer_ctx, &ctx->data, 1, xmodem_received_reply, ctx);
                break;
        }
}

static void
xmodem_timeout(void *cbdata)
{
        struct xmodem_ctx *ctx = cbdata;

        ctx->state = XMODEM_STOP;
        /* XXX abort recv */
        ctx->cb(XMODEM_TIMEOUT, NULL, 0, cbdata);
}

static void
xmodem_send_done(void *cbdata)
{
        struct xmodem_ctx *ctx = cbdata;

        /* XXX flush data */
        timeout_add(&ctx->timeout, 3000, xmodem_timeout, ctx);
        ctx->xfer_funcs->recv(ctx->xfer_ctx, &ctx->data, 1, xmodem_received_reply, ctx);
}

static void
xmodem_send_block(struct xmodem_ctx *ctx)
{
        ctx->data.op = XMODEM_SOH;
        ctx->data.blocknum = ctx->curblock;
        ctx->data.blocknum_rev = ~ctx->curblock;
        ctx->data.csum8 = 0;
        for (size_t i = 0; i < sizeof(ctx->data.block); ++i)
                ctx->data.csum8 += ctx->data.block[i];
        ctx->xfer_funcs->send(ctx->xfer_ctx,
                              &ctx->data, sizeof(ctx->data),
                              xmodem_send_done, ctx);
}

void
xmodem_start_send(struct xmodem_ctx *ctx, const struct xmodem_xfer_funcs *xfer_funcs, void *xfer_ctx, xmodem_send_cb_t cb, void *cbdata)
{
        ctx->cb = cb;
        ctx->cbdata = cbdata;
        ctx->xfer_funcs = xfer_funcs;
        ctx->xfer_ctx = xfer_ctx;
        ctx->curblock = 1;
        ctx->state = XMODEM_SENDING;
}
