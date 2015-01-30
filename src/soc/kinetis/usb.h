#define USB_DECL_BUFS_MD(ep_in, ep_out)                                 \
	struct USB_BD_t kinetis_bdt[(ep_in > ep_out ? ep_in : ep_out) * 2 *2] __attribute__((section(".usb_bdt")))

extern struct USB_BD_t kinetis_bdt[];
extern struct USB_BD_t _usb_bdt_end;


struct usbd_ep_pipe_state_t;
struct usb_xfer_info;

void *usb_get_xfer_data(struct usb_xfer_info *);
enum usb_tok_pid usb_get_xfer_pid(struct usb_xfer_info *);
int usb_get_xfer_ep(struct usb_xfer_info *);
enum usb_ep_dir usb_get_xfer_dir(struct usb_xfer_info *);
void usb_enable_xfers(void);
void usb_set_addr(int);
void usb_ep_stall(int);
size_t usb_ep_get_transfer_size(struct usbd_ep_pipe_state_t *);
void usb_queue_next(struct usbd_ep_pipe_state_t *, void *, size_t);
void usb_pipe_stall(struct usbd_ep_pipe_state_t *);
void usb_pipe_unstall(struct usbd_ep_pipe_state_t *);
void usb_pipe_enable(struct usbd_ep_pipe_state_t *s);
void usb_pipe_disable(struct usbd_ep_pipe_state_t *s);
#ifdef VUSB
void vusb_main_loop(void);
#else
void usb_poll(void);
#endif
int usb_tx_serialno(size_t reqlen);
