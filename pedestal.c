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
#include "pedestal.h"
#include "cam_cmd_define.h"

#include "protocol.h"
#include "misc.h"

#include "main_window.h"


#define MIN_VALUE 0x000
#define MAX_VALUE 0x12C

void set_pedestal_label_AW_HE130 (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.pedestal == 0x096) gtk_frame_set_label (GTK_FRAME (rcp->pedestal_frame), "Pedestal: 0");
	else {
		sprintf (label, "Pedestal: %+d", rcp->current_scene.pedestal - 0x096);
		gtk_frame_set_label (GTK_FRAME (rcp->pedestal_frame), label);
	}
}

CAM_CMD_FUNCS_S(pedestal,"OTP:",3,AW_HE130)

BUTTON_PRESSED_PLUS_FUNC_S(pedestal,"OTP:",3,10,AW_HE130)
BUTTON_PRESSED_PLUS_FUNC_S(pedestal,"OTP:",3,1,AW_HE130)

BUTTON_PRESSED_MINUS_FUNC_S(pedestal,"OTP:",3,1,AW_HE130)
BUTTON_PRESSED_MINUS_FUNC_S(pedestal,"OTP:",3,10,AW_HE130)

void pedestal_raz_button_clicked_AW_HE130 (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_HE130);
}

GtkWidget *create_pedestal_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;

	frame = gtk_frame_new ("Pedestal: 0");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
			gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			gtk_range_set_value (GTK_RANGE (widget), PEDESTAL_DEFAULT_AW_HE130);
			rcp->pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (pedestal_value_changed_AW_HE130), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
			rcp->pedestal_scale = widget;

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_10_button_pressed_AW_HE130), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_plus_10_button = widget;

				widget = gtk_button_new_with_label ("+1");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_1_button_pressed_AW_HE130), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_plus_1_button = widget;

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_1_button_pressed_AW_HE130), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_minus_1_button = widget;

				widget = gtk_button_new_with_label ("-10");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_10_button_pressed_AW_HE130), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_minus_10_button = widget;
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("raz");
				g_signal_connect (widget, "clicked", G_CALLBACK (pedestal_raz_button_clicked_AW_HE130), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->pedestal_frame = frame;

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x738
#define MAX_VALUE 0x8C8

void set_pedestal_label_AW_UE150 (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.pedestal == 0x800) gtk_frame_set_label (GTK_FRAME (rcp->pedestal_frame), "Pedestal: 0");
	else {
		sprintf (label, "Pedestal: %+d", rcp->current_scene.pedestal - 0x800);
		gtk_frame_set_label (GTK_FRAME (rcp->pedestal_frame), label);
	}
}

CAM_CMD_FUNCS_S(pedestal,"OSJ:0F:",3,AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(pedestal,"OSJ:0F:",3,10,AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(pedestal,"OSJ:0F:",3,1,AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(pedestal,"OSJ:0F:",3,1,AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(pedestal,"OSJ:0F:",3,10,AW_UE150)

void pedestal_raz_button_clicked_AW_UE150 (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_UE150);
}

GtkWidget *create_pedestal_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;

	frame = gtk_frame_new ("Pedestal: 0");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
			gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
			gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			gtk_range_set_value (GTK_RANGE (widget), PEDESTAL_DEFAULT_AW_UE150);
			rcp->pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (pedestal_value_changed_AW_UE150), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
			rcp->pedestal_scale = widget;

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_10_button_pressed_AW_UE150), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_plus_10_button = widget;

				widget = gtk_button_new_with_label ("+1");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_1_button_pressed_AW_UE150), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_plus_1_button = widget;

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_1_button_pressed_AW_UE150), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_minus_1_button = widget;

				widget = gtk_button_new_with_label ("-10");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_10_button_pressed_AW_UE150), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				rcp->pedestal_minus_10_button = widget;
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("raz");
				g_signal_connect (widget, "clicked", G_CALLBACK (pedestal_raz_button_clicked_AW_UE150), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->pedestal_frame = frame;

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

