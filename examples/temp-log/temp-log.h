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
extern uint32_t flash_id;

extern unsigned long templog_interval;

void flash_flush_data(size_t writelen, flush_cb_t flush_cb_, void *cbdata);
void templog_start(void);
void templog_stop(void);
bool templog_stopped(void);
int16_t templog_last_temp(void);

typedef void (flash_read_cb)(const void *, size_t);
size_t flash_total(void);
size_t flash_used(void);
void flash_init(void);
void flash_eraseall(void);
bool flash_initializing_p(void);
bool flash_full_p(void);
bool flash_ready_p(void);
int flash_read_page(uint32_t addr, flash_read_cb *cb);
