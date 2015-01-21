#include <usb/usb-common.h>

struct USB_BD_t {
        struct USB_BD_BITS_t {
                union {
                        struct {
                                uint32_t _rsvd0  : 2;
                                uint32_t stall   : 1;
                                uint32_t dts     : 1;
                                uint32_t ninc    : 1;
                                uint32_t keep    : 1;
                                enum usb_data01 data01 : 1;
                                uint32_t own     : 1;
                                uint32_t _rsvd1  : 8;
                                uint32_t bc      : 10;
                                uint32_t _rsvd2  : 6;
                        };
                        struct /* processor */ {
                                uint32_t _rsvd5  : 2;
                                enum usb_tok_pid tok_pid : 4;
                                uint32_t _rsvd6  : 26;
                        };
                        uint32_t raw;
                };
        };
        void *addr;
};
CTASSERT_SIZE_BYTE(struct USB_BD_t, 8);
