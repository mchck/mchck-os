struct wcid_os_descriptor {
        uint8_t bLength;
        enum usb_desc_type bDescriptorType : 8;
        char16_t qwSignature[7];
        uint8_t bMS_VendorCode;
        uint8_t bPad;
} __packed;

struct wcid_compat_id_header {
        uint32_t dwLength;
        struct usb_bcd_t bcdVersion;
        uint16_t wIndex;
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

struct wcid_function {
        struct usbd_function usb_func;
};

usbd_func_init_t wcid_init;
usbd_func_control_t wcid_handle_control;

extern const struct usbd_function wcid_function;
