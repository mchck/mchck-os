#include <mchck.h>

#include "dfu.desc.h"


/**
 * Unfortunately we can't DMA directly to FlexRAM, so we'll have to stage here.
 */
static char staging[FLASH_SECTOR_SIZE];

enum dfu_status
setup_write(struct dfu_ctx *ctx, size_t off, size_t len, void **buf)
{
        static int last = 0;

        if (len > sizeof(staging))
                return (DFU_STATUS_errADDRESS);
        /**
         * We only allow the last write to be less than one sector size.
         */
        if (off == 0)
                last = 0;
        if (last && len != 0)
                return (DFU_STATUS_errADDRESS);
        if (len != FLASH_SECTOR_SIZE) {
                last = 1;
                memset(staging, 0xff, sizeof(staging));
        }

        *buf = staging;
        return (DFU_STATUS_OK);
}

enum dfu_status
finish_write(struct dfu_ctx *ctx, void *buf, size_t off, size_t len)
{
        if (len == 0)
                return (DFU_STATUS_OK);

        if (flash_program_sector(buf, off + (uintptr_t)&_app_rom, FLASH_SECTOR_SIZE) != 0)
                return (DFU_STATUS_errADDRESS);
        return (DFU_STATUS_OK);
}

/*
 * Called on USB reset
 */
void
reset_to_app(void)
{
        sys_reset();
}


void
main(void)
{
        flash_prepare_flashing();

        usb_init(&dfu_device);
#ifdef SHORT_ISR
        for (;;) {
                usb_poll();
        }
#else
        sys_yield_for_frogs();
#endif
}

__attribute__((noreturn))
static inline void
jump_to_app(uintptr_t addr)
{
        /* addr is in r0 */
        __asm__("ldm %[addr]!, {r0,r1}\n"
                "mov sp, r0\n"
                "mov pc, r1\n"
                :: [addr] "r" (addr));
        /* NOTREACHED */
        __builtin_unreachable();
}

void
Reset_Handler(void)
{
	/**
         * Disable Watchdog.  We spend too much time in here comparing
         * the loader magic, which will lead to a watchdog
         * configuration timeout.
         */
        watchdog_disable();
        /* XXX maybe re-enable watchdog later? */
        /* XXX this is permanent on some platforms */

        /**
         * We treat _app_rom as pointer to directly read the stack
         * pointer and check for valid app code.  This is no fool
         * proof method, but it should help for the first flash.
         */
        if (bf_get_reg(RCM_SRS0, RCM_SRS0_PIN) ||
            _app_rom == 0xffffffff ||
            memcmp(sys_register_file, sys_reset_to_loader_magic, sizeof(sys_reset_to_loader_magic)) == 0) {
                extern void Default_Reset_Handler(void);

                memset(sys_register_file, 0, sizeof(sys_register_file));
                Default_Reset_Handler();
        } else {
                uint32_t addr = (uintptr_t)&_app_rom;
                SCB->VTOR = addr;   /* relocate vector table */
                jump_to_app(addr);
        }
}
