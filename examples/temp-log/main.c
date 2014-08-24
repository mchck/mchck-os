#include <mchck.h>

#include "onewire.h"
#include "ds18b20.h"

#include "temp-log.desc.h"


enum {
        SPIFLASH_PAGE_SIZE = 256,
        SPIFLASH_SECTOR_SIZE = 4096,
};

enum flash_state {
        FLASH_INIT,
        FLASH_FIND_EMPTY_SECTOR,
        FLASH_READY,
        FLASH_PAGE_FILLED,
        FLASH_WRITING,
        FLASH_ERASING,
        FLASH_FULL,
};

enum templog_state {
        TEMPLOG_IDLE,
        TEMPLOG_READING,
        TEMPLOG_SLEEPING,
};

struct templog_entry {
        uint32_t time;
        uint32_t temp;
};


static void got_temp_data(struct ds18b20_ctx *ctx, uint16_t temp, void *cbdata);
static void flash_flush_data(void);
static void flash_state_machine(void);
static void templog_statemachine(void);


static struct cdc_ctx cdc;
static struct ow_ctx ow_ctx;
static struct ds18b20_ctx ds;

static struct spiflash_transaction flash_trans;
static uint8_t flash_page[SPIFLASH_PAGE_SIZE];
static size_t flash_total_size = (1<<20); /* XXX read dynamically */

static enum flash_state flash_state;
static size_t flash_addr;
static size_t flash_pagepos;

static enum templog_state templog_state;
static struct timeout_ctx templog_timeout;


static int
page_space_available(void)
{
        if (flash_pagepos + sizeof(struct templog_entry) <= SPIFLASH_PAGE_SIZE)
                return (1);
        else
                return (0);
}

static void
got_temp_data(struct ds18b20_ctx *ctx, uint16_t temp, void *cbdata)
{
        struct templog_entry e;

        if (flash_state != FLASH_FULL && page_space_available()) {
                e.time = 1;
                e.temp = temp;
                memcpy(&flash_page[flash_pagepos], &e, sizeof(e));
                flash_pagepos += sizeof(e);
        }

        printf("pagepos %d, pos %d, free %d, temp %d %d/16\r\n",
               flash_pagepos,
               flash_addr,
               flash_total_size - flash_addr,
               temp >> 4, temp & 0xf);

        if (!page_space_available())
                flash_flush_data();

        onboard_led(ONBOARD_LED_ON);

        templog_statemachine();
}

static void
read_temp_again(void *cbdata)
{
        templog_statemachine();
}

static void
templog_statemachine(void)
{
        switch (templog_state) {
        case TEMPLOG_IDLE:
        case TEMPLOG_SLEEPING:
                onboard_led(ONBOARD_LED_OFF);
                ds_read(&ds, got_temp_data, NULL);
                templog_state = TEMPLOG_READING;
                break;
        case TEMPLOG_READING:
                timeout_add(&templog_timeout, 1000, read_temp_again, NULL);
                templog_state = TEMPLOG_SLEEPING;
                break;
        }
}


static int
flash_completely_full(void)
{
        if (flash_addr >= flash_total_size)
                return 1;
        else
                return 0;
}

static void
flush_flash_done(void *cbdata)
{
        flash_state_machine();
}

static void
flash_flush_data(void)
{
        flash_state = FLASH_PAGE_FILLED;
        flash_state_machine();
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
                if (flash_completely_full())
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
                break;
        case FLASH_PAGE_FILLED:
                spiflash_program_page(&onboard_flash, &flash_trans,
                                      flash_addr, flash_page, flash_pagepos,
                                      flush_flash_done, NULL);
                flash_state = FLASH_WRITING;
                flash_pagepos = 0;
                break;
        case FLASH_WRITING:
                flash_addr += SPIFLASH_PAGE_SIZE;
                if (flash_completely_full()) {
                        flash_state = FLASH_FULL;
                } else {
                        if ((flash_addr & (SPIFLASH_SECTOR_SIZE - 1)) == 0) {
                                spiflash_erase_sector(&onboard_flash, &flash_trans,
                                                      flash_addr,
                                                      flush_flash_done, NULL);
                                flash_state = FLASH_ERASING;
                        } else {
                                flash_state = FLASH_READY;
                        }
                }
                break;
        case FLASH_ERASING:
                flash_state = FLASH_READY;
                break;
        case FLASH_FULL:
                break;
        }
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
