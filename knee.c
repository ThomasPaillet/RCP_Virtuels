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


char auto_knee_response_tooltip[] = "Permet de régler la vitesse de la réponse du coude auto (auto knee).\nPlus la vitesse de réponse augmente, plus la valeur de réglage est basse.";


#define MIN_VALUE 0x22
#define MAX_VALUE 0xB6

void set_knee_point_label (rcp_t *rcp)
{
	float value;
	char label[8];

	value = 70.0 + (float)(rcp->current_scene.knee_point - 0x22) * 0.25;
	sprintf (label, "%5.2f%%", value);

	gtk_label_set_text (GTK_LABEL (rcp->knee_point_value), label);
}

CAM_CMD_FUNCS(knee_point,"OSA:20:",2)

BUTTON_PRESSED_PLUS_FUNC(knee_point,"OSA:20:",2,10)
BUTTON_PRESSED_PLUS_FUNC(knee_point,"OSA:20:",2,1)

BUTTON_PRESSED_MINUS_FUNC(knee_point,"OSA:20:",2,1)
BUTTON_PRESSED_MINUS_FUNC(knee_point,"OSA:20:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x00
#define MAX_VALUE 0x63

void set_knee_slope_label (rcp_t *rcp)
{
	char label[4];

	sprintf (label, "%d", rcp->current_scene.knee_slope);

	gtk_label_set_text (GTK_LABEL (rcp->knee_slope_value), label);
}

CAM_CMD_FUNCS(knee_slope,"OSA:24:",2)

BUTTON_PRESSED_PLUS_FUNC(knee_slope,"OSA:24:",2,10)
BUTTON_PRESSED_PLUS_FUNC(knee_slope,"OSA:24:",2,1)

BUTTON_PRESSED_MINUS_FUNC(knee_slope,"OSA:24:",2,1)
BUTTON_PRESSED_MINUS_FUNC(knee_slope,"OSA:24:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

void knee_point_slope_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(knee_point,KNEE_POINT_DEFAULT);

	if (rcp->model == AW_UE150) {
		RAZ_IHM_UPDATE_SCALE(knee_slope,KNEE_SLOPE_DEFAULT_AW_UE150)
	} else {
		RAZ_IHM_UPDATE_SCALE(knee_slope,KNEE_SLOPE_DEFAULT_AW_HE130)
	}
}

void create_knee_window (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new ("Knee");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x22, 0xB6, 1.0);
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->knee_point_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (knee_point_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->knee_point_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Point");
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			rcp->knee_point_value = gtk_label_new (NULL);
			gtk_box_pack_start (GTK_BOX (box2), rcp->knee_point_value, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+2.5");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_point_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+0.25");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_point_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-0.25");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_point_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-2.5");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_point_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x00, 0x63, 1.0);
		gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->knee_slope_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (knee_slope_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->knee_slope_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Slope");
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			rcp->knee_slope_value = gtk_label_new (NULL);
			gtk_box_pack_start (GTK_BOX (box2), rcp->knee_slope_value, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_slope_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_slope_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_slope_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_slope_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (knee_point_slope_raz_button_clicked), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->knee_window = window;
}

void set_auto_knee_response_label (rcp_t *rcp)
{
	char label[24];

	sprintf (label, "Auto Knee Response: %d", rcp->current_scene.auto_knee_response);
	gtk_frame_set_label (GTK_FRAME (rcp->auto_knee_response_frame), label);
}

void set_auto_knee_response (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSG:97:", rcp->current_scene.auto_knee_response, TRUE);
}

gboolean set_auto_knee_response_delayed (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	send_cam_control_command_1_digit (rcp, "OSG:97:", rcp->current_scene.auto_knee_response, FALSE);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
}

void auto_knee_response_value_changed (GtkRange *auto_knee_response_scale, rcp_t *rcp)
{
	int value;
	struct timeval current_time, elapsed_time;

	value = (int)gtk_range_get_value (auto_knee_response_scale);

	if (rcp->current_scene.auto_knee_response != value) {
		rcp->current_scene.auto_knee_response = value;

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_auto_knee_response_delayed, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}

			rcp->last_time = current_time;
			send_cam_control_command_1_digit (rcp, "OSG:97:", value, FALSE);
		}

		set_auto_knee_response_label (rcp);
	}
}

void create_auto_knee_response_window (rcp_t *rcp)
{
	GtkWidget *window, *frame, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
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
	gtk_widget_set_tooltip_text (frame, auto_knee_response_tooltip);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x1, 0x8, 1.0);
		gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->auto_knee_response_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (auto_knee_response_value_changed), rcp);
		rcp->auto_knee_response_scale = widget;
	gtk_container_add (GTK_CONTAINER (frame), widget);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->auto_knee_response_frame = frame;
	rcp->auto_knee_response_window = window;
}

void set_knee_settings (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSA:2D:", rcp->current_scene.knee_settings, TRUE);
}

void knee_settings_changed (GtkComboBox *knee_settings_combo_box, rcp_t *rcp)
{
	rcp->current_scene.knee_settings = gtk_combo_box_get_active (knee_settings_combo_box);

	if (rcp->current_scene.knee_settings == 1) gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	else if ((rcp->model == AW_UE150) && (rcp->current_scene.knee_settings == 2)) gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	else gtk_widget_set_sensitive (rcp->knee_button, FALSE);

	set_knee_settings (rcp);
}

void show_rcp_knee_window (rcp_t *rcp)
{

	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	if ((rcp->model == AW_UE150) && (rcp->current_scene.knee_settings == 2)) {
		popup_window = rcp->auto_knee_response_window;
		gtk_widget_show_all (rcp->auto_knee_response_window);
	} else {
		popup_window = rcp->knee_window;
		gtk_widget_show_all (rcp->knee_window);
	}
}

GtkWidget *create_knee_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("Knee");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "OFF");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "MANUAL");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "AUTO");
		rcp->knee_settings_handler_id = g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (knee_settings_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->knee_settings_combo_box = widget;

		create_knee_window (rcp);

		if (rcp->model == AW_UE150) create_auto_knee_response_window (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_knee_window), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->knee_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

#define MIN_VALUE 0x1C
#define MAX_VALUE 0xD0

void set_HLG_knee_point_label (rcp_t *rcp)
{
	float value;
	char label[8];

	value = 55.0 + (float)(rcp->current_scene.HLG_knee_point - 0x1C) * 0.25;
	sprintf (label, "%5.2f%%", value);

	gtk_label_set_text (GTK_LABEL (rcp->HLG_knee_point_value), label);
}

CAM_CMD_FUNCS(HLG_knee_point,"OSI:41:",2)

BUTTON_PRESSED_PLUS_FUNC(HLG_knee_point,"OSI:41:",2,10)
BUTTON_PRESSED_PLUS_FUNC(HLG_knee_point,"OSI:41:",2,1)

BUTTON_PRESSED_MINUS_FUNC(HLG_knee_point,"OSI:41:",2,1)
BUTTON_PRESSED_MINUS_FUNC(HLG_knee_point,"OSI:41:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x00
#define MAX_VALUE 0x64

void set_HLG_knee_slope_label (rcp_t *rcp)
{
	char label[4];

	sprintf (label, "%d", rcp->current_scene.HLG_knee_slope);

	gtk_label_set_text (GTK_LABEL (rcp->HLG_knee_slope_value), label);
}

CAM_CMD_FUNCS(HLG_knee_slope,"OSI:42:",2)

BUTTON_PRESSED_PLUS_FUNC(HLG_knee_slope,"OSI:42:",2,10)
BUTTON_PRESSED_PLUS_FUNC(HLG_knee_slope,"OSI:42:",2,1)

BUTTON_PRESSED_MINUS_FUNC(HLG_knee_slope,"OSI:42:",2,1)
BUTTON_PRESSED_MINUS_FUNC(HLG_knee_slope,"OSI:42:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

void HLG_knee_point_slope_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(HLG_knee_point,HLG_KNEE_POINT_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(HLG_knee_slope,HLG_KNEE_SLOPE_DEFAULT)
}

void create_HLG_knee_window_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new ("HLG Knee");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_widget_set_margin_start (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x1C, 0xD0, 1.0);
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->HLG_knee_point_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (HLG_knee_point_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->HLG_knee_point_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Point");
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			rcp->HLG_knee_point_value = gtk_label_new (NULL);
			gtk_box_pack_start (GTK_BOX (box2), rcp->HLG_knee_point_value, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+2.5");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_point_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+0.25");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_point_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-0.25");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_point_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-2.5");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_point_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x00, 0x64, 1.0);
		gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
		rcp->HLG_knee_slope_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (HLG_knee_slope_value_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->HLG_knee_slope_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Slope");
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			rcp->HLG_knee_slope_value = gtk_label_new (NULL);
			gtk_box_pack_start (GTK_BOX (box2), rcp->HLG_knee_slope_value, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_slope_plus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_slope_plus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1");
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_slope_minus_1_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-10");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (HLG_knee_slope_minus_10_button_pressed), rcp);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (HLG_knee_point_slope_raz_button_clicked), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->HLG_knee_window = window;
}

void set_HLG_knee (rcp_t *rcp)
{
	if (rcp->current_scene.HLG_knee) send_cam_control_command (rcp, "OSI:40:1");
	else send_cam_control_command (rcp, "OSI:40:0");
}

void HLG_knee_toggle_button_clicked (GtkToggleButton *HLG_knee_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (HLG_knee_toggle_button)) {
		gtk_button_set_label (GTK_BUTTON (rcp->HLG_knee_toggle_button), "On");
		gtk_widget_set_sensitive (rcp->HLG_knee_button, TRUE);
		rcp->current_scene.HLG_knee = TRUE;
	} else {
		rcp->current_scene.HLG_knee = FALSE;
		gtk_widget_set_sensitive (rcp->HLG_knee_button, FALSE);
		gtk_button_set_label (GTK_BUTTON (rcp->HLG_knee_toggle_button), "Off");
	}

	set_HLG_knee (rcp);
}

void show_rcp_HLG_knee_window (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	popup_window = rcp->HLG_knee_window;
	gtk_widget_show_all (rcp->HLG_knee_window);
}

GtkWidget *create_HLG_knee_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("HLG Knee");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_toggle_button_new_with_label ("Off");
		rcp->HLG_knee_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (HLG_knee_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->HLG_knee_toggle_button = widget;

		create_HLG_knee_window_AW_UE150 (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_HLG_knee_window), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->HLG_knee_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

