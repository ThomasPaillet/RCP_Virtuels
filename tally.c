/*
 * copyright (c) 2018 2019 2020 Thomas Paillet <thomas.paillet@net-c.fr

 * This file is part of RCP-Virtuels.

 * RCP-Virtuels is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * RCP-Virtuels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with RCP-Virtuels.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "rcp.h"


typedef struct {
	guint16 total_byte_count;
	guint8 minor_version_number;
	guint8 flags;
	guint16 screen;

	guint16 index;
	guint16 control;
	guint16 length;
	char text[2038];
} tsl_umd_v5_packet_t;


struct sockaddr_in tsl_umd_v5_adresse;
SOCKET tsl_umd_v5_socket;

GThread *tsl_umd_v5_thread = NULL;


gboolean name_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp)
{
	PangoLayout *pl;
	PangoFontDescription *desc;

	if (rcp->tally_data & 0x30) {
		if ((rcp->tally_data & 0x10) && !(rcp->tally_data & 0x20)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
		else if ((rcp->tally_data & 0x20) && !(rcp->tally_data & 0x10)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
		else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
	} else if (rcp == rcp_vision) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.933333333);
	else if (rcp->tally_data & 0x03) {
		if (!rcp->tally_1_is_on) {
			if ((rcp->tally_data & 0x01) && !(rcp->tally_data & 0x02)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
			else if ((rcp->tally_data & 0x02) && !(rcp->tally_data & 0x01)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
			else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
		}
	} else if (rcp == rcp_pgm) cairo_set_source_rgb (cr, 0.933333333, 0.0, 0.0);
	else if (rcp == rcp_pvw) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.0);
	else if (theme) cairo_set_source_rgb (cr, 0.925490196, 0.925490196, 0.925490196);
	else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);

	cairo_paint (cr);

	if (theme) cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.75);
	else cairo_set_source_rgb (cr, 0.933333333, 0.933333333, 0.925490196);

	pl = pango_cairo_create_layout (cr);

	if (rcp->name[1] == '\0') cairo_translate (cr, 18, 0);
	else cairo_translate (cr, 5, 0);

	pango_layout_set_text (pl, rcp->name, -1);
	desc = pango_font_description_from_string ("Courier Bold 30");
	pango_layout_set_font_description (pl, desc);
	pango_font_description_free (desc);

	pango_cairo_show_layout (cr, pl);

	g_object_unref(pl);

	return GDK_EVENT_PROPAGATE;
}

gboolean tally_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp)
{
	if (rcp->tally_data & 0x30) {
		if ((rcp->tally_data & 0x10) && !(rcp->tally_data & 0x20)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
		else if ((rcp->tally_data & 0x20) && !(rcp->tally_data & 0x10)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
		else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
	} else if (rcp == rcp_vision) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.933333333);
	else if (rcp->tally_data & 0x03) {
		if (!rcp->tally_1_is_on) {
			if ((rcp->tally_data & 0x01) && !(rcp->tally_data & 0x02)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
			else if ((rcp->tally_data & 0x02) && !(rcp->tally_data & 0x01)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
			else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
		}
	} else if (rcp == rcp_pgm) cairo_set_source_rgb (cr, 0.933333333, 0.0, 0.0);
	else if (rcp == rcp_pvw) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.0);
	else {
		if (theme) cairo_set_source_rgb (cr, 0.925490196, 0.925490196, 0.925490196);
		else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);
	}

	cairo_paint (cr);

	return GDK_EVENT_PROPAGATE;
}

gboolean g_source_rcp_queue_draw (rcp_t *rcp)
{
	gtk_widget_queue_draw (rcp->name_drawing_area);
	gtk_widget_queue_draw (rcp->tally_widgets[0]);
	gtk_widget_queue_draw (rcp->tally_widgets[1]);
	gtk_widget_queue_draw (rcp->tally_widgets[2]);
	gtk_widget_queue_draw (rcp->tally_widgets[3]);

	return G_SOURCE_REMOVE;
}

void init_tally (void)
{
	memset (&tsl_umd_v5_adresse, 0, sizeof (struct sockaddr_in));
	tsl_umd_v5_adresse.sin_family = AF_INET;
	tsl_umd_v5_adresse.sin_port = htons (TSL_UMD_V5_UDP_PORT);
	tsl_umd_v5_adresse.sin_addr.s_addr = inet_addr (my_ip_adresse);
}

gpointer receive_tsl_umd_v5_msg (gpointer data)
{
	int msg_len;
	tsl_umd_v5_packet_t packet;
	cameras_set_t *cameras_set_itr;
	rcp_t *rcp;

	while ((msg_len = recv (tsl_umd_v5_socket, (char*)&packet, 2048, 0)) > 1) {
		for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
			if (packet.index < cameras_set_itr->number_of_cameras) {
				rcp = cameras_set_itr->rcp_ptr_array[packet.index];
				rcp->tally_data = packet.control;

				if ((packet.control & 0x80) && (packet.control & 0x40)) rcp->tally_brightness = 1.0;
				else if (packet.control & 0x80) rcp->tally_brightness = 0.8;
				else if (packet.control & 0x40) rcp->tally_brightness = 0.6;
				else rcp->tally_brightness = 0.4;

				if (packet.control & 0x30) rcp->tally_1_is_on = TRUE;
				else rcp->tally_1_is_on = FALSE;

				g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp);
			}
		}
	}

	return NULL;
}

void start_tally (void)
{
	tsl_umd_v5_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind (tsl_umd_v5_socket, (struct sockaddr *)&tsl_umd_v5_adresse, sizeof (struct sockaddr_in));
	tsl_umd_v5_thread = g_thread_new (NULL, receive_tsl_umd_v5_msg, NULL);
}

void stop_tally (void)
{
	closesocket (tsl_umd_v5_socket);

//	if (tsl_umd_v5_thread != NULL) g_thread_join (tsl_umd_v5_thread);
	tsl_umd_v5_thread = NULL;
}

