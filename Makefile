# Project: 68k-emu
TOPDIR	=	.
include config.mk

# Sub directories to build
SUBDIRS		=	common server client

#LIBS		=	$(addsuffix /out.a,$(SUBDIRS))

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@echo " [ LD ] bin/$(SERVER)"
	@$(CC) -o bin/$(SERVER) $(CFLAGS) server/out.a common/out.a $(LDFLAGS)
	
	@echo " [ LD ] bin/$(CLIENT)"
	@$(CC) -o bin/$(CLIENT) $(CFLAGS) client/out.a common/out.a $(LDFLAGS)
	
	@cp res/DejaVuSansMono.ttf bin/font.ttf
	
	@echo "Build complete."
	@echo 

bin:
	@echo " [INIT] bin/"
	@$(MKDIR) bin/

clean: $(SUBDIRS)
	@echo " [ RM ] bin/"
	@$(RMDIR) bin/
	@echo
	@echo "Source tree cleaned."
	@echo

$(SUBDIRS): | bin
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)
