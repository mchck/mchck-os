#include <mchck.h>

void
watchdog_disable(void)
{
        bf_set(SIM_COPC, SIM_COPC_COPT, 0);
}
