SOC+=	kinetis/MK20D5
TARGET=	MK20DX32VLF5

DFUVID?=	2323
DFUPID?=	0001

SRCS-board=	board.c
SRCS-board.dir= board/mchck
SRCS.libs+=	board
