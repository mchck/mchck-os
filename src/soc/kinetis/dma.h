enum dma_status {
        DMA_STATUS_HALF,
        DMA_STATUS_FULL,
        DMA_STATUS_ERROR,
};

enum dma_flags {
        DMA_SRC_STICKY = 1,
        DMA_DST_STICKY = 2,
        DMA_DOUBLEBUF = 4,
        DMA_LOOP = 8,
};

struct dma_ctx;

typedef void (*dma_cb_t)(struct dma_ctx *ctx, enum dma_status status, void *cbdata);

struct dma_ctx {
        dma_cb_t cb;
        void *cbdata;
};

void dma_init(void);
struct dma_ctx *dma_setup(enum dmamux_channel dmach, const volatile void *src, void *dst, size_t elemsz, size_t nelem, enum dma_flags flags, dma_cb_t cb, void *cbdata);
