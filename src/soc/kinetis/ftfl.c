#include <mchck.h>

uint32_t flash_ALLOW_BRICKABLE_ADDRESSES;

/* This will have to live in SRAM. */
__attribute__((section(".ramtext.ftfl_submit_cmd"), long_call))
int
ftfl_submit_cmd(void)
{
        FTFL.fstat.raw = ((struct FTFL_FSTAT_t){
                        .ccif = 1,
                                .rdcolerr = 1,
                                .accerr = 1,
                                .fpviol = 1
                                }).raw;
        struct FTFL_FSTAT_t stat;
        while (!(stat = FTFL.fstat).ccif)
                /* NOTHING */; /* XXX maybe WFI? */
        stat.ccif = 0;
        return (stat.raw != 0);
}

int
flash_prepare_flashing(void)
{
        /* switch to FlexRAM */
        if (!FTFL.fcnfg.ramrdy) {
                FTFL.fccob.set_flexram.fcmd = FTFL_FCMD_SET_FLEXRAM;
                FTFL.fccob.set_flexram.flexram_function = FTFL_FLEXRAM_RAM;
                return (ftfl_submit_cmd());
        }
        return (0);
}

int
flash_erase_sector(uintptr_t addr)
{
        if (addr < (uintptr_t)&_app_rom &&
                flash_ALLOW_BRICKABLE_ADDRESSES != 0x00023420)
                return (-1);
        FTFL.fccob.erase.fcmd = FTFL_FCMD_ERASE_SECTOR;
        FTFL.fccob.erase.addr = addr;
        return (ftfl_submit_cmd());
}

int
flash_program_section(uintptr_t addr, size_t len)
{
        FTFL.fccob.program_section.fcmd = FTFL_FCMD_PROGRAM_SECTION;
        FTFL.fccob.program_section.addr = addr;
        FTFL.fccob.program_section.num_elems = len / FLASH_ELEM_SIZE;
        return (ftfl_submit_cmd());
}
