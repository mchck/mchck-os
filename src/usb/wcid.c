#include <mchck.h>
#include <usb/usb.h>

int
wcid_handle_control(const struct usbd_global *f, struct usb_ctrl_req_t *req)
{
        const struct wcid_function *wcid = (const void *)f;

        if (req->bRequest != WCID_REQ_ID)
                return (0);

        size_t pos, len;
        const struct wcid_generic_header *desc;

        switch (req->wIndex) {
        case WCID_DESC_COMPAT_OS:
                desc = wcid->compat_id;
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
