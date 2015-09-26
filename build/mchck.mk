_libdir:=       $(abspath $(dir $(lastword ${MAKEFILE_LIST}))/..)

-include .mchckrc
-include ${_libdir}/.mchckrc

is-make-clean=	$(filter clean realclean,${MAKECMDGOALS})

define srcpath
$(abspath $(filter /%,$(1)) $(addprefix ${_libdir}/$(2)/,$(filter-out /%,$(1))))
endef

define _include_libs
_libdir-$(1):=	$(call srcpath,${SRCS-$(1).dir},src)
_forceobjs-$(1)=	$$(addsuffix .o, $$(basename $$(addprefix $(1)-lib-,$${SRCS.force-$(1)})))
FORCEOBJS+=	$${_forceobjs-$(1)}
_objs-$(1)=	$$(addsuffix .o, $$(basename $$(addprefix $(1)-lib-,$${SRCS-$(1)}))) $${_forceobjs-$(1)}
_allobjs+=	$${_objs-$(1)}
_libobjs+=	$${_objs-$(1)}
CLEANFILES+=	$${_objs-$(1)}
_deps-$(1)=	$$(addsuffix .d, $$(basename $$(addprefix $(1)-lib-,$${SRCS-$(1)} $${SRCS.force-$(1)})))
DEPS+=	$${_deps-$(1)}
#INCLUDEDIRS+=	$${_libdir-$(1)}

$${LIBDEPCACHE}/$(1)-lib-%.o: $${_libdir-$(1)}/%.c
	$$(COMPILE.c) $$(OUTPUT_OPTION) $$<
$(1)-lib-%.o: $${_libdir-$(1)}/%.c
	$$(COMPILE.c) $$(OUTPUT_OPTION) $$<
$(1)-lib-%.d: $${_libdir-$(1)}/%.c
	$$(GENERATE.d)
endef

GENERATE.d=	$(CC) -MM ${CPPFLAGS} -MT $@ -MT ${@:.d=.o} -MP -MF $@ $<


# Common config

INCLUDEDIRS+=	include . $(call srcpath,${BOARD},src/board) $(foreach _soc,${SOC},$(call srcpath,${_soc},src/soc))
CPPFLAGS+=	-std=gnu11
CFLAGS+=	-fplan9-extensions
CFLAGS+=	-ffreestanding
CFLAGS+=	-ggdb3 -ffunction-sections -fdata-sections
ifndef DEBUG
CFLAGS+=	${COPTFLAGS}
else
NO_LTO=		no-lto
endif
CFLAGS+=	${CWARNFLAGS}

ifeq (${SRCS},)
SRCS?=	${PROG}.c
_no_src_given:=	true
endif

_compilesrc=	$(filter %.c,${SRCS})
_objs=	$(addsuffix .o, $(basename ${_compilesrc} ${_gensrc}))
CLEANFILES+=	${_objs}
OBJS+=	${_objs}
_allobjs+=	${OBJS}
DEPS+=	$(addsuffix .d, $(basename ${_compilesrc} ${_gensrc}))
CLEANFILES+=	${DEPS}


# This should be in linkdep.mk, but it is needed before, in the
# expansion of _include_libs.
LIBDEPCACHE=	${_libdir}/cache


# Host config (VUSB)

ifeq (${TARGET},host)
CPPFLAGS+=	-DTARGET_HOST
CFLAGS+=	-fshort-enums

all: ${PROG}

include ${_libdir}/src/Makefile.part
$(foreach _uselib,${SRCS.libs},$(eval $(call _include_libs,$(_uselib))))

${PROG}: ${_allobjs}
	$(LINK.c) -Wl,--start-group $^ -Wl,--end-group ${LDLIBS} -o $@

CLEANFILES+=	${PROG}
else

# MCHCK config

CC=	arm-none-eabi-gcc
LD=	arm-none-eabi-ld
AR=	arm-none-eabi-ar
AS=	arm-none-eabi-as
OBJCOPY=	arm-none-eabi-objcopy
SIZE=	arm-none-eabi-size
GDB=	arm-none-eabi-gdb
DFUUTIL?=	dfu-util
RUBY?=	ruby

ifeq ($(shell which $(CC) 2>/dev/null),)
SATDIR?=	$(HOME)/sat
endif
ifdef SATDIR
PATH:=	${SATDIR}/bin:${PATH}
export PATH
endif

LINKER_COMPILER_PATH=	${_libdir}/build/scripts

BOARD?=	mchck

define includesoc1
ifeq ($$(filter ${1},$${_socdone}),)
_socdone:= $${_socdone} ${1}
include $$(call srcpath,${1}/build.mk,src/soc)
$$(call includesoc)
endif
endef

define includesoc
$(foreach _soc,${SOC},$(eval $(call includesoc1,${_soc})))
endef

include $(call srcpath,${BOARD}/build.mk,src/board)
$(call includesoc)

ifndef MAKECMDGOALS
.DEFAULT_GOAL:=
endif

COPTFLAGS?=	-Os
CWARNFLAGS?=	-Wall -Wno-main -Wshadow -fno-common

CFLAGS+=	-fstrict-volatile-bitfields
ifndef NO_LTO
CFLAGS+=	-flto
endif

LDFLAGS.first+= ${LDFLAGS}
LDFLAGS.first+=	-fwhole-program -specs nano.specs -nostartfiles
LDFLAGS.first+= -Wl,-Map=${PROG}.lto.map

LDFLAGS.final+=	-nostartfiles -nostdlib
LDFLAGS.final+=	-Wl,-Map=${PROG}.map


CLEANFILES+=	${PROG}.hex ${PROG}.elf ${PROG}.bin ${PROG}.map ${PROG}.lto.map

all: ${PROG}.bin ${PROG}.hex

# This has to go before the rule, because for some reason the updates to OBJS
# are not incorporated into the target dependencies
include ${_libdir}/src/Makefile.part
$(foreach _uselib,${SRCS.libs},$(eval $(call _include_libs,$(_uselib))))


CPPFLAGS+=	$(addprefix -I,$(call srcpath,${INCLUDEDIRS},src))


# linkdep defines LINKOBJS
include ${_libdir}/build/mk/linkdep.mk

${PROG}.elf: ${PROG}.lto.o ${PROG}.ld
	${CC} -o $@ ${CFLAGS} ${LDFLAGS.final} -T ${PROG}.ld $<

${PROG}.lto.o: ${LINKOBJS} ${LDLIBS}
	${CC} -o $@ -Wl,-i ${CFLAGS} ${LDFLAGS.first} -Wl,--start-group ${LINKOBJS} ${LDLIBS} -Wl,--end-group

ifndef HAVE_LDSCRIPT_GENERATOR
${PROG}.ld: ${LDSCRIPTS}
	rm -f $@; cat </dev/null $+ > $@
endif
CLEANFILES+=	${PROG}.ld ${PROG}.lto.o


ifneq ('${BINSIZE}','')
define check-size
	@${SIZE} $< | awk 'END { \
		used_flash=$$1; \
		used_ram=$$2+$$3; \
		binsize=${BINSIZE}; \
		ramsize=${RAM_SIZE}; \
		printf "%d bytes of FLASH available\n", (binsize - used_flash); \
		printf "%d bytes of RAM available (static allocations only)\n", (ramsize - used_ram); \
		if (used_flash > binsize || used_ram > ramsize) { \
			exit 1; \
		} \
	}'
endef
endif

%.bin: %.elf
	$(check-size)
	${OBJCOPY} -O binary $< $@

%.hex: %.elf
	$(check-size)
	${OBJCOPY} -O ihex $< $@

gdb: check-programmer ${PROG}.elf
	${RUBY} ${_libdir}/programmer/gdbserver.rb ${MCHCKADAPTER} -- ${GDB} -readnow -ex 'target extended-remote :1234' ${PROG}.elf

flash: ${PROG}.bin
	${DFUUTIL} -d ${DFUVID}:${DFUPID} -D ${PROG}.bin

swd-flash: check-programmer ${PROG}.bin
	${RUBY} ${_libdir}/programmer/flash.rb ${MCHCKADAPTER} ${PROG}.bin ${LOADADDR}

check-programmer:
	cd ${_libdir} && git submodule update --init programmer
endif

# from the make info manual
%.d: %.c
	$(GENERATE.d)

ifeq ($(call is-make-clean),)
-include $(patsubst %.o,%.d,${LINKOBJS})
endif

${_objs} $(patsubst %.o,%.d,${_objs}): ${_gensrc}

clean:
	-rm -f ${CLEANFILES}

realclean:
	-rm -f ${REALCLEANFILES} ${CLEANFILES}
