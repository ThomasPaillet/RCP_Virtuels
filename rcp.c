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

#include <string.h>
#include <unistd.h>


extern GtkCssProvider *css_provider_store;


gpointer start_camera (rcp_t *rcp)
{
	if (rcp->error_code == 0x30) send_update_start_cmd (rcp);

	if (rcp->error_code != 0x30) send_ptz_control_command (rcp, "#O1");

	return NULL;
}

gpointer stop_camera (rcp_t *rcp)
{
	if (rcp->error_code == 0x30) send_update_start_cmd (rcp);

	if (rcp->error_code != 0x30) send_ptz_control_command (rcp, "#O0");

	return NULL;
}

void on_standby_switch_activated (GtkSwitch *on_standby_switch, GParamSpec *pspec, rcp_t *rcp)
{
	if (rcp->thread != NULL) g_thread_join (rcp->thread);

	if (gtk_switch_get_active (on_standby_switch)) rcp->thread = g_thread_new (NULL, (GThreadFunc)start_camera, rcp);
	else rcp->thread = g_thread_new (NULL, (GThreadFunc)stop_camera, rcp);
}

#define SET_RCP_PARAMETER(l,v) \
	if (rcp->current_scene.l != v) { \
		rcp->current_scene.l = v; \
		set_##l (rcp); \
	}

#define SET_RCP_LINEAR_MATRIX_PARAMETER(l,v) \
	if (rcp->current_scene.linear_matrix.l != v) { \
		rcp->current_scene.linear_matrix.l = v; \
		set_linear_matrix_##l (rcp); \
	}

#define SET_RCP_CC_SATURATION_PARAMETER(l,v) \
	if (rcp->current_scene.cc_saturation[l] != v) { \
		rcp->current_scene.cc_saturation[l] = v; \
		set_cc_saturation_##l (rcp, TRUE); \
	}

#define SET_RCP_CC_PHASE_PARAMETER(l,v) \
	if (rcp->current_scene.cc_phase[l] != v) { \
		rcp->current_scene.cc_phase[l] = v; \
		set_cc_phase_##l (rcp, TRUE); \
	}

gpointer load_standard (rcp_t *rcp)
{
	GSList *gslist_itr;

	if (rcp->mire) {
		rcp->mire = FALSE;
		send_cam_control_command (rcp, "DCB:0");
	}
	if (rcp->day_night) {
		rcp->day_night = FALSE;
		send_ptz_control_command (rcp, "#D60");
	}

	SET_RCP_PARAMETER(ND_filter,ND_FILTER_DEFAULT)
	SET_RCP_PARAMETER(gain,GAIN_DEFAULT)

	SET_RCP_PARAMETER(gamma_type,GAMMA_TYPE_DEFAULT)
	SET_RCP_PARAMETER(gamma,GAMMA_DEFAULT)
	SET_RCP_PARAMETER(drs,DRS_DEFAULT)

	SET_RCP_PARAMETER(color_temperature,COLOR_TEMPERATURE_DEFAULT)

	SET_RCP_PARAMETER(knee_settings,KNEE_SETTINGS_DEFAULT)
	SET_RCP_PARAMETER(knee_point,KNEE_POINT_DEFAULT)
	SET_RCP_PARAMETER(knee_slope,KNEE_SLOPE_DEFAULT)

	SET_RCP_PARAMETER(matrix_type,MATRIX_TYPE_DEFAULT)

	SET_RCP_LINEAR_MATRIX_PARAMETER(R_G,LINEAR_MATRIX_DEFAULT)
	SET_RCP_LINEAR_MATRIX_PARAMETER(R_B,LINEAR_MATRIX_DEFAULT)
	SET_RCP_LINEAR_MATRIX_PARAMETER(G_R,LINEAR_MATRIX_DEFAULT)
	SET_RCP_LINEAR_MATRIX_PARAMETER(G_B,LINEAR_MATRIX_DEFAULT)
	SET_RCP_LINEAR_MATRIX_PARAMETER(B_R,LINEAR_MATRIX_DEFAULT)
	SET_RCP_LINEAR_MATRIX_PARAMETER(B_G,LINEAR_MATRIX_DEFAULT)

	SET_RCP_CC_SATURATION_PARAMETER(R,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(R_R_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(R_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(R_Yl_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Yl_G,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(G,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(G_Cy,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Cy,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Cy_B,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(B,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(B_Mg,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Mg,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Mg_R,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_SATURATION_PARAMETER(Mg_R_R,COLOR_CORRECTION_DEFAULT)

	SET_RCP_CC_PHASE_PARAMETER(R,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(R_R_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(R_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(R_Yl_Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Yl,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Yl_G,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(G,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(G_Cy,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Cy,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Cy_B,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(B,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(B_Mg,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Mg,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Mg_R,COLOR_CORRECTION_DEFAULT)
	SET_RCP_CC_PHASE_PARAMETER(Mg_R_R,COLOR_CORRECTION_DEFAULT)

	SET_RCP_PARAMETER(detail,DETAIL_DEFAULT)
	SET_RCP_PARAMETER(master_detail,MASTER_DETAIL_DEFAULT)
	SET_RCP_PARAMETER(v_detail_level,V_DETAIL_LEVEL_DEFAULT)
	SET_RCP_PARAMETER(detail_band,DETAIL_BAND_DEFAULT)
	SET_RCP_PARAMETER(noise_suppress,NOISE_SUPPRESS_DEFAULT)
	SET_RCP_PARAMETER(fleshtone_noisesup,FLESHTONE_NOISESUP_DEFAULT)

	SET_RCP_PARAMETER(saturation,SATURATION_DEFAULT)

	SET_RCP_PARAMETER(r_gain,R_GAIN_DEFAULT)
	SET_RCP_PARAMETER(b_gain,B_GAIN_DEFAULT)

	SET_RCP_PARAMETER(r_pedestal,R_PEDESTAL_DEFAULT)
	SET_RCP_PARAMETER(b_pedestal,B_PEDESTAL_DEFAULT)

	SET_RCP_PARAMETER(shutter_type,SHUTTER_TYPE_DEFAULT);
	SET_RCP_PARAMETER(shutter_step,-1);
	SET_RCP_PARAMETER(shutter_synchro,SHUTTER_SYNCHRO_DEFAULT)

	SET_RCP_PARAMETER(pedestal,PEDESTAL_DEFAULT)

	SET_RCP_PARAMETER(iris,IRIS_DEFAULT)
	SET_RCP_PARAMETER(iris_auto,IRIS_AUTO_DEFAULT)

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) copy_rcp ((rcp_t*)(gslist_itr->data), rcp);
	g_mutex_unlock (&rcp->other_rcp_mutex);

	g_idle_add ((GSourceFunc)rcp_work_end, rcp);

	return NULL;
}

void standard_button_clicked (GtkButton *button, rcp_t *rcp)
{
	rcp_work_start (rcp, (GThreadFunc)load_standard);
}

void mire_toggle_button_clicked (GtkToggleButton *mire_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (mire_toggle_button)) send_cam_control_command (rcp, "DCB:1");
	else send_cam_control_command (rcp, "DCB:0");
}

void day_night_toggle_button_clicked (GtkToggleButton *day_night_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (day_night_toggle_button)) {
		send_ptz_control_command (rcp, "#D61");
		gtk_widget_set_sensitive (rcp->ND_filter_combo_box, FALSE);	//ND filter switching is not possible in Night mode
	} else {
		send_ptz_control_command (rcp, "#D60");
		gtk_widget_set_sensitive (rcp->ND_filter_combo_box, TRUE);
	}
}

#define COPY_RCP_SCENE_PARAMETER(l) \
	if (rcp->current_scene.l != rcp->scenes[rcp->scene_to_load].l) { \
		rcp->current_scene.l = rcp->scenes[rcp->scene_to_load].l; \
		set_##l (rcp); \
	}

#define COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(l) \
	if (rcp->current_scene.linear_matrix.l != rcp->scenes[rcp->scene_to_load].linear_matrix.l) { \
		rcp->current_scene.linear_matrix.l = rcp->scenes[rcp->scene_to_load].linear_matrix.l; \
		set_linear_matrix_##l (rcp); \
	}

#define COPY_RCP_SCENE_CC_SATURATION_PARAMETER(l) \
	if (rcp->current_scene.cc_saturation[l] != rcp->scenes[rcp->scene_to_load].cc_saturation[l]) { \
		rcp->current_scene.cc_saturation[l] = rcp->scenes[rcp->scene_to_load].cc_saturation[l]; \
		set_cc_saturation_##l (rcp, TRUE); \
	}

#define COPY_RCP_SCENE_CC_PHASE_PARAMETER(l) \
	if (rcp->current_scene.cc_phase[l] != rcp->scenes[rcp->scene_to_load].cc_phase[l]) { \
		rcp->current_scene.cc_phase[l] = rcp->scenes[rcp->scene_to_load].cc_phase[l]; \
		set_cc_phase_##l (rcp, TRUE); \
	}

gpointer load_scene (rcp_t *rcp)
{
	GSList *gslist_itr;

	if (rcp->mire) {
		rcp->mire = FALSE;
		send_cam_control_command (rcp, "DCB:0");
	}
	if (rcp->day_night) {
		rcp->day_night = FALSE;
		send_ptz_control_command (rcp, "#D60");
	}

	COPY_RCP_SCENE_PARAMETER(ND_filter)
	COPY_RCP_SCENE_PARAMETER(gain)

	COPY_RCP_SCENE_PARAMETER(gamma_type)
	COPY_RCP_SCENE_PARAMETER(gamma)
	COPY_RCP_SCENE_PARAMETER(drs)

	COPY_RCP_SCENE_PARAMETER(color_temperature)

	COPY_RCP_SCENE_PARAMETER(knee_settings)
	COPY_RCP_SCENE_PARAMETER(knee_point)
	COPY_RCP_SCENE_PARAMETER(knee_slope)

	COPY_RCP_SCENE_PARAMETER(matrix_type)

	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(R_G)
	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(R_B)
	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(G_R)
	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(G_B)
	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(B_R)
	COPY_RCP_SCENE_LINEAR_MATRIX_PARAMETER(B_G)

	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(R)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(R_R_Yl)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(R_Yl)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(R_Yl_Yl)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Yl)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Yl_G)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(G)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(G_Cy)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Cy)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Cy_B)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(B)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(B_Mg)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Mg)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Mg_R)
	COPY_RCP_SCENE_CC_SATURATION_PARAMETER(Mg_R_R)

	COPY_RCP_SCENE_CC_PHASE_PARAMETER(R)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(R_R_Yl)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(R_Yl)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(R_Yl_Yl)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Yl)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Yl_G)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(G)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(G_Cy)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Cy)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Cy_B)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(B)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(B_Mg)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Mg)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Mg_R)
	COPY_RCP_SCENE_CC_PHASE_PARAMETER(Mg_R_R)

	COPY_RCP_SCENE_PARAMETER(detail)
	COPY_RCP_SCENE_PARAMETER(master_detail)
	COPY_RCP_SCENE_PARAMETER(v_detail_level)
	COPY_RCP_SCENE_PARAMETER(detail_band)
	COPY_RCP_SCENE_PARAMETER(noise_suppress)
	COPY_RCP_SCENE_PARAMETER(fleshtone_noisesup)

	COPY_RCP_SCENE_PARAMETER(saturation)

	COPY_RCP_SCENE_PARAMETER(r_gain)
	COPY_RCP_SCENE_PARAMETER(b_gain)

	COPY_RCP_SCENE_PARAMETER(r_pedestal)
	COPY_RCP_SCENE_PARAMETER(b_pedestal)

	COPY_RCP_SCENE_PARAMETER(shutter_type)
	COPY_RCP_SCENE_PARAMETER(shutter_step)
	COPY_RCP_SCENE_PARAMETER(shutter_synchro)

	COPY_RCP_SCENE_PARAMETER(pedestal)

	COPY_RCP_SCENE_PARAMETER(iris)
	COPY_RCP_SCENE_PARAMETER(iris_auto)

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) copy_rcp ((rcp_t*)(gslist_itr->data), rcp);
	g_mutex_unlock (&rcp->other_rcp_mutex);

	g_idle_add ((GSourceFunc)rcp_work_end, rcp);

	return NULL;
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
		rcp_work_start (rcp, (GThreadFunc)load_scene);
	}
}

void ABB_button_clicked (GtkButton *button, rcp_t *rcp)
{
	GSList *gslist_itr;
	rcp_t *other_rcp;

	if (send_ABB_execution_control_command (rcp)) {
		g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
		rcp->g_gain = G_GAIN_DEFAULT;

		g_mutex_lock (&rcp->other_rcp_mutex);
		for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
			other_rcp = (rcp_t*)(gslist_itr->data);

			g_signal_handler_block (other_rcp->g_pedestal_scale, other_rcp->g_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (other_rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
			g_signal_handler_unblock (other_rcp->g_pedestal_scale, other_rcp->g_pedestal_handler_id);
			other_rcp->g_gain = G_GAIN_DEFAULT;
		}
		g_mutex_unlock (&rcp->other_rcp_mutex);

		rcp_work_start (rcp, NULL);
	} else {
		rcp->error_code = 0x30;
		camera_is_unreachable (rcp);
	}
}

void set_ND_filter (rcp_t *rcp)
{
	if (rcp->current_scene.ND_filter == 1) {
		send_cam_control_command (rcp, "OFT:4");
	} else if (rcp->current_scene.ND_filter == 2) {
		send_cam_control_command (rcp, "OFT:3");
	} else {
		send_cam_control_command (rcp, "OFT:0");
	}
}

void ND_filter_changed (GtkComboBox *ND_filter_combo_box, rcp_t *rcp)
{
	rcp->current_scene.ND_filter = gtk_combo_box_get_active (ND_filter_combo_box);

	set_ND_filter (rcp);
}

void set_gain (rcp_t *rcp)
{
	if (rcp->current_scene.gain == 37) send_cam_control_command (rcp, "OGU:80");
	else send_cam_control_command_2_digits (rcp, "OGU:", 0x2C - rcp->current_scene.gain, TRUE);
}

void gain_changed (GtkComboBox *gain_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gain = gtk_combo_box_get_active (gain_combo_box);

	set_gain (rcp);
}

gboolean ghost_rcp_draw (GtkWidget *widget, cairo_t *cr)
{
	if (theme) cairo_set_source_rgb (cr, 0.92549, 0.92549, 0.92549);
	else cairo_set_source_rgb (cr, 0.137254902, 0.152941176, 0.160784314);

	cairo_paint (cr);

	return GDK_EVENT_PROPAGATE;
}

void create_ghost_rcp_widgets (rcp_t *rcp)
{
	GtkWidget *main_box, *tally_grid, *box, *widget;

	main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

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
#ifdef _WIN32
			gtk_widget_set_size_request (widget, 60, 735);
#elif defined (__linux)
			gtk_widget_set_size_request (widget, 60, 732);
#endif
			gtk_widget_set_events (widget, gtk_widget_get_events (widget) | GDK_BUTTON_PRESS_MASK);
			g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (ghost_rcp_draw), NULL);
			g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
			gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

			widget = gtk_drawing_area_new ();
#ifdef _WIN32
			gtk_widget_set_size_request (widget, 60, 233);
#elif defined (__linux)
			gtk_widget_set_size_request (widget, 60, 251);
#endif
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
	gtk_box_pack_start (GTK_BOX (main_box), tally_grid, FALSE, FALSE, 0);

	widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start (GTK_BOX (main_box), widget, FALSE, FALSE, 0);

	rcp->root_widget = main_box;
}

void create_rcp_widgets (rcp_t *rcp)
{
	GtkWidget *main_box, *tally_grid, *main_rcp_box, *rcp_box, *frame, *box1, *box2, *widget;
	char label[32];
	int i;

	main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	tally_grid = gtk_grid_new ();
	main_rcp_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

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

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		widget = gtk_drawing_area_new ();
		gtk_widget_set_size_request (widget, 60, 40);
		g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (name_draw), rcp);
		g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
		gtk_widget_set_events (widget, GDK_BUTTON_PRESS_MASK);
		gtk_box_set_center_widget (GTK_BOX (box1), widget);
		rcp->name_drawing_area = widget;

		rcp->spinner = gtk_spinner_new ();
		gtk_widget_set_size_request (rcp->spinner, 40, 40);
		gtk_box_pack_end (GTK_BOX (box1), rcp->spinner, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_rcp_box), box1, FALSE, FALSE, 0);

	rcp->error_label = gtk_label_new (NULL);
	gtk_box_pack_start (GTK_BOX (main_rcp_box), rcp->error_label, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_top (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		widget = gtk_switch_new ();
		rcp->on_standby_handler_id = g_signal_connect (widget, "notify::active", G_CALLBACK (on_standby_switch_activated), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		rcp->on_standby_switch = widget;

		widget = gtk_button_new_with_label ("Standard");
		g_signal_connect (widget, "clicked", G_CALLBACK (standard_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		rcp->standard_button = widget;

		widget = gtk_toggle_button_new_with_label ("Mire");
		rcp->mire_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (mire_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		rcp->mire_toggle_button = widget;

		widget = gtk_toggle_button_new_with_label ("Nuit");
		rcp->day_night_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (day_night_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		rcp->day_night_toggle_button = widget;
	gtk_box_pack_start (GTK_BOX (main_rcp_box), box1, FALSE, FALSE, 0);

	rcp_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	rcp->sensitive_widgets = rcp_box;
//Scenes
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
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("ABB");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE * 2);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (ABB_button_clicked), rcp);
			gtk_box_set_center_widget (GTK_BOX (box2), widget);
			rcp->ABB_button = widget;
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
//ND_filter
		frame = gtk_frame_new ("Filtre neutre");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Through");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/8");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/64");
			rcp->ND_filter_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (ND_filter_changed), rcp);
			gtk_container_add (GTK_CONTAINER (frame), widget);
			rcp->ND_filter_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
//Gain
		frame = gtk_frame_new ("Gain");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
			for (i = 0x08; i <= 0x2C; i++) {
				sprintf (label, "%d db", i - 0x08);
				gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
			}
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Auto");
			rcp->gain_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gain_changed), rcp);
			gtk_container_add (GTK_CONTAINER (frame), widget);
			rcp->gain_combo_box = widget;
		gtk_box_pack_end (GTK_BOX (box1), frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);
	rcp->ABB_filter_gain_box = box1;

	frame = create_gamma_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);
	rcp->gamma_frame = frame;

	frame = create_color_temperature_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);
	rcp->color_temperature_frame = frame;

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		frame = create_knee_frame (rcp);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);

		frame = create_matrix_frame (rcp);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);

		frame = create_detail_frame (rcp);
		gtk_box_pack_end (GTK_BOX (box1), frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);
	rcp->knee_matrix_detail_box = box1;

	frame = create_saturation_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	frame = create_white_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	frame = create_black_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			frame = create_shutter_frame (rcp);
			gtk_box_pack_start (GTK_BOX (box2), frame, FALSE, FALSE, 0);
			rcp->shutter_frame = frame;

			frame = create_pedestal_frame (rcp);
			gtk_box_pack_end (GTK_BOX (box2), frame, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		frame = create_iris_frame (rcp);
		gtk_box_pack_end (GTK_BOX (box1), frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (main_rcp_box), rcp_box, FALSE, FALSE, 0);

	widget = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget), FALSE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (widget), TRUE);
	g_signal_connect (G_OBJECT (widget), "button_press_event", G_CALLBACK (rcp_button_press_event), rcp);
	gtk_container_add (GTK_CONTAINER (widget), main_rcp_box);

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

	gtk_box_pack_start (GTK_BOX (main_box), tally_grid, FALSE, FALSE, 0);

	widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
	gtk_box_pack_start (GTK_BOX (main_box), widget, FALSE, FALSE, 0);

	rcp->root_widget = main_box;
}

