#include <mchck.h>

uint32_t flash_ALLOW_BRICKABLE_ADDRESSES;

/* This will have to live in SRAM. */
__attribute__((section(".ramtext.ftfl_submit_cmd"), long_call))
static int
ftfl_submit_cmd(void)
{
        FTFL_FSTAT =
                FTFL_FSTAT_CCIF_MASK |
                FTFL_FSTAT_RDCOLERR_MASK |
                FTFL_FSTAT_ACCERR_MASK |
                FTFL_FSTAT_FPVIOL_MASK;

        uint8_t stat;
        while (!((stat = FTFL_FSTAT) & FTFL_FSTAT_CCIF_MASK))
                /* NOTHING */; /* XXX maybe WFI? */
        stat &= ~FTFL_FSTAT_CCIF_MASK;
        return (stat != 0);
}

int
flash_prepare_flashing(void)
{
        /* switch to FlexRAM_BASE_PTR */
        if (!bf_get(FTFL_FCNFG, FTFL_FCNFG_RAMRDY)) {
                FTFL_FCCOB0 = FTFL_FCMD_SET_FLEXRAM;
                FTFL_FCCOB1 = FTFL_FLEXRAM_RAM;
                return (ftfl_submit_cmd());
        }
        return (0);
}

static int
flash_erase_sector(uintptr_t addr)
{
        if (addr < (uintptr_t)&_app_rom &&
                flash_ALLOW_BRICKABLE_ADDRESSES != 0x00023420)
                return (-1);
        *(volatile uint32_t *)&FTFL_FCCOB3 = addr; /* this overwrites B0 */
        FTFL_FCCOB0 = FTFL_FCMD_ERASE_SECTOR;
        return (ftfl_submit_cmd());
}

static int
flash_program_section(uintptr_t addr, size_t len)
{
        *(volatile uint32_t *)&FTFL_FCCOB3 = addr; /* this overwrites B0 */
        FTFL_FCCOB0 = FTFL_FCMD_PROGRAM_SECTION;
        *(volatile uint16_t *)&FTFL_FCCOB5 = len / FLASH_ELEM_SIZE;
        return (ftfl_submit_cmd());
}

static void *
flash_get_staging_area(uintptr_t addr, size_t len)
{
        if ((addr & (FLASH_SECTION_SIZE - 1)) != 0 ||
            len != FLASH_SECTION_SIZE)
                return (NULL);
        return (FlexRAM_BASE_PTR);
}


int
flash_program_sector(const uint8_t *buf, uintptr_t addr, size_t len)
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
