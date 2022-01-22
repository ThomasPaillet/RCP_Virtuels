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


void init_rcp (rcp_t *rcp)
{
	int i, j;

	rcp->active = TRUE;
	rcp->ip_address[0] = '\0';
	rcp->ip_address_is_valid = FALSE;

	memset (&rcp->address, 0, sizeof (struct sockaddr_in));
	rcp->address.sin_family = AF_INET;
	rcp->address.sin_port = htons (80);

	init_rcp_cmd_buffer (rcp);

	rcp->last_ctrl_cmd[0] = '\0';
	rcp->last_ctrl_cmd_len = 0;
	g_mutex_init (&rcp->cmd_mutex);

	g_mutex_init (&rcp->other_rcp_mutex);
	rcp->other_rcp = NULL;

	rcp->camera_is_on = FALSE;
	rcp->camera_is_working = FALSE;

	rcp->model = AW_UE150;

//	rcp->last_version_information_notification_time = 0;

	rcp->thread = NULL;
	rcp->last_time.tv_sec = 0;
	rcp->last_time.tv_usec = 0;
	rcp->timeout_id = 0;
	rcp->r_b = TRUE;

	rcp->jpeg_page = 1;

	rcp->error_code = 0x00;

	rcp->OSD_menu = FALSE;

	rcp->auto_focus = FALSE;

	rcp->mire = FALSE;
	rcp->day_night = FALSE;

	rcp->current_scene.ND_filter = ND_FILTER_DEFAULT;

	if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) rcp->current_scene.gain = GAIN_DEFAULT_AW_UE150;
	else rcp->current_scene.gain = GAIN_DEFAULT_AW_HE130;

	rcp->current_scene.gamma_type = GAMMA_TYPE_DEFAULT;
	rcp->current_scene.gamma = GAMMA_DEFAULT;
	rcp->current_scene.film_rec_dynamic_level = FILM_REC_DYNAMIC_LEVEL_DEFAULT;
	rcp->current_scene.film_rec_black_stretch_level = FILM_REC_BLACK_STRETCH_LEVEL_DEFAULT;
	rcp->current_scene.video_rec_knee_slope = VIDEO_REC_KNEE_SLOPE_DEFAULT;
	rcp->current_scene.video_rec_knee_point = VIDEO_REC_KNEE_POINT_DEFAULT;
	rcp->current_scene.black_gamma = BLACK_GAMMA_DEFAULT;
	rcp->current_scene.black_gamma_range = BLACK_GAMMA_RANGE_DEFAULT;
	rcp->current_scene.drs = DRS_DEFAULT;

	rcp->current_scene.chroma_phase = CHROMA_PHASE_DEFAULT;

	rcp->current_scene.color_temperature = COLOR_TEMPERATURE_DEFAULT_AW_UE150;

	rcp->current_scene.knee_settings = KNEE_SETTINGS_DEFAULT;
	rcp->current_scene.knee_point = KNEE_POINT_DEFAULT;
	rcp->current_scene.knee_slope = KNEE_SLOPE_DEFAULT_AW_UE150;

	rcp->current_scene.auto_knee_response = AUTO_KNEE_RESPONSE_DEFAULT;

	rcp->current_scene.HLG_knee = HLG_KNEE_DEFAULT;
	rcp->current_scene.HLG_knee_point = HLG_KNEE_POINT_DEFAULT;
	rcp->current_scene.HLG_knee_slope = HLG_KNEE_SLOPE_DEFAULT;

	rcp->current_scene.matrix_type = MATRIX_TYPE_DEFAULT;
	rcp->current_scene.adaptive_matrix = ADAPTIVE_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.R_G = LINEAR_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.R_B = LINEAR_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.G_R = LINEAR_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.G_B = LINEAR_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.B_R = LINEAR_MATRIX_DEFAULT;
	rcp->current_scene.linear_matrix.B_G = LINEAR_MATRIX_DEFAULT;

	for (j = 0; j < 15; j++) {
		rcp->current_scene.cc_saturation[j] = COLOR_CORRECTION_DEFAULT;
		rcp->current_scene.cc_phase[j] = COLOR_CORRECTION_DEFAULT;
	}

	rcp->current_scene.detail = FALSE;
	rcp->current_scene.master_detail = MASTER_DETAIL_DEFAULT;
	rcp->current_scene.detail_coring = DETAIL_CORING_DEFAULT;
	rcp->current_scene.v_detail_level = V_DETAIL_LEVEL_DEFAULT;
	rcp->current_scene.detail_band = DETAIL_BAND_DEFAULT;
	rcp->current_scene.noise_suppress = NOISE_SUPPRESS_DEFAULT;
	rcp->current_scene.level_depend = LEVEL_DEPEND_DEFAULT;
	rcp->current_scene.knee_aperture_level = KNEE_APERTURE_LEVEL_DEFAULT;
	rcp->current_scene.detail_gain_plus = DETAIL_GAIN_PLUS_DEFAULT;
	rcp->current_scene.detail_gain_minus = DETAIL_GAIN_MINUS_DEFAULT;
	rcp->current_scene.skin_detail = SKIN_DETAIL_DEFAULT;
	rcp->current_scene.fleshtone_noisesup = FLESHTONE_NOISESUP_DEFAULT_AW_UE150;

	rcp->current_scene.saturation = SATURATION_DEFAULT;

	rcp->current_scene.r_gain = R_GAIN_DEFAULT_AW_UE150;
	rcp->r_gain_need_update = FALSE;
	rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;
	rcp->current_scene.b_gain = B_GAIN_DEFAULT_AW_UE150;
	rcp->b_gain_need_update = FALSE;

	rcp->current_scene.r_pedestal = R_PEDESTAL_DEFAULT;
	rcp->r_pedestal_need_update = FALSE;
	rcp->current_scene.g_pedestal = G_PEDESTAL_DEFAULT;
	rcp->current_scene.b_pedestal = B_PEDESTAL_DEFAULT;
	rcp->b_pedestal_need_update = FALSE;

	rcp->current_scene.shutter_type = SHUTTER_TYPE_DEFAULT;
	rcp->current_scene.shutter_step = SHUTTER_STEP_DEFAULT_AW_UE150;
	if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) rcp->current_scene.shutter_synchro = 600;
	else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) rcp->current_scene.shutter_synchro = 500;
	else if (output_fps_AW_UE150 == fps_29_97p) rcp->current_scene.shutter_synchro = 300;
	else if (output_fps_AW_UE150 == fps_25p) rcp->current_scene.shutter_synchro = 250;
	else rcp->current_scene.shutter_synchro = 240;
	rcp->current_scene.ELC_limit = ELC_LIMIT_DEFAULT;

	rcp->current_scene.pedestal = PEDESTAL_DEFAULT_AW_UE150;

	rcp->current_scene.iris = IRIS_DEFAULT;
	rcp->current_scene.iris_auto = IRIS_AUTO_DEFAULT;

	for (i = 0; i < NB_SCENES; i++) {
		rcp->scenes[i].ND_filter = ND_FILTER_DEFAULT;

		if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) rcp->scenes[i].gain = GAIN_DEFAULT_AW_UE150;
		else rcp->scenes[i].gain = GAIN_DEFAULT_AW_HE130;

		rcp->scenes[i].gamma_type = GAMMA_TYPE_DEFAULT;
		rcp->scenes[i].gamma = GAMMA_DEFAULT;
		rcp->scenes[i].film_rec_dynamic_level = FILM_REC_DYNAMIC_LEVEL_DEFAULT;
		rcp->scenes[i].film_rec_black_stretch_level = FILM_REC_BLACK_STRETCH_LEVEL_DEFAULT;
		rcp->scenes[i].video_rec_knee_slope = VIDEO_REC_KNEE_SLOPE_DEFAULT;
		rcp->scenes[i].video_rec_knee_point = VIDEO_REC_KNEE_POINT_DEFAULT;
		rcp->scenes[i].black_gamma = BLACK_GAMMA_DEFAULT;
		rcp->scenes[i].black_gamma_range = BLACK_GAMMA_RANGE_DEFAULT;
		rcp->scenes[i].drs = DRS_DEFAULT;

		rcp->scenes[i].chroma_phase = CHROMA_PHASE_DEFAULT;

		rcp->scenes[i].color_temperature = COLOR_TEMPERATURE_DEFAULT_AW_UE150;

		rcp->scenes[i].knee_settings = KNEE_SETTINGS_DEFAULT;
		rcp->scenes[i].knee_point = KNEE_POINT_DEFAULT;
		rcp->scenes[i].knee_slope = KNEE_SLOPE_DEFAULT_AW_UE150;

		rcp->scenes[i].auto_knee_response = AUTO_KNEE_RESPONSE_DEFAULT;

		rcp->scenes[i].HLG_knee = HLG_KNEE_DEFAULT;
		rcp->scenes[i].HLG_knee_point = HLG_KNEE_POINT_DEFAULT;
		rcp->scenes[i].HLG_knee_slope = HLG_KNEE_SLOPE_DEFAULT;

		rcp->scenes[i].matrix_type = MATRIX_TYPE_DEFAULT;
		rcp->scenes[i].adaptive_matrix = ADAPTIVE_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.R_G = LINEAR_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.R_B = LINEAR_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.G_R = LINEAR_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.G_B = LINEAR_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.B_R = LINEAR_MATRIX_DEFAULT;
		rcp->scenes[i].linear_matrix.B_G = LINEAR_MATRIX_DEFAULT;
		for (j = 0; j < 15; j++) {
			rcp->scenes[i].cc_saturation[j] = COLOR_CORRECTION_DEFAULT;
			rcp->scenes[i].cc_phase[j] = COLOR_CORRECTION_DEFAULT;
		}

		rcp->scenes[i].detail = DETAIL_DEFAULT;
		rcp->scenes[i].master_detail = MASTER_DETAIL_DEFAULT;
		rcp->scenes[i].detail_coring = DETAIL_CORING_DEFAULT;
		rcp->scenes[i].v_detail_level = V_DETAIL_LEVEL_DEFAULT;
		rcp->scenes[i].detail_band = DETAIL_BAND_DEFAULT;
		rcp->scenes[i].noise_suppress = NOISE_SUPPRESS_DEFAULT;
		rcp->scenes[i].level_depend = LEVEL_DEPEND_DEFAULT;
		rcp->scenes[i].knee_aperture_level = KNEE_APERTURE_LEVEL_DEFAULT;
		rcp->scenes[i].detail_gain_plus = DETAIL_GAIN_PLUS_DEFAULT;
		rcp->scenes[i].detail_gain_minus = DETAIL_GAIN_MINUS_DEFAULT;
		rcp->scenes[i].skin_detail = SKIN_DETAIL_DEFAULT;
		rcp->scenes[i].fleshtone_noisesup = FLESHTONE_NOISESUP_DEFAULT_AW_UE150;

		rcp->scenes[i].saturation = SATURATION_DEFAULT;

		rcp->scenes[i].r_gain = R_GAIN_DEFAULT_AW_UE150;
		rcp->scenes[i].b_gain = B_GAIN_DEFAULT_AW_UE150;

		rcp->scenes[i].r_pedestal = R_PEDESTAL_DEFAULT;
		rcp->scenes[i].g_pedestal = G_PEDESTAL_DEFAULT;
		rcp->scenes[i].b_pedestal = B_PEDESTAL_DEFAULT;

		rcp->scenes[i].shutter_type = SHUTTER_TYPE_DEFAULT;
		rcp->scenes[i].shutter_step = SHUTTER_STEP_DEFAULT_AW_UE150;
		if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) rcp->scenes[i].shutter_synchro = 600;
		else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) rcp->scenes[i].shutter_synchro = 500;
		else if (output_fps_AW_UE150 == fps_29_97p) rcp->scenes[i].shutter_synchro = 300;
		else if (output_fps_AW_UE150 == fps_25p) rcp->scenes[i].shutter_synchro = 250;
		else rcp->scenes[i].shutter_synchro = 240;
		rcp->scenes[i].ELC_limit = ELC_LIMIT_DEFAULT;

		rcp->scenes[i].pedestal = PEDESTAL_DEFAULT_AW_UE150;

		rcp->scenes[i].iris = IRIS_DEFAULT;
		rcp->scenes[i].iris_auto = IRIS_AUTO_DEFAULT;
	}

	rcp->knee_window = NULL;
	rcp->HLG_knee_window = NULL;
	rcp->matrix_window = NULL;
	rcp->selected_color = nothing;
	rcp->detail_window = NULL;

	rcp->shutter_synchro_window = NULL;

	rcp->tally_data = 0x00;
	rcp->ip_tally_is_on = FALSE;

	rcp->root_widget = NULL;

	rcp->source_rcp_list_box_row = NULL;
	rcp->destination_rcp_list_box_row = NULL;
}

gboolean rcp_start_working (rcp_t *rcp)
{
	rcp->camera_is_working = TRUE;

	gtk_widget_show (rcp->spinner);
	gtk_spinner_start (GTK_SPINNER (rcp->spinner));

	gtk_widget_set_sensitive (rcp->on_standby_switch, FALSE);
	gtk_widget_set_sensitive (rcp->standard_button, FALSE);
	gtk_widget_set_sensitive (rcp->mire_toggle_button, FALSE);
	gtk_widget_set_sensitive (rcp->day_night_toggle_button, FALSE);
	gtk_widget_set_sensitive (rcp->sensitive_widgets, FALSE);

	return G_SOURCE_REMOVE;
}

gboolean set_rcp_off (rcp_t *rcp)
{
	int i, j;
	cameras_set_t *cameras_set;

	rcp->camera_is_on = FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->thread != NULL) {
		g_thread_join (rcp->thread);
		rcp->thread = NULL;
	}

	if (rcp->root_widget == NULL) {
		g_mutex_clear (&rcp->cmd_mutex);
		g_free (rcp);

		return G_SOURCE_REMOVE;
	}

	g_signal_handler_block (rcp->on_standby_switch, rcp->on_standby_handler_id);
	gtk_switch_set_active (GTK_SWITCH (rcp->on_standby_switch), FALSE);
	g_signal_handler_unblock (rcp->on_standby_switch, rcp->on_standby_handler_id);

	gtk_widget_set_sensitive (rcp->on_standby_switch, TRUE);
	gtk_widget_set_sensitive (rcp->standard_button, FALSE);
	gtk_widget_set_sensitive (rcp->mire_toggle_button, FALSE);
	gtk_widget_set_sensitive (rcp->day_night_toggle_button, FALSE);
	gtk_widget_set_sensitive (rcp->sensitive_widgets, FALSE);

	g_signal_handler_block (rcp->mire_toggle_button, rcp->mire_handler_id);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->mire_toggle_button), FALSE);
	g_signal_handler_unblock (rcp->mire_toggle_button, rcp->mire_handler_id);

	g_signal_handler_block (rcp->day_night_toggle_button, rcp->day_night_handler_id);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->day_night_toggle_button), FALSE);
	g_signal_handler_unblock (rcp->day_night_toggle_button, rcp->day_night_handler_id);

	g_signal_handler_block (rcp->ND_filter_combo_box, rcp->ND_filter_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), -1);
	g_signal_handler_unblock (rcp->ND_filter_combo_box, rcp->ND_filter_handler_id);

	g_signal_handler_block (rcp->gain_combo_box, rcp->gain_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gain_combo_box), -1);
	g_signal_handler_unblock (rcp->gain_combo_box, rcp->gain_handler_id);

	g_signal_handler_block (rcp->gamma_type_combo_box, rcp->gamma_type_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), -1);
	g_signal_handler_unblock (rcp->gamma_type_combo_box, rcp->gamma_type_handler_id);

	g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), -1);
	g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

	g_signal_handler_block (rcp->knee_settings_combo_box, rcp->knee_settings_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->knee_settings_combo_box), -1);
	g_signal_handler_unblock (rcp->knee_settings_combo_box, rcp->knee_settings_handler_id);

	g_signal_handler_block (rcp->matrix_type_combo_box, rcp->matrix_type_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->matrix_type_combo_box), -1);
	g_signal_handler_unblock (rcp->matrix_type_combo_box, rcp->matrix_type_handler_id);

	g_signal_handler_block (rcp->detail_toggle_button, rcp->detail_handler_id);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->detail_toggle_button), FALSE);
	gtk_button_set_label (GTK_BUTTON (rcp->detail_toggle_button), "Off");
	g_signal_handler_unblock (rcp->detail_toggle_button, rcp->detail_handler_id);

	g_signal_handler_block (rcp->saturation_scale, rcp->saturation_handler_id);
	gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), SATURATION_DEFAULT);
	g_signal_handler_unblock (rcp->saturation_scale, rcp->saturation_handler_id);
	rcp->current_scene.saturation = SATURATION_DEFAULT;
	set_saturation_label (rcp);

	g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), R_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

	g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
	rcp->current_scene.g_pedestal = G_PEDESTAL_DEFAULT;

	g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), B_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

	g_signal_handler_block (rcp->shutter_type_combo_box, rcp->shutter_type_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_type_combo_box), -1);
	g_signal_handler_unblock (rcp->shutter_type_combo_box, rcp->shutter_type_handler_id);

	g_signal_handler_block (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_step_combo_box), -1);
	g_signal_handler_unblock (rcp->shutter_step_combo_box, rcp->shutter_step_handler_id);
	rcp->current_scene.shutter_step = -1;

	gtk_widget_hide (rcp->shutter_step_combo_box);
	gtk_widget_hide (rcp->shutter_synchro_button);

	g_signal_handler_block (rcp->iris_scale, rcp->iris_handler_id);
	gtk_range_set_value (GTK_RANGE (rcp->iris_scale), IRIS_DEFAULT);
	g_signal_handler_unblock (rcp->iris_scale, rcp->iris_handler_id);

	g_signal_handler_block (rcp->iris_auto_toggle_button, rcp->iris_auto_handler_id);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->iris_auto_toggle_button), FALSE);
	g_signal_handler_unblock (rcp->iris_auto_toggle_button, rcp->iris_auto_handler_id);

	if (rcp->model == AW_UE150) {
		g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), R_GAIN_DEFAULT_AW_UE150);
		g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;
		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);

		g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), B_GAIN_DEFAULT_AW_UE150);
		g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

		g_signal_handler_block (rcp->HLG_knee_toggle_button, rcp->HLG_knee_handler_id);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->HLG_knee_toggle_button), FALSE);
		gtk_button_set_label (GTK_BUTTON (rcp->HLG_knee_toggle_button), "Off");
		g_signal_handler_unblock (rcp->HLG_knee_toggle_button, rcp->HLG_knee_handler_id);

		g_signal_handler_block (rcp->ELC_limit_combo_box, rcp->ELC_limit_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ELC_limit_combo_box), ELC_LIMIT_DEFAULT);
		g_signal_handler_unblock (rcp->ELC_limit_combo_box, rcp->ELC_limit_handler_id);

		gtk_widget_hide (rcp->ELC_limit_label);
		gtk_widget_hide (rcp->ELC_limit_combo_box);

		g_signal_handler_block (rcp->pedestal_scale, rcp->pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_UE150);
		rcp->current_scene.pedestal = PEDESTAL_DEFAULT_AW_UE150;
		set_pedestal_label_AW_UE150 (rcp);
		g_signal_handler_unblock (rcp->pedestal_scale, rcp->pedestal_handler_id);
	} else {
		g_signal_handler_block (rcp->gamma_combo_box, rcp->gamma_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_combo_box), -1);
		g_signal_handler_unblock (rcp->gamma_combo_box, rcp->gamma_handler_id);

		g_signal_handler_block (rcp->drs_combo_box, rcp->drs_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->drs_combo_box), -1);
		g_signal_handler_unblock (rcp->drs_combo_box, rcp->drs_handler_id);

		g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), R_GAIN_DEFAULT_AW_HE130);
		g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_HE130;
		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_HE130);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);

		g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), B_GAIN_DEFAULT_AW_HE130);
		g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

		g_signal_handler_block (rcp->pedestal_scale, rcp->pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_HE130);
		rcp->current_scene.pedestal = PEDESTAL_DEFAULT_AW_HE130;
		set_pedestal_label_AW_HE130 (rcp);
		g_signal_handler_unblock (rcp->pedestal_scale, rcp->pedestal_handler_id);
	}

	cameras_set = (cameras_set_t*)(rcp->cameras_set);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		if (!cameras_set->rcp_ptr_array[i]->active) j++;
		else if (!cameras_set->rcp_ptr_array[i]->camera_is_on) j++;
	}

	if (j == cameras_set->number_of_cameras) {
		g_signal_handler_block (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
		gtk_switch_set_active (GTK_SWITCH (cameras_set->master_rcp.on_standby_switch), FALSE);
		g_signal_handler_unblock (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
	}

	gtk_spinner_stop (GTK_SPINNER (rcp->spinner));
	gtk_widget_hide (rcp->spinner);

	rcp->camera_is_working = FALSE;

	return G_SOURCE_REMOVE;
}

gpointer check_if_camera_is_on (rcp_t *rcp)
{
	int response;
	GSList *gslist_itr;

	send_update_start_cmd (rcp);

	if (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR) {
		send_ptz_request_command (rcp, "#O", &response);

		if (response == 1) {
			rcp->camera_is_on = TRUE;

			g_mutex_lock (&rcp->other_rcp_mutex);
			for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next)
				((rcp_t*)(gslist_itr->data))->camera_is_on = TRUE;
			g_mutex_unlock (&rcp->other_rcp_mutex);

			start_rcp (rcp);
		} else {
			g_idle_add ((GSourceFunc)set_rcp_off, rcp);

			g_mutex_lock (&rcp->other_rcp_mutex);
			for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next)
				g_idle_add ((GSourceFunc)set_rcp_off, gslist_itr->data);
			g_mutex_unlock (&rcp->other_rcp_mutex);
		}
	}

	return NULL;
}

gpointer start_rcp (rcp_t *rcp)
{
	int response;
	char response_string[16];
	GSList *gslist_itr;
	rcp_t *other_rcp;

	send_cam_request_command_string (rcp, "QID", response_string);
	if (rcp->model == AW_UE150) {
		if (memcmp (response_string, "AW-HE130", 8) == 0) {
			init_rcp_AW_HE130 (rcp);
			g_idle_add ((GSourceFunc)switch_to_AW_HE130, rcp);

			g_mutex_lock (&rcp->other_rcp_mutex);
			for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
				init_rcp_AW_HE130 (gslist_itr->data);
				g_idle_add ((GSourceFunc)switch_to_AW_HE130, gslist_itr->data);
			}
			g_mutex_unlock (&rcp->other_rcp_mutex);
		}
	} else {
		if (memcmp (response_string, "AW-UE150", 8) == 0) {
			init_rcp_AW_UE150 (rcp);
			g_idle_add ((GSourceFunc)switch_to_AW_UE150, rcp);

			g_mutex_lock (&rcp->other_rcp_mutex);
			for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
				init_rcp_AW_UE150 (gslist_itr->data);
				g_idle_add ((GSourceFunc)switch_to_AW_UE150, gslist_itr->data);
			}
			g_mutex_unlock (&rcp->other_rcp_mutex);
		}
	}

	if (rcp->model == AW_UE150) check_camera_settings_AW_UE150 (rcp);
	else check_camera_settings_AW_HE130 (rcp);

	check_camera_settings (rcp);

	send_cam_request_command (rcp, "QAF", &response);
	if (response) rcp->auto_focus = TRUE;
	else rcp->auto_focus = FALSE;

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.auto_focus != rcp->auto_focus)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.auto_focus = rcp->auto_focus;
		send_auto_focus_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_cam_request_command (rcp, "QBR", &response);
	if (response) rcp->mire = TRUE;
	else rcp->mire = FALSE;

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.mire != rcp->mire)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.mire = rcp->mire;
		send_mire_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_ptz_request_command (rcp, "#D6", &response);
	if (response) rcp->day_night = TRUE;
	else rcp->day_night = FALSE;

	send_cam_request_command (rcp, "QFT", &rcp->current_scene.ND_filter);

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.ND_filter != rcp->current_scene.ND_filter)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.ND_filter = rcp->current_scene.ND_filter;
		send_ND_filter_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_cam_request_command (rcp, "QGU", &response);
	if ((rcp->model == AW_UE150) && (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1)) {
		if (response == 0x80) rcp->current_scene.gain = 43;
		else rcp->current_scene.gain = 0x32 - response;

		response = 0x2C - response;

		if ((physical_rcp.connected) && (rcp == rcp_vision) && (response >= 0) && (physical_rcp.gain != response)) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.gain = response;
			send_gain_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	} else {
		if (response == 0x80) rcp->current_scene.gain = 37;
		else rcp->current_scene.gain = 0x2C - response;

		if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.gain != rcp->current_scene.gain)) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.gain = rcp->current_scene.gain;
			send_gain_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}

	send_cam_request_command (rcp, "QSE:72", &rcp->current_scene.gamma_type);
	send_cam_request_command (rcp, "QSA:6A", &response);
	rcp->current_scene.gamma = 0x94 - response;
	send_cam_request_command (rcp, "QSE:33", &response);
	rcp->current_scene.drs = 3 - response;

	send_cam_request_command (rcp, "QSA:2D", &rcp->current_scene.knee_settings);

	send_cam_request_command (rcp, "QSA:20", &rcp->current_scene.knee_point);
	send_cam_request_command (rcp, "QSA:24", &rcp->current_scene.knee_slope);

	send_cam_request_command (rcp, "QSE:31", &rcp->current_scene.matrix_type);

	send_cam_request_command (rcp, "QSD:A4", &rcp->current_scene.linear_matrix.R_G);
	send_cam_request_command (rcp, "QSD:A5", &rcp->current_scene.linear_matrix.R_B);
	send_cam_request_command (rcp, "QSD:A6", &rcp->current_scene.linear_matrix.G_R);
	send_cam_request_command (rcp, "QSD:A7", &rcp->current_scene.linear_matrix.G_B);
	send_cam_request_command (rcp, "QSD:A8", &rcp->current_scene.linear_matrix.B_R);
	send_cam_request_command (rcp, "QSD:A9", &rcp->current_scene.linear_matrix.B_G);

	send_cam_request_command (rcp, "QSD:86", &response);
	rcp->current_scene.cc_saturation[R] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9C", &response);
	rcp->current_scene.cc_saturation[R_R_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:88", &response);
	rcp->current_scene.cc_saturation[R_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9E", &response);
	rcp->current_scene.cc_saturation[R_Yl_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8A", &response);
	rcp->current_scene.cc_saturation[Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8C", &response);
	rcp->current_scene.cc_saturation[Yl_G] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8E", &response);
	rcp->current_scene.cc_saturation[G] = response - 0x80;
	send_cam_request_command (rcp, "QSD:90", &response);
	rcp->current_scene.cc_saturation[G_Cy] = response - 0x80;
	send_cam_request_command (rcp, "QSD:92", &response);
	rcp->current_scene.cc_saturation[Cy] = response - 0x80;
	send_cam_request_command (rcp, "QSD:94", &response);
	rcp->current_scene.cc_saturation[Cy_B] = response - 0x80;
	send_cam_request_command (rcp, "QSD:96", &response);
	rcp->current_scene.cc_saturation[B] = response - 0x80;
	send_cam_request_command (rcp, "QSD:80", &response);
	rcp->current_scene.cc_saturation[B_Mg] = response - 0x80;
	send_cam_request_command (rcp, "QSD:82", &response);
	rcp->current_scene.cc_saturation[Mg] = response - 0x80;
	send_cam_request_command (rcp, "QSD:84", &response);
	rcp->current_scene.cc_saturation[Mg_R] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9A", &response);
	rcp->current_scene.cc_saturation[Mg_R_R] = response - 0x80;

	send_cam_request_command (rcp, "QSD:87", &response);
	rcp->current_scene.cc_phase[R] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9D", &response);
	rcp->current_scene.cc_phase[R_R_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:89", &response);
	rcp->current_scene.cc_phase[R_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9F", &response);
	rcp->current_scene.cc_phase[R_Yl_Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8B", &response);
	rcp->current_scene.cc_phase[Yl] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8D", &response);
	rcp->current_scene.cc_phase[Yl_G] = response - 0x80;
	send_cam_request_command (rcp, "QSD:8F", &response);
	rcp->current_scene.cc_phase[G] = response - 0x80;
	send_cam_request_command (rcp, "QSD:91", &response);
	rcp->current_scene.cc_phase[G_Cy] = response - 0x80;
	send_cam_request_command (rcp, "QSD:93", &response);
	rcp->current_scene.cc_phase[Cy] = response - 0x80;
	send_cam_request_command (rcp, "QSD:95", &response);
	rcp->current_scene.cc_phase[Cy_B] = response - 0x80;
	send_cam_request_command (rcp, "QSD:97", &response);
	rcp->current_scene.cc_phase[B] = response - 0x80;
	send_cam_request_command (rcp, "QSD:81", &response);
	rcp->current_scene.cc_phase[B_Mg] = response - 0x80;
	send_cam_request_command (rcp, "QSD:83", &response);
	rcp->current_scene.cc_phase[Mg] = response - 0x80;
	send_cam_request_command (rcp, "QSD:85", &response);
	rcp->current_scene.cc_phase[Mg_R] = response - 0x80;
	send_cam_request_command (rcp, "QSD:9B", &response);
	rcp->current_scene.cc_phase[Mg_R_R] = response - 0x80;

	send_cam_request_command (rcp, "QDT", &response);
	if (response) rcp->current_scene.detail = TRUE;
	else rcp->current_scene.detail = FALSE;

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.detail != rcp->current_scene.detail)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.detail = rcp->current_scene.detail;
		send_detail_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_cam_request_command (rcp, "QSA:30", &rcp->current_scene.master_detail);

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.master_detail != rcp->current_scene.master_detail)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.master_detail = rcp->current_scene.master_detail;
		send_master_detail_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_cam_request_command (rcp, "QSD:A1", &rcp->current_scene.v_detail_level);
	send_cam_request_command (rcp, "QSD:A2", &rcp->current_scene.detail_band);
	send_cam_request_command (rcp, "QSD:A3", &rcp->current_scene.fleshtone_noisesup);

	send_cam_request_command (rcp, "QSD:B0", &response);
	if (response == 0x00) rcp->current_scene.saturation = 0x80;
	else rcp->current_scene.saturation = response;

	send_cam_request_command (rcp, "QRP", &rcp->current_scene.r_pedestal);

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.r_pedestal != rcp->current_scene.r_pedestal)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
		send_r_pedestal_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_cam_request_command (rcp, "QBP", &rcp->current_scene.b_pedestal);

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.b_pedestal != rcp->current_scene.b_pedestal)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
		send_b_pedestal_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	send_ptz_request_command_string (rcp, "#GI", response_string);
	if (response_string[2] <= '9') response = ((int)(response_string[2] - '0')) << 8;
	else response = ((int)(response_string[2] - '7')) << 8;
	if (response_string[3] <= '9') response += (response_string[3] - '0') << 4;
	else response += ((int)(response_string[3] - '7')) << 4;
	if (response_string[4] <= '9') response += response_string[4] - '0';
	else response += response_string[4] - '7';
	rcp->current_scene.iris = response;

	if (response_string[5] == '1') rcp->current_scene.iris_auto = TRUE;
	else rcp->current_scene.iris_auto = FALSE;

	if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.iris_auto != rcp->current_scene.iris_auto)) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.iris_auto = rcp->current_scene.iris_auto;
		send_iris_auto_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}

	if (rcp->model == AW_UE150) {
		send_cam_request_command (rcp, "QSA:10", &response);
		rcp->current_scene.film_rec_dynamic_level = 4 - response;

		send_cam_request_command (rcp, "QSA:0F", &rcp->current_scene.film_rec_black_stretch_level);

		send_cam_request_command (rcp, "QSA:25", &response);
		rcp->current_scene.video_rec_knee_slope = 0x83 - response;

		send_cam_request_command (rcp, "QSA:21", &rcp->current_scene.video_rec_knee_point);

		send_cam_request_command (rcp, "QSA:07", &rcp->current_scene.black_gamma);

		send_cam_request_command (rcp, "QSJ:1B", &response);
		rcp->current_scene.black_gamma_range = 3 - response;

		send_cam_request_command (rcp, "QSJ:0B", &rcp->current_scene.chroma_phase);

		send_cam_request_command_2 (rcp, "QSI:20", &response);
		rcp->current_scene.color_temperature = find_color_temperature_index (response);

		send_cam_request_command (rcp, "QSG:97", &rcp->current_scene.auto_knee_response);

		send_cam_request_command (rcp, "QSI:40", &response);
		if (response) rcp->current_scene.HLG_knee = TRUE;
		else rcp->current_scene.HLG_knee = FALSE;

		send_cam_request_command (rcp, "QSI:41", &rcp->current_scene.HLG_knee_point);
		send_cam_request_command (rcp, "QSI:42", &rcp->current_scene.HLG_knee_slope);

		send_cam_request_command (rcp, "QSJ:4F", &response);
		if (response) rcp->current_scene.adaptive_matrix = TRUE;
		else rcp->current_scene.adaptive_matrix = FALSE;

		send_cam_request_command (rcp, "QSJ:12", &rcp->current_scene.detail_coring);
		send_cam_request_command (rcp, "QSJ:13", &rcp->current_scene.level_depend);
		send_cam_request_command (rcp, "QSG:3F", &rcp->current_scene.knee_aperture_level);
		send_cam_request_command (rcp, "QSA:38", &rcp->current_scene.detail_gain_plus);
		send_cam_request_command (rcp, "QSA:39", &rcp->current_scene.detail_gain_minus);
		send_cam_request_command (rcp, "QSA:40", &response);
		if (response) rcp->current_scene.skin_detail = TRUE;
		else rcp->current_scene.skin_detail = FALSE;

		send_cam_request_command (rcp, "QSG:39", &rcp->current_scene.r_gain);

		if ((physical_rcp.connected) && (rcp == rcp_vision)) {
			response = (rcp->current_scene.r_gain - 0x738) * 0.75;

			if (physical_rcp.r_gain != response) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = response;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		}

		send_cam_request_command (rcp, "QSG:3A", &rcp->current_scene.b_gain);

		if ((physical_rcp.connected) && (rcp == rcp_vision)) {
			response = (rcp->current_scene.b_gain - 0x738) * 0.75;

			if (physical_rcp.b_gain != response) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = response;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		}

		send_cam_request_command (rcp, "QSJ:10", &rcp->current_scene.g_pedestal);

		send_cam_request_command (rcp, "QSJ:03", &rcp->current_scene.shutter_type);

		send_cam_request_command (rcp, "QSJ:06", &rcp->current_scene.shutter_step);

		send_cam_request_command (rcp, "QSJ:09", &rcp->current_scene.shutter_synchro);

		send_cam_request_command (rcp, "QSD:BF", &response);
		rcp->current_scene.ELC_limit = response - 2;

		send_cam_request_command (rcp, "QSJ:0F", &rcp->current_scene.pedestal);
	} else {
		send_cam_request_command (rcp, "QSD:B1", &response);
		rcp->current_scene.color_temperature = 0x078 - response;

		send_cam_request_command (rcp, "QSD:22", &rcp->current_scene.noise_suppress);

		send_cam_request_command (rcp, "QSH", &response);
		if (response == 0x0) rcp->current_scene.shutter_type = 0;
		else if (response == 0xB) rcp->current_scene.shutter_type = 2;
		else if (response == 0xC) rcp->current_scene.shutter_type = 3;
		else {
			rcp->current_scene.shutter_type = 1;

			if (response == 0x2) rcp->current_scene.shutter_step = 0;
			else if (response == 0x3) {
				if ((output_fps_AW_HE130 == fps_59_94p) || (output_fps_AW_HE130 == fps_59_94i)) rcp->current_scene.shutter_step = 0;
				else rcp->current_scene.shutter_step = 1;
			} else if (response == 0x4) rcp->current_scene.shutter_step = 1;
			else if (response == 0x5) rcp->current_scene.shutter_step = 2;
			else if (response == 0x6) rcp->current_scene.shutter_step = 3;
			else if (response == 0x7) rcp->current_scene.shutter_step = 4;
			else if (response == 0x8) rcp->current_scene.shutter_step = 5;
			else if (response == 0x9) rcp->current_scene.shutter_step = 6;
			else if (response == 0xA) rcp->current_scene.shutter_step = 7;
			else rcp->current_scene.shutter_step = 8;
		}

		if ((physical_rcp.connected) && (rcp == rcp_vision) && ((physical_rcp.shutter_type != rcp->current_scene.shutter_type) || \
			((physical_rcp.shutter_type == 1) && (physical_rcp.shutter_step != rcp->current_scene.shutter_step)))) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.shutter_type = rcp->current_scene.shutter_type;
			physical_rcp.shutter_step = rcp->current_scene.shutter_step;
			send_shutter_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}

		send_cam_request_command (rcp, "QRI", &rcp->current_scene.r_gain);

		if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.r_gain != rcp->current_scene.r_gain)) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.r_gain = rcp->current_scene.r_gain;
			send_r_gain_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}

		send_cam_request_command (rcp, "QBI", &rcp->current_scene.b_gain);

		if ((physical_rcp.connected) && (rcp == rcp_vision) && (physical_rcp.b_gain != rcp->current_scene.b_gain)) {
			g_mutex_lock (&physical_rcp.mutex);
			physical_rcp.b_gain = rcp->current_scene.b_gain;
			send_b_gain_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}

		send_cam_request_command (rcp, "QMS", &rcp->current_scene.shutter_synchro);

		send_cam_request_command (rcp, "QTP", &rcp->current_scene.pedestal);
	}

	send_ptz_control_command (rcp, "#LPC1"); 	//Lens information notification

	g_idle_add ((GSourceFunc)rcp_work_end, rcp);

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
		other_rcp = (rcp_t*)(gslist_itr->data);

		other_rcp->last_time = rcp->last_time;

		other_rcp->auto_focus = rcp->auto_focus;

		other_rcp->mire = rcp->mire;
		other_rcp->day_night = rcp->day_night;

		other_rcp->current_scene = rcp->current_scene;

		g_idle_add ((GSourceFunc)rcp_work_end, other_rcp);
	}
	g_mutex_unlock (&rcp->other_rcp_mutex);

	return NULL;
}

void copy_rcp (rcp_t *rcp_dest, rcp_t *rcp_src)
{
	rcp_dest->camera_is_working = TRUE;

	rcp_dest->camera_is_on = rcp_src->camera_is_on;

	rcp_dest->last_time = rcp_src->last_time;

	if (rcp_dest->error_code != rcp_src->error_code) {
		rcp_dest->error_code = rcp_src->error_code;
		gtk_label_set_text (GTK_LABEL (rcp_dest->error_label), gtk_label_get_text (GTK_LABEL (rcp_src->error_label)));
	}

	rcp_dest->auto_focus = rcp_src->auto_focus;

	rcp_dest->mire = rcp_src->mire;
	rcp_dest->day_night = rcp_src->day_night;

	rcp_dest->current_scene = rcp_src->current_scene;

	rcp_work_end (rcp_dest);
}


#define IHM_UPDATE_TOGGLE_BUTTON(l) \
	g_signal_handler_block (rcp->l##_toggle_button, rcp->l##_handler_id); \
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->l##_toggle_button), rcp->current_scene.l); \
	g_signal_handler_unblock (rcp->l##_toggle_button, rcp->l##_handler_id);

#define IHM_UPDATE_TOGGLE_BUTTON_2(l) \
	g_signal_handler_block (rcp->l##_toggle_button, rcp->l##_handler_id); \
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->l##_toggle_button), rcp->l); \
	g_signal_handler_unblock (rcp->l##_toggle_button, rcp->l##_handler_id);

#define IHM_UPDATE_COMBO_BOX(l) \
	g_signal_handler_block (rcp->l##_combo_box, rcp->l##_handler_id); \
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->l##_combo_box), rcp->current_scene.l); \
	g_signal_handler_unblock (rcp->l##_combo_box, rcp->l##_handler_id);

#define IHM_UPDATE_SCALE(l) \
	g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
	gtk_range_set_value (GTK_RANGE (rcp->l##_scale), rcp->current_scene.l); \
	g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id);

#define IHM_UPDATE_LINEAR_MATRIX_SCALE(l) \
	g_signal_handler_block (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
	gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), rcp->current_scene.linear_matrix.l); \
	g_signal_handler_unblock (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id);


gboolean rcp_work_end (rcp_t *rcp)
{
	int i, j;
	cameras_set_t *cameras_set;

	if (rcp->thread != NULL) {
		g_thread_join (rcp->thread);
		rcp->thread = NULL;
	}

	if (rcp->root_widget == NULL) {
		g_mutex_clear (&rcp->cmd_mutex);
		g_free (rcp);

		return G_SOURCE_REMOVE;
	}

	if (!rcp->active) return G_SOURCE_REMOVE;

	if (!rcp->camera_is_on) {
		set_rcp_off (rcp);

		return G_SOURCE_REMOVE;
	}

	gtk_widget_set_sensitive (rcp->on_standby_switch, TRUE);
	gtk_widget_set_sensitive (rcp->standard_button, TRUE);
	gtk_widget_set_sensitive (rcp->mire_toggle_button, TRUE);
	gtk_widget_set_sensitive (rcp->day_night_toggle_button, TRUE);
	gtk_widget_set_sensitive (rcp->sensitive_widgets, TRUE);

	g_signal_handler_block (rcp->on_standby_switch, rcp->on_standby_handler_id);
	gtk_switch_set_active (GTK_SWITCH (rcp->on_standby_switch), TRUE);
	g_signal_handler_unblock (rcp->on_standby_switch, rcp->on_standby_handler_id);

	IHM_UPDATE_TOGGLE_BUTTON_2(mire)
	IHM_UPDATE_TOGGLE_BUTTON_2(day_night)

	update_ND_filter_combo_box (rcp);
	IHM_UPDATE_COMBO_BOX(gain)

	update_gamma_type_combo_box (rcp);
	IHM_UPDATE_COMBO_BOX(gamma)
	IHM_UPDATE_COMBO_BOX(drs)

	IHM_UPDATE_COMBO_BOX(color_temperature)

	IHM_UPDATE_COMBO_BOX(knee_settings)
	IHM_UPDATE_SCALE(knee_point)
	IHM_UPDATE_SCALE(knee_slope)

	IHM_UPDATE_COMBO_BOX(matrix_type)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(R_G)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(R_B)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(G_R)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(G_B)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(B_R)
	IHM_UPDATE_LINEAR_MATRIX_SCALE(B_G)

	IHM_UPDATE_TOGGLE_BUTTON(detail)
	IHM_UPDATE_SCALE(master_detail)
	IHM_UPDATE_SCALE(v_detail_level)
	IHM_UPDATE_SCALE(detail_band)
	IHM_UPDATE_SCALE(fleshtone_noisesup)

	IHM_UPDATE_SCALE(saturation)

	IHM_UPDATE_SCALE(r_gain)
	IHM_UPDATE_SCALE(b_gain)

	IHM_UPDATE_SCALE(r_pedestal)
	IHM_UPDATE_SCALE(b_pedestal)

	IHM_UPDATE_COMBO_BOX(shutter_type)
	IHM_UPDATE_SCALE(shutter_synchro)

	IHM_UPDATE_SCALE(pedestal)

	IHM_UPDATE_SCALE(iris)
	IHM_UPDATE_TOGGLE_BUTTON(iris_auto)

	if (rcp->day_night) gtk_widget_set_sensitive (rcp->ND_filter_combo_box, FALSE);	//ND filter switching is not possible in Night mode
	else gtk_widget_set_sensitive (rcp->ND_filter_combo_box, TRUE);

	if (rcp->current_scene.drs != 3) {	//When DRS is set to On, the knee setting is disabled
		gtk_widget_set_sensitive (rcp->knee_settings_combo_box, FALSE);
		gtk_widget_set_sensitive (rcp->knee_button, FALSE);
	} else {
		gtk_widget_set_sensitive (rcp->knee_settings_combo_box, TRUE);
		gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	}

	if (rcp->current_scene.knee_settings == 1) gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	else if ((rcp->model == AW_UE150) && (rcp->current_scene.knee_settings == 2)) gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	else gtk_widget_set_sensitive (rcp->knee_button, FALSE);

	if (rcp->current_scene.detail) {
		gtk_button_set_label (GTK_BUTTON (rcp->detail_toggle_button), "On");
		gtk_widget_set_sensitive (rcp->detail_settings_button, TRUE);
	} else {
		gtk_widget_set_sensitive (rcp->detail_settings_button, FALSE);
		gtk_button_set_label (GTK_BUTTON (rcp->detail_toggle_button), "Off");
	}

	if (rcp->current_scene.iris_auto) gtk_widget_set_sensitive (rcp->iris_sensitive_widgets, FALSE);
	else gtk_widget_set_sensitive (rcp->iris_sensitive_widgets, TRUE);

	set_knee_point_label (rcp);
	set_knee_slope_label (rcp);

	set_linear_matrix_R_G_label (rcp);
	set_linear_matrix_R_B_label (rcp);
	set_linear_matrix_G_R_label (rcp);
	set_linear_matrix_G_B_label (rcp);
	set_linear_matrix_B_R_label (rcp);
	set_linear_matrix_B_G_label (rcp);

	set_cc_saturation_R_label (rcp);
	set_cc_saturation_R_R_Yl_label (rcp);
	set_cc_saturation_R_Yl_label (rcp);
	set_cc_saturation_R_Yl_Yl_label (rcp);
	set_cc_saturation_Yl_label (rcp);
	set_cc_saturation_Yl_G_label (rcp);
	set_cc_saturation_G_label (rcp);
	set_cc_saturation_G_Cy_label (rcp);
	set_cc_saturation_Cy_label (rcp);
	set_cc_saturation_Cy_B_label (rcp);
	set_cc_saturation_B_label (rcp);
	set_cc_saturation_B_Mg_label (rcp);
	set_cc_saturation_Mg_label (rcp);
	set_cc_saturation_Mg_R_label (rcp);
	set_cc_saturation_Mg_R_R_label (rcp);

	set_cc_phase_R_label (rcp);
	set_cc_phase_R_R_Yl_label (rcp);
	set_cc_phase_R_Yl_label (rcp);
	set_cc_phase_R_Yl_Yl_label (rcp);
	set_cc_phase_Yl_label (rcp);
	set_cc_phase_Yl_G_label (rcp);
	set_cc_phase_G_label (rcp);
	set_cc_phase_G_Cy_label (rcp);
	set_cc_phase_Cy_label (rcp);
	set_cc_phase_Cy_B_label (rcp);
	set_cc_phase_B_label (rcp);
	set_cc_phase_B_Mg_label (rcp);
	set_cc_phase_Mg_label (rcp);
	set_cc_phase_Mg_R_label (rcp);
	set_cc_phase_Mg_R_R_label (rcp);

	set_master_detail_label (rcp);
	set_v_detail_level_label (rcp);
	set_detail_band_label (rcp);
	set_fleshtone_noisesup_label (rcp);

	set_saturation_label (rcp);

	if (rcp->model == AW_UE150) {
		IHM_UPDATE_COMBO_BOX(film_rec_dynamic_level)
		IHM_UPDATE_SCALE(film_rec_black_stretch_level)
		IHM_UPDATE_COMBO_BOX(video_rec_knee_slope)
		IHM_UPDATE_SCALE(video_rec_knee_point)
		IHM_UPDATE_SCALE(black_gamma)
		IHM_UPDATE_COMBO_BOX(black_gamma_range)

		IHM_UPDATE_SCALE(chroma_phase)

		IHM_UPDATE_SCALE(auto_knee_response)

		IHM_UPDATE_TOGGLE_BUTTON(HLG_knee)
		IHM_UPDATE_SCALE(HLG_knee_point)
		IHM_UPDATE_SCALE(HLG_knee_slope)

		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_1), rcp->current_scene.adaptive_matrix);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);

		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_2), rcp->current_scene.adaptive_matrix);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);

		if (rcp->current_scene.adaptive_matrix) {
			gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_1), "On");
			gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_2), "On");
		} else {
			gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_1), "Off");
			gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_2), "Off");
		}

		IHM_UPDATE_SCALE(detail_coring)
		IHM_UPDATE_SCALE(level_depend)
		IHM_UPDATE_SCALE(knee_aperture_level)
		IHM_UPDATE_SCALE(detail_gain_plus)
		IHM_UPDATE_SCALE(detail_gain_minus)
		IHM_UPDATE_TOGGLE_BUTTON(skin_detail)

		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;
		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);

		IHM_UPDATE_SCALE(g_pedestal)

		update_shutter_step_combo_box_AW_UE150 (rcp);

		IHM_UPDATE_COMBO_BOX(ELC_limit)

		if (rcp->current_scene.HLG_knee) {
			gtk_button_set_label (GTK_BUTTON (rcp->HLG_knee_toggle_button), "On");
			gtk_widget_set_sensitive (rcp->HLG_knee_button, TRUE);
		} else {
			gtk_widget_set_sensitive (rcp->HLG_knee_button, FALSE);
			gtk_button_set_label (GTK_BUTTON (rcp->HLG_knee_toggle_button), "Off");
		}

		set_film_rec_black_stretch_level_label (rcp);

		set_video_rec_knee_point_label (rcp);

		set_black_gamma_label (rcp);

		set_chroma_phase_label (rcp);

		set_auto_knee_response_label (rcp);

		set_HLG_knee_point_label (rcp);
		set_HLG_knee_slope_label (rcp);

		set_detail_coring_label (rcp);
		set_level_depend_label (rcp);
		set_knee_aperture_level_label (rcp);
		set_detail_gain_plus_label (rcp);
		set_detail_gain_minus_label (rcp);
		if (rcp->current_scene.skin_detail) {
			gtk_button_set_label (GTK_BUTTON (rcp->skin_detail_toggle_button), "On");
			gtk_widget_set_sensitive (rcp->fleshtone_noisesup_frame, TRUE);
		} else {
			gtk_widget_set_sensitive (rcp->fleshtone_noisesup_frame, FALSE);
			gtk_button_set_label (GTK_BUTTON (rcp->skin_detail_toggle_button), "Off");
		}

		switch (rcp->current_scene.shutter_type) {
			case 1: gtk_widget_hide (rcp->ELC_limit_combo_box);
				gtk_widget_hide (rcp->ELC_limit_label);
				gtk_widget_hide (rcp->shutter_synchro_button);
				gtk_widget_show (rcp->shutter_step_combo_box);
				break;
			case 2: gtk_widget_hide (rcp->ELC_limit_combo_box);
				gtk_widget_hide (rcp->ELC_limit_label);
				gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_show (rcp->shutter_synchro_button);
				break;
			case 3: gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);
				gtk_widget_show (rcp->ELC_limit_label);
				gtk_widget_show (rcp->ELC_limit_combo_box);
				break;
			default: gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);
				gtk_widget_hide (rcp->ELC_limit_label);
				gtk_widget_hide (rcp->ELC_limit_combo_box);
		}

		set_shutter_synchro_label_AW_UE150 (rcp);

		set_pedestal_label_AW_UE150 (rcp);
	} else {
		IHM_UPDATE_SCALE(noise_suppress)

		set_noise_suppress_label (rcp);

		if (rcp->current_scene.matrix_type != 3) gtk_widget_set_sensitive (rcp->matrix_settings_button, FALSE);
		else gtk_widget_set_sensitive (rcp->matrix_settings_button, TRUE);

		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_HE130;
		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_HE130);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);

		rcp->current_scene.g_pedestal = G_PEDESTAL_DEFAULT;
		g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);

		switch (rcp->current_scene.shutter_type) {
			case 1: gtk_widget_hide (rcp->shutter_synchro_button);
				gtk_widget_show (rcp->shutter_step_combo_box);
				IHM_UPDATE_COMBO_BOX(shutter_step)
				break;
			case 2: gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_show (rcp->shutter_synchro_button);
				break;
			default: gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);
		}

		set_shutter_synchro_label_AW_HE130 (rcp);

		set_pedestal_label_AW_HE130 (rcp);
	}

	cameras_set = (cameras_set_t*)(rcp->cameras_set);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		if (!cameras_set->rcp_ptr_array[i]->active) j++;
		else if (cameras_set->rcp_ptr_array[i]->camera_is_on) j++;
	}

	if (j == cameras_set->number_of_cameras) {
		g_signal_handler_block (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
		gtk_switch_set_active (GTK_SWITCH (cameras_set->master_rcp.on_standby_switch), TRUE);
		g_signal_handler_unblock (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
	}

	gtk_spinner_stop (GTK_SPINNER (rcp->spinner));
	gtk_widget_hide (rcp->spinner);

	rcp->camera_is_working = FALSE;

	return G_SOURCE_REMOVE;
}

gboolean remove_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp)
{
	gtk_widget_unset_state_flags (button, GTK_STATE_FLAG_ACTIVE);

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	return FALSE;
}

gboolean remove_green_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp)
{
	gtk_widget_unset_state_flags (button, GTK_STATE_FLAG_ACTIVE);

	if (rcp->need_last_call && !rcp->green_is_ahead) rcp->green_timeout_return_value = G_SOURCE_REMOVE;
	else {
		if (rcp->timeout_id != 0) {
			g_source_remove (rcp->timeout_id);
			rcp->timeout_id = 0;
		}
	}

	return FALSE;
}

