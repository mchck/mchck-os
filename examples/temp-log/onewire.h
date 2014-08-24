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


void ow_init(struct ow_ctx *ctx, struct uart_ctx *uart);
int ow_read(struct ow_ctx *ctx, uint8_t funccmd, void *buf, size_t len, ow_data_cb_t cb, void *cbdata);
int ow_write(struct ow_ctx *ctx, uint8_t funccmd, const void *buf, size_t len, ow_data_cb_t cb, void *cbdata);
