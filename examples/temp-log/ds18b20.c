#include <mchck.h>

#include "onewire.h"
#include "ds18b20.h"

/* XXX remove constants */
/* XXX read config and adjust read timeout */

static void
ds_data_received(struct ow_ctx *ow_ctx, void *data, size_t len, void *cbdata)
{
        struct ds18b20_ctx *ctx = cbdata;

        /* XXX crc */
        if (ctx->cb)
                ctx->cb(ctx, ctx->ds.temp, ctx->cbdata);
}

static void
get_result(void *cbdata)
{
        struct ds18b20_ctx *ctx = cbdata;

        ow_read(ctx->ow_ctx, 0xbe, &ctx->ds, sizeof(ctx->ds), ds_data_received, ctx);
}

void
ds_read(struct ds18b20_ctx *ctx, temp_cb_t cb, void *cbdata)
{
        ow_write(ctx->ow_ctx, 0x44, NULL, 0, NULL, NULL);
        ctx->cb = cb;
        ctx->cbdata = cbdata;
        timeout_add(&ctx->to, 750, get_result, ctx);
}

void
ds_init(struct ds18b20_ctx *ctx, struct ow_ctx *ow)
{
        ctx->ow_ctx = ow;
}
