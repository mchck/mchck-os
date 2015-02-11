MEMORY
{
        /**
         * Our two-stage optimizing linker has trouble when the first link stage
         * runs out of space.  We artificially keep this size too large for now.
         * The overflow will be caught later.
         *
         * Do the same for memory to make size debugging easier.
         */
        loader_rom (rx) : ORIGIN = LOADER_ADDR, LENGTH = 1M
        app_rom (rx) : ORIGIN = APP_ADDR, LENGTH = 1M
        ram (rwx) : ORIGIN = 0x20000000 - 64K, LENGTH = 1M
}

#define TARGET_SECTIONS_LDSCRIPT "MK24F12-sections.ld"

/* common peripheral / misc data for K20 series MCUs */

_app_rom = ORIGIN(app_rom);
_eram = ORIGIN(ram) + LENGTH(ram);

/* PACK SECTIONS FIX .isr_vector = 0 */
/* PACK SECTIONS FIX .flash_config = 0x400 */
/* PACK SECTIONS IGNORE .pin_hooks.* */
