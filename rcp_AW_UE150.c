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


void init_rcp_AW_UE150 (rcp_t *rcp)
{
	int i;

	rcp->model = AW_UE150;

	if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) rcp->current_scene.gain = GAIN_DEFAULT_AW_UE150;
	else rcp->current_scene.gain = GAIN_DEFAULT_AW_HE130;

	rcp->current_scene.color_temperature = COLOR_TEMPERATURE_DEFAULT_AW_UE150;
	rcp->current_scene.knee_slope = KNEE_SLOPE_DEFAULT_AW_UE150;
	rcp->current_scene.fleshtone_noisesup = FLESHTONE_NOISESUP_DEFAULT_AW_UE150;
	rcp->current_scene.r_gain = R_GAIN_DEFAULT_AW_UE150;
	rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;
	rcp->current_scene.b_gain = B_GAIN_DEFAULT_AW_UE150;
	rcp->current_scene.shutter_step = SHUTTER_STEP_DEFAULT_AW_UE150;
	if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) rcp->current_scene.shutter_synchro = 600;
	else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) rcp->current_scene.shutter_synchro = 500;
	else if (output_fps_AW_UE150 == fps_29_97p) rcp->current_scene.shutter_synchro = 300;
	else if (output_fps_AW_UE150 == fps_25p) rcp->current_scene.shutter_synchro = 250;
	else rcp->current_scene.shutter_synchro = 240;
	rcp->current_scene.pedestal = PEDESTAL_DEFAULT_AW_UE150;

	for (i = 0; i < NB_SCENES; i++) {
		rcp->scenes[i].color_temperature = COLOR_TEMPERATURE_DEFAULT_AW_UE150;
		rcp->scenes[i].knee_slope = KNEE_SLOPE_DEFAULT_AW_UE150;
		rcp->scenes[i].fleshtone_noisesup = FLESHTONE_NOISESUP_DEFAULT_AW_UE150;
		rcp->scenes[i].r_gain = R_GAIN_DEFAULT_AW_UE150;
		rcp->scenes[i].b_gain = B_GAIN_DEFAULT_AW_UE150;
		rcp->scenes[i].shutter_step = SHUTTER_STEP_DEFAULT_AW_UE150;
		if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) rcp->scenes[i].shutter_synchro = 600;
		else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) rcp->scenes[i].shutter_synchro = 500;
		else if (output_fps_AW_UE150 == fps_29_97p) rcp->scenes[i].shutter_synchro = 300;
		else if (output_fps_AW_UE150 == fps_25p) rcp->scenes[i].shutter_synchro = 250;
		else rcp->scenes[i].shutter_synchro = 240;
		rcp->scenes[i].pedestal = PEDESTAL_DEFAULT_AW_UE150;
	}
}

gboolean switch_to_AW_UE150 (rcp_t *rcp)
{
	gtk_widget_destroy (rcp->sensitive_widgets);

	create_rcp_widgets_AW_UE150 (rcp);
	gtk_box_pack_start (GTK_BOX (rcp->main_box), rcp->sensitive_widgets, FALSE, FALSE, 0);

	gtk_widget_show_all (rcp->sensitive_widgets);
	gtk_widget_hide (rcp->scenes_bank_2_box);
	gtk_widget_hide (rcp->HLG_knee_frame);
	gtk_widget_hide (rcp->shutter_step_combo_box);
	gtk_widget_hide (rcp->shutter_synchro_button);
	gtk_widget_hide (rcp->ELC_limit_label);
	gtk_widget_hide (rcp->ELC_limit_combo_box);
	gtk_widget_set_sensitive (rcp->sensitive_widgets, FALSE);

	return G_SOURCE_REMOVE;
}

#define SET_RCP_PARAMETER(l,v) \
	if (rcp->current_scene.l != v) { \
		rcp->current_scene.l = v; \
		set_##l (rcp); \
	}

#define SET_RCP_PARAMETER_AW_UE150(l,v) \
	if (rcp->current_scene.l != v) { \
		rcp->current_scene.l = v; \
		set_##l##_AW_UE150 (rcp); \
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

gpointer load_standard_AW_UE150 (rcp_t *rcp)
{
	if (rcp->mire) {
		rcp->mire = FALSE;
		send_cam_control_command (rcp, "DCB:0");

		if ((rcp == rcp_vision) && physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			if (physical_rcp.mire) {
				physical_rcp.mire = FALSE;
				send_mire_update_notification ();
			}
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}

	if (rcp->day_night) {
		rcp->day_night = FALSE;
		send_ptz_control_command (rcp, "#D60");
	}

	SET_RCP_PARAMETER(ND_filter,ND_FILTER_DEFAULT)

	if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) {
		SET_RCP_PARAMETER_AW_UE150(gain,GAIN_DEFAULT_AW_UE150)
	} else {
		SET_RCP_PARAMETER_AW_UE150(gain,GAIN_DEFAULT_AW_HE130)
	}

	SET_RCP_PARAMETER(gamma_type,GAMMA_TYPE_DEFAULT)
	SET_RCP_PARAMETER(gamma,GAMMA_DEFAULT)
	SET_RCP_PARAMETER(drs,DRS_DEFAULT)

	SET_RCP_PARAMETER_AW_UE150(color_temperature,COLOR_TEMPERATURE_DEFAULT_AW_UE150)

	SET_RCP_PARAMETER(knee_settings,KNEE_SETTINGS_DEFAULT)
	SET_RCP_PARAMETER(knee_point,KNEE_POINT_DEFAULT)
	SET_RCP_PARAMETER(knee_slope,KNEE_SLOPE_DEFAULT_AW_UE150)

	SET_RCP_PARAMETER(auto_knee_response,AUTO_KNEE_RESPONSE_DEFAULT)

	SET_RCP_PARAMETER(HLG_knee,HLG_KNEE_DEFAULT)
	SET_RCP_PARAMETER(HLG_knee_point,HLG_KNEE_POINT_DEFAULT)
	SET_RCP_PARAMETER(HLG_knee_slope,HLG_KNEE_SLOPE_DEFAULT)

	SET_RCP_PARAMETER(matrix_type,MATRIX_TYPE_DEFAULT)
	SET_RCP_PARAMETER(adaptive_matrix,ADAPTIVE_MATRIX_DEFAULT)

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
	SET_RCP_PARAMETER(detail_coring,DETAIL_CORING_DEFAULT)
	SET_RCP_PARAMETER(v_detail_level,V_DETAIL_LEVEL_DEFAULT)
	SET_RCP_PARAMETER(detail_band,DETAIL_BAND_DEFAULT)
	SET_RCP_PARAMETER(noise_suppress,NOISE_SUPPRESS_DEFAULT)
	SET_RCP_PARAMETER(level_depend,LEVEL_DEPEND_DEFAULT)
	SET_RCP_PARAMETER(knee_aperture_level,KNEE_APERTURE_LEVEL_DEFAULT)
	SET_RCP_PARAMETER(detail_gain_plus,DETAIL_GAIN_PLUS_DEFAULT)
	SET_RCP_PARAMETER(detail_gain_minus,DETAIL_GAIN_MINUS_DEFAULT)
	SET_RCP_PARAMETER(skin_detail,SKIN_DETAIL_DEFAULT)
	SET_RCP_PARAMETER(fleshtone_noisesup,FLESHTONE_NOISESUP_DEFAULT_AW_UE150)

	SET_RCP_PARAMETER(saturation,SATURATION_DEFAULT)

	SET_RCP_PARAMETER_AW_UE150(r_gain,R_GAIN_DEFAULT_AW_UE150)
	SET_RCP_PARAMETER_AW_UE150(b_gain,B_GAIN_DEFAULT_AW_UE150)

	SET_RCP_PARAMETER(r_pedestal,R_PEDESTAL_DEFAULT)
	SET_RCP_PARAMETER_AW_UE150(g_pedestal,G_PEDESTAL_DEFAULT)
	SET_RCP_PARAMETER(b_pedestal,B_PEDESTAL_DEFAULT)

	SET_RCP_PARAMETER_AW_UE150(shutter_type,SHUTTER_TYPE_DEFAULT)
	SET_RCP_PARAMETER_AW_UE150(shutter_step,SHUTTER_STEP_DEFAULT_AW_UE150)
	if ((output_fps_AW_UE150 == fps_59_94p) || (output_fps_AW_UE150 == fps_59_94i)) {
		SET_RCP_PARAMETER_AW_UE150(shutter_synchro,600)
	} else if ((output_fps_AW_UE150 == fps_50p) || (output_fps_AW_UE150 == fps_50i)) {
		SET_RCP_PARAMETER_AW_UE150(shutter_synchro,500)
	} else if (output_fps_AW_UE150 == fps_29_97p) {
		SET_RCP_PARAMETER_AW_UE150(shutter_synchro,300)
	} else if (output_fps_AW_UE150 == fps_25p) {
		SET_RCP_PARAMETER_AW_UE150(shutter_synchro,250)
	} else {
		SET_RCP_PARAMETER_AW_UE150(shutter_synchro,240)
	}
	SET_RCP_PARAMETER(ELC_limit,ELC_LIMIT_DEFAULT)

	SET_RCP_PARAMETER_AW_UE150(pedestal,PEDESTAL_DEFAULT_AW_UE150)

	SET_RCP_PARAMETER(iris_auto,IRIS_AUTO_DEFAULT)
	SET_RCP_PARAMETER(iris,IRIS_DEFAULT)

	g_idle_add ((GSourceFunc)rcp_work_end, rcp);

	return NULL;
}

#define COPY_RCP_SCENE_PARAMETER(l) \
	if (rcp->current_scene.l != rcp->scenes[rcp->scene_to_load].l) { \
		rcp->current_scene.l = rcp->scenes[rcp->scene_to_load].l; \
		set_##l (rcp); \
	}

#define COPY_RCP_SCENE_PARAMETER_AW_UE150(l) \
	if (rcp->current_scene.l != rcp->scenes[rcp->scene_to_load].l) { \
		rcp->current_scene.l = rcp->scenes[rcp->scene_to_load].l; \
		set_##l##_AW_UE150 (rcp); \
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

gpointer load_scene_AW_UE150 (rcp_t *rcp)
{
	if (rcp->mire) {
		rcp->mire = FALSE;
		send_cam_control_command (rcp, "DCB:0");

		if ((rcp == rcp_vision) && physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			if (physical_rcp.mire) {
				physical_rcp.mire = FALSE;
				send_mire_update_notification ();
			}
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}

	if (rcp->day_night) {
		rcp->day_night = FALSE;
		send_ptz_control_command (rcp, "#D60");
	}

	COPY_RCP_SCENE_PARAMETER(ND_filter)

	COPY_RCP_SCENE_PARAMETER_AW_UE150(gain)

	COPY_RCP_SCENE_PARAMETER(gamma_type)
	COPY_RCP_SCENE_PARAMETER(gamma)
	COPY_RCP_SCENE_PARAMETER(drs)

	COPY_RCP_SCENE_PARAMETER_AW_UE150(color_temperature)

	COPY_RCP_SCENE_PARAMETER(knee_settings)
	COPY_RCP_SCENE_PARAMETER(knee_point)
	COPY_RCP_SCENE_PARAMETER(knee_slope)

	COPY_RCP_SCENE_PARAMETER(auto_knee_response)

	COPY_RCP_SCENE_PARAMETER(HLG_knee)
	COPY_RCP_SCENE_PARAMETER(HLG_knee_point)
	COPY_RCP_SCENE_PARAMETER(HLG_knee_slope)

	COPY_RCP_SCENE_PARAMETER(matrix_type)
	COPY_RCP_SCENE_PARAMETER(adaptive_matrix)

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
	COPY_RCP_SCENE_PARAMETER(detail_coring)
	COPY_RCP_SCENE_PARAMETER(v_detail_level)
	COPY_RCP_SCENE_PARAMETER(detail_band)
	COPY_RCP_SCENE_PARAMETER(noise_suppress)
	COPY_RCP_SCENE_PARAMETER(level_depend)
	COPY_RCP_SCENE_PARAMETER(knee_aperture_level)
	COPY_RCP_SCENE_PARAMETER(detail_gain_plus)
	COPY_RCP_SCENE_PARAMETER(detail_gain_minus)
	COPY_RCP_SCENE_PARAMETER(skin_detail)
	COPY_RCP_SCENE_PARAMETER(fleshtone_noisesup)

	COPY_RCP_SCENE_PARAMETER(saturation)

	COPY_RCP_SCENE_PARAMETER_AW_UE150(r_gain)
	COPY_RCP_SCENE_PARAMETER_AW_UE150(b_gain)

	COPY_RCP_SCENE_PARAMETER(r_pedestal)
	COPY_RCP_SCENE_PARAMETER_AW_UE150(g_pedestal)
	COPY_RCP_SCENE_PARAMETER(b_pedestal)

	COPY_RCP_SCENE_PARAMETER_AW_UE150(shutter_type)
	COPY_RCP_SCENE_PARAMETER_AW_UE150(shutter_step)
	COPY_RCP_SCENE_PARAMETER_AW_UE150(shutter_synchro)
	COPY_RCP_SCENE_PARAMETER(ELC_limit)

	COPY_RCP_SCENE_PARAMETER_AW_UE150(pedestal)

	COPY_RCP_SCENE_PARAMETER(iris_auto)
	COPY_RCP_SCENE_PARAMETER(iris)

	g_idle_add ((GSourceFunc)rcp_work_end, rcp);

	return NULL;
}

void create_rcp_widgets_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *rcp_box, *frame, *box1, *box2, *widget;

	rcp_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
//Scenes
	frame = create_scenes_frame (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
//ABB
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("ABB");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE * 2);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (ABB_button_clicked), rcp);
			gtk_box_set_center_widget (GTK_BOX (box2), widget);
			rcp->ABB_button = widget;
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
//ND_filter
		frame = create_ND_filter_frame_AW_UE150 (rcp);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
//Gain
		frame = create_gain_frame_AW_UE150 (rcp);
		gtk_box_pack_end (GTK_BOX (box1), frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);
	rcp->ABB_filter_gain_box = box1;

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
//Gamma
		frame = create_gamma_frame_AW_UE150 (rcp);
		gtk_box_pack_start (GTK_BOX (box1), frame, FALSE, FALSE, 0);
		rcp->gamma_frame = frame;
//Chroma phase
		rcp->chroma_phase_frame = create_chroma_phase_frame (rcp);
		gtk_box_pack_end (GTK_BOX (box1), rcp->chroma_phase_frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);
//Color temperature
	frame = create_color_temperature_frame_AW_UE150 (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);
	rcp->color_temperature_frame = frame;

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
//Knee
		rcp->knee_frame = create_knee_frame (rcp);
		gtk_box_pack_start (GTK_BOX (box1), rcp->knee_frame, TRUE, TRUE, 0);

		rcp->HLG_knee_frame = create_HLG_knee_frame (rcp);
		gtk_box_pack_start (GTK_BOX (box1), rcp->HLG_knee_frame, TRUE, TRUE, 0);
//Matrix
		frame = create_matrix_frame (rcp);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
//Detail
		frame = create_detail_frame (rcp);
		gtk_box_pack_end (GTK_BOX (box1), frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);
	rcp->knee_matrix_detail_box = box1;
//Saturation
	frame = create_saturation_frame_AW_UE150 (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);
//White
	frame = create_white_frame_AW_UE150 (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);
//Black
	frame = create_black_frame_AW_UE150 (rcp);
	gtk_box_pack_start (GTK_BOX (rcp_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
//Shutter
			frame = create_shutter_frame_AW_UE150 (rcp);
			gtk_box_pack_start (GTK_BOX (box2), frame, FALSE, FALSE, 0);
			rcp->shutter_frame = frame;
//Pedestal
			frame = create_pedestal_frame_AW_UE150 (rcp);
			gtk_box_pack_end (GTK_BOX (box2), frame, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
//Iris
		frame = create_iris_frame (rcp);
		gtk_box_pack_end (GTK_BOX (box1), frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (rcp_box), box1, FALSE, FALSE, 0);

	rcp->sensitive_widgets = rcp_box;
}

