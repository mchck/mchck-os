struct wcid_os_descriptor {
        uint8_t bLength;
        enum usb_desc_type bDescriptorType : 8;
        char16_t qwSignature[7];
        uint8_t bMS_VendorCode;
        uint8_t bPad;
} __packed;

#define WCID_REQ_ID 0xee
/* XXX define table offset */
#define USB_DESC_WCID_OS                                        \
        (const struct usb_desc_string_t *)&(const struct wcid_os_descriptor) {      \
                .bLength = sizeof(struct wcid_os_descriptor),   \
                        .bDescriptorType = USB_DESC_STRING,     \
                        .qwSignature = u"MSFT100",              \
                        .bMS_VendorCode = WCID_REQ_ID,          \
                        .bPad = 0                               \
        }

enum wcid_desc_index {
        WCID_DESC_COMPAT_OS = 0x4,
        WCID_DESC_EXTENDED_PROP = 0x5,
};

struct wcid_generic_header {
        uint32_t dwLength;
        struct usb_bcd_t bcdVersion;
        uint16_t wIndex;
} __packed;

struct wcid_compat_id_header {
        struct wcid_generic_header;
        uint8_t bCount;
        uint8_t _rsvd[7];
} __packed;

#define WCID_COMPAT_ID_NONE "\0\0\0\0\0\0\0\0"
#define WCID_COMPAT_ID_WINUSB "WINUSB\0\0"

struct wcid_compat_id_function {
        uint8_t bFirstInterfaceNumber;
        uint8_t _rsvd;
        char compatibleID[8];
        char subCompatibleID[8];
        uint8_t reserved[6];
} __packed;

struct wcid_extended_prop_header {
        struct wcid_generic_header;
        uint16_t wCount;
} __packed;

struct wcid_function {
        struct usbd_global head;
        const struct wcid_compat_id_header *compat_id;
};

int wcid_handle_control(const struct usbd_global *f, struct usb_ctrl_req_t *req);
