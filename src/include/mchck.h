#ifndef __MCHCK_H
#define __MCHCK_H

#include <sys/types.h>
#include <sys/cdefs.h>
#include <stdint.h>
#include <stdfix.h>
#include <stdarg.h>

#include <mchck-cdefs.h>

#ifdef TARGET_HOST

#include <host/host.h>

#else

#ifndef __MCHCK_INTERNAL_H
#error Build system error: mchck_internal.h not included by compiler
#endif

#ifdef __cplusplus
 extern "C" {
#if 0                           /* to make emacs indent properly */
 }
#endif
#endif

#include <board.h>
#include <soc.h>

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _app_rom;

#include <intnums.h>

#ifdef __cplusplus
}
#endif

#endif  /* !host */

#ifdef __cplusplus
extern "C" {
#if 0                           /* to make emacs indent properly */
}
#endif
#endif

#include <common/common.h>

#include <usb/usb.h>

#include <nrf/nrf.h>

#ifdef __cplusplus
}
#endif

#endif
