struct sys_register_file {
        uint8_t data[32];
};

#define sys_register_file (((struct sys_register_file *)0x40041000u)->data)

__attribute__((__noreturn__)) void sys_reset_to_loader(void);
