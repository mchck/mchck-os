#include <mchck.h>

#include "temp-log.desc.h"

#include "temp-log.h"

#define templog_min_interval 2
unsigned long templog_interval = templog_min_interval;

static uint32_t time;

static struct usbd_function_ctx_header fh;

enum templog_req {
        REQ_SET_TIME     = 0,
        REQ_GET_TIME     = 1,
        REQ_SET_INTERVAL = 2,
        REQ_GET_INTERVAL = 3,
        REQ_SET_MODE     = 4,
        REQ_GET_MODE     = 5,
        REQ_ERASE_DATA   = 6,
        REQ_GET_COUNT    = 7,
        REQ_GET_TEMP     = 9,
        REQ_GET_DATA     = 11,
};


static void
time_done_cb(void *buf, ssize_t len, void *cbdata)
{
        rtc_set_time(time);
        rtc_start_counter();
        usb_handle_control_status(0);
}

int
control_func(struct usb_ctrl_req_t *req, void *cbdata)
{
        int fail = 1;

        if (req->type != USB_CTRL_REQ_VENDOR)
                return (0);

        switch ((enum templog_req)req->bRequest) {
        case REQ_SET_TIME:
                usb_ep0_rx(&time, sizeof(time), time_done_cb, NULL);
                break;
        case REQ_GET_TIME: {
                uint32_t curtime = rtc_get_time();
                usb_ep0_tx_cp(&curtime, sizeof(curtime), req->wLength, NULL, NULL);
                break;
        }
        case REQ_SET_INTERVAL:
                if (req->wValue < templog_min_interval)
                        goto out;
                templog_interval = req->wValue;
                break;
        case REQ_GET_INTERVAL: {
                uint16_t interval = templog_interval;
                usb_ep0_tx_cp(&interval, sizeof(interval), req->wLength, NULL, NULL);
                break;
        }
        case REQ_SET_MODE:
                if (req->wValue)
                        templog_start();
                else
                        templog_stop();
                break;
        case REQ_GET_MODE: {
                uint8_t mode;

                mode = !templog_stopped();
                usb_ep0_tx_cp(&mode, sizeof(mode), req->wLength, NULL, NULL);
                break;
        }
        case REQ_ERASE_DATA:
                if (!templog_stopped())
                        goto out;
                flash_eraseall();
                break;
        case REQ_GET_COUNT: {
                struct {
                        uint32_t count;
                        uint32_t free;
                } count = {
                        flash_used() / sizeof(templog_entry),
                        flash_free() / sizeof(templog_entry),
                };
                usb_ep0_tx_cp(&count, sizeof(count), req->wLength, NULL, NULL);
                break;
        }
        case REQ_GET_TEMP: {
                int16_t val = templog_last_temp();
                usb_ep0_tx_cp(&val, sizeof(val), req->wLength, NULL, NULL);
                break;
        }
        case REQ_GET_DATA:
                break;

        default:
                goto out;
        }

        fail = 0;

out:
        usb_handle_control_status(fail);
        return (1);
}

void
init_func(const struct usbd_function *func, int enable)
{
        if (enable) {
                usb_attach_function(func, &fh);
        }
}
