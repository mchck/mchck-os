#include <mchck.h>

uint32_t flash_ALLOW_BRICKABLE_ADDRESSES;

/* This will have to live in SRAM. */
__attribute__((section(".ramtext.ftfa_submit_cmd"), long_call))
int
ftfa_submit_cmd(void)
{
        /* XXX disable interrupts? */
        FTFA.fstat.raw = ((struct FTFL_FSTAT_t){
                        .ccif = 1,
                                .rdcolerr = 1,
                                .accerr = 1,
                                .fpviol = 1
                                }).raw;
        struct FTFL_FSTAT_t stat;
        while (!(stat = FTFA.fstat).ccif)
                /* NOTHING */; /* XXX maybe WFI? */
        stat.ccif = 0;
        return (stat.raw != 0);
}

int
flash_prepare_flashing(void)
{
        return (0);
}

int
flash_erase_sector(uintptr_t addr)
{
        if (addr < (uintptr_t)&_app_rom &&
                flash_ALLOW_BRICKABLE_ADDRESSES != 0x00023420)
                return (-1);
        FTFA.fccob.erase.fcmd = FTFL_FCMD_ERASE_SECTOR;
        FTFA.fccob.erase.addr = addr;
        return (ftfa_submit_cmd());
}

int
flash_program_section(uintptr_t addr, size_t len)
{
        int r;
        uint32_t *buf = addr;

        while (len > 0) {
                FTFA.fccob.program_longword.fcmd = FTFL_FCMD_PROGRAM_LONGWORD;
                FTFA.fccob.program_longword.addr = addr;
                FTFA.fccob.program_longword.data_be = *buf;
                r = ftfa_submit_cmd();
                if (r != 0)
                        break;
                buf++;
                len -= sizeof(*buf);
        }
        return r;
}
