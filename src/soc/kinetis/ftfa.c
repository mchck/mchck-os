#include <mchck.h>

uint32_t flash_ALLOW_BRICKABLE_ADDRESSES;

/* This will have to live in SRAM. */
__attribute__((section(".ramtext.ftfa_submit_cmd"), long_call))
static int
ftfa_submit_cmd(void)
{
        /* XXX disable interrupts? */
        FTFA_FSTAT =
                FTFA_FSTAT_CCIF_MASK |
                FTFA_FSTAT_RDCOLERR_MASK |
                FTFA_FSTAT_ACCERR_MASK |
                FTFA_FSTAT_FPVIOL_MASK;

        uint8_t stat;
        while (!((stat = FTFA_FSTAT) & FTFA_FSTAT_CCIF_MASK))
                /* NOTHING */; /* XXX maybe WFI? */
        stat &= ~FTFA_FSTAT_CCIF_MASK;
        return (stat != 0);
}

int
flash_prepare_flashing(void)
{
        return (0);
}

static int
flash_erase_sector(uintptr_t addr)
{
        if (addr < (uintptr_t)&_app_rom &&
            flash_ALLOW_BRICKABLE_ADDRESSES != 0x00023420)
                return (-1);
        FTFA_FCCOB0 = FTFA_FCMD_ERASE_SECTOR;
        FTFA_FCCOB1 = addr >> 16;
        FTFA_FCCOB2 = addr >> 8;
        FTFA_FCCOB3 = addr;
        return (ftfa_submit_cmd());
}

static int
flash_program_section(const uint8_t *buf, uintptr_t addr, size_t len)
{
        int r;

        while (len > 0) {
                FTFA_FCCOB0 = FTFA_FCMD_PROGRAM_LONGWORD;
                FTFA_FCCOB1 = addr >> 16;
                FTFA_FCCOB2 = addr >> 8;
                FTFA_FCCOB3 = addr;
                FTFA_FCCOB4 = buf[3];
                FTFA_FCCOB5 = buf[2];
                FTFA_FCCOB6 = buf[1];
                FTFA_FCCOB7 = buf[0];
                r = ftfa_submit_cmd();
                if (r != 0)
                        break;
                buf += 4;
                addr += 4;
                len -= 4;
        }
        return r;
}


int
flash_program_sector(const uint8_t *buf, uintptr_t addr, size_t len)
{
        int ret = 0;

        ret = ret || (len != FLASH_SECTOR_SIZE);
        ret = ret || ((addr & (FLASH_SECTOR_SIZE - 1)) != 0);
        ret = ret || flash_erase_sector(addr);
        ret = ret || flash_program_section(buf, addr, len);
        return (ret);
}
