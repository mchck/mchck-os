#include <mchck.h>

static size_t
cdc_stdio_write(const uint8_t *buf, size_t len, void *data)
{
        return (cdc_write(buf, len, data));
}

static const struct _stdio_file_ops cdc_ops = {
        .write = cdc_stdio_write,
};

void
cdc_set_stdout(struct cdc_ctx *cdc)
{
        stdout->ops_data = cdc;
        stdout->ops = &cdc_ops;
}
