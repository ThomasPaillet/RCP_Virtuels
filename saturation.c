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


char *saturation_label = "Saturation: 0%";


#define MIN_VALUE 0x1C
#define MAX_VALUE 0xA8

void set_saturation (rcp_t *rcp)
{
	if (rcp->current_scene.saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, TRUE);
	else send_cam_control_command_2_digits (rcp, "OSD:B0:", rcp->current_scene.saturation, TRUE);
}

gboolean set_saturation_delayed (rcp_t *rcp)
{
	if (rcp->current_scene.saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
	else send_cam_control_command_2_digits (rcp, "OSD:B0:", rcp->current_scene.saturation, FALSE);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
}

void set_saturation_label (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.saturation == 0x80) gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), saturation_label);
	else if (rcp->current_scene.saturation == 0x1C) gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), "Saturation: Off");
	else {
		sprintf (label, "Saturation: %+d%%", rcp->current_scene.saturation - 0x80);
		gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), label);
	}
}

void saturation_value_changed (GtkRange *saturation_scale, rcp_t *rcp)
{
	int saturation;
	struct timeval current_time, elapsed_time;

	saturation = (int)gtk_range_get_value (saturation_scale);

	if (rcp->current_scene.saturation != saturation) {
		rcp->current_scene.saturation = saturation;

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			if (rcp->timeout_id == 0)
				rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_saturation_delayed, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}
			if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
			else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, FALSE);
		}
		set_saturation_label (rcp);
	}
}

gboolean saturation_button_held (rcp_t *rcp)
{
	int saturation;

	saturation = rcp->current_scene.saturation + rcp->timeout_value;
	if (saturation > MAX_VALUE) saturation = MAX_VALUE;
	else if (saturation < MIN_VALUE) saturation = MIN_VALUE;

	if (rcp->current_scene.saturation != saturation) {
		rcp->current_scene.saturation = saturation;
		g_signal_handler_block (rcp->saturation_scale, rcp->saturation_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), saturation);
		g_signal_handler_unblock (rcp->saturation_scale, rcp->saturation_handler_id);

		if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, FALSE);

		set_saturation_label (rcp);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}


#define SATURATION_BUTTON_PRESSED_PLUS_FUNC(v) \
gboolean saturation_plus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int saturation; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		saturation = rcp->current_scene.saturation - v; \
		if (saturation < MIN_VALUE) saturation = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else { \
		saturation = rcp->current_scene.saturation + v; \
		if (saturation > MAX_VALUE) saturation = MAX_VALUE; \
		rcp->timeout_value = v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.saturation != saturation) { \
		rcp->current_scene.saturation = saturation; \
 \
		g_signal_handler_block (rcp->saturation_scale, rcp->saturation_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), saturation); \
		g_signal_handler_unblock (rcp->saturation_scale, rcp->saturation_handler_id); \
 \
		if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, TRUE); \
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, TRUE); \
 \
		set_saturation_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)saturation_button_held, rcp); \
	} \
 \
	return FALSE; \
}

#define SATURATION_BUTTON_PRESSED_MINUS_FUNC(v) \
gboolean saturation_minus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int saturation; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		saturation = rcp->current_scene.saturation + v; \
		if (saturation > MAX_VALUE) saturation = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else { \
		saturation = rcp->current_scene.saturation - v; \
		if (saturation < MIN_VALUE) saturation = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.saturation != saturation) { \
		rcp->current_scene.saturation = saturation; \
		g_signal_handler_block (rcp->saturation_scale, rcp->saturation_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), saturation); \
		g_signal_handler_unblock (rcp->saturation_scale, rcp->saturation_handler_id); \
 \
		if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, TRUE); \
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, TRUE); \
 \
		set_saturation_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)saturation_button_held, rcp); \
	} \
 \
	return FALSE; \
}


SATURATION_BUTTON_PRESSED_MINUS_FUNC(10)

SATURATION_BUTTON_PRESSED_MINUS_FUNC(1)

void saturation_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), SATURATION_DEFAULT);
}

SATURATION_BUTTON_PRESSED_PLUS_FUNC(1)

SATURATION_BUTTON_PRESSED_PLUS_FUNC(10)

#undef MIN_VALUE
#undef MAX_VALUE

GtkWidget *create_saturation_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *box3, *widget;

	frame = gtk_frame_new (saturation_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x1C, 0xA8, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), TRUE);
		gtk_range_set_value (GTK_RANGE (widget), SATURATION_DEFAULT);
		rcp->saturation_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (saturation_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->saturation_scale = widget;

		box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_container_set_border_width (GTK_CONTAINER (box2), MARGIN_VALUE);
			widget = gtk_button_new_with_label ("-10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("0%");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (saturation_0_button_clicked), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_set_center_widget (GTK_BOX (box3), box2);
		gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->saturation_frame = frame;

	return frame;
}

