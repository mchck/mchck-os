#include <mchck.h>
#include <usb/usb.h>

struct wcid_ctx {
        struct usbd_function_ctx_header header;
        const struct wcid_function *wcid;
};

static struct wcid_ctx wcid_ctx;

void
wcid_init(const struct usbd_function *f, int enable)
{
        const struct wcid_function *wcidf = (const void *)f;

        if (enable) {
                wcid_ctx.wcid = wcidf;
        } else {
                wcid_ctx.wcid = NULL;
        }
}

int
wcid_handle_control(struct usb_ctrl_req_t *req, void *data)
{
        if (req->bRequest != WCID_REQ_ID)
                return (0);

        size_t pos, len;
        const struct wcid_generic_header *desc;

        switch (req->wIndex) {
        case WCID_DESC_COMPAT_OS:
                desc = wcid_ctx.wcid->compat_id;
                break;

        case WCID_DESC_EXTENDED_PROP: {
                static const struct wcid_extended_prop_header ex_prop = {
                        .dwLength = sizeof(ex_prop),
                        .bcdVersion = { .raw = 0x0100 },
                        .wIndex = WCID_DESC_EXTENDED_PROP,
                        .wCount = 0,
                };

                desc = &ex_prop;
                break;
        }

        default:
                return (0);
        }

        pos = (req->wValue & 0xff00) << 8;
        len = req->wLength;
        if (pos > desc->dwLength)
                return (0);
        if (pos + len > desc->dwLength)
                len = desc->dwLength - pos;
        usb_ep0_tx((uint8_t *)desc + pos, len, req->wLength, NULL, NULL);
        usb_handle_control_status(0);
        return (1);
}
