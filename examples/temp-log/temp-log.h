enum {
        SPIFLASH_PAGE_SIZE = 256,
        SPIFLASH_SECTOR_SIZE = 4096,
};

struct templog_entry {
        uint32_t time;
        uint32_t temp;
};


typedef void (*flush_cb_t)(void *cbdata);

extern struct ow_ctx ow_ctx;
extern struct ds18b20_ctx ds;

extern uint8_t flash_page[SPIFLASH_PAGE_SIZE];

extern unsigned long templog_interval;

void flash_flush_data(size_t writelen, flush_cb_t flush_cb_, void *cbdata);
void templog_start(void);
void templog_stop(void);
bool templog_stopped(void);
size_t templog_free(void);
int16_t templog_last_temp(void);

size_t flash_free(void);
void flash_init(void);
void flash_eraseall(void);
bool flash_initializing_p(void);
bool flash_full_p(void);
