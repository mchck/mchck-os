#include <mchck.h>

void
watchdog_disable(void)
{
        bf_set_reg(SIM_COPC, SIM_COPC_COPT, 0);
}
