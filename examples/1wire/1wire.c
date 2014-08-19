#include <mchck.h>

enum ow_bus_state {
        OW_BUS_EMPTY,
        OW_BUS_DEVICES,
};

enum ow_dir {
        OW_RX,
        OW_TX
};

enum ow_state {
        OW_IDLE,
        OW_RESET,
        OW_ROM,
        OW_FUNC,
        OW_PAYLOAD,
};

struct ow_ctx;

typedef void (*ow_data_cb_t)(struct ow_ctx *, void *, size_t, void *);

struct ow_ctx {
        struct uart_ctx *uart;
        struct uart_trans_ctx tx;
        struct uart_trans_ctx rx;

        enum ow_bus_state bus_state : 1;
        enum ow_dir dir : 1;
        enum ow_state state;
        uint8_t romcmd;
        uint8_t funccmd;

        ow_data_cb_t data_cb;
        void *cbdata;
        uint8_t *buf;
        size_t buf_len;
        size_t buf_pos;
        uint8_t write_data[8];
        uint8_t read_data[8];
};


void ow_statemachine(struct ow_ctx *ctx);


static uint8_t
ow_get_data(struct ow_ctx *ctx)
{
        uint8_t val = 0;

        for (size_t i = 0; i < 8; ++i)
                val = (val << 1) | (ctx->read_data[i] < 0xff);
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

void
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

                if (ctx->dir == OW_TX)
                        ow_set_data(ctx, ctx->buf[ctx->buf_pos++]);
                else
                        ow_set_data(ctx, 0xff);
                len = 8;
                ctx->state = OW_PAYLOAD;
                break;
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
                switch (ctx->dir) {
                case OW_TX:
                        ow_set_data(ctx, ctx->buf[ctx->buf_pos++]);
                        break;
                case OW_RX:
                        ow_set_data(ctx, 0xff);
                        break;
                }
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


struct ds18b20 {
        uint16_t temp;
        uint8_t th;
        uint8_t tl;
        struct ds18b20_config {
                UNION_STRUCT_START(8);
                uint8_t _reserved1 : 5;
                enum ds18b20_config_res {
                        DS18B20_9BIT = 0b00,
                        DS18B20_10BIT = 0b01,
                        DS18B20_11BIT = 0b10,
                        DS18B20_12BIT = 0b11,
                } res : 2;
                uint8_t _reserved2 : 1;
                UNION_STRUCT_END;
        } config;
        uint8_t _reserved1;
        uint8_t _reserved2;
        uint8_t _reserved3;
        uint8_t crc;
};

static struct ow_ctx ow_ctx;
static struct ds18b20 ds;


static void probe_ow(void *cbdata);

static void
ds_data_received(struct ow_ctx *ctx, void *data, size_t len, void *cbdata)
{
        static struct timeout_ctx t;

        onboard_led(ONBOARD_LED_ON);
        timeout_add(&t, 1000, probe_ow, cbdata);
}

static void
get_result(void *cbdata)
{
        ow_read(cbdata, 0xeb, &ds, sizeof(ds), ds_data_received, cbdata);
}

static void
probe_ow(void *cbdata)
{
        static struct timeout_ctx t;

        onboard_led(ONBOARD_LED_OFF);
        ow_write(cbdata, 0x44, NULL, 0, NULL, NULL);
        timeout_add(&t, 100, get_result, cbdata);
}

void
main(void)
{
        pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        pin_mode(PIN_PTC3, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        ow_init(&ow_ctx, &uart1);

        timeout_init();
        probe_ow(&ow_ctx);

        sys_yield_for_frogs();
}
