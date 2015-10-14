SOC+=	kinetis/MK22F51212
TARGET=	MK22FN512VLH12

DFUVID?=	2323
DFUPID?=	0001

SRCS-board=	board.c
SRCS-board.dir= board/current-monitor
SRCS.libs+=	board
