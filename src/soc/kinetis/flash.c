#include <mchck.h>

int
flash_program_sector(const char *buf, uintptr_t addr, size_t len)
{
        int ret = 0;

        ret = ret || (len != FLASH_SECTOR_SIZE);
        ret = ret || ((addr & (FLASH_SECTOR_SIZE - 1)) != 0);
        ret = ret || flash_erase_sector(addr);

        for (int i = FLASH_SECTOR_SIZE / FLASH_SECTION_SIZE; i > 0; --i) {
                memcpy(flash_get_staging_area(addr, FLASH_SECTION_SIZE),
                       buf,
                       FLASH_SECTION_SIZE);
                ret = ret || flash_program_section(addr, FLASH_SECTION_SIZE);
                buf += FLASH_SECTION_SIZE;
                addr += FLASH_SECTION_SIZE;
        }

        return (ret);
}

void *
flash_get_staging_area(uintptr_t addr, size_t len)
{
        if ((addr & (FLASH_SECTION_SIZE - 1)) != 0 ||
            len != FLASH_SECTION_SIZE)
                return (NULL);
        return (FlexRAM);
}
