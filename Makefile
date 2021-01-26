CC = cc
CFLAGS = -c -Wall -D_REENTRANT `pkg-config --cflags gtk+-3.0`
LDFLAGS = -lm `pkg-config --libs gtk+-3.0`

PROG = RCP-Virtuels
BINDIR = $(PROG)

SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)

PNG = $(wildcard Pixbufs/*.png)
PIXBUF = $(wildcard Pixbufs/*.h)

$(PROG): Linux/gresources.o Linux/Linux.o $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

Linux/gresources.o: Linux/gresources.xml $(shell glib-compile-resources --generate-dependencies Linux/gresources.xml)
	@(cd Linux && $(MAKE) gresources.o)

Linux/Linux.o: Linux/Linux.c rcp.h
	@(cd Linux && $(MAKE) Linux.o)

rcp.o: rcp.h

gamma.o: rcp.h

color_temperature.o: rcp.h

knee.o: rcp.h cam_cmd_define.h

matrix.o: rcp.h

detail.o: rcp.h cam_cmd_define.h

saturation.o: rcp.h cam_cmd_define.h

white.o: rcp.h cam_cmd_define.h

black.o: rcp.h cam_cmd_define.h

shutter.o: rcp.h cam_cmd_define.h

iris.o: rcp.h ptz_cmd_define.h

pedestal.o: rcp.h cam_cmd_define.h

master_rcp.o: rcp.h

cameras_set.o: rcp.h

settings.o: rcp.h

scenes.o: rcp.h

protocol.o: rcp.h

update_notification.o: rcp.h

misc.o: rcp.h

tally.o: rcp.h

sw_p_08.o: rcp.h

physical_rcp.o: rcp.h

main_window.o: rcp.h

error.o: rcp.h

.c.o:
	$(CC) $(CFLAGS) $<

Pixbufs/pixbufs.o: Pixbufs/pixbufs.c $(PNG) $(PIXBUF)
	@(cd Pixbufs && $(MAKE))

$(PROG).exe: $(OBJS) Pixbufs/pixbufs.o Win32.o
	$(CC) -o $@ $^ $(LDFLAGS) -lwsock32

Win32.o: Win32/Win32.c rcp.h
	$(CC) $(CFLAGS) $<

install-win32: $(PROG).exe
	strip --strip-unneeded $(PROG).exe
	@mkdir -p c:/$(BINDIR)/resources
	cp -Ru resources/* c:/$(BINDIR)/resources
	cp -u $(PROG).exe $(PROG).ico $(PROG).pdf COPYING c:/$(BINDIR)
	@$(SHELL) Win32/install-gtk-dll $(BINDIR)

clean:
	rm *.o

clean-all:
	@(cd Linux && $(MAKE) clean)
	@(cd Pixbufs && $(MAKE) clean)
	rm -f *.o
	rm -f $(PROG)

