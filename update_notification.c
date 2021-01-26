/*
 * copyright (c) 2018-2021 Thomas Paillet <thomas.paillet@net-c.fr

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

#include <unistd.h>


SOCKET update_notification_socket;
struct sockaddr_in update_notification_address;

gboolean update_notification_started = FALSE;

GThread *update_notification_thread;


#define NO_POST_ACTION 0

#define NIGHT_POST_ACTION 1
#define DRS_POST_ACTION 2

#define KNEE_SETTINGS_POST_ACTION 3
#define KNEE_POINT_POST_ACTION 4
#define KNEE_SLOPE_POST_ACTION 5

#define MATRIX_TYPE_POST_ACTION 6
#define LINEAR_MATRIX_R_G_POST_ACTION 7
#define LINEAR_MATRIX_R_B_POST_ACTION 8
#define LINEAR_MATRIX_G_R_POST_ACTION 9
#define LINEAR_MATRIX_G_B_POST_ACTION 10
#define LINEAR_MATRIX_B_R_POST_ACTION 11
#define LINEAR_MATRIX_B_G_POST_ACTION 12

#define DETAIL_POST_ACTION 43
#define MASTER_DETAIL_POST_ACTION 44
#define V_DETAIL_LEVEL_POST_ACTION 45
#define DETAIL_BAND_POST_ACTION 46
#define NOISE_SUPPRESS_POST_ACTION 47
#define FLESHTONE_NOISESUP_POST_ACTION 48

#define SATURATION_POST_ACTION 49

#define SHUTTER_TYPE_POST_ACTION 50
#define SHUTTER_SYNCHRO_POST_ACTION 51

#define PEDESTAL_POST_ACTION 52
#define IRIS_AUTO_POST_ACTION 53

typedef struct {
	GtkWidget *widget;
	int handler_id;
	int *value;
	int post_action;
	rcp_t *rcp;
} int_widget_t;


gboolean update_toggle_button (int_widget_t *int_widget)
{
	g_signal_handler_block (int_widget->widget, int_widget->handler_id);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (int_widget->widget), *(int_widget->value));
	g_signal_handler_unblock (int_widget->widget, int_widget->handler_id);

	switch (int_widget->post_action) {
		case NIGHT_POST_ACTION:
			if (int_widget->rcp->day_night) gtk_widget_set_sensitive (int_widget->rcp->ND_filter_combo_box, FALSE);	//ND filter switching is not possible in Night mode
			else gtk_widget_set_sensitive (int_widget->rcp->ND_filter_combo_box, TRUE);
			break;
		case DETAIL_POST_ACTION:
			if (int_widget->rcp->current_scene.detail) {
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->detail_toggle_button), "On");
				gtk_widget_set_sensitive (int_widget->rcp->detail_settings_button, TRUE);
			} else {
				gtk_widget_set_sensitive (int_widget->rcp->detail_settings_button, FALSE);
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->detail_toggle_button), "Off");
			}
			break;
		case IRIS_AUTO_POST_ACTION:
			if (int_widget->rcp->current_scene.iris_auto) gtk_widget_set_sensitive (int_widget->rcp->iris_sensitive_widgets, FALSE);
			else gtk_widget_set_sensitive (int_widget->rcp->iris_sensitive_widgets, TRUE);
	}

	g_free (int_widget);

	return G_SOURCE_REMOVE;
}

gboolean update_combo_box (int_widget_t *int_widget)
{
	g_signal_handler_block (int_widget->widget, int_widget->handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->widget), *(int_widget->value));
	g_signal_handler_unblock (int_widget->widget, int_widget->handler_id);

	switch (int_widget->post_action) {
		case DRS_POST_ACTION:
			if (int_widget->rcp->current_scene.drs != 3) {	//When DRS is set to On, the knee setting is disabled
				gtk_widget_set_sensitive (int_widget->rcp->knee_settings_combo_box, FALSE);
				gtk_widget_set_sensitive (int_widget->rcp->knee_button, FALSE);
			} else {
				gtk_widget_set_sensitive (int_widget->rcp->knee_settings_combo_box, TRUE);
				gtk_widget_set_sensitive (int_widget->rcp->knee_button, TRUE);
			}
			break;
		case KNEE_SETTINGS_POST_ACTION:
			if (int_widget->rcp->current_scene.knee_settings != 1) gtk_widget_set_sensitive (int_widget->rcp->knee_button, FALSE);
			else gtk_widget_set_sensitive (int_widget->rcp->knee_button, TRUE);
			break;
		case MATRIX_TYPE_POST_ACTION:
			if (int_widget->rcp->current_scene.matrix_type != 3) gtk_widget_set_sensitive (int_widget->rcp->matrix_color_button, FALSE);
			else gtk_widget_set_sensitive (int_widget->rcp->matrix_color_button, TRUE);
			break;
		case SHUTTER_TYPE_POST_ACTION:
			switch (int_widget->rcp->current_scene.shutter_type) {
				case 1: gtk_widget_hide (int_widget->rcp->shutter_synchro_button);
					gtk_widget_show (int_widget->rcp->shutter_step_combo_box);
					g_signal_handler_block (int_widget->rcp->shutter_step_combo_box, int_widget->rcp->shutter_step_handler_id);
					gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), int_widget->rcp->current_scene.shutter_step);
					g_signal_handler_unblock (int_widget->rcp->shutter_step_combo_box, int_widget->rcp->shutter_step_handler_id);
					break;
				case 2: gtk_widget_hide (int_widget->rcp->shutter_step_combo_box);
					gtk_widget_show (int_widget->rcp->shutter_synchro_button);
					break;
				default: gtk_widget_hide (int_widget->rcp->shutter_step_combo_box);
					gtk_widget_hide (int_widget->rcp->shutter_synchro_button);
			}
	}

	g_free (int_widget);

	return G_SOURCE_REMOVE;
}

gboolean update_scale (int_widget_t *int_widget)
{
	g_signal_handler_block (int_widget->widget, int_widget->handler_id);
	gtk_range_set_value (GTK_RANGE (int_widget->widget), *(int_widget->value));
	g_signal_handler_unblock (int_widget->widget, int_widget->handler_id);

	switch (int_widget->post_action) {
		case KNEE_POINT_POST_ACTION:
			set_knee_point_label (int_widget->rcp);
			break;
		case KNEE_SLOPE_POST_ACTION:
			set_knee_slope_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_R_G_POST_ACTION:
			set_linear_matrix_R_G_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_R_B_POST_ACTION:
			set_linear_matrix_R_B_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_G_R_POST_ACTION:
			set_linear_matrix_G_R_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_G_B_POST_ACTION:
			set_linear_matrix_G_B_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_B_R_POST_ACTION:
			set_linear_matrix_B_R_label (int_widget->rcp);
			break;
		case LINEAR_MATRIX_B_G_POST_ACTION:
			set_linear_matrix_B_G_label (int_widget->rcp);
			break;
		case MASTER_DETAIL_POST_ACTION:
			set_master_detail_label (int_widget->rcp);
			break;
		case V_DETAIL_LEVEL_POST_ACTION:
			set_v_detail_level_label (int_widget->rcp);
			break;
		case DETAIL_BAND_POST_ACTION:
			set_detail_band_label (int_widget->rcp);
			break;
		case NOISE_SUPPRESS_POST_ACTION:
			set_noise_suppress_label (int_widget->rcp);
			break;
		case FLESHTONE_NOISESUP_POST_ACTION:
			set_fleshtone_noisesup_label (int_widget->rcp);
			break;
		case SATURATION_POST_ACTION:
			set_saturation_label (int_widget->rcp);
			break;
		case SHUTTER_SYNCHRO_POST_ACTION:
			set_shutter_synchro_label (int_widget->rcp);
			break;
		case PEDESTAL_POST_ACTION:
			set_pedestal_label (int_widget->rcp);
	}

	g_free (int_widget);

	return G_SOURCE_REMOVE;
}

gboolean update_saturation_color_correction_frame (int_widget_t *int_widget)
{
	gtk_widget_queue_draw (int_widget->rcp->color_correction_drawing_area);

	set_cc_saturation_label_funcs[int_widget->post_action] (int_widget->rcp);

	g_free (int_widget);

	return G_SOURCE_REMOVE;
}

gboolean update_phase_color_correction_frame (int_widget_t *int_widget)
{
	gtk_widget_queue_draw (int_widget->rcp->color_correction_drawing_area);

	set_cc_phase_label_funcs[int_widget->post_action] (int_widget->rcp);

	g_free (int_widget);

	return G_SOURCE_REMOVE;
}


#define IHM_UPDATE_TOGGLE_BUTTON(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_toggle_button; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_toggle_button, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.l = rcp->current_scene.l; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = other_rcp->l##_toggle_button; \
		int_widget->handler_id = other_rcp->l##_handler_id; \
		int_widget->value = &other_rcp->current_scene.l; \
		int_widget->post_action = p; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_toggle_button, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex);

#define IHM_UPDATE_TOGGLE_BUTTON_2(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_toggle_button; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_toggle_button, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->l = rcp->l; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = other_rcp->l##_toggle_button; \
		int_widget->handler_id = other_rcp->l##_handler_id; \
		int_widget->value = &other_rcp->l; \
		int_widget->post_action = p; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_toggle_button, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex);

#define IHM_UPDATE_COMBO_BOX(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_combo_box; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_combo_box, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.l = rcp->current_scene.l; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = other_rcp->l##_combo_box; \
		int_widget->handler_id = other_rcp->l##_handler_id; \
		int_widget->value = &other_rcp->current_scene.l; \
		int_widget->post_action = p; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_combo_box, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex);

#define IHM_UPDATE_SCALE(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_scale; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_scale, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.l = rcp->current_scene.l; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = other_rcp->l##_scale; \
		int_widget->handler_id = other_rcp->l##_handler_id; \
		int_widget->value = &other_rcp->current_scene.l; \
		int_widget->post_action = p; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_scale, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex);

#define IHM_UPDATE_LINEAR_MATRIX_SCALE(l,p) { \
	sscanf (buffer + 7, "%x", &rcp->current_scene.linear_matrix.l); \
 \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->linear_matrix_##l##_scale; \
	int_widget->handler_id = rcp->linear_matrix_##l##_handler_id; \
	int_widget->value = &rcp->current_scene.linear_matrix.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_scale, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.linear_matrix.l = rcp->current_scene.linear_matrix.l; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = other_rcp->linear_matrix_##l##_scale; \
		int_widget->handler_id = other_rcp->linear_matrix_##l##_handler_id; \
		int_widget->value = &other_rcp->current_scene.linear_matrix.l; \
		int_widget->post_action = p; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_scale, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex); }

#define IHM_UPDATE_CC_SATURATION(l) { \
	sscanf (buffer + 7, "%x", &data); \
	rcp->current_scene.cc_saturation[l] = data - 0x80; \
 \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->post_action = l; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_saturation_color_correction_frame, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.cc_saturation[l] = rcp->current_scene.cc_saturation[l]; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->post_action = l; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_saturation_color_correction_frame, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex); }

#define IHM_UPDATE_CC_PHASE(l) { \
	sscanf (buffer + 7, "%x", &data); \
	rcp->current_scene.cc_phase[l] = data - 0x80; \
 \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->post_action = l; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_phase_color_correction_frame, int_widget); \
 \
	g_mutex_lock (&rcp->other_rcp_mutex); \
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) { \
		other_rcp = (rcp_t*)(gslist_itr->data); \
 \
		other_rcp->current_scene.cc_phase[l] = rcp->current_scene.cc_phase[l]; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->post_action = l; \
		int_widget->rcp = other_rcp; \
		g_idle_add ((GSourceFunc)update_phase_color_correction_frame, int_widget); \
	} \
	g_mutex_unlock (&rcp->other_rcp_mutex); }


gboolean ABB_rcp_work_end (rcp_t *rcp)
{
	GSList *gslist_itr;
	rcp_t *other_rcp;

	if (rcp->thread != NULL) {
		g_thread_join (rcp->thread);
		rcp->thread = NULL;
	}

	if (rcp->root_widget == NULL) {
		g_slist_free (rcp->other_rcp);
		g_mutex_clear (&rcp->other_rcp_mutex);
		g_mutex_clear (&rcp->cmd_mutex);
		g_free (rcp);
		return G_SOURCE_REMOVE;
	}

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
		other_rcp = (rcp_t*)(gslist_itr->data);

		gtk_widget_set_sensitive (other_rcp->on_standby_switch, TRUE);
		gtk_widget_set_sensitive (other_rcp->standard_button, TRUE);
		gtk_widget_set_sensitive (other_rcp->mire_toggle_button, TRUE);
		gtk_widget_set_sensitive (other_rcp->day_night_toggle_button, TRUE);
		gtk_widget_set_sensitive (other_rcp->sensitive_widgets, TRUE);

		gtk_spinner_stop (GTK_SPINNER (other_rcp->spinner));
		gtk_widget_hide (other_rcp->spinner);

		other_rcp->camera_is_working = FALSE;
	}
	g_mutex_unlock (&rcp->other_rcp_mutex);

	gtk_widget_set_sensitive (rcp->on_standby_switch, TRUE);
	gtk_widget_set_sensitive (rcp->standard_button, TRUE);
	gtk_widget_set_sensitive (rcp->mire_toggle_button, TRUE);
	gtk_widget_set_sensitive (rcp->day_night_toggle_button, TRUE);
	gtk_widget_set_sensitive (rcp->sensitive_widgets, TRUE);

	gtk_spinner_stop (GTK_SPINNER (rcp->spinner));
	gtk_widget_hide (rcp->spinner);

	rcp->camera_is_working = FALSE;

	return G_SOURCE_REMOVE;
}

gboolean decrease_on_standby_count (cameras_set_t *cameras_set)
{
	cameras_set->on_standby_count--;

	if (cameras_set->on_standby_count == 0) {
		g_signal_handler_block (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
		gtk_switch_set_active (GTK_SWITCH (cameras_set->master_rcp.on_standby_switch), FALSE);
		g_signal_handler_unblock (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
	}

	return G_SOURCE_REMOVE;
}

#ifdef MAIN_SETTINGS_READ_ONLY
#define UPDATE_COMBO_BOX_TEXT_FUNC(l,i) \
gboolean update_##l##_combo_box_text (void) \
{ \
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box)); \
 \
	if ((settings_parameters_indexes_array[i] >= 0) || (settings_parameters_indexes_array[i] < settings_array[i].nb_parameters)) { \
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box), settings_array[i].parameters[settings_parameters_indexes_array[i]].label); \
		gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array[i].combo_box), 0); \
	} \
 \
	return G_SOURCE_REMOVE; \
}

UPDATE_COMBO_BOX_TEXT_FUNC(frequency,0)
UPDATE_COMBO_BOX_TEXT_FUNC(format,1)
UPDATE_COMBO_BOX_TEXT_FUNC(down_conversion_mode,2)
UPDATE_COMBO_BOX_TEXT_FUNC(color_bar_setup_level,3)
UPDATE_COMBO_BOX_TEXT_FUNC(preset_scope,4)
UPDATE_COMBO_BOX_TEXT_FUNC(white_balance_mode,5)
UPDATE_COMBO_BOX_TEXT_FUNC(white_clip,6)
UPDATE_COMBO_BOX_TEXT_FUNC(white_clip_level,7)
UPDATE_COMBO_BOX_TEXT_FUNC(agc_max_gain,8)
UPDATE_COMBO_BOX_TEXT_FUNC(frame_mix,9)
UPDATE_COMBO_BOX_TEXT_FUNC(dnr,10)
UPDATE_COMBO_BOX_TEXT_FUNC(tally_brightness,11)
UPDATE_COMBO_BOX_TEXT_FUNC(osd_off_with_tally,12)

gboolean update_format_parameter (void)
{
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array[1].combo_box));
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array[1].combo_box), settings_array[1].parameters[settings_parameters_indexes_array[1]].label);
	gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array[1].combo_box), 0);

	return G_SOURCE_REMOVE;
}
#endif

gpointer ask_format (rcp_t *rcp)
{
	char response[8];
#ifdef MAIN_SETTINGS_READ_ONLY
	int i;
#endif

	usleep (45000000);

	send_update_start_cmd (rcp);

	send_cam_request_command_string (rcp, settings_array[1].query_cmd, response);

#ifdef MAIN_SETTINGS_READ_ONLY
	for (i = 0; i < settings_array[1].nb_parameters; i++) {
		if (strcmp (settings_array[1].answers[i], response) == 0) {
			settings_parameters_indexes_array[1] = i;
			g_idle_add ((GSourceFunc)update_format_parameter, NULL);

			break;
		}
	}
#else
	if (strcmp (settings_array[1].answers[settings_parameters_indexes_array[1]], response) != 0)
		send_cam_control_command_string (rcp, settings_array[1].control_cmd, settings_array[1].parameters[settings_parameters_indexes_array[1]].value);
#endif

	return NULL;
}

void proceed_update_notification (rcp_t *rcp, const char *buffer)
{
	int data;
	int_widget_t *int_widget;
	GSList *gslist_itr;
	rcp_t *other_rcp;
#ifdef MAIN_SETTINGS_READ_ONLY
	int i;
#endif
	if ((buffer[0] == 'd') && (buffer[1] == '6')) {	//Option switch Day/Night
		if (buffer[2] == '1') rcp->day_night = TRUE;
		else rcp->day_night = FALSE;
		IHM_UPDATE_TOGGLE_BUTTON_2(day_night,NIGHT_POST_ACTION)
	} else if ((buffer[0] == 'D') && (buffer[1] == 'C')) {
		if ((buffer[2] == 'B') && (buffer[3] == ':')) {	//Color bar/Camera
			if (buffer[4] == '1') rcp->mire = TRUE;
			else rcp->mire = FALSE;
			IHM_UPDATE_TOGGLE_BUTTON_2(mire,NO_POST_ACTION)
		} else if ((buffer[2] == 'S') && (buffer[3] == ':')) {	//Color bar setup level
#ifdef MAIN_SETTINGS_READ_ONLY
			for (i = 0; i < settings_array[3].nb_parameters; i++) {
				if (buffer[4] == settings_array[3].answers[i][0]) {
					settings_parameters_indexes_array[3] = i;
					g_idle_add ((GSourceFunc)update_color_bar_setup_level_combo_box_text, NULL);
					break;
				}
			}
#else
			if (buffer[4] != settings_array[3].answers[settings_parameters_indexes_array[3]][0])
				send_cam_control_command_string (rcp, settings_array[3].control_cmd, settings_array[3].parameters[settings_parameters_indexes_array[3]].value);
#endif
		}
	} else if ((buffer[0] == 'E') && (buffer[1] == 'R')) {
		if ((buffer[2] == '3') && (buffer[3] == ':') && (buffer[4] == 'O') && (buffer[5] == 'A') && (buffer[6] == 'S')) {	//ABB execution failed
			g_idle_add ((GSourceFunc)ABB_execution_failed, rcp);
			g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);
		} else if ((buffer[2] == '2') && (buffer[3] == ':') && (buffer[4] == 'O') && (buffer[5] == 'A') && (buffer[6] == 'S')) {	//ABB execution failed (busy status)
			g_idle_add ((GSourceFunc)ABB_execution_failed_busy_status, rcp);
			g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);
		}
	} else if ((buffer[0] == 'l') && (buffer[1] == 'P') && (buffer[2] == 'I')) { 	//Lens information
		if (buffer[9] <= '9') data = (buffer[9] - '0') * 256;
		else data = (buffer[9] - '7') * 256;
		if (buffer[10] <= '9') data += (buffer[10] - '0') * 16;
		else data += (buffer[10] - '7') *16;
		if (buffer[11] <= '9') data += buffer[11] - '0';
		else data += buffer[11] - '7';

		if (data != rcp->current_scene.iris) {
			rcp->current_scene.iris = data;
			IHM_UPDATE_SCALE(iris,NO_POST_ACTION)
		}
	} else if (buffer[0] == 'O') {
		if (buffer[1] == 'A') {
			if (buffer[2] == 'S') g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);	//ABB execution successful
			else if ((buffer[2] == 'W') && (buffer[3] == ':')) {	//White Balance Mode
#ifdef MAIN_SETTINGS_READ_ONLY
				for (i = 0; i < settings_array[5].nb_parameters; i++) {
					if (buffer[4] == settings_array[5].answers[i][0]) {
						settings_parameters_indexes_array[5] = i;
						g_idle_add ((GSourceFunc)update_white_balance_mode_combo_box_text, NULL);
						break;
					}
				}
#else
				if (buffer[4] != settings_array[5].answers[settings_parameters_indexes_array[5]][0])
					send_cam_control_command_string (rcp, settings_array[5].control_cmd, settings_array[5].parameters[settings_parameters_indexes_array[5]].value);
#endif
			}
		} else if (buffer[1] == 'B') {
			if ((buffer[2] == 'I') && (buffer[3] == ':')) {	//B gain
				sscanf (buffer + 4, "%x", &rcp->current_scene.b_gain);
				IHM_UPDATE_SCALE(b_gain,NO_POST_ACTION)
			} else if ((buffer[2] == 'P') && (buffer[3] == ':')) {	//B pedestal
				sscanf (buffer + 4, "%x", &rcp->current_scene.b_pedestal);
				IHM_UPDATE_SCALE(b_pedestal,NO_POST_ACTION)
			}
		} else if (buffer[1] == 'D') {
			if ((buffer[2] == 'T') && (buffer[3] == ':')) {	//Detail
				if (buffer[4] == '0') rcp->current_scene.detail = FALSE;
				else rcp->current_scene.detail = TRUE;
				IHM_UPDATE_TOGGLE_BUTTON(detail,DETAIL_POST_ACTION)
			}
		} else if ((buffer[1] == 'F') && (buffer[2] == 'T') && (buffer[3] == ':')) {	//ND filter
			if (buffer[4] == '3') rcp->current_scene.ND_filter = 2;
			else if (buffer[4] == '4') rcp->current_scene.ND_filter = 1;
			else rcp->current_scene.ND_filter = 0;
			IHM_UPDATE_COMBO_BOX(ND_filter,NO_POST_ACTION)
		} else if ((buffer[1] == 'G') && (buffer[2] == 'U') && (buffer[3] == ':')) {	//Gain
			sscanf (buffer + 4, "%x", &data);
			if (data == 0x80) rcp->current_scene.gain = 37;
			else rcp->current_scene.gain = 0x2C - data;
			IHM_UPDATE_COMBO_BOX(gain,NO_POST_ACTION)
		} else if ((buffer[1] == 'M') && (buffer[2] == 'S') && (buffer[3] == ':')) {	//Synchro scan
			sscanf (buffer + 4, "%x", &rcp->current_scene.shutter_synchro);
			IHM_UPDATE_SCALE(shutter_synchro,SHUTTER_SYNCHRO_POST_ACTION)
		} else if (buffer[1] == 'R') {
			if ((buffer[2] == 'I') && (buffer[3] == ':')) {	//R gain
				sscanf (buffer + 4, "%x", &rcp->current_scene.r_gain);
				IHM_UPDATE_SCALE(r_gain,NO_POST_ACTION)
			} else if ((buffer[2] == 'P') && (buffer[3] == ':')) {	//R pedestal
				sscanf (buffer + 4, "%x", &rcp->current_scene.r_pedestal);
				IHM_UPDATE_SCALE(r_pedestal,NO_POST_ACTION)
			} else if ((buffer[2] == 'S') && (buffer[3] == ':')) {	//Iris Auto/Manual
				if (buffer[4] == '1') rcp->current_scene.iris_auto = TRUE;
				else rcp->current_scene.iris_auto = FALSE;
				IHM_UPDATE_TOGGLE_BUTTON(iris_auto,IRIS_AUTO_POST_ACTION)
			} else if ((buffer[2] == 'V') && (buffer[3] == ':')) {	//Iris volume
				sscanf (buffer + 4, "%x", &rcp->current_scene.iris);
				IHM_UPDATE_SCALE(iris,NO_POST_ACTION)
			}
		} else if (buffer[1] == 'S') {
			if ((buffer[2] == 'A') && (buffer[3] == ':')) {
				if (buffer[4] == '2') {
					if ((buffer[5] == '0') && (buffer[6] == ':')) {	//Knee Point
						sscanf (buffer + 7, "%x", &rcp->current_scene.knee_point);
						IHM_UPDATE_SCALE(knee_point,KNEE_POINT_POST_ACTION)
					} else if ((buffer[5] == '4') && (buffer[6] == ':')) {	//Knee Slope
						sscanf (buffer + 7, "%x", &rcp->current_scene.knee_slope);
						IHM_UPDATE_SCALE(knee_slope,KNEE_SLOPE_POST_ACTION)
					} else if ((buffer[5] == 'A') && (buffer[6] == ':')) {	//White Clip Level
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[7].nb_parameters; i++) {
							if ((buffer[7] == settings_array[7].answers[i][0]) && (buffer[8] == settings_array[7].answers[i][1])) {
								settings_parameters_indexes_array[7] = i;
								g_idle_add ((GSourceFunc)update_white_clip_level_combo_box_text, NULL);
								break;
							}
						}
#else
						if ((buffer[7] != settings_array[7].answers[settings_parameters_indexes_array[7]][0]) || (buffer[8] != settings_array[7].answers[settings_parameters_indexes_array[7]][1]))
							send_cam_control_command_string (rcp, settings_array[7].control_cmd, settings_array[7].parameters[settings_parameters_indexes_array[7]].value);
#endif
					} else if ((buffer[5] == 'D') && (buffer[6] == ':')) {	//Knee settings
						rcp->current_scene.knee_settings = (int)buffer[7] - 48;
						IHM_UPDATE_COMBO_BOX(knee_settings,KNEE_SETTINGS_POST_ACTION)
					} else if ((buffer[5] == 'E') && (buffer[6] == ':')) {	//White Clip settings
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[6].nb_parameters; i++) {
							if (buffer[7] == settings_array[6].answers[i][0]) {
								settings_parameters_indexes_array[6] = i;
								g_idle_add ((GSourceFunc)update_white_clip_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[7] != settings_array[6].answers[settings_parameters_indexes_array[6]][0])
							send_cam_control_command_string (rcp, settings_array[6].control_cmd, settings_array[6].parameters[settings_parameters_indexes_array[6]].value);
#endif
					}
				} else if ((buffer[4] == '3') && (buffer[5] == '0') && (buffer[6] == ':')) {	//TOTAL DTL LEVEL
					sscanf (buffer + 7, "%x", &rcp->current_scene.master_detail);
					IHM_UPDATE_SCALE(master_detail,MASTER_DETAIL_POST_ACTION)
				} else if (buffer[4] == '6') {
					if ((buffer[5] == '5') && (buffer[6] == ':')) {	//Frame mix
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[9].nb_parameters; i++) {
							if ((buffer[7] == settings_array[9].answers[i][0]) && (buffer[8] == settings_array[9].answers[i][1])) {
								settings_parameters_indexes_array[9] = i;
								g_idle_add ((GSourceFunc)update_frame_mix_combo_box_text, NULL);
								break;
							}
						}
#else
						if ((buffer[7] != settings_array[9].answers[settings_parameters_indexes_array[9]][0]) || (buffer[8] != settings_array[9].answers[settings_parameters_indexes_array[9]][1]))
							send_cam_control_command_string (rcp, settings_array[9].control_cmd, settings_array[9].parameters[settings_parameters_indexes_array[9]].value);
#endif
					} else if ((buffer[5] == 'A') && (buffer[6] == ':')) {	//Gamma
						sscanf (buffer + 7, "%x", &data);
						rcp->current_scene.gamma = 0x94 - data;
						IHM_UPDATE_COMBO_BOX(gamma,NO_POST_ACTION)
					}
				} else if ((buffer[4] == '8') && (buffer[5] == '7') && (buffer[6] == ':')) {	//Format
#ifdef MAIN_SETTINGS_READ_ONLY
					if (buffer[7] == '1') {
						for (i = 0; i < settings_array[1].nb_parameters; i++) {
							if (settings_array[1].answers[i][0] == '1') {
								if (buffer[8] == settings_array[1].answers[i][1]) {
									settings_parameters_indexes_array[1] = i;
									g_idle_add ((GSourceFunc)update_format_combo_box_text, NULL);
									break;
								}
							}
						}
					} else {
						for (i = 0; i < settings_array[1].nb_parameters; i++) {
							if (buffer[7] == settings_array[1].answers[i][0]) {
								settings_parameters_indexes_array[1] = i;
								g_idle_add ((GSourceFunc)update_format_combo_box_text, NULL);
								break;
							}
						}
					}
#else
					if (buffer[7] != settings_array[1].answers[settings_parameters_indexes_array[1]][0])
						send_cam_control_command_string (rcp, settings_array[1].control_cmd, settings_array[1].parameters[settings_parameters_indexes_array[1]].value);
					else if ((buffer[7] == '1') && (buffer[8] != settings_array[1].answers[settings_parameters_indexes_array[1]][1]))
						send_cam_control_command_string (rcp, settings_array[1].control_cmd, settings_array[1].parameters[settings_parameters_indexes_array[1]].value);
#endif
				} else if ((buffer[4] == 'D') && (buffer[5] == '3') && (buffer[6] == ':')) {	//Tally Brightness settings
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[11].nb_parameters; i++) {
						if (buffer[7] == settings_array[11].answers[i][0]) {
							settings_parameters_indexes_array[11] = i;
							g_idle_add ((GSourceFunc)update_tally_brightness_combo_box_text, NULL);
							break;
						}
					}
#else
					if (buffer[7] != settings_array[11].answers[settings_parameters_indexes_array[11]][0])
						send_cam_control_command_string (rcp, settings_array[11].control_cmd, settings_array[11].parameters[settings_parameters_indexes_array[11]].value);
#endif
				}
			} else if ((buffer[2] == 'D') && (buffer[3] == ':')) {
				if ((buffer[4] == '2') && (buffer[5] == '2') && (buffer[6] == ':')) {	//NOISE SUPPRESS/CRISP
					sscanf (buffer + 7, "%x", &rcp->current_scene.noise_suppress);
					IHM_UPDATE_SCALE(noise_suppress,NOISE_SUPPRESS_POST_ACTION)
				} else if ((buffer[4] == '3') && (buffer[5] == 'A') && (buffer[6] == ':') && (buffer[7] == '0')) {	//Digital noise reduction
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[10].nb_parameters; i++) {
						if (buffer[8] == settings_array[10].answers[i][1]) {
							settings_parameters_indexes_array[10] = i;
							g_idle_add ((GSourceFunc)update_dnr_combo_box_text, NULL);
							break;
						}
					}
#else
					if (buffer[8] != settings_array[10].answers[settings_parameters_indexes_array[10]][1])
						send_cam_control_command_string (rcp, settings_array[10].control_cmd, settings_array[10].parameters[settings_parameters_indexes_array[10]].value);
#endif
				} else if ((buffer[4] == '4') && (buffer[5] == '8') && (buffer[6] == ':')) {	//Picture level Iris offset
					sscanf (buffer + 7, "%x", &data);
#ifdef MAIN_SETTINGS_READ_ONLY
					picture_level = data - 0x32;
					g_idle_add ((GSourceFunc)update_picture_level_scale, NULL);
#else
					if (picture_level != (data - 0x32)) send_cam_control_command_2_digits (rcp, "OSD:48:", picture_level + 0x32, TRUE);
#endif
				} else if ((buffer[4] == '6') && (buffer[5] == '9') && (buffer[6] == ':') && (buffer[7] == '0')) {	//AGC maximum gain value
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[8].nb_parameters; i++) {
						if (buffer[8] == settings_array[8].answers[i][1]) {
							settings_parameters_indexes_array[8] = i;
							g_idle_add ((GSourceFunc)update_agc_max_gain_combo_box_text, NULL);
							break;
						}
					}
#else
					if (buffer[8] != settings_array[8].answers[settings_parameters_indexes_array[8]][1])
						send_cam_control_command_string (rcp, settings_array[8].control_cmd, settings_array[8].parameters[settings_parameters_indexes_array[8]].value);
#endif
				} else if (buffer[4] == '8') {
					if ((buffer[5] == '0') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(B_Mg)
					else if ((buffer[5] == '1') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(B_Mg)
					else if ((buffer[5] == '2') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Mg)
					else if ((buffer[5] == '3') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Mg)
					else if ((buffer[5] == '4') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Mg_R)
					else if ((buffer[5] == '5') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Mg_R)
					else if ((buffer[5] == '6') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(R)
					else if ((buffer[5] == '7') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(R)
					else if ((buffer[5] == '8') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(R_Yl)
					else if ((buffer[5] == '9') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(R_Yl)
					else if ((buffer[5] == 'A') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Yl)
					else if ((buffer[5] == 'B') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Yl)
					else if ((buffer[5] == 'C') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Yl_G)
					else if ((buffer[5] == 'D') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Yl_G)
					else if ((buffer[5] == 'E') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(G)
					else if ((buffer[5] == 'F') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(G)
				} else if (buffer[4] == '9') {
					if ((buffer[5] == '0') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(G_Cy)
					else if ((buffer[5] == '1') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(G_Cy)
					else if ((buffer[5] == '2') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Cy)
					else if ((buffer[5] == '3') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Cy)
					else if ((buffer[5] == '4') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Cy_B)
					else if ((buffer[5] == '5') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Cy_B)
					else if ((buffer[5] == '6') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(B)
					else if ((buffer[5] == '7') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(B)
					else if ((buffer[5] == 'A') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(Mg_R_R)
					else if ((buffer[5] == 'B') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(Mg_R_R)
					else if ((buffer[5] == 'C') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(R_R_Yl)
					else if ((buffer[5] == 'D') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(R_R_Yl)
					else if ((buffer[5] == 'E') && (buffer[6] == ':')) IHM_UPDATE_CC_SATURATION(R_Yl_Yl)
					else if ((buffer[5] == 'F') && (buffer[6] == ':')) IHM_UPDATE_CC_PHASE(R_Yl_Yl)
				} else if (buffer[4] == 'A') {
					if ((buffer[5] == '1') && (buffer[6] == ':')) {	//V DTL LEVEL
						sscanf (buffer + 7, "%x", &rcp->current_scene.v_detail_level);
						IHM_UPDATE_SCALE(v_detail_level,V_DETAIL_LEVEL_POST_ACTION)
					} else if ((buffer[5] == '2') && (buffer[6] == ':')) {	//DETAIL BAND
						sscanf (buffer + 7, "%x", &rcp->current_scene.detail_band);
						IHM_UPDATE_SCALE(detail_band,DETAIL_BAND_POST_ACTION)
					} else if ((buffer[5] == '3') && (buffer[6] == ':')) {	//FLESH TONE NOISE SUPPRESS
						sscanf (buffer + 7, "%x", &rcp->current_scene.fleshtone_noisesup);
						IHM_UPDATE_SCALE(fleshtone_noisesup,FLESHTONE_NOISESUP_POST_ACTION)
					} else if ((buffer[5] == '4') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_G,LINEAR_MATRIX_R_G_POST_ACTION)
					else if ((buffer[5] == '5') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_B,LINEAR_MATRIX_R_B_POST_ACTION)
					else if ((buffer[5] == '6') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_R,LINEAR_MATRIX_G_R_POST_ACTION)
					else if ((buffer[5] == '7') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_B,LINEAR_MATRIX_G_B_POST_ACTION)
					else if ((buffer[5] == '8') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_R,LINEAR_MATRIX_B_R_POST_ACTION)
					else if ((buffer[5] == '9') && (buffer[6] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_G,LINEAR_MATRIX_B_G_POST_ACTION)
				} else if (buffer[4] == 'B') {
					if ((buffer[5] == '0') && (buffer[6] == ':')) {	//Chroma level
						sscanf (buffer + 7, "%x", &data);
						if (data == 0x00) rcp->current_scene.saturation = 0x80;
						else rcp->current_scene.saturation = data;
						IHM_UPDATE_SCALE(saturation,SATURATION_POST_ACTION)
					} else if ((buffer[5] == '1') && (buffer[6] == ':')) {	//Color Temperature
						sscanf (buffer + 7, "%x", &data);
						rcp->current_scene.color_temperature = 0x078 - data;
						IHM_UPDATE_COMBO_BOX(color_temperature,NO_POST_ACTION)
					}
				}
			} else if ((buffer[2] == 'E') && (buffer[3] == ':')) {
				if ((buffer[4] == '2') && (buffer[5] == '0') && (buffer[6] == ':')) {	//Down-conversion mode
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[2].nb_parameters; i++) {
						if (buffer[7] == settings_array[2].answers[i][0]) {
							settings_parameters_indexes_array[2] = i;
							g_idle_add ((GSourceFunc)update_down_conversion_mode_combo_box_text, NULL);
							break;
						}
					}
#else
					if (buffer[7] != settings_array[2].answers[settings_parameters_indexes_array[2]][0])
						send_cam_control_command_string (rcp, settings_array[2].control_cmd, settings_array[2].parameters[settings_parameters_indexes_array[2]].value);
#endif
				} else if (buffer[4] == '3') {
					if ((buffer[5] == '1') && (buffer[6] == ':')) {	//Color matrix
						sscanf (buffer + 7, "%x", &rcp->current_scene.matrix_type);
						IHM_UPDATE_COMBO_BOX(matrix_type,MATRIX_TYPE_POST_ACTION)
					} else if ((buffer[5] == '3') && (buffer[6] == ':')) {	//DRS
						sscanf (buffer + 7, "%x", &data);
						rcp->current_scene.drs = 3 - data;
						IHM_UPDATE_COMBO_BOX(drs,DRS_POST_ACTION)
					}
				} else if (buffer[4] == '7') {
					if ((buffer[5] == '1') && (buffer[6] == ':')) {	//Preset playback range
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[4].nb_parameters; i++) {
							if (buffer[7] == settings_array[4].answers[i][0]) {
								settings_parameters_indexes_array[4] = i;
								g_idle_add ((GSourceFunc)update_preset_scope_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[7] != settings_array[4].answers[settings_parameters_indexes_array[4]][0])
							send_cam_control_command_string (rcp, settings_array[4].control_cmd, settings_array[4].parameters[settings_parameters_indexes_array[4]].value);
#endif
					} else if ((buffer[5] == '2') && (buffer[6] == ':')) {	//Gamma type
						sscanf (buffer + 7, "%x", &rcp->current_scene.gamma_type);
						IHM_UPDATE_COMBO_BOX(gamma_type,NO_POST_ACTION)
					} else if ((buffer[5] == '5') && (buffer[6] == ':')) {	//OSD Off With TALLY
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[12].nb_parameters; i++) {
							if (buffer[7] == settings_array[12].answers[i][0]) {
								settings_parameters_indexes_array[12] = i;
								g_idle_add ((GSourceFunc)update_osd_off_with_tally_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[7] != settings_array[12].answers[settings_parameters_indexes_array[12]][0])
							send_cam_control_command_string (rcp, settings_array[12].control_cmd, settings_array[12].parameters[settings_parameters_indexes_array[12]].value);
#endif
					} else if ((buffer[5] == '7') && (buffer[6] == ':')) {	//Frequency
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[0].nb_parameters; i++) {
							if (buffer[7] == settings_array[0].answers[i][0]) {
								settings_parameters_indexes_array[0] = i;

								if (settings_parameters_indexes_array[0] == 1) {
									format_is_50Hz = TRUE;
									settings_array[1].nb_parameters = 6;
									settings_array[1].parameters = format_parameters_array_pal;
									settings_array[1].answers = format_answers_array_pal;
									if ((settings_parameters_indexes_array[1] == 2) || (settings_parameters_indexes_array[1] == 5)) output_fps = _25fps;
									else output_fps = _50fps;
								} else {
									format_is_50Hz = FALSE;
									settings_array[1].nb_parameters = 8;
									settings_array[1].parameters = format_parameters_array_ntsc;
									settings_array[1].answers = format_answers_array_ntsc;
									if ((settings_parameters_indexes_array[1] == 3) || (settings_parameters_indexes_array[1] == 7)) output_fps = _23_98fps;
									else if ((settings_parameters_indexes_array[1] == 2) || (settings_parameters_indexes_array[1] == 6)) output_fps = _29_97fps;
									else output_fps = _59_94fps;
								}

								g_idle_add ((GSourceFunc)update_frequency_combo_box_text, NULL);
								g_thread_new (NULL, (GThreadFunc)ask_format, rcp);
								break;
							}
						}
#else
						if (buffer[7] != settings_array[0].answers[settings_parameters_indexes_array[0]][0]) {
							send_cam_control_command_string (rcp, settings_array[0].control_cmd, settings_array[0].parameters[settings_parameters_indexes_array[0]].value);
							g_thread_new (NULL, (GThreadFunc)ask_format, rcp);
						}
#endif
					} else if ((buffer[5] == 'B') && (buffer[6] == ':')) {	//OSD Mix
#ifdef MAIN_SETTINGS_READ_ONLY
						sscanf (buffer + 7, "%x", &osd_mix);
						g_idle_add ((GSourceFunc)update_osd_mix, NULL);
#else
						sscanf (buffer + 7, "%x", &data);
						if (data != osd_mix) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix, TRUE);
#endif
					}
				}
			} else if ((buffer[2] == 'H') && (buffer[3] == ':')) {	//Shutter type & step
				if (buffer[4] == '0') rcp->current_scene.shutter_type = 0;
				else if (buffer[4] == 'B') rcp->current_scene.shutter_type = 2;
				else if (buffer[4] == 'C') rcp->current_scene.shutter_type = 3;
				else {
					rcp->current_scene.shutter_type = 1;

					if (buffer[4] == '2') rcp->current_scene.shutter_step = 0;
					else if (buffer[4] == '3') {
						if (output_fps == _59_94fps) rcp->current_scene.shutter_step = 0;
						else rcp->current_scene.shutter_step = 1;
					} else if (buffer[4] == '4') rcp->current_scene.shutter_step = 1;
					else if (buffer[4] == '5') rcp->current_scene.shutter_step = 2;
					else if (buffer[4] == '6') rcp->current_scene.shutter_step = 3;
					else if (buffer[4] == '7') rcp->current_scene.shutter_step = 4;
					else if (buffer[4] == '8') rcp->current_scene.shutter_step = 5;
					else if (buffer[4] == '9') rcp->current_scene.shutter_step = 6;
					else if (buffer[4] == 'A') rcp->current_scene.shutter_step = 7;
					else rcp->current_scene.shutter_step = 8;
				}
				IHM_UPDATE_COMBO_BOX(shutter_type,SHUTTER_TYPE_POST_ACTION)
			}
		} else if ((buffer[1] == 'T') && (buffer[2] == 'P') && (buffer[3] == ':')) {	//Pedestal
			sscanf (buffer + 4, "%x", &rcp->current_scene.pedestal);
			IHM_UPDATE_SCALE(pedestal,PEDESTAL_POST_ACTION)
		}
	} else if (buffer[0] == 'p') {	//Power On/Standby
		if (buffer[1] == '0') {
			if ((rcp->error_code != 0x30) && (rcp->error_code != 0x00)) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
			if (rcp->camera_is_on) {
				g_idle_add ((GSourceFunc)set_rcp_off, rcp);
				g_idle_add ((GSourceFunc)decrease_on_standby_count, rcp->camera_set);

				g_mutex_lock (&rcp->other_rcp_mutex);
				for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
					other_rcp = (rcp_t*)(gslist_itr->data);

					if ((other_rcp->error_code != 0x30) && (other_rcp->error_code != 0x00)) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
					g_idle_add ((GSourceFunc)set_rcp_off, other_rcp);
					g_idle_add ((GSourceFunc)decrease_on_standby_count, other_rcp->camera_set);
				}
				g_mutex_unlock (&rcp->other_rcp_mutex);
			}
		} else if (buffer[1] == '1') {
			if (rcp->camera_is_on == FALSE) {
				rcp_work_start (rcp, (GThreadFunc)start_rcp);
#ifdef MAIN_SETTINGS_READ_ONLY
				if (camera_settings_checked == FALSE) {
					camera_settings_checked = TRUE;
					g_thread_new (NULL, (GThreadFunc)check_camera_settings_ro, rcp);
				}
#endif
			}
		} else if (buffer[1] == '3') rcp_work_start (rcp, NULL);
	} else if ((buffer[0] == 'r') && (buffer[1] == 'E') && (buffer[2] == 'R')) {	//Error information
		if (buffer[3] == '0') {
			if (buffer[4] == '0') g_idle_add ((GSourceFunc)clear_rcp_error, rcp); //Normal
			else if ((buffer[4] == '1') || (buffer[4] == '2')) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
			else if (buffer[4] == '3') g_idle_add ((GSourceFunc)Motor_Driver_Error, rcp);
			else if (buffer[4] == '4') g_idle_add ((GSourceFunc)Pan_Sensor_Error, rcp);
			else if (buffer[4] == '5') g_idle_add ((GSourceFunc)Tilt_Sensor_Error, rcp);
			else if (buffer[4] == '6') g_idle_add ((GSourceFunc)Controller_RX_Over_run_Error, rcp);
			else if (buffer[4] == '7') g_idle_add ((GSourceFunc)Controller_RX_Framing_Error, rcp);
			else if (buffer[4] == '8') g_idle_add ((GSourceFunc)Network_RX_Over_run_Error, rcp);
			else if (buffer[4] == '9') g_idle_add ((GSourceFunc)Network_RX_Framing_Error, rcp);
			else if ((buffer[4] == 'A') || (buffer[4] == 'B')) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
		} else if (buffer[3] == '1') {
			if (buffer[4] == '7') g_idle_add ((GSourceFunc)Controller_RX_Command_Buffer_Overflow, rcp);
			else if (buffer[4] == '9') g_idle_add ((GSourceFunc)Network_RX_Command_Buffer_Overflow, rcp);
		} else if (buffer[3] == '2') {
			if (buffer[4] == '1') g_idle_add ((GSourceFunc)System_Error, rcp);
			else if (buffer[4] == '2') g_idle_add ((GSourceFunc)Spec_Limit_Over, rcp);
			else if (buffer[4] == '4') g_idle_add ((GSourceFunc)Network_communication_Error, rcp);
			else if (buffer[4] == '5') g_idle_add ((GSourceFunc)CAMERA_communication_Error, rcp);
			else if (buffer[4] == '6') g_idle_add ((GSourceFunc)CAMERA_RX_Over_run_Error, rcp);
			else if (buffer[4] == '7') g_idle_add ((GSourceFunc)CAMERA_RX_Framing_Error, rcp);
			else if (buffer[4] == '8') g_idle_add ((GSourceFunc)CAMERA_RX_Command_Buffer_Overflow, rcp);
		}
	} else if (buffer[0] == 't') {
		if ((buffer[1] == 'A') && (buffer[2] == 'E')) {	//Tally input
#ifdef MAIN_SETTINGS_READ_ONLY
			tally_input = buffer[3] - 48;
			g_idle_add ((GSourceFunc)update_tally_input_combo_box_text, NULL);
#else
			if ((buffer[3] - 48) != tally_input) {
				if (tally_input == 1) send_ptz_control_command (rcp, "#TAE1");
				else send_ptz_control_command (rcp, "#TAE0");
			}
#endif
		}
	} else if ((buffer[0] == 'X') && (buffer[1] == 'S') && (buffer[2] == 'F') && (buffer[3] == ':')) {	//Scene file
		send_cam_request_command (rcp, "QAW", &data);
		if (data != 9) send_cam_control_command (rcp, "OAW:9");
	}
}

#include <stdio.h>
FILE *un_file;

gpointer receive_update_notification (gpointer data)
{
#ifdef _WIN32
	int addrlen;
#elif defined (__linux)
	socklen_t addrlen;
#endif
	char buffer[556];
	struct sockaddr_in src_addr;
	GList *glist_itr;
	rcp_t *rcp;
	SOCKET src_socket;
	int memcmp_rcp_last_ctrl_cmd;

int msg_len;

	while (update_notification_started) {
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (update_notification_socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) break;

		msg_len = recv (src_socket, buffer, 556, 0);
fprintf (un_file, "%d\n", msg_len);
fwrite (buffer, 1, msg_len, un_file);
fflush (un_file);
		closesocket (src_socket);

		g_mutex_lock (&rcp_start_glist_mutex);
		for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
			rcp = (rcp_t*)(glist_itr->data);

			if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
				g_mutex_lock (&rcp->cmd_mutex);
				if (rcp->last_ctrl_cmd_len == 0) memcmp_rcp_last_ctrl_cmd = 1;
				else memcmp_rcp_last_ctrl_cmd = memcmp (buffer + 30, rcp->last_ctrl_cmd, rcp->last_ctrl_cmd_len);
				g_mutex_unlock (&rcp->cmd_mutex);

				if (memcmp_rcp_last_ctrl_cmd == 0) {
					rcp->last_ctrl_cmd[0] = '\0';
					rcp->last_ctrl_cmd_len = 0;
				} else proceed_update_notification (rcp, buffer + 30);

				break;
			}
		}
		g_mutex_unlock (&rcp_start_glist_mutex);
	}

	return NULL;
}

void init_update_notification (void)
{
	memset (&update_notification_address, 0, sizeof (struct sockaddr_in));
	update_notification_address.sin_family = AF_INET;
	update_notification_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);
	update_notification_address.sin_addr.s_addr = inet_addr (my_ip_address);

un_file = fopen ("update_notification.txt", "w");
}

void start_update_notification (void)
{
	update_notification_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind (update_notification_socket, (struct sockaddr *)&update_notification_address, sizeof (struct sockaddr_in)) == 0) {
		if (listen (update_notification_socket, MAX_CAMERAS * MAX_CAMERAS_SET) == 0) {
			update_notification_started = TRUE;
			update_notification_thread = g_thread_new (NULL, receive_update_notification, NULL);
		}
	}
}

void stop_update_notification (void)
{
	GList *glist_itr;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		if (((rcp_t*)(glist_itr->data))->error_code != 0x30) send_update_stop_cmd ((rcp_t*)(glist_itr->data));
	}

	update_notification_started = FALSE;

	closesocket (update_notification_socket);

//	if (update_notification_thread != NULL) g_thread_join (update_notification_thread);
	update_notification_thread = NULL;
}

