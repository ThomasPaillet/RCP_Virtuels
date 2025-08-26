/*
 * copyright (c) 2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#ifndef __SETTINGS_H
#define __SETTINGS_H


#include "rcp.h"


#define NB_SETTINGS 10

#define WHITE_BALANCE_MODE_INDEX 0
#define PRESET_SCOPE_INDEX 1
#define WHITE_CLIP_INDEX 2
#define WHITE_CLIP_LEVEL_INDEX 3
#define AGC_MAX_GAIN_INDEX 4
#define FRAME_MIX_INDEX 5
#define DNR_INDEX 6
#define OSD_OFF_WITH_TALLY_INDEX 7
#define OSD_STATUS_INDEX 8
#define TALLY_BRIGHTNESS_INDEX 9

#define NB_SETTINGS_AW_HE130 4

#define FREQUENCY_INDEX_AW_HE130 0
#define FORMAT_INDEX_AW_HE130 1
#define DOWN_CONVERSION_MODE_INDEX_AW_HE130 2
#define COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130 3

#define NB_SETTINGS_AW_UE150 23

#define FREQUENCY_INDEX_AW_UE150 0
#define FORMAT_INDEX_AW_UE150 1
#define PEDESTAL_OFFSET_INDEX_AW_UE150 2
#define AUTO_IRIS_SPEED_INDEX_AW_UE150 3
#define AUTO_IRIS_WINDOW_INDEX_AW_UE150 4
#define SUPER_GAIN_INDEX_AW_UE150 5
#define SHOOTING_MODE_INDEX_AW_UE150 6
#define COLOR_SETTING_INDEX_AW_UE150 7
#define SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150 8
#define SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150 9
#define SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150 10
#define SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150 11
#define SDI_3G_FORMAT_INDEX_AW_UE150 12
#define SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150 13
#define SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150 14
#define SDI_3G_3G_SDI_OUT_INDEX_AW_UE150 15
#define MONI_FORMAT_INDEX_AW_UE150 16
#define MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150 17 
#define MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150 18
#define HDMI_FORMAT_INDEX_AW_UE150 19
#define HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150 20
#define HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150 21
#define HDMI_VIDEO_SAMPLING_INDEX_AW_UE150 22


typedef struct {
	char *value;
	char *label;
} parameter_t;

typedef struct {
	char *name;
	char *control_cmd;
	char *query_cmd;
	int nb_parameters;
	parameter_t *parameters;
	char **answers;
	GtkWidget *combo_box;
	char *tooltip;
	int default_value;
	gboolean sensitive;
} setting_t;


extern gboolean backup_needed;

extern setting_t settings_array[NB_SETTINGS];
extern setting_t settings_array_AW_HE130[NB_SETTINGS_AW_HE130];
extern setting_t settings_array_AW_UE150[NB_SETTINGS_AW_UE150];
extern int settings_parameters_indexes_array[NB_SETTINGS];
extern int settings_parameters_indexes_array_AW_HE130[NB_SETTINGS_AW_HE130];
extern int settings_parameters_indexes_array_AW_UE150[NB_SETTINGS_AW_UE150];

extern int status_lamp;
extern int tally_input;
extern gboolean send_ip_tally;
extern int picture_level;

extern int osd_mix_AW_HE130;
extern int osd_mix_AW_UE150;

extern gboolean theme;
extern gboolean show_master_rcp;

extern fps_t output_fps_AW_HE130;
extern fps_t output_fps_AW_UE150;

extern GList *rcp_glist;
extern GList *rcp_start_glist;
extern GList *ghost_rcp_glist;

extern GtkWidget *settings_page;
extern GtkWidget *settings_list_box;
extern GtkWidget *settings_configuration_button;
extern GtkWidget *settings_new_button;
extern GtkWidget *settings_delete_button;

extern GtkWidget *ip_waiting_label;
extern GtkWidget *ip_connected_label_1;
extern GtkWidget *ip_connected_label_2;
extern GtkWidget *rs_ok_label;

#ifdef MAIN_SETTINGS_READ_ONLY
extern parameter_t format_parameters_array_ntsc_AW_HE130[8];
extern char *format_answers_array_ntsc_AW_HE130[8];

extern parameter_t format_parameters_array_pal_AW_HE130[6];
extern char *format_answers_array_pal_AW_HE130[6];

extern parameter_t format_parameters_array_ntsc_AW_UE150[8];
extern char *format_answers_array_ntsc_AW_UE150[8];

extern parameter_t format_parameters_array_pal_AW_UE150[7];
extern char *format_answers_array_pal_AW_UE150[7];

extern parameter_t format_parameters_array_24p_AW_UE150[2];
extern char *format_answers_array_24p_AW_UE150[2];

extern parameter_t format_parameters_array_23_98p_AW_UE150[3];
extern char *format_answers_array_23_98p_AW_UE150[3];

extern gboolean camera_settings_checked;
extern gboolean camera_settings_AW_HE130_checked;
extern gboolean camera_settings_AW_UE150_checked;


gboolean update_settings_array_combo_box_text (gpointer index);
gboolean update_settings_array_AW_HE130_combo_box_text (gpointer index);
gboolean update_settings_array_AW_UE150_combo_box_text (gpointer index);

gboolean update_status_lamp_combo_box_text (void);
gboolean update_tally_input_combo_box_text (void);
gboolean update_picture_level_scale (void);

gboolean update_osd_mix_AW_HE130 (void);
gboolean update_osd_mix_AW_UE150 (void);
#endif

void update_outputs_parameters_AW_UE150 (void);

void check_camera_settings_AW_HE130 (rcp_t *rcp);

void check_camera_settings_AW_UE150 (rcp_t *rcp);

void check_camera_settings (rcp_t *rcp);

void create_settings_page (void);

void load_settings_from_config_file (void);

void load_cameras_sets_from_config_file (void);

void save_settings_and_cameras_sets_to_config_file (void);


#endif

