#include <mchck.h>

#include "onewire.h"
#include "ds18b20.h"

#include "temp-log.desc.h"

enum flash_state {
        FLASH_INIT,
        FLASH_FIND_EMPTY_SECTOR,
        FLASH_READY,
        FLASH_FULL
};

enum {
        SPIFLASH_PAGE_SIZE = 256,
        SPIFLASH_SECTOR_SIZE = 4096,
};


static void got_temp_data(struct ds18b20_ctx *ctx, uint16_t temp, void *cbdata);
static void flash_state_machine(void);


static struct cdc_ctx cdc;

static struct ow_ctx ow_ctx;
static struct ds18b20_ctx ds;

static uint8_t flash_page[SPIFLASH_PAGE_SIZE];
static struct spiflash_transaction flash_trans;
static size_t flash_total_size = (1<<20); /* XXX read dynamically */
static size_t flash_addr;
static enum flash_state flash_state;


static void
read_temp_again(void *cbdata)
{
        onboard_led(ONBOARD_LED_OFF);
        ds_read(&ds, got_temp_data, NULL);
}

static void
got_temp_data(struct ds18b20_ctx *ctx, uint16_t temp, void *cbdata)
{
        static struct timeout_ctx t;

        printf("pos %d, free %d, temp %d %d/16\r\n",
               flash_addr,
               flash_total_size - flash_addr,
               temp >> 4, temp & 0xf);
        onboard_led(ONBOARD_LED_ON);
        timeout_add(&t, 1000, read_temp_again, NULL);
}

void
init_usb(int enable)
{
        onboard_led(ONBOARD_LED_ON);
        if (enable) {
                cdc_init(NULL, NULL, &cdc);
                cdc_set_stdout(&cdc);
                ds_read(&ds, got_temp_data, NULL);
        }
}


static void
flash_find_empty_sector(void *cbdata)
{
        int empty = 1;
        for (size_t i = 0; i < sizeof(flash_page); ++i) {
                if (flash_page[i] != 0xff) {
                        empty = 0;
                        break;
                }
        }
        if (empty) {
                flash_state = FLASH_READY;
        } else {
                flash_addr += SPIFLASH_SECTOR_SIZE;
                if (flash_addr >= flash_total_size)
                        flash_state = FLASH_FULL;
        }
        flash_state_machine();
}

static void
flash_state_machine(void)
{
again:
        switch (flash_state) {
        case FLASH_INIT:
                /* XXX read size */
                flash_state = FLASH_FIND_EMPTY_SECTOR;
                goto again;
        case FLASH_FIND_EMPTY_SECTOR:
                spiflash_read_page(&onboard_flash, &flash_trans,
                                   flash_page, flash_addr, sizeof(flash_page),
                                   flash_find_empty_sector, NULL);
                break;
        case FLASH_READY:
        case FLASH_FULL:
                break;
        }
}


void
main(void)
{
        timeout_init();

        pin_mode(PIN_PTC4, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        pin_mode(PIN_PTC3, PIN_MODE_MUX_ALT3 | PIN_MODE_OPEN_DRAIN_ON);
        ow_init(&ow_ctx, &uart1);
        ds_init(&ds, &ow_ctx);

        spiflash_pins_init();
        spi_init();

        flash_state_machine();

        usb_init(&cdc_device);

        sys_yield_for_frogs();
}
