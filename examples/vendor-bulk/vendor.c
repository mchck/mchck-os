#include <mchck.h>
#include <usb/usb.h>

#include "vendor.desc.h"

static struct usbd_function_ctx_header fh;
static struct usbd_ep_pipe_state_t *tx_pipe, *rx_pipe;

static uint8_t buf[1024];
static size_t buflen = 0;

void
rx_done(void *buf_, ssize_t len, void *cbdata)
{
	if (len > sizeof(buf))
		len = sizeof(buf);
	buflen = len;
	usb_tx(tx_pipe, buf, buflen, sizeof(buf), NULL, NULL);
	usb_rx(rx_pipe, buf, sizeof(buf), rx_done, NULL);
}

void
init_my_func(const struct usbd_function *func, int enable)
{
	if (enable) {
		usb_attach_function(func, &fh);
		tx_pipe = usb_init_ep(&fh, 1, USB_EP_TX, 64);
		rx_pipe = usb_init_ep(&fh, 1, USB_EP_RX, 64);

		usb_rx(rx_pipe, buf, sizeof(buf), rx_done, NULL);
	}
}

void
init_my_vendor(int config)
{
}

void
main(void)
{
	usb_init(&vendor_dev);
	sys_yield_for_frogs();
}
