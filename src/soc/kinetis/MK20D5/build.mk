SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m4 -msoft-float -mthumb

TARGETLD= MK20D5.ld
CPPFLAGS.ld+= -I$(call srcpath,src/soc/kinetis/MK20D5) #XXX thispath

ifneq ($(findstring 32,${TARGET}),)
RAM_SIZE=8192
FLASH_SIZE=32768
else
ifneq ($(findstring 128,${TARGET},)
RAM_SIZE=16384
FLASH_SIZE=131072
endif
endif

FIXED_SECTIONS+=	-s 0:.isr_vector
FIXED_SECTIONS+=	-s 0x400:.flash_config

ifneq ($(filter noboot,${TARGET_MODE}),)
LOADER=		yes
LOADER_SIZE=	${FLASH_SIZE}
else
LOADER_SIZE=	3072
endif

LOADER_ADDR=	0
APP_SIZE=	${FLASH_SIZE}-${LOADER_SIZE}
APP_ADDR=	${LOADER_SIZE}

SRCS-kinetis=	adc.c crc.c flashconfig.c ftfl.c ftm.c gpio.c i2c.c pin.c pin_change.c pit.c rtc.c spi.c stdio.c timeout.c uart.c uart-fifo.c usb.c wdog.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MK20D5=	startup.c
SRCS-kinetis-MK20D5.dir=	soc/kinetis/MK20D5
SRCS.libs+=	kinetis-MK20D5

ifdef LOADER
SRCS.force-kinetis-MK20D5+=	flashconfig.c
endif
