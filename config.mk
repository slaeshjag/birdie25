# Project: birdie25
# Makefile configurations
MAKEFLAGS	+=	--no-print-directory

#Binaries
BINFILE		=	birdie25

#General flags
LDFLAGS		+=	-ldarnit
CFLAGS		+=	-Wall -std=c99 -g -O0 -D_XOPEN_SOURCE=600
ASFLAGS		+=

#Extra install targets
INSTARG		=	

#Makefile tools
MKDIR		=	mkdir -p
RMDIR		=	rmdir --ignore-fail-on-non-empty
