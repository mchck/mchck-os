#include <mchck.h>

void
pin_change_handler(PORT_MemMapPtr port,
                   const struct pin_change_handler *handlers,
                   const struct pin_change_handler *end)
{
        const struct pin_change_handler *i;
        for (i = handlers; i < end; i++) {
                uint32_t bit = 1 << pin_physpin_from_pin(i->pin_id);
                if (PORT_ISFR_REG(port) & bit) {
                        PORT_ISFR_REG(port) = bit;
                        i->cb(i->cbdata);
                }
        }
}

#define PORT_CHANGE_HANDLER(port) \
        extern const struct pin_change_handler pin_hooks_##port, pin_hooks_##port##_end; \
        \
        void \
        PORT##port##_Handler(void) \
        { \
                pin_change_handler(PORT##port##_BASE_PTR, &pin_hooks_##port, \
                                   &pin_hooks_##port##_end);       \
        }

PORT_CHANGE_HANDLER(A);
PORT_CHANGE_HANDLER(B);
PORT_CHANGE_HANDLER(C);
PORT_CHANGE_HANDLER(D);

#define pin_change_init_port(port)                                      \
        for (const struct pin_change_handler *i = &pin_hooks_##port;    \
             i < &pin_hooks_##port##_end; i++) {                        \
                bf_set(SIM_SCGC5, SIM_SCGC5_PORT##port, 1);             \
                PORT_MemMapPtr p = PORT##port##_BASE_PTR;               \
                bf_set(PORT_PCR_REG(p, pin_physpin_from_pin(i->pin_id)), PORT_PCR_IRQC, i->polarity); \
                int_enable(IRQ_PORT##port);                             \
        }

void
pin_change_init(void)
{
        pin_change_init_port(A);
        pin_change_init_port(B);
        pin_change_init_port(C);
        pin_change_init_port(D);
}
