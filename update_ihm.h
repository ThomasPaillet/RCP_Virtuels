/*
 * copyright (c) 2022 Thomas Paillet <thomas.paillet@net-c.fr>

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

#ifndef __UPDATE_IHM_H
#define __UPDATE_IHM_H


#define NO_POST_ACTION 0

#define NIGHT_POST_ACTION 1

#define FILM_REC_BLACK_STRETCH_LEVEL_POST_ACTION 70
#define VIDEO_REC_KNEE_POINT_POST_ACTION 71
#define BLACK_GAMMA_POST_ACTION 72
#define DRS_POST_ACTION 73

#define CHROMA_PHASE_POST_ACTION 74

#define KNEE_SETTINGS_POST_ACTION 3
#define KNEE_POINT_POST_ACTION 4
#define KNEE_SLOPE_POST_ACTION 5

#define AUTO_KNEE_RESPONSE_POST_ACTION 6

#define HLG_KNEE_POST_ACTION 7
#define HLG_KNEE_POINT_POST_ACTION 8
#define HLG_KNEE_SLOPE_POST_ACTION 9

#define MATRIX_TYPE_POST_ACTION 10
#define ADAPTIVE_MATRIX_POST_ACTION 11
#define LINEAR_MATRIX_R_G_POST_ACTION 12
#define LINEAR_MATRIX_R_B_POST_ACTION 13
#define LINEAR_MATRIX_G_R_POST_ACTION 14
#define LINEAR_MATRIX_G_B_POST_ACTION 15
#define LINEAR_MATRIX_B_R_POST_ACTION 16
#define LINEAR_MATRIX_B_G_POST_ACTION 17

#define DETAIL_POST_ACTION 20
#define MASTER_DETAIL_POST_ACTION 21
#define DETAIL_CORING_POST_ACTION 22
#define V_DETAIL_LEVEL_POST_ACTION 23
#define DETAIL_BAND_POST_ACTION 24
#define NOISE_SUPPRESS_POST_ACTION 25
#define LEVEL_DEPEND_POST_ACTION 26
#define KNEE_APERTURE_LEVEL_POST_ACTION 27
#define DETAIL_GAIN_PLUS_POST_ACTION 28
#define DETAIL_GAIN_MINUS_POST_ACTION 29
#define SKIN_DETAIL_POST_ACTION 30
#define FLESHTONE_NOISESUP_POST_ACTION 31

#define SATURATION_POST_ACTION 49

#define SHUTTER_TYPE_POST_ACTION_AW_HE130 50
#define SHUTTER_TYPE_POST_ACTION_AW_UE150 51
#define SHUTTER_STEP_POST_ACTION_AW_UE150 52
#define SHUTTER_SYNCHRO_POST_ACTION_AW_HE130 53
#define SHUTTER_SYNCHRO_POST_ACTION_AW_UE150 54

#define PEDESTAL_POST_ACTION_AW_HE130 60
#define PEDESTAL_POST_ACTION_AW_UE150 61
#define IRIS_AUTO_POST_ACTION 62


typedef struct {
	GtkWidget *widget;
	int handler_id;
	int *value;
	int post_action;
	rcp_t *rcp;
} int_widget_t;


gboolean update_toggle_button (int_widget_t *int_widget);

gboolean update_combo_box (int_widget_t *int_widget);

gboolean update_scale (int_widget_t *int_widget);

gboolean update_saturation_color_correction_frame (int_widget_t *int_widget);

gboolean update_phase_color_correction_frame (int_widget_t *int_widget);


#endif

