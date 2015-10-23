CFLAGS+= -Werror
export CFLAGS

SUBDIRS= bootloader examples

include build/mk/subdir.mk
