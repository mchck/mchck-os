SOC+=	cortex-m

LDSCRIPTS.first+= $(call srcpath,src/soc/kinetis/kinetis.first.ld)

ifdef LOADER
LDSCRIPTS:=	${LDSCRIPTS} ${_libdir}/build/ld/loader.ld
BINSIZE=	${LOADER_SIZE}
LOADADDR=	${LOADER_ADDR}
else
LDSCRIPTS:=	${LDSCRIPTS} ${_libdir}/build/ld/app.ld
BINSIZE=	${APP_SIZE}
LOADADDR=	${APP_ADDR}
endif

LDSCRIPTS+= $(call srcpath,src/soc/kinetis/kinetis.ld)

ifdef LOADER
HAVE_LDSCRIPT_GENERATOR= yes
${PROG}.ld: ${LDSCRIPTS} ${PROG}.lto.o
	${_libdir}/src/soc/kinetis/scripts/pack-linker-script -o $@ $(addprefix -T ,${LDSCRIPTS}) ${PROG}.lto.o
endif
