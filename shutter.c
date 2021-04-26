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

#include "cam_cmd_define.h"

#include <math.h>


char *synchro_printf = "Synchro: %5.2lf Hz";


#define MIN_VALUE 0x001
#define MAX_VALUE 0x0FF

void set_shutter_synchro_label (rcp_t *rcp)
{
	char label[24];

	if (format_is_50Hz) sprintf (label, synchro_printf, 50.15 + (rcp->current_scene.shutter_synchro - 1) * 1.9);
	else sprintf (label, synchro_printf, 60.15 + (rcp->current_scene.shutter_synchro - 1) * 2.2);

	gtk_frame_set_label (GTK_FRAME (rcp->shutter_synchro_frame), label);
	gtk_button_set_label (GTK_BUTTON (rcp->shutter_synchro_button), label + 9);
}

CAM_CMD_FUNCS(shutter_synchro,"OMS:",3)

BUTTON_PRESSED_MINUS_FUNC(shutter_synchro,"OMS:",3,50)

BUTTON_PRESSED_PLUS_FUNC(shutter_synchro,"OMS:",3,50)

#undef MIN_VALUE
#undef MAX_VALUE

void create_shutter_synchro_window (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;
	char label[24];

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Synchro");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);

	if (format_is_50Hz) sprintf (label, synchro_printf, 50.15);
	else sprintf (label, synchro_printf, 60.15);

	frame = gtk_frame_new (label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x001, 0x0FF, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

void set_shutter_type (rcp_t *rcp)
{
	if (rcp->current_scene.shutter_type == 0) send_cam_control_command (rcp, "OSH:0");
	else if (rcp->current_scene.shutter_type == 2) send_cam_control_command (rcp, "OSH:B");
	else if (rcp->current_scene.shutter_type == 3) send_cam_control_command (rcp, "OSH:C");

	if (rcp->current_scene.shutter_type != 1) {
		if ((rcp == rcp_vision) && physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.shutter_type = rcp->current_scene.shutter_type;
			send_shutter_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}
}

void shutter_type_changed (GtkComboBox *shutter_type_combo_box, rcp_t *rcp)
{
	int active_item;

	g_signal_handler_block (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), -1);
	g_signal_handler_unblock (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	rcp->current_scene.shutter_step = -1;

	active_item = gtk_combo_box_get_active (shutter_type_combo_box);

	rcp->current_scene.shutter_type = active_item;

	if (active_item == 1) {
		gtk_widget_hide (rcp->shutter_synchro_button);
		gtk_widget_show (rcp->shutter_step_combo_box);
	} else if (active_item == 2) {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_show (rcp->shutter_synchro_button);
	} else if (active_item == 3) {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
	} else {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
	}

	set_shutter_type (rcp);
}

void populate_shutter_step_combo_box (GtkComboBoxText *combo_box)
{
	gtk_combo_box_text_remove_all (combo_box);

	if (output_fps == _59_94fps) gtk_combo_box_text_append_text (combo_box, "1/100");
	else gtk_combo_box_text_append_text (combo_box, "1/60");
	gtk_combo_box_text_append_text (combo_box, "1/120");
	gtk_combo_box_text_append_text (combo_box, "1/250");
	gtk_combo_box_text_append_text (combo_box, "1/500");
	gtk_combo_box_text_append_text (combo_box, "1/1000");
	gtk_combo_box_text_append_text (combo_box, "1/2000");
	gtk_combo_box_text_append_text (combo_box, "1/4000");
	gtk_combo_box_text_append_text (combo_box, "1/10000");
	if (output_fps == _23_98fps) gtk_combo_box_text_append_text (combo_box, "1/24");
	else if (output_fps == _25fps) gtk_combo_box_text_append_text (combo_box, "1/25");
	else if (output_fps == _29_97fps) gtk_combo_box_text_append_text (combo_box, "1/30");
}

void set_shutter_step (rcp_t *rcp)
{
	switch (rcp->current_scene.shutter_step) {
		case 0: if (output_fps == _59_94fps) send_cam_control_command (rcp, "OSH:3");
			else send_cam_control_command (rcp, "OSH:2");
			break;
		case 1: if ((output_fps == _25fps) || (output_fps == _50fps)) send_cam_control_command (rcp, "OSH:3");
			else send_cam_control_command (rcp, "OSH:4");
			break;
		case 2: send_cam_control_command (rcp, "OSH:5");
			break;
		case 3: send_cam_control_command (rcp, "OSH:6");
			break;
		case 4: send_cam_control_command (rcp, "OSH:7");
			break;
		case 5: send_cam_control_command (rcp, "OSH:8");
			break;
		case 6: send_cam_control_command (rcp, "OSH:9");
			break;
		case 7: send_cam_control_command (rcp, "OSH:A");
			break;
		case 8: if (output_fps == _23_98fps) send_cam_control_command (rcp, "OSH:D");
			else if (output_fps == _25fps) send_cam_control_command (rcp, "OSH:E");
			else if (output_fps == _29_97fps) send_cam_control_command (rcp, "OSH:F");
	}

	if (rcp->current_scene.shutter_type == 1) {
		if ((rcp == rcp_vision) && physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.shutter_type = 1;
			physical_rcp.shutter_step = rcp->current_scene.shutter_step;
			send_shutter_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}
}

void shutter_step_changed (GtkComboBox *shutter_step_combo_box, rcp_t *rcp)
{
	rcp->current_scene.shutter_step = gtk_combo_box_get_active (shutter_step_combo_box);

	set_shutter_step (rcp);
}

GtkWidget *create_shutter_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("Shutter");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Off");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Step");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Synchro");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "ELC");
		rcp->shutter_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_type_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_type_combo_box = widget;

		widget = gtk_combo_box_text_new ();
		populate_shutter_step_combo_box (GTK_COMBO_BOX_TEXT (widget));
		rcp->shutter_step_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_step_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_step_combo_box = widget;

		create_shutter_synchro_window (rcp);

		if (format_is_50Hz) widget = gtk_button_new_with_label ("50.15");
		else widget = gtk_button_new_with_label ("60.15");
		g_signal_connect_swapped (widget, "clicked", G_CALLBACK (gtk_widget_show_all), rcp->shutter_synchro_window);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

