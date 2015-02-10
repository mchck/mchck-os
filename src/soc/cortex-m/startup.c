/**
 * Freescale K20 ISR table and startup code.
 */

#include <stdint.h>
#include <mchck-cdefs.h>

#ifndef STACK_SIZE
#define STACK_SIZE 0x400
#endif

__attribute__ ((__section__(".co_stack")))
__attribute__ ((__used__))
static uint32_t sys_stack[STACK_SIZE / 4];

/**
 * What follows is some macro magic to populate the
 * ISR vector table and to declare weak symbols for the handlers.
 *
 * We start by defining the macros VH and V, which by themselves
 * just call the (yet undefined) macro V_handler().
 *
 * V_handler will then be defined separately for each use of the
 * vector list.
 *
 * V_reserved is just used to properly skip the reserved entries
 * in the vector table.
 */

typedef void (isr_handler_t)(void);

isr_handler_t Default_Handler __attribute__((__weak__, __alias__("__Default_Handler")));
isr_handler_t Default_Reset_Handler;


#define VH(num, handler, default)                               \
	V_handler(num, handler, _CONCAT(handler, _Handler), default)
#define V(num, x)                               \
	VH(num, x, Default_Handler)

/**
 * Declare the weak symbols.  By default they will be aliased
 * to Default_Handler, but the default handler can be specified
 * by using VH() instead of V().
 */

#define V_handler(num, name, handler, default)                          \
	isr_handler_t handler __attribute__((__weak__, __alias__(#default)));	\
	isr_handler_t _CONCAT(name, _IRQHandler) __attribute__((__weak__, __alias__(_STR(handler))));
#include "vecs.h"
#undef V_handler

/**
 * Define the vector table.  We simply fill in all (weak) vector symbols
 * and the occasional `0' for the reserved entries.
 */

__attribute__ ((__section__(".isr_vector"), __used__))
isr_handler_t * const isr_vectors[] =
{
	(isr_handler_t *)&sys_stack[sizeof(sys_stack)/sizeof(*sys_stack)],
#define V_handler(num, name, handler, default)	[num] = handler,
#include "vecs.h"
#undef V_handler
};

#undef V
#undef VH


static void
__Default_Handler(void)
{
	for (;;)
		/* NOTHING */;
}
