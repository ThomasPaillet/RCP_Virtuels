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

#include "cam_cmd_define.h"

#include <math.h>


void set_shutter_type_AW_HE130 (rcp_t *rcp)
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

void shutter_type_changed_AW_HE130 (GtkComboBox *shutter_type_combo_box, rcp_t *rcp)
{
	g_signal_handler_block (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), -1);
	g_signal_handler_unblock (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	rcp->current_scene.shutter_step = -1;

	rcp->current_scene.shutter_type = gtk_combo_box_get_active (shutter_type_combo_box);

	if (rcp->current_scene.shutter_type == 1) {
		gtk_widget_hide (rcp->shutter_synchro_button);
		gtk_widget_show (rcp->shutter_step_combo_box);
	} else if (rcp->current_scene.shutter_type == 2) {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_show (rcp->shutter_synchro_button);
	} else if (rcp->current_scene.shutter_type == 3) {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
	} else {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
	}

	set_shutter_type_AW_HE130 (rcp);
}

void populate_shutter_step_combo_box_AW_HE130 (GtkComboBoxText *combo_box)
{
	gtk_combo_box_text_remove_all (combo_box);

	if ((output_fps_AW_HE130 == fps_59_94p) || (output_fps_AW_HE130 == fps_59_94i)) gtk_combo_box_text_append_text (combo_box, "1/100");
	else gtk_combo_box_text_append_text (combo_box, "1/60");
	gtk_combo_box_text_append_text (combo_box, "1/120");
	gtk_combo_box_text_append_text (combo_box, "1/250");
	gtk_combo_box_text_append_text (combo_box, "1/500");
	gtk_combo_box_text_append_text (combo_box, "1/1000");
	gtk_combo_box_text_append_text (combo_box, "1/2000");
	gtk_combo_box_text_append_text (combo_box, "1/4000");
	gtk_combo_box_text_append_text (combo_box, "1/10000");
	if (output_fps_AW_HE130 == fps_23_98p) gtk_combo_box_text_append_text (combo_box, "1/24");
	else if (output_fps_AW_HE130 == fps_25p) gtk_combo_box_text_append_text (combo_box, "1/25");
	else if (output_fps_AW_HE130 == fps_29_97p) gtk_combo_box_text_append_text (combo_box, "1/30");
}

void set_shutter_step_AW_HE130 (rcp_t *rcp)
{
	switch (rcp->current_scene.shutter_step) {
		case 0: if ((output_fps_AW_HE130 == fps_59_94p) || (output_fps_AW_HE130 == fps_59_94i)) send_cam_control_command (rcp, "OSH:3");
			else send_cam_control_command (rcp, "OSH:2");
			break;
		case 1: if ((output_fps_AW_HE130 == fps_50p) || (output_fps_AW_HE130 == fps_50i) || (output_fps_AW_HE130 == fps_25p)) send_cam_control_command (rcp, "OSH:3");
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
		case 8: if (output_fps_AW_HE130 == fps_23_98p) send_cam_control_command (rcp, "OSH:D");
			else if (output_fps_AW_HE130 == fps_25p) send_cam_control_command (rcp, "OSH:E");
			else if (output_fps_AW_HE130 == fps_29_97p) send_cam_control_command (rcp, "OSH:F");
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

void shutter_step_changed_AW_HE130 (GtkComboBox *shutter_step_combo_box, rcp_t *rcp)
{
	rcp->current_scene.shutter_step = gtk_combo_box_get_active (shutter_step_combo_box);

	set_shutter_step_AW_HE130 (rcp);
}

#define MIN_VALUE 0x001
#define MAX_VALUE 0x0FF

void set_shutter_synchro_label_AW_HE130 (rcp_t *rcp)
{
	char label[24];

	if (settings_parameters_indexes_array_AW_HE130[0] == 1) sprintf (label, "Synchro: %5.2lf Hz", 50.15 + (rcp->current_scene.shutter_synchro - 1) * 1.9);
	else sprintf (label, "Synchro: %5.2lf Hz", 60.15 + (rcp->current_scene.shutter_synchro - 1) * 2.2);

	gtk_frame_set_label (GTK_FRAME (rcp->shutter_synchro_frame), label);
	gtk_button_set_label (GTK_BUTTON (rcp->shutter_synchro_button), label + 9);
}

CAM_CMD_FUNCS_S(shutter_synchro,"OMS:",3,AW_HE130)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OMS:",3,50,AW_HE130)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OMS:",3,10,AW_HE130)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OMS:",3,1,AW_HE130)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OMS:",3,1,AW_HE130)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OMS:",3,10,AW_HE130)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OMS:",3,50,AW_HE130)

void create_shutter_synchro_window_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_AW_HE130), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_AW_HE130), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

void show_rcp_shutter_synchro_window (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	popup_window = rcp->shutter_synchro_window;
	gtk_widget_show_all (rcp->shutter_synchro_window);
}

GtkWidget *create_shutter_frame_AW_HE130 (rcp_t *rcp)
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
		rcp->shutter_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_type_changed_AW_HE130), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_type_combo_box = widget;

		widget = gtk_combo_box_text_new ();
		populate_shutter_step_combo_box_AW_HE130 (GTK_COMBO_BOX_TEXT (widget));
		rcp->shutter_step_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_step_changed_AW_HE130), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_step_combo_box = widget;

		create_shutter_synchro_window_AW_HE130 (rcp);

		widget = gtk_button_new ();
		g_signal_connect_swapped (widget, "clicked", G_CALLBACK (show_rcp_shutter_synchro_window), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

void set_shutter_type_AW_UE150 (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSJ:03:", rcp->current_scene.shutter_type, TRUE);

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.shutter_type = rcp->current_scene.shutter_type;
		if (rcp->current_scene.shutter_step == 60) physical_rcp.shutter_step = 0;
		else if (rcp->current_scene.shutter_step == 100) physical_rcp.shutter_step = 0;
		else if (rcp->current_scene.shutter_step == 120) physical_rcp.shutter_step = 1;
		else if (rcp->current_scene.shutter_step == 250) physical_rcp.shutter_step = 2;
		else if (rcp->current_scene.shutter_step == 500) physical_rcp.shutter_step = 3;
		else if (rcp->current_scene.shutter_step == 1000) physical_rcp.shutter_step = 4;
		else if (rcp->current_scene.shutter_step == 2000) physical_rcp.shutter_step = 5;
		else if (rcp->current_scene.shutter_step == 4000) physical_rcp.shutter_step = 6;
		else if (rcp->current_scene.shutter_step == 8000) physical_rcp.shutter_step = 6;
		else if (rcp->current_scene.shutter_step == 10000) physical_rcp.shutter_step = 7;
		else if (rcp->current_scene.shutter_step == 24) physical_rcp.shutter_step = 8;
		else if (rcp->current_scene.shutter_step == 25) physical_rcp.shutter_step = 8;
		else if (rcp->current_scene.shutter_step == 30) physical_rcp.shutter_step = 8;
		else physical_rcp.shutter_step = 0;
		send_shutter_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void shutter_type_changed_AW_UE150 (GtkComboBox *shutter_type_combo_box, rcp_t *rcp)
{
	rcp->current_scene.shutter_type = gtk_combo_box_get_active (shutter_type_combo_box);

	if (rcp->current_scene.shutter_type == 1) {
		gtk_widget_hide (rcp->ELC_limit_combo_box);
		gtk_widget_hide (rcp->ELC_limit_label);
		gtk_widget_hide (rcp->shutter_synchro_button);
		gtk_widget_show (rcp->shutter_step_combo_box);
	} else if (rcp->current_scene.shutter_type == 2) {
		gtk_widget_hide (rcp->ELC_limit_combo_box);
		gtk_widget_hide (rcp->ELC_limit_label);
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_show (rcp->shutter_synchro_button);
	} else if (rcp->current_scene.shutter_type == 3) {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
		gtk_widget_show (rcp->ELC_limit_label);
		gtk_widget_show (rcp->ELC_limit_combo_box);
	} else {
		gtk_widget_hide (rcp->shutter_step_combo_box);
		gtk_widget_hide (rcp->shutter_synchro_button);
		gtk_widget_hide (rcp->ELC_limit_label);
		gtk_widget_hide (rcp->ELC_limit_combo_box);
	}

	set_shutter_type_AW_UE150 (rcp);
}

void populate_shutter_step_combo_box_AW_UE150 (GtkComboBoxText *combo_box_text)
{
	gtk_combo_box_text_remove_all (combo_box_text);

	if ((output_fps_AW_UE150 == fps_23_98p) || (output_fps_AW_UE150 == fps_24p)) {
		gtk_combo_box_text_append_text (combo_box_text, "1/24");
		gtk_combo_box_text_append_text (combo_box_text, "1/48");
	} else if (output_fps_AW_UE150 == fps_25p) {
		gtk_combo_box_text_append_text (combo_box_text, "1/25");
		gtk_combo_box_text_append_text (combo_box_text, "1/50");
	} else if (output_fps_AW_UE150 == fps_29_97p) {
		gtk_combo_box_text_append_text (combo_box_text, "1/30");
	}

	gtk_combo_box_text_append_text (combo_box_text, "1/60");
	gtk_combo_box_text_append_text (combo_box_text, "1/100");
	gtk_combo_box_text_append_text (combo_box_text, "1/120");
	gtk_combo_box_text_append_text (combo_box_text, "1/250");
	gtk_combo_box_text_append_text (combo_box_text, "1/500");
	gtk_combo_box_text_append_text (combo_box_text, "1/1000");
	gtk_combo_box_text_append_text (combo_box_text, "1/2000");
	gtk_combo_box_text_append_text (combo_box_text, "1/4000");
	gtk_combo_box_text_append_text (combo_box_text, "1/8000");
	gtk_combo_box_text_append_text (combo_box_text, "1/10000");
}

void update_shutter_step_combo_box_AW_UE150 (rcp_t *rcp)
{
		g_signal_handler_block (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
		if ((output_fps_AW_UE150 == fps_23_98p) || (output_fps_AW_UE150 == fps_24p)) {
			if (rcp->current_scene.shutter_step == 24) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 0);
			else if (rcp->current_scene.shutter_step == 48) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 1);
			else if (rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 2);
			else if (rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 3);
			else if (rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 4);
			else if (rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 5);
			else if (rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 6);
			else if (rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 7);
			else if (rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 8);
			else if (rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 9);
			else if (rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 10);
			else gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 11);
		} else if (output_fps_AW_UE150 == fps_25p) {
			if (rcp->current_scene.shutter_step == 25) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 0);
			else if (rcp->current_scene.shutter_step == 50) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 1);
			else if (rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 2);
			else if (rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 3);
			else if (rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 4);
			else if (rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 5);
			else if (rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 6);
			else if (rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 7);
			else if (rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 8);
			else if (rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 9);
			else if (rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 10);
			else gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 11);
		} else if (output_fps_AW_UE150 == fps_29_97p) {
			if (rcp->current_scene.shutter_step == 30) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 0);
			else if (rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 1);
			else if (rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 2);
			else if (rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 3);
			else if (rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 4);
			else if (rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 5);
			else if (rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 6);
			else if (rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 7);
			else if (rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 8);
			else if (rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 9);
			else gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 10);
		} else {
			if (rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 0);
			else if (rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 1);
			else if (rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 2);
			else if (rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 3);
			else if (rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 4);
			else if (rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 5);
			else if (rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 6);
			else if (rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 7);
			else if (rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 8);
			else gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), 9);
		}
		g_signal_handler_unblock (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
}

void set_shutter_step_AW_UE150 (rcp_t *rcp)
{
	send_cam_control_command_4_digits (rcp, "OSJ:06:", rcp->current_scene.shutter_step, TRUE);

	if (rcp->current_scene.shutter_type == 1) {
		if ((rcp == rcp_vision) && physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.shutter_type = 1;
			if (rcp->current_scene.shutter_step == 60) physical_rcp.shutter_step = 0;
			else if (rcp->current_scene.shutter_step == 100) physical_rcp.shutter_step = 0;
			else if (rcp->current_scene.shutter_step == 120) physical_rcp.shutter_step = 1;
			else if (rcp->current_scene.shutter_step == 250) physical_rcp.shutter_step = 2;
			else if (rcp->current_scene.shutter_step == 500) physical_rcp.shutter_step = 3;
			else if (rcp->current_scene.shutter_step == 1000) physical_rcp.shutter_step = 4;
			else if (rcp->current_scene.shutter_step == 2000) physical_rcp.shutter_step = 5;
			else if (rcp->current_scene.shutter_step == 4000) physical_rcp.shutter_step = 6;
			else if (rcp->current_scene.shutter_step == 8000) physical_rcp.shutter_step = 6;
			else if (rcp->current_scene.shutter_step == 10000) physical_rcp.shutter_step = 7;
			else if (rcp->current_scene.shutter_step == 24) physical_rcp.shutter_step = 8;
			else if (rcp->current_scene.shutter_step == 25) physical_rcp.shutter_step = 8;
			else if (rcp->current_scene.shutter_step == 30) physical_rcp.shutter_step = 8;
			else physical_rcp.shutter_step = 0;
			send_shutter_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}
}

void shutter_step_changed_AW_UE150 (GtkComboBox *shutter_step_combo_box, rcp_t *rcp)
{
	int active_item = gtk_combo_box_get_active (shutter_step_combo_box);

	if ((output_fps_AW_UE150 == fps_23_98p) || (output_fps_AW_UE150 == fps_24p)) {
		if (active_item == 0) {
			rcp->current_scene.shutter_step = 24;
			set_shutter_step_AW_UE150 (rcp);
			return;
		} else if (active_item == 1) {
			rcp->current_scene.shutter_step = 48;
			set_shutter_step_AW_UE150 (rcp);
			return;
		} else active_item -= 2;
	} else if (output_fps_AW_UE150 == fps_25p) {
		if (active_item == 0) {
			rcp->current_scene.shutter_step = 25;
			set_shutter_step_AW_UE150 (rcp);
			return;
		} else if (active_item == 1) {
			rcp->current_scene.shutter_step = 50;
			set_shutter_step_AW_UE150 (rcp);
			return;
		} else active_item -= 2;
	} else if (output_fps_AW_UE150 == fps_29_97p) {
		if (active_item == 0) {
			rcp->current_scene.shutter_step = 30;
			set_shutter_step_AW_UE150 (rcp);
			return;
		} else active_item--;
	}

	if (active_item == 0) rcp->current_scene.shutter_step = 60;
	else if (active_item == 1) rcp->current_scene.shutter_step = 100;
	else if (active_item == 2) rcp->current_scene.shutter_step = 120;
	else if (active_item == 3) rcp->current_scene.shutter_step = 250;
	else if (active_item == 4) rcp->current_scene.shutter_step = 500;
	else if (active_item == 5) rcp->current_scene.shutter_step = 1000;
	else if (active_item == 6) rcp->current_scene.shutter_step = 2000;
	else if (active_item == 7) rcp->current_scene.shutter_step = 4000;
	else if (active_item == 8) rcp->current_scene.shutter_step = 8000;
	else rcp->current_scene.shutter_step = 10000;

	set_shutter_step_AW_UE150 (rcp);
}

void set_shutter_synchro_label_AW_UE150 (rcp_t *rcp)
{
	char label[24];

	sprintf (label, "Synchro: %5.1f Hz", (float)rcp->current_scene.shutter_synchro / 10.0);

	gtk_frame_set_label (GTK_FRAME (rcp->shutter_synchro_frame), label);
	gtk_button_set_label (GTK_BUTTON (rcp->shutter_synchro_button), label + 9);
}

#define MIN_VALUE 600
#define MAX_VALUE 72000

#define set_shutter_synchro_label_59_94Hz_AW_UE150 set_shutter_synchro_label_AW_UE150

CAM_CMD_FUNCS_S(shutter_synchro,"OSJ:09:",5,59_94Hz_AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,59_94Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,59_94Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,59_94Hz_AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,59_94Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,59_94Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,59_94Hz_AW_UE150)

void create_shutter_synchro_window_59_94Hz_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_59_94Hz_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_59_94Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_59_94Hz_AW_UE150), rcp);

			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_59_94Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_59_94Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_59_94Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_59_94Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 500
#define MAX_VALUE 72000

#define set_shutter_synchro_label_50Hz_AW_UE150 set_shutter_synchro_label_AW_UE150

CAM_CMD_FUNCS_S(shutter_synchro,"OSJ:09:",5,50Hz_AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,50Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,50Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,50Hz_AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,50Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,50Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,50Hz_AW_UE150)

void create_shutter_synchro_window_50Hz_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_50Hz_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_50Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 300
#define MAX_VALUE 72000

#define set_shutter_synchro_label_29_97Hz_AW_UE150 set_shutter_synchro_label_AW_UE150

CAM_CMD_FUNCS_S(shutter_synchro,"OSJ:09:",5,29_97Hz_AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,29_97Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,29_97Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,29_97Hz_AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,29_97Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,29_97Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,29_97Hz_AW_UE150)

void create_shutter_synchro_window_29_97Hz_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_29_97Hz_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_29_97Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 250
#define MAX_VALUE 72000

#define set_shutter_synchro_label_25Hz_AW_UE150 set_shutter_synchro_label_AW_UE150

CAM_CMD_FUNCS_S(shutter_synchro,"OSJ:09:",5,25Hz_AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,25Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,25Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,25Hz_AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,25Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,25Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,25Hz_AW_UE150)

void create_shutter_synchro_window_25Hz_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_25Hz_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_25Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 240
#define MAX_VALUE 72000

#define set_shutter_synchro_label_24Hz_AW_UE150 set_shutter_synchro_label_AW_UE150

CAM_CMD_FUNCS_S(shutter_synchro,"OSJ:09:",5,24Hz_AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,24Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,24Hz_AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,24Hz_AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,1,24Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,10,24Hz_AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(shutter_synchro,"OSJ:09:",5,50,24Hz_AW_UE150)

void create_shutter_synchro_window_24Hz_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 50);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->shutter_synchro_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (shutter_synchro_value_changed_24Hz_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_50_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_10_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_minus_1_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_1_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_10_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+50");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (shutter_synchro_plus_50_button_pressed_24Hz_AW_UE150), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->shutter_synchro_frame = frame;
	rcp->shutter_synchro_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

void set_shutter_synchro_AW_UE150 (rcp_t *rcp)
{
	if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) set_shutter_synchro_59_94Hz_AW_UE150 (rcp);
	else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) set_shutter_synchro_50Hz_AW_UE150 (rcp);
	else if (output_fps_AW_UE150 == fps_29_97p) set_shutter_synchro_29_97Hz_AW_UE150 (rcp);
	else if (output_fps_AW_UE150 == fps_25p) set_shutter_synchro_25Hz_AW_UE150 (rcp);
	else set_shutter_synchro_24Hz_AW_UE150 (rcp);
}

void create_shutter_synchro_window_AW_UE150 (rcp_t *rcp)
{
	if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) create_shutter_synchro_window_59_94Hz_AW_UE150 (rcp);
	else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) create_shutter_synchro_window_50Hz_AW_UE150 (rcp);
	else if (output_fps_AW_UE150 == fps_29_97p) create_shutter_synchro_window_29_97Hz_AW_UE150 (rcp);
	else if (output_fps_AW_UE150 == fps_25p) create_shutter_synchro_window_25Hz_AW_UE150 (rcp);
	else create_shutter_synchro_window_24Hz_AW_UE150 (rcp);
}

void set_ELC_limit (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSD:BF:", rcp->current_scene.ELC_limit + 2, TRUE);
}

void ELC_limit_changed (GtkComboBox *ELC_limit_combo_box, rcp_t *rcp)
{
	rcp->current_scene.ELC_limit = gtk_combo_box_get_active (ELC_limit_combo_box);

	set_ELC_limit (rcp);
}

GtkWidget *create_shutter_frame_AW_UE150 (rcp_t *rcp)
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
		rcp->shutter_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_type_changed_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_type_combo_box = widget;

		widget = gtk_combo_box_text_new ();

		populate_shutter_step_combo_box_AW_UE150 (GTK_COMBO_BOX_TEXT (widget));

		rcp->shutter_step_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_step_changed_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_step_combo_box = widget;

		create_shutter_synchro_window_AW_UE150 (rcp);

		widget = gtk_button_new ();
		g_signal_connect_swapped (widget, "clicked", G_CALLBACK (show_rcp_shutter_synchro_window), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->shutter_synchro_button = widget;

		rcp->ELC_limit_label = gtk_label_new ("Limit");
		gtk_widget_set_halign (rcp->ELC_limit_label, GTK_ALIGN_START);
		gtk_box_pack_start (GTK_BOX (box), rcp->ELC_limit_label, FALSE, FALSE, 0);

		widget = gtk_combo_box_text_new ();
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/100");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/120");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/250");
		rcp->ELC_limit_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (ELC_limit_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->ELC_limit_combo_box = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

