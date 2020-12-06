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


int color_temperature_array[0x079] = {2000, 2010, 2020, 2040, 2050, 2070, 2080, 2090, 2110, 2120, 2140, 2150, 2170, 2180, 2200, 2210, 2230, 2240, 2260, \
	2280, 2300, 2310, 2330, 2340, 2360, 2380, 2400, 2420, 2440, 2460, 2480, 2500, 2520, 2540, 2560, 2600, 2620, 2640, 2680, 2700, 2720, 2740, 2780, \
	2800, 2820, 2850, 2870, 2920, 2950, 2970, 3000, 3020, 3070, 3100, 3120, 3150, 3200, 3250, 3270, 3330, 3360, 3420, 3450, 3510, 3570, 3600, 3660, \
	3720, 3780, 3840, 3870, 3930, 3990, 4050, 4110, 4170, 4240, 4320, 4360, 4440, 4520, 4600, 4680, 4760, 4840, 4920, 5000, 5100, 5200, 5300, 5400, \
	5500, 5600, 5750, 5850, 6000, 6150, 6300, 6450, 6650, 6800, 7000, 7150, 7400, 7600, 7800, 8100, 8300, 8600, 8900, 9200, 9600, 10000, 10500, 11000, \
	11500, 12000, 12500, 13000, 14000, 15000};


void set_color_temperature (rcp_t *rcp)
{
	send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, TRUE);
}

gboolean set_color_temperature_delayed (rcp_t *rcp)
{
	send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, FALSE);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
}

void color_temperature_changed (GtkComboBox *color_temperature_combo_box, rcp_t *rcp)
{
	struct timeval current_time, elapsed_time;

	rcp->current_scene.color_temperature = gtk_combo_box_get_active (color_temperature_combo_box);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);

	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
		if (rcp->timeout_id == 0)
			rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_color_temperature_delayed, rcp);
	} else {
		if (rcp->timeout_id != 0) {
			g_source_remove (rcp->timeout_id);
			rcp->timeout_id = 0;
		}
		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, FALSE);
	}
}

gboolean color_temperature_button_held (rcp_t *rcp)
{
	int index;

	index = rcp->current_scene.color_temperature + rcp->timeout_value;
	if (index > 0x078) index = 0x078;
	else if (index < 0) index = 0;

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, FALSE);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}

gboolean color_temperature_minus_10_button_clicked (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	} else {
		index = rcp->current_scene.color_temperature + 10;
		if (index > 0x078) index = 0x078;
		rcp->timeout_value = 10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held, rcp);
	}

	return FALSE;
}

gboolean color_temperature_minus_1_button_clicked (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	} else {
		index = rcp->current_scene.color_temperature + 1;
		if (index > 0x078) index = 0x078;
		rcp->timeout_value = 1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_1_button_clicked (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		index = rcp->current_scene.color_temperature + 1;
		if (index > 0x078) index = 0x078;
		rcp->timeout_value = 1;
	} else {
		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_10_button_clicked (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		index = rcp->current_scene.color_temperature + 10;
		if (index > 0x078) index = 0x078;
		rcp->timeout_value = 10;
	} else {
		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 0x078 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held, rcp);
	}

	return FALSE;
}

GtkWidget *create_color_temperature_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;
	int i;
	char label[8];

	frame = gtk_frame_new ("Température de couleur");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.04, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		for (i = 0x078; i >= 0x000; i--) {
			sprintf (label, "%dK", color_temperature_array[i]);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		rcp->color_temperature_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (color_temperature_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->color_temperature_combo_box = widget;

		widget = gtk_button_new_with_label ("--");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_10_button_clicked), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("-");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_1_button_clicked), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("+");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_1_button_clicked), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("++");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_10_button_clicked), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

