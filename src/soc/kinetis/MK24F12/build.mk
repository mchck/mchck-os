SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb

TARGETLD= MK24F12.ld
CPPFLAGS.ld+= -I$(call srcpath,src/soc/kinetis/MK24F12) #XXX thispath

RAM_SIZE=256*1024
FLASH_SIZE=1024*1024

FIXED_SECTIONS+=	-s 0:.isr_vector
FIXED_SECTIONS+=	-s 0x400:.flash_config

ifneq ($(filter noboot,${TARGET_MODE}),)
LOADER=		yes
LOADER_SIZE=	${FLASH_SIZE}
APP_ADDR=	0
else
LOADER_SIZE=	64*1024
APP_ADDR=	96*1024
endif

LOADER_ADDR=	0
APP_SIZE=	${FLASH_SIZE}-${APP_ADDR}

SRCS-kinetis=	adc.c crc.c flashconfig.c ftfe.c ftm.c gpio.c i2c.c pin.c pin_change.c pit.c rtc.c dspi.c stdio.c timeout.c uart.c uart-fifo.c usb.c wdog.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MK24F12=	startup.c
SRCS-kinetis-MK24F12.dir=	soc/kinetis/MK24F12
SRCS.libs+=	kinetis-MK24F12

ifdef LOADER
SRCS.force-kinetis-MK24F12+=	flashconfig.c
endif
