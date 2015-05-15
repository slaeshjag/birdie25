# Project: birdie25
# Makefile configurations
MAKEFLAGS	+=	--no-print-directory

#Binaries
CLIENT		=	birdie25
SERVER		=	birdie25-server

#General flags
LDFLAGS		+=	-ldarnit_s -lm -pthread -lSDL -lbz2 -lasound -lGL -lmodplug
CFLAGS		+=	-Wall -std=c99 -g -O0 -D_XOPEN_SOURCE=600 -I$(TOPDIR)/common -pthread
ASFLAGS		+=

#Extra install targets
INSTARG		=	

#Makefile tools
MKDIR		=	mkdir -p
RMDIR		=	rmdir --ignore-fail-on-non-empty
