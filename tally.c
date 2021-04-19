/*
 * copyright (c) 2018-2021 Thomas Paillet <thomas.paillet@net-c.fr>

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
 * along with RCP-Virtuels. If not, see <https://www.gnu.org/licenses/>.
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


gboolean send_ip_tally = FALSE;

struct sockaddr_in tsl_umd_v5_address;
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
	} else if (rcp == rcp_pgm) {
		if (theme) cairo_set_source_rgb (cr, 0.933333333, 0.0, 0.0);
		else cairo_set_source_rgb (cr, 0.745098039, 0.0, 0.0);
	} else if (rcp == rcp_vision) {
		if (theme) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.925490196);
		else cairo_set_source_rgb (cr, 0.0, 0.745098039, 0.737254902);
	} else if (rcp->tally_data & 0x03) {
		if ((rcp->tally_data & 0x01) && !(rcp->tally_data & 0x02)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
		else if ((rcp->tally_data & 0x02) && !(rcp->tally_data & 0x01)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
		else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
	} else if (rcp == rcp_pvw) {
		if (theme) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.0);
		else cairo_set_source_rgb (cr, 0.0, 0.745098039, 0.0);
	} else if (theme) cairo_set_source_rgb (cr, 0.925490196, 0.925490196, 0.925490196);
	else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);

	cairo_paint (cr);

	if (theme) cairo_set_source_rgb (cr, 0.129411765, 0.129411765, 0.129411765);
	else cairo_set_source_rgb (cr, 0.745098039, 0.745098039, 0.737254902);

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
	if (rcp == rcp_vision) {
		if (theme) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.933333333);
		else cairo_set_source_rgb (cr, 0.0, 0.745098039, 0.745098039);
	} else if (rcp->tally_data & 0x30) {
		if ((rcp->tally_data & 0x10) && !(rcp->tally_data & 0x20)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
		else if ((rcp->tally_data & 0x20) && !(rcp->tally_data & 0x10)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
		else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
	} else if (rcp->tally_data & 0x03) {
		if ((rcp->tally_data & 0x01) && !(rcp->tally_data & 0x02)) cairo_set_source_rgb (cr, rcp->tally_brightness, 0.0, 0.0);
		else if ((rcp->tally_data & 0x02) && !(rcp->tally_data & 0x01)) cairo_set_source_rgb (cr, 0.0, rcp->tally_brightness, 0.0);
		else cairo_set_source_rgb (cr, 0.941176471 * rcp->tally_brightness, 0.764705882 * rcp->tally_brightness, 0.0);
	} else if (rcp == rcp_pgm) {
		if (theme) cairo_set_source_rgb (cr, 0.933333333, 0.0, 0.0);
		else cairo_set_source_rgb (cr, 0.745098039, 0.0, 0.0);
	} else if (rcp == rcp_pvw) {
		if (theme) cairo_set_source_rgb (cr, 0.0, 0.933333333, 0.0);
		else cairo_set_source_rgb (cr, 0.0, 0.745098039, 0.0);
	} else if (theme) cairo_set_source_rgb (cr, 0.925490196, 0.925490196, 0.925490196);
	else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);

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
	memset (&tsl_umd_v5_address, 0, sizeof (struct sockaddr_in));
	tsl_umd_v5_address.sin_family = AF_INET;
	tsl_umd_v5_address.sin_port = htons (TSL_UMD_V5_UDP_PORT);
	tsl_umd_v5_address.sin_addr.s_addr = inet_addr (my_ip_address);
}

gpointer receive_tsl_umd_v5_msg (gpointer data)
{
	int msg_len;
	tsl_umd_v5_packet_t packet;
	cameras_set_t *cameras_set_itr;
	rcp_t *rcp;

	while ((msg_len = recv (tsl_umd_v5_socket, &packet, sizeof (tsl_umd_v5_packet_t), 0)) > 1) {
		g_mutex_lock (&cameras_sets_mutex);
		for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
			if (packet.index < cameras_set_itr->number_of_cameras) {
				rcp = cameras_set_itr->rcp_ptr_array[packet.index];
				rcp->tally_data = packet.control;

				if ((packet.control & 0x80) && (packet.control & 0x40)) rcp->tally_brightness = 0.745098039;
				else if (packet.control & 0x80) rcp->tally_brightness = 0.596078431;
				else if (packet.control & 0x40) rcp->tally_brightness = 0.447058823;
				else rcp->tally_brightness = 0.298039216;

				g_mutex_lock (&current_cameras_set_mutex);

				if (packet.control & 0x30) {
					if ((current_cameras_set == cameras_set_itr) && send_ip_tally && !rcp->ip_tally_is_on && rcp->camera_is_on) {
						g_mutex_unlock (&current_cameras_set_mutex);

						send_tally_on_control_command (rcp);
					} else g_mutex_unlock (&current_cameras_set_mutex);
				} else {
					if ((current_cameras_set == cameras_set_itr) && send_ip_tally && rcp->ip_tally_is_on && rcp->camera_is_on) {
						g_mutex_unlock (&current_cameras_set_mutex);

						send_tally_off_control_command (rcp);
					} else g_mutex_unlock (&current_cameras_set_mutex);
				}

				if (cameras_set_itr == current_cameras_set) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp);
			}
		}
		g_mutex_unlock (&cameras_sets_mutex);
	}

	return NULL;
}

void start_tally (void)
{
	tsl_umd_v5_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind (tsl_umd_v5_socket, (struct sockaddr *)&tsl_umd_v5_address, sizeof (struct sockaddr_in));
	tsl_umd_v5_thread = g_thread_new (NULL, receive_tsl_umd_v5_msg, NULL);
}

void stop_tally (void)
{
	cameras_set_t *cameras_set_itr;
	int i;
	rcp_t *rcp;

	shutdown (tsl_umd_v5_socket, SHUT_RD);
	closesocket (tsl_umd_v5_socket);

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
			rcp = cameras_set_itr->rcp_ptr_array[i];

			rcp->tally_data = 0x00;

			if (send_ip_tally && rcp->ip_tally_is_on && rcp->camera_is_on) send_tally_off_control_command (rcp);
		}
	}

	if (tsl_umd_v5_thread != NULL) {
		g_thread_join (tsl_umd_v5_thread);
		tsl_umd_v5_thread = NULL;
	}
}

