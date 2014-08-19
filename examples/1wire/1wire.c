#include <mchck.h>

enum ow_bus_state {
        OW_BUS_EMPTY,
        OW_BUS_DEVICES,
};

struct ow_ctx;

typedef void (*ow_reset_cb_t)(struct ow_ctx *, enum ow_bus_state, void *);

struct ow_ctx {
        struct uart_ctx *uart;
        struct uart_trans_ctx tx;
        struct uart_trans_ctx rx;
        enum ow_bus_state bus_state;
        union {
                void *cb;
                ow_reset_cb_t reset_cb;
        };
        void *cbdata;
};


void
ow_init(struct ow_ctx *ctx, struct uart_ctx *uart)
{
        ctx->uart = uart;
        uart_init(ctx->uart);
}

static void
ow_reset_done(const void *buf, size_t len, void *cbdata)
{
        struct ow_ctx *ctx = cbdata;
        uint8_t result = *(const uint8_t *)buf;

        if (result != 0xf0)
                ctx->bus_state = OW_BUS_DEVICES;
        else
                ctx->bus_state = OW_BUS_EMPTY;

        if (ctx->cb)
                ctx->reset_cb(ctx, ctx->bus_state, ctx->cbdata);
}

void
ow_reset(struct ow_ctx *ctx, ow_reset_cb_t cb, void *cbdata)
{
        const static uint8_t reset = 0xf0;
        static uint8_t presence;

        ctx->cb = cb;
        ctx->cbdata = cbdata;

        uart_set_baudrate(ctx->uart, 9600);
        uart_read(ctx->uart, &ctx->rx, &presence, 1, ow_reset_done, ctx);
        uart_write(ctx->uart, &ctx->tx, &reset, 1, NULL, NULL);
}


struct ow_ctx ow_ctx;

void
ow_result(struct ow_ctx *ctx, enum ow_bus_state bus_state, void *cbdata)
{
        onboard_led(bus_state == OW_BUS_DEVICES ? ONBOARD_LED_ON : ONBOARD_LED_OFF);
}

void
probe_ow(void *cbdata)
{
        static struct timeout_ctx t;

        onboard_led(ONBOARD_LED_OFF);
        ow_reset(&ow_ctx, ow_result, ow_result);
        timeout_add(&t, 1000, probe_ow, NULL);
}

void
main(void)
{
        pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        pin_mode(PIN_PTC3, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        ow_init(&ow_ctx, &uart1);

        timeout_init();
        probe_ow(NULL);

        sys_yield_for_frogs();
}
