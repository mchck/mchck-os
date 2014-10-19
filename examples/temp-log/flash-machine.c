#include <mchck.h>

#include "temp-log.h"


enum flash_state {
        FLASH_INIT,
        FLASH_FIND_EMPTY_SECTOR,
        FLASH_READY,
        FLASH_PAGE_FILLED,
        FLASH_WRITING,
        FLASH_ERASING,
        FLASH_FULL,
};


uint8_t flash_page[SPIFLASH_PAGE_SIZE];

static struct spiflash_transaction flash_trans;
static size_t flash_total_size = 0;

uint32_t flash_id;

static enum flash_state flash_state = FLASH_INIT;
static size_t flash_addr;

static size_t flash_flush_count;
static flush_cb_t flush_cb;
static void *flush_cbdata;


static void flash_state_machine(void);


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

void
flash_flush_data(size_t writelen, flush_cb_t flush_cb_, void *cbdata)
{
        if (flash_state != FLASH_READY) {
                flash_flush_count = writelen;
                flush_cb = flush_cb_;
                flush_cbdata = cbdata;
                return;
        }

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
flash_got_id(void *cbdata, uint8_t manuf, uint8_t memtype, uint8_t capa)
{
        flash_id = (manuf << 16) | (memtype << 8) | capa;

        flash_total_size = 1 << capa;
        flash_state_machine();
}

static void
flash_state_machine(void)
{
        switch (flash_state) {
        case FLASH_INIT:
                spiflash_get_id(&onboard_flash, &flash_trans,
                                flash_got_id, NULL);
                flash_state = FLASH_FIND_EMPTY_SECTOR;
                break;
        case FLASH_FIND_EMPTY_SECTOR:
                spiflash_read_page(&onboard_flash, &flash_trans,
                                   flash_page, flash_addr, sizeof(flash_page),
                                   flash_find_empty_sector, NULL);
                break;
        case FLASH_READY:
                break;
        case FLASH_PAGE_FILLED:
                spiflash_program_page(&onboard_flash, &flash_trans,
                                      flash_addr, flash_page, flash_flush_count,
                                      flush_flash_done, NULL);
                flash_state = FLASH_WRITING;
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
                flash_flush_count = 0;
                if (flush_cb) {
                        flush_cb(flush_cbdata);
                        flush_cb = 0;
                }
                break;
        case FLASH_ERASING:
                flash_state = FLASH_READY;
                break;
        case FLASH_FULL:
                break;
        }
}

size_t
flash_free(void)
{
        return (flash_total_size - flash_addr);
}

void
flash_init(void)
{
        flash_state_machine();
}


static void
erase_all_done(void *cbdata)
{
        flash_state = FLASH_INIT;
        flash_addr = 0;
        flash_state_machine();
}

void
flash_eraseall(void)
{
        spiflash_erase_device(&onboard_flash, &flash_trans, erase_all_done, NULL);
        flash_state = FLASH_ERASING;
}

bool
flash_initializing_p(void)
{
        return (flash_state < FLASH_READY);
}

bool
flash_full_p(void)
{
        return (flash_state == FLASH_FULL);
}
