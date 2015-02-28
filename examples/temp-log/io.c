#include <mchck.h>

#include "temp-log.desc.h"

#include "temp-log.h"

enum templog_req {
        REQ_GET_VERSION  = 0,
        REQ_SET_TIME     = 1,
        REQ_GET_TIME     = 2,
        REQ_SET_INTERVAL = 3,
        REQ_GET_INTERVAL = 4,
        REQ_SET_MODE     = 5,
        REQ_GET_MODE     = 6,
        REQ_ERASE_DATA   = 7,
        REQ_GET_COUNT    = 8,
        REQ_GET_TEMP     = 10,
        REQ_GET_DATA     = 12,
};


#define templog_min_interval 2
unsigned long templog_interval = templog_min_interval;

static uint32_t time;
static struct usbd_function_ctx_header fh;
static size_t get_data_remaining;
static size_t get_data_pos;

static void get_data_next(void *bud, ssize_t len, void *cbdata);

static void
get_data_ready(const void *buf, size_t len)
{
        size_t remaining = get_data_remaining;

        if (remaining > len)
                remaining = len;
        get_data_remaining -= remaining;
        get_data_pos += len;
        usb_ep0_tx(buf, len, remaining, get_data_next, NULL);
}

static void
get_data_next(void *buf, ssize_t len, void *cbdata)
{
        if (get_data_remaining == 0) {
                usb_handle_control_status(0);
                return;
        }
        if (get_data_pos >= flash_used()) {
                usb_handle_control_status(1);
                return;
        }
        flash_read_page(get_data_pos, get_data_ready);
}

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
        case REQ_GET_VERSION: {
                uint32_t version = 0;
                usb_ep0_tx_cp(&version, sizeof(version), req->wLength, NULL, NULL);
                break;
        }
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
                        flash_total(),
                        flash_used(),
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
                if (!templog_stopped() && (flash_ready_p() || flash_full_p()))
                        goto out;
                if (req->wValue == 0)
                        get_data_pos = 0;
                get_data_remaining = req->wLength;
                get_data_next(NULL, 0, NULL);
                return (1);

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
