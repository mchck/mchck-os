
/**
 * Internal driver structures
 */

/**
 * USB state machine
 * =================
 *
 * Device configuration states:
 *
 * Attached <-> Powered
 * Powered -(reset)-> Default
 * Default -(SET_ADDRESS)-> Address
 * Address -(SET_CONFIGURATION)-> Configured
 * Configured -(SET_CONFIGURATION 0)-> Address
 * Address -(SET_ADDRESS 0)-> Default
 * [Default, Configured, Address] -(reset)-> Default
 */


struct usbd_ep_pipe_state_t {
	size_t transfer_size;
	size_t pos;
	uint8_t *data_buf;
	const uint8_t *copy_source;
	ep_callback_t callback;
	void *callback_data;

	uint16_t ep_maxsize;

	int short_transfer : 8;
	enum usb_ep_pingpong pingpong : 8; /* next desc to use */
	enum usb_data01 data01 : 8;
	/* constant */
	enum usb_ep_dir ep_dir : 8;
	int ep_num : 8;
};

struct usbd_t {
	struct usbd_function_ctx_header functions;
	struct usbd_function control_function;
	const struct usbd_device *identity;
	int address;
	int config;
	enum usbd_dev_state {
		USBD_STATE_DISABLED = 0,
		USBD_STATE_DEFAULT,
		USBD_STATE_SETTING_ADDRESS,
		USBD_STATE_ADDRESS,
		USBD_STATE_CONFIGURED
	} state;
	enum usb_ctrl_req_dir ctrl_dir;
	uint8_t pipe_count;
};

extern struct usbd_t usb;

#define USB_DECL_BUFS(ep_in, ep_out)					\
	USB_DECL_BUFS_MD(ep_in, ep_out);				\
	struct usbd_ep_pipe_state_t usbd_pipe_state[ep_in + ep_out]

#define USBD_PIPE_EP0_RX 0
#define USBD_PIPE_EP0_TX 1
extern struct usbd_ep_pipe_state_t usbd_pipe_state[];


void usb_restart(void);
void usb_enable(void);
const struct usbd_config *usb_get_config_data(int config);
