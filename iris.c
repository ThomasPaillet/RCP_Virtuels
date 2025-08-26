/*
 * copyright (c) 2018-2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "iris.h"

#include "main_window.h"
#include "misc.h"
#include "physical_rcp.h"
#include "protocol.h"
#include "ptz_cmd_define.h"
#include "sw_p_08.h"


#define MIN_VALUE 0x555
#define MAX_VALUE 0xFFF

PTZ_CMD_FUNCS(iris,"#AXI",3)

BUTTON_PRESSED_PLUS_FUNC(iris,"#AXI",3,50)
BUTTON_PRESSED_PLUS_FUNC(iris,"#AXI",3,20)
BUTTON_PRESSED_PLUS_FUNC(iris,"#AXI",3,10)
BUTTON_PRESSED_PLUS_FUNC(iris,"#AXI",3,1)

BUTTON_PRESSED_MINUS_FUNC(iris,"#AXI",3,1)
BUTTON_PRESSED_MINUS_FUNC(iris,"#AXI",3,10)
BUTTON_PRESSED_MINUS_FUNC(iris,"#AXI",3,20)
BUTTON_PRESSED_MINUS_FUNC(iris,"#AXI",3,50)

void set_iris_auto (rcp_t *rcp)
{
	if (rcp->current_scene.iris_auto) send_cam_control_command (rcp, "ORS:1");
	else send_cam_control_command (rcp, "ORS:0");

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.iris_auto = rcp->current_scene.iris_auto;
		send_iris_auto_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void iris_auto_toggle_button_clicked (GtkToggleButton *iris_auto_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (iris_auto_toggle_button)) {
		gtk_widget_set_sensitive (rcp->iris_sensitive_widgets, FALSE);
		rcp->current_scene.iris_auto = TRUE;
	} else {
		rcp->current_scene.iris_auto = FALSE;
		gtk_widget_set_sensitive (rcp->iris_sensitive_widgets, TRUE);
	}

	set_iris_auto (rcp);
}

GtkWidget *create_iris_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *box3, *widget;

	frame = gtk_frame_new ("Iris");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, MARGIN_VALUE);
		box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
			gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			gtk_range_set_value (GTK_RANGE (widget), IRIS_DEFAULT);
			rcp->iris_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (iris_value_changed), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			rcp->iris_scale = widget;

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("+50");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_50_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_plus_50_button = widget;

				widget = gtk_button_new_with_label ("+20");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_20_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_plus_20_button = widget;

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_plus_10_button = widget;

				widget = gtk_button_new_with_label ("+1");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_plus_1_button = widget;

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_minus_1_button = widget;

				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_minus_10_button = widget;

				widget = gtk_button_new_with_label ("-20");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_20_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_minus_20_button = widget;

				widget = gtk_button_new_with_label ("-50");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_50_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_white), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->iris_minus_50_button = widget;
			gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		rcp->iris_sensitive_widgets = box3;

		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
			widget = gtk_toggle_button_new_with_label ("Auto");
			rcp->iris_auto_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (iris_auto_toggle_button_clicked), rcp);
			gtk_box_set_center_widget (GTK_BOX (box2), widget);
			rcp->iris_auto_toggle_button = widget;
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

