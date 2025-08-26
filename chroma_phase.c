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
 * GNU General Public License for more chroma_phases.

 * You should have received a copy of the GNU General Public License
 * along with RCP-Virtuels. If not, see <https://www.gnu.org/licenses/>.
*/

#include "chroma_phase.h"

#include "cam_cmd_define.h"
#include "cameras_set.h"
#include "protocol.h"
#include "main_window.h"
#include "misc.h"


#define MIN_VALUE 0x61
#define MAX_VALUE 0x9F

void set_chroma_phase_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.chroma_phase == 0x80) gtk_frame_set_label (GTK_FRAME (rcp->chroma_phase_label_frame), "Chroma Phase: 0");
	else {
		sprintf (label, "%s%+d", "Chroma Phase: ", rcp->current_scene.chroma_phase - 0x80);
		gtk_frame_set_label (GTK_FRAME (rcp->chroma_phase_label_frame), label);
	}
}

CAM_CMD_FUNCS(chroma_phase,"OSJ:0B:",2)

BUTTON_PRESSED_MINUS_FUNC(chroma_phase,"OSJ:0B:",2,10)
BUTTON_PRESSED_MINUS_FUNC(chroma_phase,"OSJ:0B:",2,1)

void chroma_phase_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->chroma_phase_scale), 0x80);
}

BUTTON_PRESSED_PLUS_FUNC(chroma_phase,"OSJ:0B:",2,1)
BUTTON_PRESSED_PLUS_FUNC(chroma_phase,"OSJ:0B:",2,10)

void create_chroma_phase_window (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box1, *box2, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
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
		gtk_container_set_border_width (GTK_CONTAINER (frame), WINDOW_MARGIN_VALUE);
		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, MIN_VALUE, MAX_VALUE, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->chroma_phase_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (chroma_phase_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
			rcp->chroma_phase_scale = widget;

			box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box2, MARGIN_VALUE);
			gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (chroma_phase_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (chroma_phase_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (chroma_phase_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (chroma_phase_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (chroma_phase_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->chroma_phase_label_frame = frame;
	rcp->chroma_phase_window = window;
}

#undef MIN_VALUE
#undef MAX_VALUE

void show_rcp_chroma_phase_window (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	popup_window = rcp->chroma_phase_window;
	gtk_widget_show_all (rcp->chroma_phase_window);
}

GtkWidget *create_chroma_phase_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *button;

	create_chroma_phase_window (rcp);

	frame = gtk_frame_new ("Chroma");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
		box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box, MARGIN_VALUE);
		gtk_widget_set_margin_end (box, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
			button = gtk_button_new_with_label ("Phase");
			g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (show_rcp_chroma_phase_window), rcp);
		gtk_box_set_center_widget (GTK_BOX (box), button);
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

