#include <mchck.h>

static struct spi_ctx_bare *spi_ctx;

static void
spi_start_xfer(void)
{
        SPI0_MCR =
                SPI_MCR_MSTR_MASK |
                SPI_MCR_DCONF(SPI_DCONF_SPI) |
                SPI_MCR_ROOE_MASK |
                SPI_MCR_PCSIS(0b11111) |
                SPI_MCR_CLR_TXF_MASK |
                SPI_MCR_CLR_RXF_MASK;
        SPI0_RSER =
                SPI_RSER_TFFF_RE_MASK |
                (spi_ctx->rx != NULL ? SPI_RSER_RFDF_RE_MASK : 0) |
                SPI_RSER_EOQF_RE_MASK;
        SPI0_SR = 0xffffffff;   /* clear all clags */
}

static void
spi_stop_xfer(void)
{
        bf_set_reg(SPI0_MCR, SPI_MCR_HALT, 1);
        SPI0_RSER = 0;
        SPI0_SR = 0xffffffff;   /* clear all clags */
}

bool spi_is_idle(void)
{
        return spi_ctx == NULL;
}

int
spi_is_xfer_active(void)
{
        return (!bf_get_reg(SPI0_MCR, SPI_MCR_HALT));
}

int
spi_queue_xfer(struct spi_ctx *ctx,
               enum spi_pcs pcs,
               const uint8_t *txbuf, uint16_t txlen,
               uint8_t *rxbuf, uint16_t rxlen,
               spi_cb *cb, void *cbdata)
{
        return spi_queue_xfer_sg(&ctx->ctx, pcs,
                                 sg_init(&ctx->tx_sg, (void *)txbuf, txlen),
                                 sg_init(&ctx->rx_sg, rxbuf, rxlen),
                                 cb, cbdata);
}

int
spi_queue_xfer_sg(struct spi_ctx_bare *ctx,
                  enum spi_pcs pcs,
                  struct sg *tx, struct sg *rx,
                  spi_cb *cb, void *cbdata)
{
        if (ctx->queued)
                return 1;

        *ctx = (struct spi_ctx_bare){
                .tx = sg_simplify(tx),
                .rx = sg_simplify(rx),
                .pcs = pcs,
                .cb = cb,
                .cbdata = cbdata,
                .queued = true,
                .next = NULL
        };

        size_t tx_len = sg_total_length(ctx->tx);
        size_t rx_len = sg_total_length(ctx->rx);
        if (rx_len > tx_len)
                ctx->rx_tail = rx_len - tx_len;

        crit_enter();
        /* search for tail and append */
        for (struct spi_ctx_bare **c = &spi_ctx; ; c = &(*c)->next) {
                if (*c == NULL) {
                        *c = ctx;
                        /* we're at the head, so start xfer */
                        if (c == &spi_ctx)
                                spi_start_xfer();
                        break;
                }
        }
        crit_exit();
        return 0;
}

void
SPI0_Handler(void)
{
again:
        if (spi_ctx == NULL)
                return;

        for (;;) {
                uint32_t status, flags;

                status = SPI0_SR;
                flags = status & SPI0_RSER;

                if (bf_get_reg(flags, SPI_SR_RFDF) && spi_ctx->rx) {
                        for (int i = bf_get_reg(status, SPI_SR_RXCTR); i > 0 && spi_ctx->rx; --i, sg_move(&spi_ctx->rx, 1)) {
                                uint8_t d = SPI0_POPR;
                                if (sg_data(spi_ctx->rx) != NULL)
                                        *sg_data(spi_ctx->rx) = d;
                        }
                        /* disable interrupt if we're done receiving */
                        if (!spi_ctx->rx)
                                bf_set_reg(SPI0_RSER, SPI_RSER_RFDF_RE, 0);
                        SPI0_SR = SPI_SR_RFDF_MASK;
                } else if ((spi_ctx->tx || spi_ctx->rx_tail > 0) && bf_get_reg(flags, SPI_SR_TFFF)) {
                        int more = 0;
                        uint8_t data;

                        if (spi_ctx->tx) {
                                data = *sg_data(spi_ctx->tx);
                                if (sg_move(&spi_ctx->tx, 1) != SG_END)
                                        more = 1;
                                else
                                        more = 0;
                        } else {
                                data = 0xff;
                                --spi_ctx->rx_tail;
                        }
                        more = more || (spi_ctx->rx_tail > 0);

                        SPI0_PUSHR =
                                (more ? SPI_PUSHR_CONT_MASK : 0) |
                                (!more ? SPI_PUSHR_EOQ_MASK : 0) |
                                SPI_PUSHR_PCS(spi_ctx->pcs) |
                                SPI_PUSHR_TXDATA(data);
                        SPI0_SR = SPI_SR_TFFF_MASK;
                } else if (bf_get_reg(flags, SPI_SR_EOQF) && !spi_ctx->tx && !spi_ctx->rx) {
                        /* transfer done */
                        struct spi_ctx_bare *ctx = spi_ctx;

                        crit_enter();
                        ctx->queued = false;
                        spi_ctx = ctx->next;
                        if (spi_ctx != NULL)
                                spi_start_xfer();
                        else
                                spi_stop_xfer();
                        crit_exit();
                        if (ctx->cb != NULL)
                                ctx->cb(ctx->cbdata);
                        goto again;
                } else {
                        break;
                }
        }
}

void
spi_init(void)
{
        bf_set_reg(SIM_SCGC6, SIM_SCGC6_SPI0, 1); /* enable SPI clock */
        SPI0_CTAR0 =
                SPI_CTAR_FMSZ(7) |
                SPI_CTAR_CSSCK(0b1000) |
                SPI_CTAR_ASC(0b1000) |
                SPI_CTAR_DT(0b1000) |
                SPI_CTAR_BR(0b1000);
        int_enable(IRQ_SPI0);
}
