enum xmodem_op {
        XMODEM_SOH = 0x01,
        XMODEM_EOT = 0x04,
        XMODEM_ACK = 0x06,
        XMODEM_NAK = 0x15,
        XMODEM_CAN = 0x18,
        XMODEM_EOF = 0x1a,
};

enum xmodem_result {
        XMODEM_IN_PROGRESS,
        XMODEM_CANCELED,
        XMODEM_TIMEOUT,
        XMODEM_DONE
};

enum xmodem_state {
        XMODEM_INIT,
        XMODEM_SENDING,
        XMODEM_FINISHING,
        XMODEM_STOP,
};

struct xmodem_block {
        enum xmodem_op op : 8;
        uint8_t blocknum;
        uint8_t blocknum_rev;
        uint8_t block[128];
        uint8_t csum8;
};

typedef size_t (*xmodem_send_cb_t)(enum xmodem_result, uint8_t *buf, size_t buflen, void *cbdata);
typedef void (*xmodem_xfer_cb_t)(void *cbdata);

struct xmodem_xfer_funcs {
        void (*send)(void *xfer_ctx, const void *buf, size_t len, xmodem_xfer_cb_t cb, void *cbdata);
        void (*recv)(void *xfer_ctx, void *buf, size_t len, xmodem_xfer_cb_t cb, void *cbdata);
};

struct xmodem_ctx {
        struct timeout_ctx timeout;
        enum xmodem_state state;
        const struct xmodem_xfer_funcs *xfer_funcs;
        void *xfer_ctx;
        xmodem_send_cb_t cb;
        void *cbdata;
        uint8_t curblock;
        struct xmodem_block data;
};

void xmodem_start_send(struct xmodem_ctx *ctx, const struct xmodem_xfer_funcs *xfer_funcs, void *xfer_ctx, xmodem_send_cb_t cb, void *cbdata);
