/*
 * copyright (c) 2018-2022 Thomas Paillet <thomas.paillet@net-c.fr>

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
#include "saturation.h"

#include "protocol.h"
#include "misc.h"

#include "main_window.h"


void set_saturation_label (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.saturation == 0x80) gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), "Saturation: 0%");
	else if (rcp->current_scene.saturation == 0x1C) gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), "Saturation: Off");
	else {
		sprintf (label, "Saturation: %+d%%", rcp->current_scene.saturation - 0x80);
		gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), label);
	}
}

void set_saturation (rcp_t *rcp)
{
	if (rcp->current_scene.saturation == 0x1C) send_cam_control_command (rcp, "OSD:B0:00");
	else send_cam_control_command_2_digits (rcp, "OSD:B0:", rcp->current_scene.saturation, TRUE);
}

gboolean set_saturation_delayed (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	if (rcp->current_scene.saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
	else send_cam_control_command_2_digits (rcp, "OSD:B0:", rcp->current_scene.saturation, FALSE);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
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
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_saturation_delayed, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}

			rcp->last_time = current_time;

			if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
			else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, FALSE);
		}

		set_saturation_label (rcp);
	}
}

void saturation_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), SATURATION_DEFAULT);
}


#define SATURATION_BUTTON_PRESSED_MINUS_FUNC(v,m) \
gboolean saturation_minus_##v##_button_pressed_##m (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int saturation; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		saturation = rcp->current_scene.saturation - v; \
		if (saturation < MIN_VALUE) saturation = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		saturation = rcp->current_scene.saturation + v; \
		if (saturation > MAX_VALUE) saturation = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else return FALSE; \
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
		if (saturation == 0x1C) send_cam_control_command (rcp, "OSD:B0:00"); \
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, TRUE); \
 \
		set_saturation_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)saturation_button_held_##m, rcp); \
	} \
 \
	return FALSE; \
}

#define SATURATION_BUTTON_PRESSED_PLUS_FUNC(v,m) \
gboolean saturation_plus_##v##_button_pressed_##m (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int saturation; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		saturation = rcp->current_scene.saturation + v; \
		if (saturation > MAX_VALUE) saturation = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		saturation = rcp->current_scene.saturation - v; \
		if (saturation < MIN_VALUE) saturation = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else return FALSE; \
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
		if (saturation == 0x1C) send_cam_control_command (rcp, "OSD:B0:00"); \
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, TRUE); \
 \
		set_saturation_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)saturation_button_held_##m, rcp); \
	} \
 \
	return FALSE; \
}


#define MIN_VALUE 0x1C
#define MAX_VALUE 0xA8

gboolean saturation_button_held_AW_HE130 (rcp_t *rcp)
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

		rcp->last_time.tv_usec += 130000;
		if (rcp->last_time.tv_usec >= 1000000) {
			rcp->last_time.tv_sec++;
			rcp->last_time.tv_usec -= 1000000;
		}

		if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, FALSE);

		set_saturation_label (rcp);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}

SATURATION_BUTTON_PRESSED_MINUS_FUNC(10,AW_HE130)
SATURATION_BUTTON_PRESSED_MINUS_FUNC(1,AW_HE130)

SATURATION_BUTTON_PRESSED_PLUS_FUNC(1,AW_HE130)
SATURATION_BUTTON_PRESSED_PLUS_FUNC(10,AW_HE130)

GtkWidget *create_saturation_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;

	frame = gtk_frame_new ("Saturation: 0%");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), TRUE);
		gtk_range_set_value (GTK_RANGE (widget), SATURATION_DEFAULT);
		rcp->saturation_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (saturation_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->saturation_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_10_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_minus_10_button = widget;

			widget = gtk_button_new_with_label ("-1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_1_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_minus_1_button = widget;

			widget = gtk_button_new_with_label ("0%");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (saturation_0_button_clicked), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_1_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_plus_1_button = widget;

			widget = gtk_button_new_with_label ("+10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_10_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_plus_10_button = widget;
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->saturation_frame = frame;

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x1C
#define MAX_VALUE 0xE3

gboolean saturation_button_held_AW_UE150 (rcp_t *rcp)
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

		rcp->last_time.tv_usec += 130000;
		if (rcp->last_time.tv_usec >= 1000000) {
			rcp->last_time.tv_sec++;
			rcp->last_time.tv_usec -= 1000000;
		}

		if (saturation == 0x1C) send_cam_control_command_2_digits (rcp, "OSD:B0:", 0x00, FALSE);
		else send_cam_control_command_2_digits (rcp, "OSD:B0:", saturation, FALSE);

		set_saturation_label (rcp);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}

SATURATION_BUTTON_PRESSED_MINUS_FUNC(10,AW_UE150)
SATURATION_BUTTON_PRESSED_MINUS_FUNC(1,AW_UE150)

SATURATION_BUTTON_PRESSED_PLUS_FUNC(1,AW_UE150)
SATURATION_BUTTON_PRESSED_PLUS_FUNC(10,AW_UE150)

GtkWidget *create_saturation_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;

	frame = gtk_frame_new ("Saturation: 0%");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), TRUE);
		gtk_range_set_value (GTK_RANGE (widget), SATURATION_DEFAULT);
		rcp->saturation_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (saturation_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->saturation_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_10_button_pressed_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_minus_10_button = widget;

			widget = gtk_button_new_with_label ("-1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_1_button_pressed_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_minus_1_button = widget;

			widget = gtk_button_new_with_label ("0%");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (saturation_0_button_clicked), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_1_button_pressed_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_plus_1_button = widget;

			widget = gtk_button_new_with_label ("+10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_10_button_pressed_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->saturation_plus_10_button = widget;
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->saturation_frame = frame;

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

