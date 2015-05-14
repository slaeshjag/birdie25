# Project: 68k-emu
TOPDIR	=	.
include config.mk

# Sub directories to build
SUBDIRS		=	src client

LIBS		=	$(addsuffix /out.a,$(SUBDIRS))

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@echo " [ LD ] bin/$(BINFILE)"
	@$(CC) -o bin/$(BINFILE) $(CFLAGS) $(LIBS) $(LDFLAGS)
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
