#include <mchck.h>

void
sys_reset_to_loader(void)
{
        memcpy(sys_register_file, sys_reset_to_loader_magic, sizeof(sys_reset_to_loader_magic));
        sys_reset();
}
