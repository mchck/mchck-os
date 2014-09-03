struct ds18b20_scratchpad {
        int16_t temp;
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
} __packed;

struct ds18b20_ctx;

typedef void (*temp_cb_t)(struct ds18b20_ctx *, int16_t, void *);

struct ds18b20_ctx {
        struct ow_ctx *ow_ctx;
        struct ds18b20_scratchpad ds;
        struct timeout_ctx to;
        temp_cb_t cb;
        void *cbdata;
};

void ds_read(struct ds18b20_ctx *ctx, temp_cb_t cb, void *cbdata);
void ds_init(struct ds18b20_ctx *ctx, struct ow_ctx *ow);
