MEMORY
{
        /**
         * Our two-stage optimizing linker has trouble when the first link stage
         * runs out of space.  We artificially keep this size too large for now.
         * The overflow will be caught later.
         *
         * Do the same for memory to make size debugging easier.
         */
        loader_rom (rx) : ORIGIN = 0, LENGTH = 64*1024
        app_rom (rx) : ORIGIN = 96*1024, LENGTH = 1M-96*1024
        ram (rwx) : ORIGIN = 0x20000000 - 64K, LENGTH = 256*1024
}

_app_rom = ORIGIN(app_rom);
_eram = ORIGIN(ram) + LENGTH(ram);
