# Modify this line to indicate the default version to build

VERSION=full

# Comment this out if you don't have Tcl/Tk on your system

GUIMODE=-DHAS_GUI

# Modify the following line so that gcc can find the libtcl.so and
# libtk.so libraries on your system. You may need to use the -L option
# to tell gcc which directory to look in. Comment this out if you
# don't have Tcl/Tk.

TKLIBS=-L/usr/lib/x86_64-linux-gnu -ltk8.6 -ltcl8.6

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

TKINC=-isystem /usr/include/tcl8.6 -DUSE_INTERP_RESULT

# Modify these two lines to choose your compiler and compile time
# flags.

CC=gcc
CFLAGS=-Wall -O2 -fcommon

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
HCL2C=$(MISCDIR)/hcl2c
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS = ../misc/yas

all: psim

# This rule builds the PIPE simulator
psim: psim.c sim.h pipe-$(VERSION).hcl $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	# Building the pipe-$(VERSION).hcl version of PIPE
	$(HCL2C) -n pipe-$(VERSION).hcl < pipe-$(VERSION).hcl > pipe-$(VERSION).c
	$(CC) $(CFLAGS) $(INC) -o psim psim.c pipe-$(VERSION).c \
		$(MISCDIR)/isa.c $(LIBS)
clean:
	rm -f psim pipe-*.c *.o *.exe *~ 


