# Project: birdie25
# Makefile configurations
MAKEFLAGS	+=	--no-print-directory

#Binaries
BINFILE		=	birdie25

#General flags
LDFLAGS		+=	-ldarnit -lm
CFLAGS		+=	-Wall -std=c99 -g -O0 -D_XOPEN_SOURCE=600 -fopenmp
ASFLAGS		+=

#Extra install targets
INSTARG		=	

#Makefile tools
MKDIR		=	mkdir -p
RMDIR		=	rmdir --ignore-fail-on-non-empty
