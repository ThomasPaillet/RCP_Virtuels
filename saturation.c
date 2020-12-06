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


char *saturation_label = "Saturation: ";


void set_saturation_label (rcp_t *rcp)
{
	char label[24];

	if (rcp->current_scene.saturation == 0x80) sprintf (label, "%s0%%", saturation_label);
	else sprintf (label, "%s%+d%%", saturation_label, rcp->current_scene.saturation - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->saturation_frame), label);
}

#define MIN_VALUE 0x1D
#define MAX_VALUE 0xA8

CAM_CMD_FUNCS(saturation,"OSD:B0:",2)

void saturation_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), SATURATION_DEFAULT);
}

GtkWidget *create_saturation_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *box3, *widget;

	frame = gtk_frame_new (saturation_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x1D, 0xA8, 1.0);
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

