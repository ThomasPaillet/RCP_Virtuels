CC = cc
CFLAGS = -c -Wall -D_REENTRANT `pkg-config --cflags gtk+-3.0`
#CFLAGS = -c -mwin32 -Wall -D_REENTRANT `pkg-config --cflags gtk+-3.0`
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

Linux/Linux.o: Linux/Linux.c operating_system.h main_window.h
	@(cd Linux && $(MAKE) Linux.o)

rcp.o: rcp_AW_HE130.h rcp_AW_UE150.h protocol.h misc.h error.h master_rcp.h scenes.h settings.h sw_p_08.h tally.h physical_rcp.h main_window.h

rcp_AW_HE130.o: protocol.h misc.h ND_filter.h gain.h gamma.h color_temperature.h knee.h matrix.h detail.h saturation.h white.h black.h shutter.h pedestal.h iris.h scenes.h sw_p_08.h physical_rcp.h

rcp_AW_UE150.o: protocol.h misc.h ND_filter.h gain.h gamma.h chroma_phase.h color_temperature.h knee.h matrix.h detail.h saturation.h white.h black.h shutter.h pedestal.h iris.h scenes.h settings.h sw_p_08.h physical_rcp.h

protocol.o: error.h update_notification.h

misc.o: rcp_AW_HE130.h rcp_AW_UE150.h protocol.h error.h ND_filter.h gamma.h chroma_phase.h color_temperature.h knee.h matrix.h detail.h saturation.h shutter.h pedestal.h cameras_set.h settings.h sw_p_08.h physical_rcp.h

update_ihm.o: gamma.h chroma_phase.h knee.h matrix.h detail.h saturation.h shutter.h pedestal.h settings.h

error.o: misc.h

update_notification.o: version.h protocol.h misc.h update_ihm.h error.h ND_filter.h gain.h gamma.h color_temperature.h cameras_set.h settings.h sw_p_08.h physical_rcp.h

ND_filter.o: protocol.h sw_p_08.h physical_rcp.h

gain.o: protocol.h settings.h sw_p_08.h physical_rcp.h

gamma.o: cam_cmd_define.h protocol.h misc.h cameras_set.h main_window.h

chroma_phase.o: cam_cmd_define.h protocol.h misc.h cameras_set.h main_window.h

color_temperature.o: protocol.h misc.h

knee.o: cam_cmd_define.h protocol.h misc.h cameras_set.h main_window.h

matrix.o: protocol.h misc.h cameras_set.h sw_p_08.h main_window.h

detail.o: cam_cmd_define.h protocol.h misc.h cameras_set.h sw_p_08.h physical_rcp.h main_window.h

saturation.o: protocol.h misc.h main_window.h

white.o: cam_cmd_define.h protocol.h misc.h sw_p_08.h physical_rcp.h main_window.h

black.o: cam_cmd_define.h protocol.h misc.h sw_p_08.h physical_rcp.h main_window.h

shutter.o: cam_cmd_define.h protocol.h misc.h cameras_set.h settings.h sw_p_08.h physical_rcp.h main_window.h

pedestal.o: cam_cmd_define.h protocol.h misc.h main_window.h

iris.o: ptz_cmd_define.h protocol.h misc.h sw_p_08.h physical_rcp.h main_window.h

master_rcp.o: master_cmd_define.h misc.h gamma.h saturation.h white.h black.h pedestal.h iris.h cameras_set.h scenes.h settings.h sw_p_08.h main_window.h

cameras_set.o: master_rcp.h rcp_AW_HE130.h rcp_AW_UE150.h protocol.h misc.h error.h scenes.h settings.h sw_p_08.h main_window.h

scenes.o: cameras_set.h gamma.h knee.h detail.h settings.h main_window.h

settings.o: version.h protocol.h misc.h error.h update_notification.h operating_system.h gain.h shutter.h cameras_set.h scenes.h sw_p_08.h tally.h physical_rcp.h main_window.h

sw_p_08.o: rcp_AW_HE130.h rcp_AW_UE150.h protocol.h misc.h operating_system.h cameras_set.h settings.h tally.h physical_rcp.h main_window.h

tally.o: protocol.h cameras_set.h settings.h sw_p_08.h main_window.h

physical_rcp.o: protocol.h update_ihm.h error.h settings.h sw_p_08.h

main_window.o: protocol.h misc.h error.h update_notification.h iris.h cameras_set.h scenes.h settings.h sw_p_08.h tally.h physical_rcp.h

%.o: %.c rcp.h %.h
	$(CC) $(CFLAGS) $<

$(PROG).exe: $(OBJS) Pixbufs/pixbufs.o Win32.o
	$(CC) -o $@ $^ $(LDFLAGS) -lwsock32

Pixbufs/pixbufs.o: Pixbufs/pixbufs.c $(PNG) $(PIXBUF)
	@(cd Pixbufs && $(MAKE))

Win32.o: Win32/Win32.c operating_system.h main_window.h
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
	rm -f Win32/Win32.o
	rm -f *.o
	rm -f $(PROG)

