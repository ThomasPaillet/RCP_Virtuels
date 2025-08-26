/*
 * copyright (c) 2021-2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "gain.h"

#include "physical_rcp.h"
#include "protocol.h"
#include "settings.h"
#include "sw_p_08.h"


void set_gain_AW_HE130 (rcp_t *rcp)
{
	if (rcp->current_scene.gain == 37) send_cam_control_command (rcp, "OGU:80");
	else send_cam_control_command_2_digits (rcp, "OGU:", 0x2C - rcp->current_scene.gain, TRUE);

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.gain = rcp->current_scene.gain;
		send_gain_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void gain_changed_AW_HE130 (GtkComboBox *gain_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gain = gtk_combo_box_get_active (gain_combo_box);

	set_gain_AW_HE130 (rcp);
}

GtkWidget *create_gain_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *widget;
	int i;
	char label[8];

	frame = gtk_frame_new ("Gain");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		for (i = 0; i <= 36; i++) {	//0x08 (0 dB) ~ 0x2C (36 dB)
			sprintf (label, "%d dB", i);
			gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Auto");
		rcp->gain_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gain_changed_AW_HE130), rcp);
		rcp->gain_combo_box = widget;
	gtk_container_add (GTK_CONTAINER (frame), widget);

	return frame;
}

gboolean populate_gain_combo_box_AW_UE150 (GtkComboBoxText *combo_box_text)
{
	int i;
	char label[8];

	gtk_combo_box_text_remove_all (combo_box_text);

	if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) {
		for (i = 0; i <= 42; i++) {	//0x08 (0 dB) ~ 0x32 (42 dB)
			sprintf (label, "%d dB", i);
			gtk_combo_box_text_prepend_text (combo_box_text, label);
		}
	} else {
		for (i = 0; i <= 36; i++) {	//0x08 (0 dB) ~ 0x2C (36 dB)
			sprintf (label, "%d dB", i);
			gtk_combo_box_text_prepend_text (combo_box_text, label);
		}
	}

	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box_text), "Auto");

	return G_SOURCE_REMOVE;
}

void set_gain_AW_UE150 (rcp_t *rcp)
{
	int gain;

	if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) {
		if (rcp->current_scene.gain == 43) send_cam_control_command (rcp, "OGU:80");
		else send_cam_control_command_2_digits (rcp, "OGU:", 0x32 - rcp->current_scene.gain, TRUE);

		gain = rcp->current_scene.gain - 6;
	} else {
		if (rcp->current_scene.gain == 37) send_cam_control_command (rcp, "OGU:80");
		else send_cam_control_command_2_digits (rcp, "OGU:", 0x2C - rcp->current_scene.gain, TRUE);

		gain = rcp->current_scene.gain;
	}

	if ((rcp == rcp_vision) && (gain >= 0) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.gain = gain;
		send_gain_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void gain_changed_AW_UE150 (GtkComboBox *gain_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gain = gtk_combo_box_get_active (gain_combo_box);

	set_gain_AW_UE150 (rcp);
}

GtkWidget *create_gain_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *widget;

	frame = gtk_frame_new ("Gain");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);

		populate_gain_combo_box_AW_UE150 (GTK_COMBO_BOX_TEXT (widget));

		rcp->gain_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gain_changed_AW_UE150), rcp);
		rcp->gain_combo_box = widget;
	gtk_container_add (GTK_CONTAINER (frame), widget);

	return frame;
}

