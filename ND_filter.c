/*
 * copyright (c) 2021-2022 Thomas Paillet <thomas.paillet@net-c.fr>

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
#include "ND_filter.h"

#include "protocol.h"

#include "sw_p_08.h"
#include "physical_rcp.h"


char *ND_filter_label = "Filtre neutre";


void set_ND_filter (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OFT:", rcp->current_scene.ND_filter, TRUE);

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.ND_filter = rcp->current_scene.ND_filter;
		send_ND_filter_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void ND_filter_changed_AW_HE130 (GtkComboBox *ND_filter_combo_box, rcp_t *rcp)
{
	switch (gtk_combo_box_get_active (ND_filter_combo_box)) {
		case 1: rcp->current_scene.ND_filter = 4;
			break;
		case 2: rcp->current_scene.ND_filter = 3;
			break;
		default: rcp->current_scene.ND_filter = 0;
	}

	set_ND_filter (rcp);
}

GtkWidget *create_ND_filter_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *widget;

	frame = gtk_frame_new (ND_filter_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Through");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/8");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/64");
		rcp->ND_filter_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (ND_filter_changed_AW_HE130), rcp);
		rcp->ND_filter_combo_box = widget;
	gtk_container_add (GTK_CONTAINER (frame), widget);

	return frame;
}

void ND_filter_changed_AW_UE150 (GtkComboBox *ND_filter_combo_box, rcp_t *rcp)
{
	rcp->current_scene.ND_filter = gtk_combo_box_get_active (ND_filter_combo_box);

	set_ND_filter (rcp);
}

GtkWidget *create_ND_filter_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *widget;

	frame = gtk_frame_new (ND_filter_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Through");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/4");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/16");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/64");
		rcp->ND_filter_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (ND_filter_changed_AW_UE150), rcp);
		rcp->ND_filter_combo_box = widget;
	gtk_container_add (GTK_CONTAINER (frame), widget);

	return frame;
}

gboolean update_ND_filter_combo_box (rcp_t *rcp)
{
	int index;

	if (rcp->model == AW_HE130) {
		if (rcp->current_scene.ND_filter == 4) index = 1;
		else if (rcp->current_scene.ND_filter == 3) index = 2;
		else index = 0;
	} else index = rcp->current_scene.ND_filter;

	g_signal_handler_block (rcp->ND_filter_combo_box, rcp->ND_filter_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), index);
	g_signal_handler_unblock (rcp->ND_filter_combo_box, rcp->ND_filter_handler_id);

	return G_SOURCE_REMOVE;
}

