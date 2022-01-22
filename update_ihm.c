/*
 * copyright (c) 2021 Thomas Paillet <thomas.paillet@net-c.fr>

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
		case HLG_KNEE_POST_ACTION:
			if (int_widget->rcp->current_scene.HLG_knee) {
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->HLG_knee_toggle_button), "On");
				gtk_widget_set_sensitive (int_widget->rcp->HLG_knee_button, TRUE);
			} else {
				gtk_widget_set_sensitive (int_widget->rcp->HLG_knee_button, FALSE);
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->HLG_knee_toggle_button), "Off");
			}
			break;
		case ADAPTIVE_MATRIX_POST_ACTION:
			g_signal_handler_block (int_widget->rcp->adaptive_matrix_toggle_button_2, int_widget->rcp->adaptive_matrix_handler_id_2);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (int_widget->rcp->adaptive_matrix_toggle_button_2), int_widget->rcp->current_scene.adaptive_matrix);
			g_signal_handler_unblock (int_widget->rcp->adaptive_matrix_toggle_button_2, int_widget->rcp->adaptive_matrix_handler_id_2);

			if (int_widget->rcp->current_scene.adaptive_matrix) {
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->adaptive_matrix_toggle_button_1), "On");
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->adaptive_matrix_toggle_button_2), "On");
			} else {
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->adaptive_matrix_toggle_button_1), "Off");
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->adaptive_matrix_toggle_button_2), "Off");
			}
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
		case SKIN_DETAIL_POST_ACTION:
			if (int_widget->rcp->current_scene.skin_detail) {
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->skin_detail_toggle_button), "On");
				gtk_widget_set_sensitive (int_widget->rcp->fleshtone_noisesup_frame, TRUE);
			} else {
				gtk_widget_set_sensitive (int_widget->rcp->fleshtone_noisesup_frame, FALSE);
				gtk_button_set_label (GTK_BUTTON (int_widget->rcp->skin_detail_toggle_button), "Off");
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
			if (int_widget->rcp->current_scene.knee_settings == 1) gtk_widget_set_sensitive (int_widget->rcp->knee_button, TRUE);
			else if ((int_widget->rcp->model == AW_UE150) && (int_widget->rcp->current_scene.knee_settings == 2))
				gtk_widget_set_sensitive (int_widget->rcp->knee_button, TRUE);
			else gtk_widget_set_sensitive (int_widget->rcp->knee_button, FALSE);
			break;
		case MATRIX_TYPE_POST_ACTION:
			if (int_widget->rcp->current_scene.matrix_type != 3) gtk_widget_set_sensitive (int_widget->rcp->matrix_settings_button, FALSE);
			else gtk_widget_set_sensitive (int_widget->rcp->matrix_settings_button, TRUE);
			break;
		case SHUTTER_TYPE_POST_ACTION_AW_HE130:
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
			break;
		case SHUTTER_TYPE_POST_ACTION_AW_UE150:
			switch (int_widget->rcp->current_scene.shutter_type) {
				case 1: gtk_widget_hide (int_widget->rcp->ELC_limit_combo_box);
					gtk_widget_hide (int_widget->rcp->ELC_limit_label);
					gtk_widget_hide (int_widget->rcp->shutter_synchro_button);
					gtk_widget_show (int_widget->rcp->shutter_step_combo_box);
					break;
				case 2: gtk_widget_hide (int_widget->rcp->ELC_limit_combo_box);
					gtk_widget_hide (int_widget->rcp->ELC_limit_label);
					gtk_widget_hide (int_widget->rcp->shutter_step_combo_box);
					gtk_widget_show (int_widget->rcp->shutter_synchro_button);
					break;
				case 3: gtk_widget_hide (int_widget->rcp->shutter_step_combo_box);
					gtk_widget_hide (int_widget->rcp->shutter_synchro_button);
					gtk_widget_show (int_widget->rcp->ELC_limit_label);
					gtk_widget_show (int_widget->rcp->ELC_limit_combo_box);
					break;
				default: gtk_widget_hide (int_widget->rcp->shutter_step_combo_box);
					gtk_widget_hide (int_widget->rcp->shutter_synchro_button);
					gtk_widget_hide (int_widget->rcp->ELC_limit_label);
					gtk_widget_hide (int_widget->rcp->ELC_limit_combo_box);
			}
			break;
		case SHUTTER_STEP_POST_ACTION_AW_UE150:
			g_signal_handler_block (int_widget->rcp->shutter_step_combo_box, int_widget->rcp->shutter_step_handler_id);
			if ((output_fps_AW_UE150 == fps_23_98p) || (output_fps_AW_UE150 == fps_24p)) {
				if (int_widget->rcp->current_scene.shutter_step == 24) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 0);
				else if (int_widget->rcp->current_scene.shutter_step == 48) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 1);
				else if (int_widget->rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 2);
				else if (int_widget->rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 3);
				else if (int_widget->rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 4);
				else if (int_widget->rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 5);
				else if (int_widget->rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 6);
				else if (int_widget->rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 7);
				else if (int_widget->rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 8);
				else if (int_widget->rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 9);
				else if (int_widget->rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 10);
				else gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 11);
			} else if (output_fps_AW_UE150 == fps_25p) {
				if (int_widget->rcp->current_scene.shutter_step == 25) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 0);
				else if (int_widget->rcp->current_scene.shutter_step == 50) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 1);
				else if (int_widget->rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 2);
				else if (int_widget->rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 3);
				else if (int_widget->rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 4);
				else if (int_widget->rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 5);
				else if (int_widget->rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 6);
				else if (int_widget->rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 7);
				else if (int_widget->rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 8);
				else if (int_widget->rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 9);
				else if (int_widget->rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 10);
				else gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 11);
			} else if (output_fps_AW_UE150 == fps_29_97p) {
				if (int_widget->rcp->current_scene.shutter_step == 30) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 0);
				else if (int_widget->rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 1);
				else if (int_widget->rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 2);
				else if (int_widget->rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 3);
				else if (int_widget->rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 4);
				else if (int_widget->rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 5);
				else if (int_widget->rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 6);
				else if (int_widget->rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 7);
				else if (int_widget->rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 8);
				else if (int_widget->rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 9);
				else gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 10);
			} else {
				if (int_widget->rcp->current_scene.shutter_step == 60) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 0);
				else if (int_widget->rcp->current_scene.shutter_step == 100) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 1);
				else if (int_widget->rcp->current_scene.shutter_step == 120) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 2);
				else if (int_widget->rcp->current_scene.shutter_step == 250) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 3);
				else if (int_widget->rcp->current_scene.shutter_step == 500) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 4);
				else if (int_widget->rcp->current_scene.shutter_step == 1000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 5);
				else if (int_widget->rcp->current_scene.shutter_step == 2000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 6);
				else if (int_widget->rcp->current_scene.shutter_step == 4000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 7);
				else if (int_widget->rcp->current_scene.shutter_step == 8000) gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 8);
				else gtk_combo_box_set_active (GTK_COMBO_BOX (int_widget->rcp->shutter_step_combo_box), 9);
			}
			g_signal_handler_unblock (int_widget->rcp->shutter_step_combo_box, int_widget->rcp->shutter_step_handler_id);
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
		case FILM_REC_BLACK_STRETCH_LEVEL_POST_ACTION:
			set_film_rec_black_stretch_level_label (int_widget->rcp);
			break;
		case VIDEO_REC_KNEE_POINT_POST_ACTION:
			set_video_rec_knee_point_label (int_widget->rcp);
			break;
		case BLACK_GAMMA_POST_ACTION:
			set_black_gamma_label (int_widget->rcp);
			break;
		case CHROMA_PHASE_POST_ACTION:
			set_chroma_phase_label (int_widget->rcp);
			break;
		case KNEE_POINT_POST_ACTION:
			set_knee_point_label (int_widget->rcp);
			break;
		case KNEE_SLOPE_POST_ACTION:
			set_knee_slope_label (int_widget->rcp);
			break;
		case AUTO_KNEE_RESPONSE_POST_ACTION:
			set_auto_knee_response_label (int_widget->rcp);
			break;
		case HLG_KNEE_POINT_POST_ACTION:
			set_HLG_knee_point_label (int_widget->rcp);
			break;
		case HLG_KNEE_SLOPE_POST_ACTION:
			set_HLG_knee_slope_label (int_widget->rcp);
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
		case DETAIL_CORING_POST_ACTION:
			set_detail_coring_label (int_widget->rcp);
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
		case LEVEL_DEPEND_POST_ACTION:
			set_level_depend_label (int_widget->rcp);
			break;
		case KNEE_APERTURE_LEVEL_POST_ACTION:
			set_knee_aperture_level_label (int_widget->rcp);
			break;
		case DETAIL_GAIN_PLUS_POST_ACTION:
			set_detail_gain_plus_label (int_widget->rcp);
			break;
		case DETAIL_GAIN_MINUS_POST_ACTION:
			set_detail_gain_minus_label (int_widget->rcp);
			break;
		case FLESHTONE_NOISESUP_POST_ACTION:
			set_fleshtone_noisesup_label (int_widget->rcp);
			break;
		case SATURATION_POST_ACTION:
			set_saturation_label (int_widget->rcp);
			break;
		case SHUTTER_SYNCHRO_POST_ACTION_AW_HE130:
			set_shutter_synchro_label_AW_HE130 (int_widget->rcp);
			break;
		case SHUTTER_SYNCHRO_POST_ACTION_AW_UE150:
			set_shutter_synchro_label_AW_UE150 (int_widget->rcp);
			break;
		case PEDESTAL_POST_ACTION_AW_HE130:
			set_pedestal_label_AW_HE130 (int_widget->rcp);
			break;
		case PEDESTAL_POST_ACTION_AW_UE150:
			set_pedestal_label_AW_UE150 (int_widget->rcp);
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

