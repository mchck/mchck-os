SOC+=	kinetis/MK24F12
TARGET=	MK24FN1M0VDC12

DFUVID?=	15a2
DFUPID?=	1000

SRCS-board=	board.c
SRCS-board.dir= board/wunderbar
SRCS.libs+=	board
