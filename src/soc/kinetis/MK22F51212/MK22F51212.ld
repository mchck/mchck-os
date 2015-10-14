MEMORY
{
        /**
         * Our two-stage optimizing linker has trouble when the first link stage
         * runs out of space.  We artificially keep this size too large for now.
         * The overflow will be caught later.
         *
         * Do the same for memory to make size debugging easier.
         */
        loader_rom (rx) : ORIGIN = 0, LENGTH = 32K
        app_rom (rx) : ORIGIN = 64K, LENGTH = 512K-64K
        ram (rwx) : ORIGIN = 0x20000000 - 128K / 2, LENGTH = 128K
}

_app_rom = ORIGIN(app_rom);
_eram = ORIGIN(ram) + LENGTH(ram);
