#include <mchck.h>

enum semihosting_op {
        SEMIHOSTING_WRITEC = 3,
        SEMIHOSTING_WRITE0 = 4,
        SEMIHOSTING_WRITE = 5,
        SEMIHOSTING_EXIT = 0x18,
};


static int
semihosting_op(enum semihosting_op op, uintptr_t args)
{
        register uint32_t op_ asm ("r0") = op;
        register uintptr_t args_ asm ("r1") = args;
        register int ret asm ("r0");

        __asm__ volatile ("bkpt 0xab" : "=r" (ret) : "0" (op_), "r" (args_) : "memory");
        return (ret);
}

static size_t
semihosting_write(const uint8_t *buf, size_t len, void *data)
{
        struct args {
                uint32_t fd;
                const void *buf;
                size_t len;
        } args = {2, buf, len};

        semihosting_op(SEMIHOSTING_WRITE, (uintptr_t)&args);
        return (len);
}

void __attribute__((noreturn))
semihosting_exit(int fail)
{
        semihosting_op(SEMIHOSTING_EXIT, fail ? -1 : 0x20026);
        __builtin_unreachable();
}

static struct _stdio_file_ops semihosting_ops = {
        .write = semihosting_write
};

extern struct _stdio_file_ops _stdout_default_ops __attribute__((alias("semihosting_ops")));
