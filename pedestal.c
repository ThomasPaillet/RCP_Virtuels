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

#include "cam_cmd_define.h"


extern GtkCssProvider *css_provider_black, *css_provider_raz;

char *pedestal_label = "Piédestal: ";


void set_pedestal_label (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.pedestal == 0x096) sprintf (label, "%s0", pedestal_label);
	else sprintf (label, "%s%+d", pedestal_label, rcp->current_scene.pedestal - 0x096);

	gtk_frame_set_label (GTK_FRAME (rcp->pedestal_frame), label);
}

#define MIN_VALUE 0x000
#define MAX_VALUE 0x12C

CAM_CMD_FUNCS(pedestal,"OTP:",3)

void pedestal_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT);
}

GtkWidget *create_pedestal_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;

	frame = gtk_frame_new (pedestal_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box1, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x000, 0x12C, 1.0);
			gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			gtk_range_set_value (GTK_RANGE (widget), PEDESTAL_DEFAULT);
			rcp->pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (pedestal_value_changed), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
			rcp->pedestal_scale = widget;

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-10");
				gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_black), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box2, MARGIN_VALUE);
				widget = gtk_button_new_with_label ("raz");
				g_signal_connect (widget, "clicked", G_CALLBACK (pedestal_raz_button_clicked), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	rcp->pedestal_frame = frame;

	return frame;
}

