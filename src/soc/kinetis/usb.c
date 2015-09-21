#define usb_xfer_info stat_wrap

#include <mchck.h>

#include <usb/usb-internal.h>

struct stat_wrap {
        union {
                struct {
                        uint8_t _rsvd0 : 2;
                        enum usb_ep_pingpong pingpong : 1;
                        enum usb_ep_dir dir : 1;
                        uint8_t ep : 4;
                };
                uint8_t raw;
        };
};


/**
 * Kinetis USB driver notes:
 * We need to manually maintain the DATA0/1 toggling for the SIE.
 * SETUP transactions always start with a DATA0.
 *
 * The SIE internally uses pingpong (double) buffering, which is
 * easily confused with DATA0/DATA1 toggling, and I think even the
 * Freescale docs confuse the two notions.  When BD->DTS is set,
 * BD->DATA01 will be used to verify/discard incoming DATAx and it
 * will set the DATAx PID for outgoing tokens.  This is not described
 * as such in the Freescale Kinetis docs, but the Microchip PIC32 OTG
 * docs are more clear on this;  it seems that both Freescale and
 * Microchip use different versions of the same USB OTG IP core.
 *
 * http://ww1.microchip.com/downloads/en/DeviceDoc/61126F.pdf
 *
 * Clear CTL->TOKEN_BUSY after SETUP tokens.
 */


static struct USB_BD_t *
usb_get_bd(struct usbd_ep_pipe_state_t *s)
{
        return (&kinetis_bdt[(s->ep_num << 2) | (s->ep_dir << 1) | s->pingpong]);
}

static struct USB_BD_t *
usb_get_bd_stat(struct usb_xfer_info *stat)
{
        return (((void *)(uintptr_t)kinetis_bdt + (stat->raw << 1)));
}

void *
usb_get_xfer_data(struct usb_xfer_info *i)
{
        return (usb_get_bd_stat(i)->addr);
}

enum usb_tok_pid
usb_get_xfer_pid(struct usb_xfer_info *i)
{
        return (usb_get_bd_stat(i)->tok_pid);
}

int
usb_get_xfer_ep(struct usb_xfer_info *i)
{
        return (i->ep);
}

enum usb_ep_dir
usb_get_xfer_dir(struct usb_xfer_info *i)
{
        return (i->dir);
}

void
usb_enable_xfers(void)
{
        USB0_CTL = USB_CTL_USBENSOFEN_MASK;
}

void
usb_set_addr(int addr)
{
        USB0_ADDR = addr;
}


void
usb_pipe_stall(struct usbd_ep_pipe_state_t *s)
{
        volatile struct USB_BD_t *bd = usb_get_bd(s);
        bd->raw = ((struct USB_BD_BITS_t){
                        .stall = 1,
                                .own = 1
                                }).raw;
}

void
usb_pipe_unstall(struct usbd_ep_pipe_state_t *s)
{
        volatile struct USB_BD_t *bd = usb_get_bd(s);
        struct USB_BD_BITS_t state = { .raw = bd->raw };

        if (state.own && state.stall)
                bd->raw = 0;
}

void
usb_pipe_enable(struct usbd_ep_pipe_state_t *s)
{
        uint32_t flags = 0;

        flags |= USB_ENDPT_EPHSHK_MASK;

        if (s->ep_dir == USB_EP_TX)
                flags |= USB_ENDPT_EPTXEN_MASK;
        else
                flags |= USB_ENDPT_EPRXEN_MASK;

        if (s->ep_num != 0)
                flags |= USB_ENDPT_EPCTLDIS_MASK;

        USB_ENDPT_REG(USB0_BASE_PTR, s->ep_num) |= flags;
}

void
usb_pipe_disable(struct usbd_ep_pipe_state_t *s)
{
        uint32_t flags = 0;

        flags |= USB_ENDPT_EPCTLDIS_MASK;

        if (s->ep_dir == USB_EP_TX)
                flags |= USB_ENDPT_EPTXEN_MASK;
        else
                flags |= USB_ENDPT_EPRXEN_MASK;

        USB_ENDPT_REG(USB0_BASE_PTR, s->ep_num) &= ~flags;
}

size_t
usb_ep_get_transfer_size(struct usbd_ep_pipe_state_t *s)
{
        struct USB_BD_t *bd = usb_get_bd(s);
        return (bd->bc);
}

void
usb_queue_next(struct usbd_ep_pipe_state_t *s, void *addr, size_t len)
{
        volatile struct USB_BD_t *bd = usb_get_bd(s);

        bd->addr = addr;
        /* damn you bitfield problems */
        bd->raw = ((struct USB_BD_BITS_t){
                        .dts = 1,
                                .own = 1,
                                .data01 = s->data01,
                                .bc = len,
                                }).raw;
}

__noinline
static void
usb_reset(void)
{
        /* reset pingpong state */
        /* For some obscure reason, we need to use or here. */
        USB0_CTL |=
                USB_CTL_TXSUSPENDTOKENBUSY_MASK | /* XXX did not get set in bitfield compiler output */
                USB_CTL_ODDRST_MASK;

        /* clear all interrupt bits - not sure if needed */
        USB0_ISTAT = 0xff;
        USB0_ERRSTAT = 0xff;
        USB0_OTGISTAT = 0xff;

        /* zap also BDT pingpong & queued transactions */
        memset(kinetis_bdt, 0, (uintptr_t)&_usb_bdt_end - (uintptr_t)kinetis_bdt);
        USB0_ADDR = 0;

        usb_restart();

        USB0_CTL = USB_CTL_USBENSOFEN_MASK;

        /* we're only interested in reset and transfers */
        USB0_INTEN =
                USB_INTEN_TOKDNEEN_MASK |
                USB_INTEN_USBRSTEN_MASK |
                USB_INTEN_STALLEN_MASK |
                USB_INTEN_SLEEPEN_MASK;

        bf_set_reg(USB0_USBTRC0, USB_USBTRC0_USBRESMEN, 0);
        bf_set_reg(USB0_USBCTRL, USB_USBCTRL_SUSP, 0);
}

void
usb_enable(void)
{
        bf_set_reg(SIM_SOPT2, SIM_SOPT2_USBSRC, 1);   /* usb from mcg */
        bf_set_reg(SIM_SCGC4, SIM_SCGC4_USBOTG, 1);   /* enable usb clock */

#if USB_FMC_MASTER == 3
        /* Allow USB to access the Flash */
        bf_set_reg(FMC_PFAPR, FMC_PFAPR_M3AP, FMC_MAP_RDWR);
#elif USB_FMC_MASTER == 4
        /* Allow USB to access the Flash */
        bf_set_reg(FMC_PFAPR, FMC_PFAPR_M4AP, FMC_MAP_RDWR);
#endif

#if defined(MCM_PLACR) && !defined(AXBS_CRS)
        /* Round robin bus masters, so that the CPU can't starve the USB */
        bf_set_reg(MCM_PLACR, MCM_PLACR_ARB, 1);
#elif defined(AXBS_CRS)
        for (int i = 0; i < sizeof(((AXBS_MemMapPtr)0)->SLAVE)/sizeof(*((AXBS_MemMapPtr)0)->SLAVE); ++i)
                bf_set_reg(AXBS_CRS(i), AXBS_CRS_ARB, 0b01);
#endif

        /* reset module - not sure if needed */
        USB0_USBTRC0 =
                USB_USBTRC0_USBRESET_MASK |
                USB_USBTRC0_USBRESMEN_MASK;
        while (bf_get_reg(USB0_USBTRC0, USB_USBTRC0_USBRESET))
                /* NOTHING */;

        USB0_BDTPAGE1 = (uintptr_t)kinetis_bdt >> 8;
        USB0_BDTPAGE2 = (uintptr_t)kinetis_bdt >> 16;
        USB0_BDTPAGE3 = (uintptr_t)kinetis_bdt >> 24;

        USB0_CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;

        USB0_USBCTRL = 0;  /* resume peripheral & disable pulldowns */
        usb_reset();       /* this will start usb processing */

        /* really only one thing we want */
        USB0_INTEN = USB_INTEN_USBRSTEN_MASK;

        /**
         * Suspend transceiver now - we'll wake up at reset again.
         */
        bf_set_reg(USB0_USBCTRL, USB_USBCTRL_SUSP, 1);
        bf_set_reg(USB0_USBTRC0, USB_USBTRC0_USBRESMEN, 1);

#ifndef SHORT_ISR
        int_enable(IRQ_USB0);
#endif
}

void
USB0_Handler(void)
{
        uint8_t istat = USB0_ISTAT;

        if (istat & USB_ISTAT_USBRST_MASK) {
                usb_reset();
                return;
        }
        if (istat & USB_ISTAT_STALL_MASK) {
                /* XXX need more work for non-0 ep */
                volatile struct USB_BD_t *bd = usb_get_bd(&usbd_ep_state[0].rx);
                if (bd->stall)
                        usb_setup_control();
        }
        if (istat & USB_ISTAT_TOKDNE_MASK) {
                struct usb_xfer_info stat = {.raw = USB0_STAT};
                usb_handle_transaction(&stat);
        }

        const struct usbd_config *c = usb_get_config_data(-1);
        if (istat & USB_ISTAT_SLEEP_MASK) {
                bf_set_reg(USB0_INTEN, USB_INTEN_SLEEPEN, 0);
                bf_set_reg(USB0_INTEN, USB_INTEN_RESUMEEN, 1);
                bf_set_reg(USB0_USBCTRL, USB_USBCTRL_SUSP, 1);
                bf_set_reg(USB0_USBTRC0, USB_USBTRC0_USBRESMEN, 1);

                /**
                 * Clear interrupts now so that we can detect a fresh
                 * resume later on.
                 */
                USB0_ISTAT = istat;

                if (c && c->suspend)
                        c->suspend();
        }
        /**
         * XXX it is unclear whether we will receive a synchronous
         * resume interrupt if we were in sleep.  This code assumes we
         * do.
         */
        if ((istat & USB_ISTAT_RESUME_MASK) || (USB0_USBTRC0 & USB_USBTRC0_USB_RESUME_INT_MASK)) {
                bf_set_reg(USB0_INTEN, USB_INTEN_RESUMEEN, 0);
                bf_set_reg(USB0_INTEN, USB_INTEN_SLEEPEN, 1);
                bf_set_reg(USB0_USBTRC0, USB_USBTRC0_USBRESMEN, 0);
                bf_set_reg(USB0_USBCTRL, USB_USBCTRL_SUSP, 0);

                if (c && c->resume)
                        c->resume();

                istat |= USB_ISTAT_RESUME_MASK; /* always clear bit */
        }
        USB0_ISTAT = istat;
}

void
usb_poll(void)
{
        USB0_Handler();
}

int
usb_tx_serialno(size_t reqlen)
{
        struct usb_desc_string_t *d;
        const size_t nregs = 3;
        /**
         * actually 4, but UIDH is 0xffffffff.  Also our output buffer
         * is only 64 bytes, and 128 bit + desc header exceeds this by
         * 2 bytes.
         */
        const size_t len = nregs * 4 * 2 * 2 + sizeof(*d);

        d = usb_ep0_tx_inplace_prepare(len);

        if (d == NULL)
                return (-1);

        d->bLength = len;
        d->bDescriptorType = USB_DESC_STRING;

        size_t bufpos = 0;
        for (size_t reg = 0; reg < nregs; ++reg) {
                /* registers run MSW first */
                uint32_t val = (&SIM_UIDMH)[reg];

                for (size_t bits = 32; bits > 0; bits -= 4, val <<= 4) {
                        int nibble = val >> 28;

                        if (nibble > 9)
                                nibble += 'a' - '9' - 1;
                        ((char16_t *)d->bString)[bufpos++] = nibble + '0';
                }
        }
        usb_ep0_tx(d, len, reqlen, NULL, NULL);
        return (0);
}
