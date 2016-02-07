#include <mchck.h>

#include "bootloader-update-dfu.desc.h"


/**
 * We know what we're doing, part 1.
 */
extern uint32_t flash_ALLOW_BRICKABLE_ADDRESSES;

/**
 * We buffer the whole bootloader in this flash area before we go on
 * to flash it to the final location.
 *
 * We initialize the variable so that it gets placed into .rodata, and
 * not into .bss.
 */
const static uint8_t staging[LOADER_SIZE] __attribute__((__aligned__(FLASH_SECTOR_SIZE), __section__(".rodata.staging"))) = {0};

static uint8_t staging_buf[FLASH_SECTOR_SIZE];

enum dfu_status
setup_write(struct dfu_ctx *ctx, size_t off, size_t len, void **buf)
{
        if (off + len > sizeof(staging))
                return (DFU_STATUS_errADDRESS);
        *buf = staging_buf;
        return (DFU_STATUS_OK);
}

enum dfu_status
finish_write(struct dfu_ctx *ctx, void *buf, size_t off, size_t len)
{
        if (len != 0) {
                flash_program_sector(staging_buf, (uintptr_t)staging + off, FLASH_SECTOR_SIZE);
                return (DFU_STATUS_OK);
        }

        NV_Type *flashconfig = (void *)&staging[(uintptr_t)FlashConfig_BASE_PTR];

        /* Make sure we don't brick ourselves */
        if (bf_get_reg(flashconfig->FSEC, NV_FSEC_SEC) != NV_FSEC_SEC_UNSECURE)
                return (DFU_STATUS_errFILE);

        /* We know what we're doing, part 2. */
        flash_ALLOW_BRICKABLE_ADDRESSES = 0x00023420;

        for (size_t pos = 0; pos < sizeof(staging); pos += FLASH_SECTOR_SIZE) {
                if (flash_program_sector(staging + pos, pos, FLASH_SECTOR_SIZE) != 0)
                        return (DFU_STATUS_errADDRESS);
        }
        return (DFU_STATUS_OK);
}

void
main(void)
{
        onboard_led(1);

        flash_prepare_flashing();

        usb_init(&dfu_device);
        sys_yield_for_frogs();
}
