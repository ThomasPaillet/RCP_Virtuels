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

