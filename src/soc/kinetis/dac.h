struct dac_ctx;

extern struct dac_ctx dac0_ctx;
extern struct dac_ctx dac1_ctx;

void dac_init(struct dac_ctx *ctx);
void dac_enable(struct dac_ctx *ctx, int enable);
void dac_output(struct dac_ctx *ctx, uint16_t val);
