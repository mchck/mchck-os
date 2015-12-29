SRCS-cortex-m.dir=	soc/cortex-m
SRCS.libs+=	cortex-m
SRCS-cortex-m=	system.c

ifneq ($(filter m3 m4,${TARGET.cortex-m}),)
SRCS-cortex-m+=	sched-m3.c
endif

ifneq ($(filter m0,${TARGET.cortex-m}),)
SRCS-cortex-m+=	sched-m0.c
endif
