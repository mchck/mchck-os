#include <mchck.h>

void
watchdog_disable(void)
{
	WDOG_UNLOCK = 0xc520;
	WDOG_UNLOCK = 0xd928;
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}
