MEMORY
{
        /**
         * Our two-stage optimizing linker has trouble when the first link stage
         * runs out of space.  We artificially keep this size too large for now.
         * The overflow will be caught later.
         *
         * Do the same for memory to make size debugging easier.
         */
        loader_rom (rx) : ORIGIN = 0, LENGTH = 1M
        app_rom (rx) : ORIGIN = 4*1024, LENGTH = 1M
        ram (rwx) : ORIGIN = 0x1ffff000, LENGTH = 1M
}

_app_rom = ORIGIN(app_rom);
_eram = ORIGIN(ram) + LENGTH(ram);
