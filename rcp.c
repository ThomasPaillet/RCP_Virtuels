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

#include <string.h>
#include <unistd.h>


gpointer start_camera (rcp_t *rcp)
{
	send_update_start_cmd (rcp);

	if (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR) send_ptz_control_command (rcp, "#O1");

	return NULL;
}

gpointer stop_camera (rcp_t *rcp)
{
	GSList *gslist_itr;

	send_update_start_cmd (rcp);

	if (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR) {
		if (send_ip_tally && rcp->ip_tally_is_on) {
			send_ptz_control_command (rcp, "#DA0");
			rcp->ip_tally_is_on = FALSE;

			for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next)
				((rcp_t*)(gslist_itr->data))->ip_tally_is_on = FALSE;
		}

		send_ptz_control_command (rcp, "#O0");
	}

	return NULL;
}

void on_standby_switch_activated (GtkSwitch *on_standby_switch, GParamSpec *pspec, rcp_t *rcp)
{
	if (gtk_switch_get_active (on_standby_switch)) {
		rcp_start_working (rcp);
		/*rcp->thread =*/ g_thread_new (NULL, (GThreadFunc)start_camera, rcp);
	} else rcp->thread = g_thread_new (NULL, (GThreadFunc)stop_camera, rcp);
}

void standard_button_clicked (GtkButton *button, rcp_t *rcp)
{
	rcp_start_working (rcp);

	if (rcp->model == AW_UE150) rcp->thread = g_thread_new (NULL, (GThreadFunc)load_standard_AW_UE150, rcp);
	else rcp->thread = g_thread_new (NULL, (GThreadFunc)load_standard_AW_HE130, rcp);
}

void mire_toggle_button_clicked (GtkToggleButton *mire_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (mire_toggle_button)) {
		rcp->mire = TRUE;
		send_cam_control_command (rcp, "DCB:1");
	} else {
		rcp->mire = FALSE;
		send_cam_control_command (rcp, "DCB:0");
	}

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.mire = rcp->mire;
		send_mire_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void day_night_toggle_button_clicked (GtkToggleButton *day_night_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (day_night_toggle_button)) {
		rcp->day_night = TRUE;
		send_ptz_control_command (rcp, "#D61");

		gtk_widget_set_sensitive (rcp->ND_filter_combo_box, FALSE);	//ND filter switching is not possible in Night mode
	} else {
		rcp->day_night = FALSE;
		send_ptz_control_command (rcp, "#D60");

		gtk_widget_set_sensitive (rcp->ND_filter_combo_box, TRUE);
	}
}

void scene_button_clicked (GtkButton *button, rcp_t *rcp)
{
	const gchar *label;
	int index;

	label = gtk_button_get_label (button);
	sscanf (label, "%d", &index);
	index--;

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rcp->store_toggle_button))) {
		rcp->scenes[index] = rcp->current_scene;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->store_toggle_button), FALSE);

		if (!triggered_by_master_rcp) save_settings_and_cameras_sets_to_config_file ();
	} else {
		rcp->scene_to_load = index;

		rcp_start_working (rcp);

		if (rcp->model == AW_UE150) rcp->thread = g_thread_new (NULL, (GThreadFunc)load_scene_AW_UE150, rcp);
		else rcp->thread = g_thread_new (NULL, (GThreadFunc)load_scene_AW_HE130, rcp);
	}
}

GtkWidget *create_scenes_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box2, *widget;
	char label[4];
	int i;

	frame = gtk_frame_new (scenes_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		rcp->scenes_bank_1_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		for (i = 0; i < 5; i++) {
			sprintf (label, "%d", i + 1);
			widget = gtk_button_new_with_label (label);
			g_signal_connect (widget, "clicked", G_CALLBACK (scene_button_clicked), rcp);
			gtk_box_pack_start (GTK_BOX (rcp->scenes_bank_1_box), widget, FALSE, FALSE, 0);
			rcp->scenes_button[i] = widget;
		}
		gtk_box_pack_start (GTK_BOX (box2), rcp->scenes_bank_1_box, FALSE, FALSE, 0);

		rcp->scenes_bank_2_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		for (i = 5; i < NB_SCENES; i++) {
			sprintf (label, "%d", i + 1);
			widget = gtk_button_new_with_label (label);
			g_signal_connect (widget, "clicked", G_CALLBACK (scene_button_clicked), rcp);
			gtk_box_pack_start (GTK_BOX (rcp->scenes_bank_2_box), widget, FALSE, FALSE, 0);
			rcp->scenes_button[i] = widget;
		}
		gtk_box_pack_start (GTK_BOX (box2), rcp->scenes_bank_2_box, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		widget = gtk_toggle_button_new_with_label ("S");
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_store), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
		gtk_box_pack_end (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->store_toggle_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box1);

	return frame;
}

void ABB_button_clicked (GtkButton *button, rcp_t *rcp)
{
	GSList *gslist_itr;

	if (send_ABB_execution_control_command (rcp)) {
		rcp_start_working (rcp);

		g_mutex_lock (&rcp->other_rcp_mutex);
		for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) rcp_start_working (gslist_itr->data);
		g_mutex_unlock (&rcp->other_rcp_mutex);
	} else {
		rcp->error_code = CAMERA_IS_UNREACHABLE_ERROR;
		camera_is_unreachable (rcp);
	}
}

gboolean ghost_rcp_draw (GtkWidget *widget, cairo_t *cr)
{
	if (theme) cairo_set_source_rgb (cr, 0.92549, 0.92549, 0.92549);
	else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);

	cairo_paint (cr);

	return GDK_EVENT_PROPAGATE;
}

gboolean rcp_button_press_event (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp)
{
	if (popup_window != NULL) {
		gtk_widget_hide (popup_window);
		popup_window = NULL;

		if (rcp_vision != NULL) {
			if (rcp_vision->active) gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
		}
		gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), FALSE);
	} else if (rcp != rcp_vision) {
		ask_to_connect_camera_to_ctrl_vision (rcp);

		if (rcp->active && physical_rcp.connected) update_physical_rcp (rcp);
	}

	return GDK_EVENT_PROPAGATE;
}

void create_ghost_rcp_widgets (rcp_t *rcp)
{
	GtkWidget *main_box_h, *tally_grid, *box, *widget;

	main_box_h = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	tally_grid = gtk_grid_new ();
		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 4, 20);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
		gtk_grid_attach (GTK_GRID (tally_grid), widget, 0, 0, 1, 3);
		rcp->tally_widgets[0] = widget;

		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 20, 4);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
		gtk_grid_attach (GTK_GRID (tally_grid), widget, 1, 0, 1, 1);
		rcp->tally_widgets[1] = widget;

		box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_drawing_area_new ();
			gtk_widget_set_size_request (widget, 60, 40);
			gtk_widget_set_events (widget, gtk_widget_get_events (widget) | GDK_BUTTON_PRESS_MASK);
			g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (name_draw), rcp);
			g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
			rcp->name_drawing_area = widget;
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

			widget = gtk_drawing_area_new ();
			gtk_widget_set_size_request (widget, 60, 743);
			gtk_widget_set_events (widget, gtk_widget_get_events (widget) | GDK_BUTTON_PRESS_MASK);
			g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (ghost_rcp_draw), NULL);
			g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

			widget = gtk_drawing_area_new ();
			gtk_widget_set_size_request (widget, 60, 243);
			gtk_widget_set_events (widget, gtk_widget_get_events (widget) | GDK_BUTTON_PRESS_MASK);
			g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (ghost_rcp_draw), NULL);
			g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
			rcp->ghost_rcp_body = widget;
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		gtk_grid_attach (GTK_GRID (tally_grid), box, 1, 1, 1, 1);

		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 4, 20);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
		gtk_grid_attach (GTK_GRID (tally_grid), widget, 2, 0, 1, 3);
		rcp->tally_widgets[2] = widget;

		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 20, 4);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
		gtk_grid_attach (GTK_GRID (tally_grid), widget, 1, 2, 1, 1);
		rcp->tally_widgets[3] = widget;
	gtk_box_pack_start (GTK_BOX (main_box_h), tally_grid, FALSE, FALSE, 0);

	widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start (GTK_BOX (main_box_h), widget, FALSE, FALSE, 0);

	rcp->root_widget = main_box_h;
}

void create_rcp_widgets (rcp_t *rcp)
{
	GtkWidget *main_box_h, *tally_grid, *main_box_v, *box, *widget;

	main_box_h = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	tally_grid = gtk_grid_new ();
	main_box_v = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	widget = gtk_drawing_area_new ();
	gtk_widget_set_size_request (widget, 4, 20);
	g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
	gtk_grid_attach (GTK_GRID (tally_grid), widget, 0, 0, 1, 3);
	rcp->tally_widgets[0] = widget;

	widget = gtk_drawing_area_new ();
	gtk_widget_set_size_request (widget, 20, 4);
	g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
	gtk_grid_attach (GTK_GRID (tally_grid), widget, 1, 0, 1, 1);
	rcp->tally_widgets[1] = widget;

	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 60, 40);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (name_draw), rcp);
		g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
		gtk_widget_set_events (widget, GDK_BUTTON_PRESS_MASK);
		gtk_box_set_center_widget (GTK_BOX (box), widget);
		rcp->name_drawing_area = widget;

		rcp->spinner = gtk_spinner_new ();
		gtk_widget_set_size_request (rcp->spinner, 40, 40);
		gtk_box_pack_end (GTK_BOX (box), rcp->spinner, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_box_v), box, FALSE, FALSE, 0);

	rcp->error_label = gtk_label_new (NULL);
	gtk_box_pack_start (GTK_BOX (main_box_v), rcp->error_label, FALSE, FALSE, 0);

	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_top (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_switch_new ();
		gtk_switch_set_active (GTK_SWITCH (widget), FALSE);
		rcp->on_standby_handler_id = g_signal_connect (widget, "notify::active", G_CALLBACK (on_standby_switch_activated), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->on_standby_switch = widget;

		widget = gtk_button_new_with_label ("Standard");
		g_signal_connect (widget, "clicked", G_CALLBACK (standard_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->standard_button = widget;

		widget = gtk_toggle_button_new_with_label ("Mire");
		rcp->mire_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (mire_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->mire_toggle_button = widget;

		widget = gtk_toggle_button_new_with_label ("Nuit");
		rcp->day_night_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (day_night_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->day_night_toggle_button = widget;
	gtk_box_pack_start (GTK_BOX (main_box_v), box, FALSE, FALSE, 0);

	if (rcp->model == AW_UE150) create_rcp_widgets_AW_UE150 (rcp);
	else create_rcp_widgets_AW_HE130 (rcp);

	gtk_box_pack_start (GTK_BOX (main_box_v), rcp->sensitive_widgets, FALSE, FALSE, 0);

	widget = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget), FALSE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (widget), TRUE);
	g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
	gtk_container_add (GTK_CONTAINER (widget), main_box_v);
	rcp->main_box = main_box_v;

	gtk_grid_attach (GTK_GRID (tally_grid), widget, 1, 1, 1, 1);
	rcp->event_box = widget;

	widget = gtk_drawing_area_new ();
	gtk_widget_set_size_request (widget, 4, 20);
	g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
	gtk_grid_attach (GTK_GRID (tally_grid), widget, 2, 0, 1, 3);
	rcp->tally_widgets[2] = widget;

	widget = gtk_drawing_area_new ();
	gtk_widget_set_size_request (widget, 20, 4);
	g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (tally_draw), rcp);
	gtk_grid_attach (GTK_GRID (tally_grid), widget, 1, 2, 1, 1);
	rcp->tally_widgets[3] = widget;

	gtk_box_pack_start (GTK_BOX (main_box_h), tally_grid, FALSE, FALSE, 0);

	widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start (GTK_BOX (main_box_h), widget, FALSE, FALSE, 0);

	rcp->root_widget = main_box_h;
}

