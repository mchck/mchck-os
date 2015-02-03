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

#define FOREACH_PIN_CHANGE(port) \
        for (const struct pin_change_handler *i = &pin_hooks_##port; i < &pin_hooks_##port##_end; i++)

#define PIN_CHANGE_ENABLE(port) \
        (&pin_hooks_##port != &pin_hooks_##port##_end)

#define PIN_CHANGE_SET_IRQC(port)                                         \
        FOREACH_PIN_CHANGE(port) {                                        \
                PORT_MemMapPtr p = PORTA_BASE_PTR;                      \
                bf_set(PORT_PCR_REG(p, pin_physpin_from_pin(i->pin_id)), PORT_PCR_IRQC, i->polarity); \
        }
