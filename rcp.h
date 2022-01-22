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

#ifndef __RCP_H
#define __RCP_H


//#define MAIN_SETTINGS_READ_ONLY


#include <gtk/gtk.h>
#include <sys/time.h>


#ifdef _WIN32
	#include <winsock2.h>

	#define SHUT_RD SD_RECEIVE

	typedef int socklen_t;

	void WSAInit (void);

	void timersub (const struct timeval* tvp, const struct timeval* uvp, struct timeval* vvp);
#elif defined (__linux)
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define SOCKET int
	#define INVALID_SOCKET -1
	#define closesocket close
	#define WSAInit()
	#define WSACleanup()
#endif


typedef enum { fps_59_94p, fps_59_94i, fps_50p, fps_50i, fps_29_97p, fps_25p, fps_24p, fps_23_98p } fps_t;


#define AW_HE130 0
#define AW_UE150 1

#define ND_FILTER_DEFAULT 0							//Through

#define GAIN_DEFAULT_AW_HE130 36					//O dB
#define GAIN_DEFAULT_AW_UE150 42					//O dB

#define GAMMA_TYPE_DEFAULT 0						//HD
#define GAMMA_DEFAULT 30							//0.45
#define FILM_REC_DYNAMIC_LEVEL_DEFAULT 1			//500%
#define FILM_REC_BLACK_STRETCH_LEVEL_DEFAULT 0		//0%
#define VIDEO_REC_KNEE_SLOPE_DEFAULT 7				//150%
#define VIDEO_REC_KNEE_POINT_DEFAULT 0x62			//30%
#define BLACK_GAMMA_DEFAULT 0x80					//0
#define BLACK_GAMMA_RANGE_DEFAULT 2					//1 (around 20%)
#define DRS_DEFAULT 3								//Off

#define CHROMA_PHASE_DEFAULT 0x80					//0

#define COLOR_TEMPERATURE_DEFAULT_AW_HE130 28		//5600K
#define COLOR_TEMPERATURE_DEFAULT_AW_UE150 79		//5600K

#define KNEE_SETTINGS_DEFAULT 0						//Off
#define AUTO_KNEE_RESPONSE_DEFAULT 4				//4
#define KNEE_POINT_DEFAULT 0x7E						//93.0%
#define KNEE_SLOPE_DEFAULT_AW_HE130 0x55			//85
#define KNEE_SLOPE_DEFAULT_AW_UE150 0x63			//99

#define HLG_KNEE_DEFAULT FALSE						//Off
#define HLG_KNEE_POINT_DEFAULT 0x1C					//55.00%
#define HLG_KNEE_SLOPE_DEFAULT 0x0A					//10

#define MATRIX_TYPE_DEFAULT 0						//Normal
#define ADAPTIVE_MATRIX_DEFAULT FALSE				//Off
#define LINEAR_MATRIX_DEFAULT 0x80					//0
#define COLOR_CORRECTION_DEFAULT 0x00				//0

#define DETAIL_DEFAULT TRUE							//On
#define MASTER_DETAIL_DEFAULT 0x80					//0 (AW_UE150) 31 (AW_HE130)
#define DETAIL_CORING_DEFAULT 0x0F					//15
#define V_DETAIL_LEVEL_DEFAULT 0x80					//0
#define DETAIL_BAND_DEFAULT 0x80					//0
#define NOISE_SUPPRESS_DEFAULT 0x01					//1
#define LEVEL_DEPEND_DEFAULT 0x80					//0
#define KNEE_APERTURE_LEVEL_DEFAULT 0x02 			//2
#define DETAIL_GAIN_PLUS_DEFAULT 0x80				//0
#define DETAIL_GAIN_MINUS_DEFAULT 0x80				//0
#define SKIN_DETAIL_DEFAULT FALSE					//Off
#define FLESHTONE_NOISESUP_DEFAULT_AW_HE130 0x80	//0
#define FLESHTONE_NOISESUP_DEFAULT_AW_UE150 0x90	//16

#define SATURATION_DEFAULT 0x80						//0%

#define R_GAIN_DEFAULT_AW_HE130 0x096				//O
#define G_GAIN_DEFAULT_AW_HE130 0x096				//O
#define B_GAIN_DEFAULT_AW_HE130 0x096				//O

#define R_GAIN_DEFAULT_AW_UE150 0x800				//O
#define G_GAIN_DEFAULT_AW_UE150 0x800				//O
#define B_GAIN_DEFAULT_AW_UE150 0x800				//O

#define R_PEDESTAL_DEFAULT 0x096					//O
#define G_PEDESTAL_DEFAULT 0x096					//O
#define B_PEDESTAL_DEFAULT 0x096					//O

#define SHUTTER_TYPE_DEFAULT 0						//Off
#define SHUTTER_STEP_DEFAULT_AW_UE150 120			//1/120
#define SHUTTER_SYNCHRO_DEFAULT_AW_HE130 0x001
#define ELC_LIMIT_DEFAULT 2							//1/250

#define PEDESTAL_DEFAULT_AW_HE130 0x096				//O
#define PEDESTAL_DEFAULT_AW_UE150 0x800				//O

#define IRIS_DEFAULT 0xAAA
#define IRIS_AUTO_DEFAULT FALSE						//Off


typedef struct {
	int R_G;
	int R_B;
	int G_R;
	int G_B;
	int B_R;
	int B_G;
} linear_matrix_t;

typedef enum { R, R_R_Yl, R_Yl, R_Yl_Yl, Yl, Yl_G, G, G_Cy, Cy, Cy_B, B, B_Mg, Mg, Mg_R, Mg_R_R, nothing } color_t;

typedef struct {
	int ND_filter;
	int gain;

	int gamma_type;
	int gamma;
	int film_rec_dynamic_level;
	int film_rec_black_stretch_level;
	int video_rec_knee_slope;
	int video_rec_knee_point;
	int black_gamma;
	int black_gamma_range;
	int drs;

	int chroma_phase;

	int color_temperature;

	int knee_settings;
	int knee_point;
	int knee_slope;

	int auto_knee_response;

	gboolean HLG_knee;
	int HLG_knee_point;
	int HLG_knee_slope;

	int matrix_type;
	gboolean adaptive_matrix;
	linear_matrix_t linear_matrix;
	int cc_saturation[15];
	int cc_phase[15];

	gboolean detail;
	int master_detail;
	int detail_coring;
	int v_detail_level;
	int detail_band;
	int noise_suppress;
	int level_depend;
	int knee_aperture_level;
	int detail_gain_plus;
	int detail_gain_minus;
	gboolean skin_detail;
	int fleshtone_noisesup;

	int saturation;

	int r_gain;
	int g_gain;
	int b_gain;

	int r_pedestal;
	int g_pedestal;
	int b_pedestal;

	int shutter_type;
	int shutter_step;
	int shutter_synchro;
	int ELC_limit;

	int pedestal;

	int iris;
	gboolean iris_auto;
} scene_t;


#define NB_SCENES 10


typedef struct {
	char name[3];
	gboolean active;
	char ip_address[16];
	char new_ip_address[16];
	gboolean ip_address_is_valid;

	int index;

	char cmd_buffer[272];
	char *last_cmd;
	gboolean cam_ptz;

	char last_ctrl_cmd[16];
	int last_ctrl_cmd_len;
	GMutex cmd_mutex;

	struct sockaddr_in address;

	GMutex other_rcp_mutex;
	GSList *other_rcp;	//with_the_same_ip_address;

	gboolean camera_is_on;
	gboolean camera_is_working;

	int model;

//	time_t last_version_information_notification_time;

	GThread *thread;
	struct timeval last_time;
	guint timeout_id;
	gboolean r_b;
	gboolean need_last_call;
	int timeout_value;
	gboolean green_is_ahead;
	gboolean green_timeout_return_value;

	int jpeg_page;

	gboolean OSD_menu;

	gboolean auto_focus;

	scene_t current_scene;
	scene_t scenes[NB_SCENES];
	int scene_to_load;

	GtkWidget *name_drawing_area;

	GtkWidget *error_label;
	int error_code;

	GtkWidget *on_standby_switch;
	GtkWidget *standard_button;
	GtkWidget *mire_toggle_button;
	GtkWidget *day_night_toggle_button;
	int on_standby_handler_id;
	int mire_handler_id;
	int day_night_handler_id;
	gboolean mire;
	gboolean day_night;

	GtkWidget *spinner;

	GtkWidget *scenes_bank_1_box;
	GtkWidget *scenes_bank_2_box;
	GtkWidget *scenes_button[NB_SCENES];
	GtkWidget *store_toggle_button;

	GtkWidget *ABB_filter_gain_box;
	GtkWidget *ABB_button;
	GtkWidget *ND_filter_combo_box;		//ND filter switching is not possible in Night mode
	GtkWidget *gain_combo_box;
	int ND_filter_handler_id;
	int gain_handler_id;

	GtkWidget *gamma_frame;
	GtkWidget *gamma_settings_button;
	GtkWidget *gamma_window;
	GtkWidget *gamma_type_combo_box;
	GtkWidget *gamma_combo_box;
	GtkWidget *film_rec_dynamic_level_combo_box;
	GtkWidget *film_rec_black_stretch_level_scale;
	GtkWidget *video_rec_knee_slope_combo_box;
	GtkWidget *video_rec_knee_point_scale;
	GtkWidget *black_gamma_scale;
	GtkWidget *black_gamma_range_combo_box;
	GtkWidget *drs_combo_box;			//When any setting except Off is used for DRS and any setting except Normal is used for the gamma type or when digital zooming is valid, the setting is accepted but it is not reflected in the images. The setting is reflected in the images when the above restrictions are released.
	int gamma_type_handler_id;
	int gamma_handler_id;
	int film_rec_dynamic_level_handler_id;
	int film_rec_black_stretch_level_handler_id;
	int video_rec_knee_slope_handler_id;
	int video_rec_knee_point_handler_id;
	int black_gamma_handler_id;
	int black_gamma_range_handler_id;
	int drs_handler_id;
	GtkWidget *gamma_box;
	GtkWidget *film_rec_dynamic_level_box;
	GtkWidget *film_rec_black_stretch_level_frame;
	GtkWidget *video_rec_knee_slope_box;
	GtkWidget *video_rec_knee_point_frame;
	GtkWidget *black_gamma_frame;
	GtkWidget *black_gamma_range_box;
	GtkWidget *drs_box;

	GtkWidget *chroma_phase_frame;
	GtkWidget *chroma_phase_scale;
	int chroma_phase_handler_id;
	GtkWidget *chroma_phase_label_frame;
	GtkWidget *chroma_phase_window;

	GtkWidget *color_temperature_frame;
	GtkWidget *color_temperature_combo_box;
	int color_temperature_handler_id;
	GtkWidget *color_temperature_plus_10_button;
	GtkWidget *color_temperature_plus_1_button;
	GtkWidget *color_temperature_minus_1_button;
	GtkWidget *color_temperature_minus_10_button;

	GtkWidget *knee_frame;
	GtkWidget *knee_matrix_detail_box;
	GtkWidget *knee_settings_combo_box;	//When DRS is set to On, the knee setting is disabled
	GtkWidget *knee_button;
	GtkWidget *knee_window;
	GtkWidget *knee_point_scale;
	GtkWidget *knee_slope_scale;
	int knee_settings_handler_id;
	int knee_point_handler_id;
	int knee_slope_handler_id;
	GtkWidget *knee_point_value;
	GtkWidget *knee_slope_value;

	GtkWidget *auto_knee_response_window;
	GtkWidget *auto_knee_response_scale;
	int auto_knee_response_handler_id;
	GtkWidget *auto_knee_response_frame;

	GtkWidget *HLG_knee_frame;
	GtkWidget *HLG_knee_toggle_button;
	GtkWidget *HLG_knee_button;
	GtkWidget *HLG_knee_window;
	GtkWidget *HLG_knee_point_scale;
	GtkWidget *HLG_knee_slope_scale;
	int HLG_knee_handler_id;
	int HLG_knee_point_handler_id;
	int HLG_knee_slope_handler_id;
	GtkWidget *HLG_knee_point_value;
	GtkWidget *HLG_knee_slope_value;

	GtkWidget *matrix_type_combo_box;
	GtkWidget *matrix_settings_button;
	GtkWidget *adaptive_matrix_window;
	GtkWidget *adaptive_matrix_toggle_button_1;
	GtkWidget *matrix_window;
	GtkWidget *adaptive_matrix_toggle_button_2;
	GtkWidget *linear_matrix_R_G_scale;
	GtkWidget *linear_matrix_R_B_scale;
	GtkWidget *linear_matrix_G_R_scale;
	GtkWidget *linear_matrix_G_B_scale;
	GtkWidget *linear_matrix_B_R_scale;
	GtkWidget *linear_matrix_B_G_scale;
	int matrix_type_handler_id;
	int adaptive_matrix_handler_id_1;
	int adaptive_matrix_handler_id_2;
	int linear_matrix_R_G_handler_id;
	int linear_matrix_R_B_handler_id;
	int linear_matrix_G_R_handler_id;
	int linear_matrix_G_B_handler_id;
	int linear_matrix_B_R_handler_id;
	int linear_matrix_B_G_handler_id;
	GtkWidget *linear_matrix_R_G_frame;
	GtkWidget *linear_matrix_R_B_frame;
	GtkWidget *linear_matrix_G_R_frame;
	GtkWidget *linear_matrix_G_B_frame;
	GtkWidget *linear_matrix_B_R_frame;
	GtkWidget *linear_matrix_B_G_frame;

	GtkWidget *color_correction_drawing_area;
	color_t selected_color;
	GtkWidget *cc_saturation_R_value;
	GtkWidget *cc_saturation_R_R_Yl_value;
	GtkWidget *cc_saturation_R_Yl_value;
	GtkWidget *cc_saturation_R_Yl_Yl_value;
	GtkWidget *cc_saturation_Yl_value;
	GtkWidget *cc_saturation_Yl_G_value;
	GtkWidget *cc_saturation_G_value;
	GtkWidget *cc_saturation_G_Cy_value;
	GtkWidget *cc_saturation_Cy_value;
	GtkWidget *cc_saturation_Cy_B_value;
	GtkWidget *cc_saturation_B_value;
	GtkWidget *cc_saturation_B_Mg_value;
	GtkWidget *cc_saturation_Mg_value;
	GtkWidget *cc_saturation_Mg_R_value;
	GtkWidget *cc_saturation_Mg_R_R_value;
	GtkWidget *cc_phase_R_value;
	GtkWidget *cc_phase_R_R_Yl_value;
	GtkWidget *cc_phase_R_Yl_value;
	GtkWidget *cc_phase_R_Yl_Yl_value;
	GtkWidget *cc_phase_Yl_value;
	GtkWidget *cc_phase_Yl_G_value;
	GtkWidget *cc_phase_G_value;
	GtkWidget *cc_phase_G_Cy_value;
	GtkWidget *cc_phase_Cy_value;
	GtkWidget *cc_phase_Cy_B_value;
	GtkWidget *cc_phase_B_value;
	GtkWidget *cc_phase_B_Mg_value;
	GtkWidget *cc_phase_Mg_value;
	GtkWidget *cc_phase_Mg_R_value;
	GtkWidget *cc_phase_Mg_R_R_value;

	GtkWidget *detail_toggle_button;
	GtkWidget *detail_settings_button;
	GtkWidget *detail_window;
	GtkWidget *master_detail_scale;
	GtkWidget *detail_coring_scale;
	GtkWidget *v_detail_level_scale;
	GtkWidget *detail_band_scale;
	GtkWidget *noise_suppress_scale;
	GtkWidget *level_depend_scale;
	GtkWidget *knee_aperture_level_scale;
	GtkWidget *detail_gain_plus_scale;
	GtkWidget *detail_gain_minus_scale;
	GtkWidget *skin_detail_toggle_button;
	GtkWidget *fleshtone_noisesup_scale;
	int detail_handler_id;
	int master_detail_handler_id;
	int detail_coring_handler_id;
	int v_detail_level_handler_id;
	int detail_band_handler_id;
	int noise_suppress_handler_id;
	int level_depend_handler_id;
	int knee_aperture_level_handler_id;
	int detail_gain_plus_handler_id;
	int detail_gain_minus_handler_id;
	int skin_detail_handler_id;
	int fleshtone_noisesup_handler_id;
	GtkWidget *master_detail_frame;
	GtkWidget *detail_coring_frame;
	GtkWidget *v_detail_level_frame;
	GtkWidget *detail_band_frame;
	GtkWidget *noise_suppress_frame;
	GtkWidget *level_depend_frame;
	GtkWidget *knee_aperture_level_frame;
	GtkWidget *detail_gain_plus_frame;
	GtkWidget *detail_gain_minus_frame;
	GtkWidget *fleshtone_noisesup_frame;

	GtkWidget *saturation_frame;
	GtkWidget *saturation_scale;
	int saturation_handler_id;
	GtkWidget *saturation_plus_10_button;
	GtkWidget *saturation_plus_1_button;
	GtkWidget *saturation_minus_1_button;
	GtkWidget *saturation_minus_10_button;

	GtkWidget *r_gain_scale;
	gboolean r_gain_need_update;
	GtkWidget *g_gain_scale;
	GtkWidget *b_gain_scale;
	gboolean b_gain_need_update;
	int r_gain_handler_id;
	int g_gain_handler_id;
	int b_gain_handler_id;
	GtkWidget *r_gain_plus_10_button;
	GtkWidget *r_gain_plus_1_button;
	GtkWidget *r_gain_minus_1_button;
	GtkWidget *r_gain_minus_10_button;
	GtkWidget *g_gain_plus_10_button;
	GtkWidget *g_gain_plus_1_button;
	GtkWidget *g_gain_minus_1_button;
	GtkWidget *g_gain_minus_10_button;
	GtkWidget *b_gain_plus_10_button;
	GtkWidget *b_gain_plus_1_button;
	GtkWidget *b_gain_minus_1_button;
	GtkWidget *b_gain_minus_10_button;

	GtkWidget *r_pedestal_scale;
	gboolean r_pedestal_need_update;
	GtkWidget *g_pedestal_scale;
	GtkWidget *b_pedestal_scale;
	gboolean b_pedestal_need_update;
	int r_pedestal_handler_id;
	int g_pedestal_handler_id;
	int b_pedestal_handler_id;
	GtkWidget *r_pedestal_plus_10_button;
	GtkWidget *r_pedestal_plus_1_button;
	GtkWidget *r_pedestal_minus_1_button;
	GtkWidget *r_pedestal_minus_10_button;
	GtkWidget *g_pedestal_plus_10_button;
	GtkWidget *g_pedestal_plus_1_button;
	GtkWidget *g_pedestal_minus_1_button;
	GtkWidget *g_pedestal_minus_10_button;
	GtkWidget *b_pedestal_plus_10_button;
	GtkWidget *b_pedestal_plus_1_button;
	GtkWidget *b_pedestal_minus_1_button;
	GtkWidget *b_pedestal_minus_10_button;

	GtkWidget *shutter_frame;
	GtkWidget *shutter_type_combo_box;
	GtkWidget *shutter_step_combo_box;
	GtkWidget *shutter_synchro_button;
	GtkWidget *shutter_synchro_window;
	GtkWidget *shutter_synchro_scale;
	GtkWidget *ELC_limit_combo_box;
	int shutter_type_handler_id;
	int shutter_step_handler_id;
	int shutter_synchro_handler_id;
	int ELC_limit_handler_id;
	GtkWidget *shutter_synchro_frame;
	GtkWidget *ELC_limit_label;

	GtkWidget *pedestal_frame;
	GtkWidget *pedestal_scale;
	int pedestal_handler_id;
	GtkWidget *pedestal_plus_10_button;
	GtkWidget *pedestal_plus_1_button;
	GtkWidget *pedestal_minus_1_button;
	GtkWidget *pedestal_minus_10_button;

	GtkWidget *iris_scale;
	GtkWidget *iris_auto_toggle_button;
	int iris_handler_id;
	int iris_auto_handler_id;
	GtkWidget *iris_plus_50_button;
	GtkWidget *iris_plus_20_button;
	GtkWidget *iris_plus_10_button;
	GtkWidget *iris_plus_1_button;
	GtkWidget *iris_minus_1_button;
	GtkWidget *iris_minus_10_button;
	GtkWidget *iris_minus_20_button;
	GtkWidget *iris_minus_50_button;
	GtkWidget *iris_sensitive_widgets;

	GtkWidget *tally_widgets[4];
	guint16 tally_data;
	double tally_brightness;
	gboolean ip_tally_is_on;

	GtkWidget *sensitive_widgets;
	GtkWidget *main_box;
	GtkWidget *event_box;

	GtkWidget *ghost_rcp_body;

	GtkWidget *root_widget;

	gpointer cameras_set;

	GtkWidget *source_rcp_list_box_row;
	GtkWidget *destination_rcp_list_box_row;
} rcp_t;


//master_rcp.h
typedef struct {
	GtkWidget *on_standby_switch;
	int on_standby_handler_id;
	GtkWidget *standard_button;
	GtkWidget *mire_button;
	GtkWidget *day_night_button;
	gboolean mire;
	gboolean day_night;

	GtkWidget *scenes_bank_1_box;
	GtkWidget *scenes_bank_2_box;
	GtkWidget *store_toggle_button;

	GtkWidget *ABB_filter_gain_box;
	int ND_filter_handler_id;
	int gain_handler_id;

	GtkWidget *gamma_frame;
	int gamma_type_handler_id;
	int gamma_handler_id;
	int drs_handler_id;

	GtkWidget *color_temperature_frame;
	int color_temperature_handler_id;

	GtkWidget *knee_matrix_detail_box;
	int knee_settings_handler_id;
	int matrix_type_handler_id;
	GtkWidget *detail_button;
	gboolean detail;

	GtkWidget *padding;

	GtkWidget *saturation_scale;
	int saturation_handler_id;
	int saturation;

	GtkWidget *r_gain_scale;
	GtkWidget *g_gain_scale;
	GtkWidget *b_gain_scale;
	int r_gain_handler_id;
	int g_gain_handler_id;
	int b_gain_handler_id;
	int r_gain;
	int g_gain;
	int b_gain;

	GtkWidget *r_pedestal_scale;
	GtkWidget *g_pedestal_scale;
	GtkWidget *b_pedestal_scale;
	int r_pedestal_handler_id;
	int g_pedestal_handler_id;
	int b_pedestal_handler_id;
	int r_pedestal;
	int g_pedestal;
	int b_pedestal;

	GtkWidget *shutter_frame;
	int shutter_type_handler_id;

	GtkWidget *pedestal_scale;
	int pedestal_handler_id;
	int pedestal;

	GtkWidget *iris_scale;
	GtkWidget *iris_auto_toggle_button;
	int iris_handler_id;
	int iris_auto_handler_id;
	GtkWidget *iris_sensitive_widgets;
	int iris;
	gboolean iris_auto;

	guint timeout_id;
	int timeout_value;

	GtkWidget *root_widget;
} master_rcp_t;


//cameras_set.h
#define CAMERAS_SET_NAME_LENGTH 20
#define MAX_CAMERAS_SET 8
#define MAX_CAMERAS 15


typedef struct cameras_set_s {
	char name[CAMERAS_SET_NAME_LENGTH + 1];

	int number_of_cameras;
	rcp_t **rcp_ptr_array;

	master_rcp_t master_rcp;

	GtkWidget *rcp_box;
	GtkWidget *page;
	gint page_num;

	GtkWidget *source_list_box_row;
	GtkWidget *destination_list_box_row;

	GtkWidget *source_rcp_list_box;
	GtkWidget *destination_rcp_list_box;

	struct cameras_set_s *next;
} cameras_set_t;


extern char *cameras_set_label;
extern char *cameras_label;

extern GMutex cameras_sets_mutex;
extern GMutex current_cameras_set_mutex;

extern int number_of_cameras_sets;

extern cameras_set_t *cameras_sets;

extern cameras_set_t *current_cameras_set;

extern cameras_set_t *new_cameras_set;

extern cameras_set_t *cameras_set_with_error;


void show_cameras_set_configuration_window (void);

void add_cameras_set (void);

void delete_cameras_set (GtkButton *button, GtkWidget *confirmation_window);


//settings.h
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


//scenes.h
extern char scenes_label[];

extern GtkWidget *scenes_page;
extern GtkWidget *source_cameras_set_list_box;
extern GtkWidget *source_rcp_list_box_box;
extern GtkWidget *copy_button;
extern GtkWidget *destination_cameras_set_list_box;
extern GtkWidget *destination_rcp_list_box_box;

extern cameras_set_t *source_cameras_set;
extern cameras_set_t *destination_cameras_set;


void source_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_cameras_set_row);
void destination_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_cameras_set_row);

gint rcp_list_box_sort (GtkListBoxRow *row1, GtkListBoxRow *row2, cameras_set_t *cameras_set);

void source_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_rcp_row);
void destination_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_rcp_row);

void create_scenes_page (void);


//protocol.h
extern char my_ip_address[16];


void init_protocol (void);

void init_rcp_cmd_buffer (rcp_t *rcp);

void send_cam_request_command (rcp_t *rcp, char* cmd, int *response);

void send_cam_request_command_2 (rcp_t *rcp, char* cmd, int *response);

void send_cam_request_command_string (rcp_t *rcp, char* cmd, char *response);

void send_cam_control_command (rcp_t *rcp, char* cmd);

void send_cam_control_command_now (rcp_t *rcp, char* cmd);

void send_cam_control_command_string (rcp_t *rcp, char* cmd, char* value);

void send_cam_control_command_1_digit (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_cam_control_command_2_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_cam_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_cam_control_command_4_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_cam_control_command_5_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_ptz_request_command (rcp_t *rcp, char* cmd, int *response);

void send_ptz_request_command_string (rcp_t *rcp, char* cmd, char *response);

void send_ptz_control_command (rcp_t *rcp, char* cmd);

void send_ptz_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_tally_on_control_command (rcp_t *rcp);

void send_tally_off_control_command (rcp_t *rcp);

gboolean send_ABB_execution_control_command (rcp_t *rcp);

void send_update_start_cmd (rcp_t *rcp);

void send_update_stop_cmd (rcp_t *rcp);

gpointer send_jpeg_image_request_cmd (rcp_t *rcp);


//update_ihm.h
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


//update_notification.h
#define UPDATE_NOTIFICATION_TCP_PORT 31004


extern struct sockaddr_in update_notification_address;


void init_update_notification (void);

void start_update_notification (void);

void stop_update_notification (void);


//misc.h
void init_rcp (rcp_t *rcp);

gboolean rcp_start_working (rcp_t *rcp);

//gboolean set_rcp_on (rcp_t *rcp);

gboolean set_rcp_off (rcp_t *rcp);

gpointer check_if_camera_is_on (rcp_t *rcp);

gpointer start_rcp (rcp_t *rcp);

void copy_rcp (rcp_t *rcp_dest, rcp_t *rcp_src);

gboolean rcp_work_end (rcp_t *rcp);

gboolean remove_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp);

gboolean remove_green_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp);


//master_rcp.h
void init_master_rcp (cameras_set_t *cameras_set);


//main_window.h
#define WINDOW_MARGIN_VALUE 5

extern GdkScreen *screen;
extern GtkCssProvider *light_css_provider;
extern GtkCssProvider *dark_css_provider;

extern GtkCssProvider *css_provider_gain_red, *css_provider_gain_green, *css_provider_gain_blue;
extern GtkCssProvider *css_provider_pedestal_red, *css_provider_pedestal_green, *css_provider_pedestal_blue;
extern GtkCssProvider *css_provider_white, *css_provider_black, *css_provider_raz, *css_provider_store;

extern GtkWidget *main_window;
extern GtkWidget *main_window_notebook;
extern GtkWidget *popup_window;

extern gboolean triggered_by_master_rcp;

extern const char *application_name_txt;
extern const char *key_info_1_txt;
extern const char *key_info_2_txt;
extern const char *warning_txt;


gboolean digit_key_press (GtkEntry *entry, GdkEventKey *event);

gboolean hide_popup_window (GtkWidget *window);

gboolean popup_window_key_press (GtkWidget *window, GdkEventKey *event);

void show_rs_connection_error_window (void);


//{operating system}.h
GSList *list_rs_port (void);

void close_rs_port (void);

int open_rs_port (const char *name);

void send_to_rs_port (const char *buffer, unsigned long buffer_len);

int receive_from_rs_port (char *buffer, unsigned long buffer_len);


//tally.h
#define TSL_UMD_V5_UDP_PORT 8900


extern struct sockaddr_in tsl_umd_v5_address;


gboolean name_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp);

gboolean tally_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp);

gboolean g_source_rcp_queue_draw (rcp_t *rcp);

void init_tally (void);

void start_tally (void);

void stop_tally (void);


//sw_p_08.h
#define SW_P_08_TCP_PORT 8000


typedef struct {
	SOCKET src_socket;
	struct sockaddr_in src_addr;
	GtkWidget *connected_label;
	char buffer[256];
	int recv_len;
	int index;
	GThread *thread;
} remote_device_t;


extern GMutex sw_p_08_mutex;

extern gboolean ip_rs;

extern struct sockaddr_in sw_p_08_address;

extern remote_device_t remote_devices[2];

extern char rs_port_name[16];

extern rcp_t *rcp_vision;
extern rcp_t *rcp_pgm;
extern rcp_t *rcp_pvw;

extern char tally_pgm;
extern char tally_pvw;


void tell_camera_set_is_selected (gint page_num);

void ask_to_connect_pgm_to_ctrl_vision (void);

void ask_to_connect_camera_to_ctrl_vision (rcp_t *rcp);

gboolean rcp_button_press_event (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp);

void init_sw_p_08 (void);

void start_sw_p_08 (void);

gboolean start_sw_p_08_tcp_server (void);

gboolean start_rs_communication (void);

void stop_sw_p_08_tcp_server (void);

void stop_rs_communication (void);


//physical_rcp.h
#define PHYSICAL_RCP_TCP_PORT 9000


typedef struct {
	int iris;
	gboolean iris_auto;

	int pedestal;

	int r_gain;
	int b_gain;

	int r_pedestal;
	int b_pedestal;

	gboolean detail;
	int master_detail;

	int ND_filter;

	int gain;

	int shutter_type;
	int shutter_step;

	gboolean mire;

	gboolean OSD_menu;

	gboolean auto_focus;

	gboolean ABB;

	gboolean connected;

	guint version_timeout_id;

	SOCKET socket;
	struct sockaddr_in address;

	SOCKET socket_for_update_notification;
	struct sockaddr_in address_for_update_notification;

	GMutex mutex;
} physical_rcp_t;


extern physical_rcp_t physical_rcp;


void send_iris_auto_update_notification (void);

void send_r_gain_update_notification (void);

void send_b_gain_update_notification (void);

void send_r_pedestal_update_notification (void);

void send_b_pedestal_update_notification (void);

void send_detail_update_notification (void);

void send_master_detail_update_notification (void);

void send_ND_filter_update_notification (void);

void send_gain_update_notification (void);

void send_shutter_update_notification (void);

void send_mire_update_notification (void);

void send_auto_focus_update_notification (void);

void send_frequency_update_notification (void);

void send_format_update_notification (void);

void send_picture_level_update_notification (void);

gboolean update_physical_rcp (rcp_t *rcp);

void init_physical_rcp (void);

void start_physical_rcp (void);

void stop_physical_rcp (void);


//error.h
#define CAMERA_IS_UNREACHABLE_ERROR 0xFFFF


gboolean camera_is_unreachable (rcp_t *rcp);

gboolean clear_rcp_error (rcp_t *rcp);

gboolean Motor_Driver_Error (rcp_t *rcp);
gboolean Pan_Sensor_Error (rcp_t *rcp);
gboolean Tilt_Sensor_Error (rcp_t *rcp);
gboolean Controller_RX_Over_run_Error (rcp_t *rcp);
gboolean Controller_RX_Framing_Error (rcp_t *rcp);
gboolean Network_RX_Over_run_Error (rcp_t *rcp);
gboolean Network_RX_Framing_Error (rcp_t *rcp);
gboolean Controller_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean Network_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean System_Error (rcp_t *rcp);
gboolean Spec_Limit_Over (rcp_t *rcp);
gboolean ABB_execution_failed (rcp_t *rcp);
gboolean ABB_execution_failed_busy_status (rcp_t *rcp);

gboolean Network_communication_Error (rcp_t *rcp);
gboolean CAMERA_communication_Error (rcp_t *rcp);
gboolean CAMERA_RX_Over_run_Error (rcp_t *rcp);
gboolean CAMERA_RX_Framing_Error (rcp_t *rcp);
gboolean CAMERA_RX_Command_Buffer_Overflow (rcp_t *rcp);

gboolean FPGA_Config_Error (rcp_t *rcp);
gboolean NET_Life_monitoring_Error (rcp_t *rcp);
gboolean BE_Life_monitoring_Error (rcp_t *rcp);
gboolean IF_BE_UART_Buffer_Overflow (rcp_t *rcp);
gboolean IF_BE_UART_Framing_Error (rcp_t *rcp);
gboolean IF_BE_UART_Buffer_Overflow_2 (rcp_t *rcp);
gboolean CAM_Life_monitoring_Error (rcp_t *rcp);
gboolean Fan1_error (rcp_t *rcp);
gboolean Fan2_error (rcp_t *rcp);
gboolean High_Temp (rcp_t *rcp);
gboolean Low_Temp (rcp_t *rcp);
gboolean Temp_Sensor_Error (rcp_t *rcp);
gboolean Lens_Initialize_Error (rcp_t *rcp);
gboolean PT_Initialize_Error (rcp_t *rcp);
gboolean MR_Level_Error (rcp_t *rcp);
gboolean MR_Offset_Error (rcp_t *rcp);
gboolean Origin_Offset_Error (rcp_t *rcp);
gboolean Angle_MR_Sensor_Error (rcp_t *rcp);
gboolean PT_Gear_Error (rcp_t *rcp);
gboolean Motor_Disconnect_Error (rcp_t *rcp);

gboolean log_clear_rcp_error (rcp_t *rcp);

gboolean log_Motor_Driver_Error (rcp_t *rcp);
gboolean log_Pan_Sensor_Error (rcp_t *rcp);
gboolean log_Tilt_Sensor_Error (rcp_t *rcp);
gboolean log_Controller_RX_Over_run_Error (rcp_t *rcp);
gboolean log_Controller_RX_Framing_Error (rcp_t *rcp);
gboolean log_Network_RX_Over_run_Error (rcp_t *rcp);
gboolean log_Network_RX_Framing_Error (rcp_t *rcp);
gboolean log_Controller_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean log_Network_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean log_System_Error (rcp_t *rcp);
gboolean log_Spec_Limit_Over (rcp_t *rcp);
gboolean log_ABB_execution_failed (rcp_t *rcp);
gboolean log_ABB_execution_failed_busy_status (rcp_t *rcp);

gboolean log_Network_communication_Error (rcp_t *rcp);
gboolean log_CAMERA_communication_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Over_run_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Framing_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Command_Buffer_Overflow (rcp_t *rcp);

gboolean log_FPGA_Config_Error (rcp_t *rcp);
gboolean log_NET_Life_monitoring_Error (rcp_t *rcp);
gboolean log_BE_Life_monitoring_Error (rcp_t *rcp);
gboolean log_IF_BE_UART_Buffer_Overflow (rcp_t *rcp);
gboolean log_IF_BE_UART_Framing_Error (rcp_t *rcp);
gboolean log_IF_BE_UART_Buffer_Overflow_2 (rcp_t *rcp);
gboolean log_CAM_Life_monitoring_Error (rcp_t *rcp);
gboolean log_Fan1_error (rcp_t *rcp);
gboolean log_Fan2_error (rcp_t *rcp);
gboolean log_High_Temp (rcp_t *rcp);
gboolean log_Low_Temp (rcp_t *rcp);
gboolean log_Temp_Sensor_Error (rcp_t *rcp);
gboolean log_Lens_Initialize_Error (rcp_t *rcp);
gboolean log_PT_Initialize_Error (rcp_t *rcp);
gboolean log_MR_Level_Error (rcp_t *rcp);
gboolean log_MR_Offset_Error (rcp_t *rcp);
gboolean log_Origin_Offset_Error (rcp_t *rcp);
gboolean log_Angle_MR_Sensor_Error (rcp_t *rcp);
gboolean log_PT_Gear_Error (rcp_t *rcp);
gboolean log_Motor_Disconnect_Error (rcp_t *rcp);

void start_error_log (void);

void stop_error_log (void);


//rcp.h
#define MARGIN_VALUE 3


GtkWidget *create_scenes_frame (rcp_t *rcp);

void ABB_button_clicked (GtkButton *button, rcp_t *rcp);

void create_ghost_rcp_widgets (rcp_t *rcp);

void create_rcp_widgets (rcp_t *rcp);


//rcp_AW_HE130.h
void init_rcp_AW_HE130 (rcp_t *rcp);

gboolean switch_to_AW_HE130 (rcp_t *rcp);

gpointer load_standard_AW_HE130 (rcp_t *rcp);

gpointer load_scene_AW_HE130 (rcp_t *rcp);

void create_rcp_widgets_AW_HE130 (rcp_t *rcp);


//rcp_AW_UE150.h
void init_rcp_AW_UE150 (rcp_t *rcp);

gboolean switch_to_AW_UE150 (rcp_t *rcp);

gpointer load_standard_AW_UE150 (rcp_t *rcp);

gpointer load_scene_AW_UE150 (rcp_t *rcp);

void create_rcp_widgets_AW_UE150 (rcp_t *rcp);


//ND_Filter.h
void set_ND_filter (rcp_t *rcp);

GtkWidget *create_ND_filter_frame_AW_HE130 (rcp_t *rcp);

GtkWidget *create_ND_filter_frame_AW_UE150 (rcp_t *rcp);

gboolean update_ND_filter_combo_box (rcp_t *rcp);


//gain.h
void set_gain_AW_HE130 (rcp_t *rcp);

GtkWidget *create_gain_frame_AW_HE130 (rcp_t *rcp);

gboolean populate_gain_combo_box_AW_UE150 (GtkComboBoxText *combo_box_text);

void set_gain_AW_UE150 (rcp_t *rcp);

GtkWidget *create_gain_frame_AW_UE150 (rcp_t *rcp);


//gamma.h
extern char gamma_type_tooltip[];
extern char gamma_tooltip[];
extern char drs_tooltip[];


void set_gamma_type (rcp_t *rcp);

void set_gamma (rcp_t *rcp);

void set_drs (rcp_t *rcp);

GtkWidget *create_gamma_frame_AW_HE130 (rcp_t *rcp);

void set_film_rec_dynamic_level (rcp_t *rcp);

void set_film_rec_black_stretch_level_label (rcp_t *rcp);

void set_film_rec_black_stretch_level (rcp_t *rcp);

void set_video_rec_knee_slope (rcp_t *rcp);

void set_video_rec_knee_point_label (rcp_t *rcp);

void set_video_rec_knee_point (rcp_t *rcp);

void set_black_gamma_label (rcp_t *rcp);

void set_black_gamma (rcp_t *rcp);

void set_black_gamma_range (rcp_t *rcp);

GtkWidget *create_gamma_frame_AW_UE150 (rcp_t *rcp);

gboolean update_gamma_type_combo_box (rcp_t *rcp);


//chroma_phase.h
void set_chroma_phase_label (rcp_t *rcp);

void set_chroma_phase (rcp_t *rcp);

GtkWidget *create_chroma_phase_frame (rcp_t *rcp);


//color_temperature.h
void set_color_temperature_AW_HE130 (rcp_t *rcp);

gboolean color_temperature_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean color_temperature_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean color_temperature_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean color_temperature_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_color_temperature_frame_AW_HE130 (rcp_t *rcp);


int find_color_temperature_index (int color_temperature);

void set_color_temperature_AW_UE150 (rcp_t *rcp);

gboolean color_temperature_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean color_temperature_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean color_temperature_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean color_temperature_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_color_temperature_frame_AW_UE150 (rcp_t *rcp);


//knee.h
extern char auto_knee_response_tooltip[];


void set_knee_point_label (rcp_t *rcp);

void set_knee_point (rcp_t *rcp);

void set_knee_slope_label (rcp_t *rcp);

void set_knee_slope (rcp_t *rcp);

void set_auto_knee_response_label (rcp_t *rcp);

void set_auto_knee_response (rcp_t *rcp);

void set_knee_settings (rcp_t *rcp);

GtkWidget *create_knee_frame (rcp_t *rcp);

void set_HLG_knee_point_label (rcp_t *rcp);

void set_HLG_knee_point (rcp_t *rcp);

void set_HLG_knee_slope_label (rcp_t *rcp);

void set_HLG_knee_slope (rcp_t *rcp);

void set_HLG_knee (rcp_t *rcp);

GtkWidget *create_HLG_knee_frame (rcp_t *rcp);


//matrix.h
extern void (*set_cc_saturation_label_funcs[15]) (rcp_t *rcp);
extern void (*set_cc_phase_label_funcs[15]) (rcp_t *rcp);


void set_linear_matrix_R_G_label (rcp_t *rcp);
void set_linear_matrix_R_B_label (rcp_t *rcp);
void set_linear_matrix_G_R_label (rcp_t *rcp);
void set_linear_matrix_G_B_label (rcp_t *rcp);
void set_linear_matrix_B_R_label (rcp_t *rcp);
void set_linear_matrix_B_G_label (rcp_t *rcp);

void set_cc_saturation_R_label (rcp_t *rcp);
void set_cc_saturation_R_R_Yl_label (rcp_t *rcp);
void set_cc_saturation_R_Yl_label (rcp_t *rcp);
void set_cc_saturation_R_Yl_Yl_label (rcp_t *rcp);
void set_cc_saturation_Yl_label (rcp_t *rcp);
void set_cc_saturation_Yl_G_label (rcp_t *rcp);
void set_cc_saturation_G_label (rcp_t *rcp);
void set_cc_saturation_G_Cy_label (rcp_t *rcp);
void set_cc_saturation_Cy_label (rcp_t *rcp);
void set_cc_saturation_Cy_B_label (rcp_t *rcp);
void set_cc_saturation_B_label (rcp_t *rcp);
void set_cc_saturation_B_Mg_label (rcp_t *rcp);
void set_cc_saturation_Mg_label (rcp_t *rcp);
void set_cc_saturation_Mg_R_label (rcp_t *rcp);
void set_cc_saturation_Mg_R_R_label (rcp_t *rcp);

void set_cc_phase_R_label (rcp_t *rcp);
void set_cc_phase_R_R_Yl_label (rcp_t *rcp);
void set_cc_phase_R_Yl_label (rcp_t *rcp);
void set_cc_phase_R_Yl_Yl_label (rcp_t *rcp);
void set_cc_phase_Yl_label (rcp_t *rcp);
void set_cc_phase_Yl_G_label (rcp_t *rcp);
void set_cc_phase_G_label (rcp_t *rcp);
void set_cc_phase_G_Cy_label (rcp_t *rcp);
void set_cc_phase_Cy_label (rcp_t *rcp);
void set_cc_phase_Cy_B_label (rcp_t *rcp);
void set_cc_phase_B_label (rcp_t *rcp);
void set_cc_phase_B_Mg_label (rcp_t *rcp);
void set_cc_phase_Mg_label (rcp_t *rcp);
void set_cc_phase_Mg_R_label (rcp_t *rcp);
void set_cc_phase_Mg_R_R_label (rcp_t *rcp);

void set_matrix_type (rcp_t *rcp);
void set_adaptive_matrix (rcp_t *rcp);
void set_linear_matrix_R_G (rcp_t *rcp);
void set_linear_matrix_R_B (rcp_t *rcp);
void set_linear_matrix_G_R (rcp_t *rcp);
void set_linear_matrix_G_B (rcp_t *rcp);
void set_linear_matrix_B_R (rcp_t *rcp);
void set_linear_matrix_B_G (rcp_t *rcp);

void set_cc_saturation_R (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_Yl_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Yl_G (rcp_t *rcp, gboolean wait);
void set_cc_saturation_G (rcp_t *rcp, gboolean wait);
void set_cc_saturation_G_Cy (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Cy (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Cy_B (rcp_t *rcp, gboolean wait);
void set_cc_saturation_B (rcp_t *rcp, gboolean wait);
void set_cc_saturation_B_Mg (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg_R (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg_R_R (rcp_t *rcp, gboolean wait);

void set_cc_phase_R (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_Yl_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_Yl_G (rcp_t *rcp, gboolean wait);
void set_cc_phase_G (rcp_t *rcp, gboolean wait);
void set_cc_phase_G_Cy (rcp_t *rcp, gboolean wait);
void set_cc_phase_Cy (rcp_t *rcp, gboolean wait);
void set_cc_phase_Cy_B (rcp_t *rcp, gboolean wait);
void set_cc_phase_B (rcp_t *rcp, gboolean wait);
void set_cc_phase_B_Mg (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg_R (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg_R_R (rcp_t *rcp, gboolean wait);

GtkWidget *create_matrix_frame (rcp_t *rcp);


//detail.h
extern char master_detail_tooltip[];
extern char detail_coring_tooltip[];
extern char v_detail_level_tooltip[];
extern char detail_band_tooltip[];
extern char noise_suppress_tooltip[];
extern char level_depend_tooltip[];
extern char knee_aperture_level_tooltip[];
extern char detail_gain_plus_tooltip[];
extern char detail_gain_minus_tooltip[];
extern char fleshtone_noisesup_tooltip[];


void set_master_detail_label (rcp_t *rcp);

void set_master_detail (rcp_t *rcp);

void set_v_detail_level_label (rcp_t *rcp);

void set_v_detail_level (rcp_t *rcp);

void set_detail_band_label (rcp_t *rcp);

void set_detail_band (rcp_t *rcp);

void set_noise_suppress_label (rcp_t *rcp);

void set_noise_suppress (rcp_t *rcp);

void set_fleshtone_noisesup_label (rcp_t *rcp);

void set_fleshtone_noisesup (rcp_t *rcp);

void set_detail_coring_label (rcp_t *rcp);

void set_detail_coring (rcp_t *rcp);

void set_level_depend_label (rcp_t *rcp);

void set_level_depend (rcp_t *rcp);

void set_knee_aperture_level_label (rcp_t *rcp);

void set_knee_aperture_level (rcp_t *rcp);

void set_detail_gain_plus_label (rcp_t *rcp);

void set_detail_gain_plus (rcp_t *rcp);

void set_detail_gain_minus_label (rcp_t *rcp);

void set_detail_gain_minus (rcp_t *rcp);

void set_skin_detail (rcp_t *rcp);

void set_detail (rcp_t *rcp);

GtkWidget *create_detail_frame (rcp_t *rcp);


//saturation.h
void set_saturation_label (rcp_t *rcp);

void set_saturation (rcp_t *rcp);

gboolean saturation_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean saturation_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_saturation_frame_AW_HE130 (rcp_t *rcp);

gboolean saturation_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean saturation_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_saturation_frame_AW_UE150 (rcp_t *rcp);


//white.h
void set_r_gain (rcp_t *rcp);

gboolean r_gain_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_gain_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean r_gain_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_gain_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define r_gain_plus_10_button_pressed_AW_HE130 r_gain_plus_10_button_pressed
#define r_gain_plus_1_button_pressed_AW_HE130 r_gain_plus_1_button_pressed

#define r_gain_minus_1_button_pressed_AW_HE130 r_gain_minus_1_button_pressed
#define r_gain_minus_10_button_pressed_AW_HE130 r_gain_minus_10_button_pressed

gboolean g_gain_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_gain_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_gain_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_gain_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

void set_b_gain (rcp_t *rcp);

gboolean b_gain_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_gain_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean b_gain_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_gain_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define b_gain_plus_10_button_pressed_AW_HE130 b_gain_plus_10_button_pressed
#define b_gain_plus_1_button_pressed_AW_HE130 b_gain_plus_1_button_pressed

#define b_gain_minus_1_button_pressed_AW_HE130 b_gain_minus_1_button_pressed
#define b_gain_minus_10_button_pressed_AW_HE130 b_gain_minus_10_button_pressed

GtkWidget *create_white_frame_AW_HE130 (rcp_t *rcp);


void set_r_gain_AW_UE150 (rcp_t *rcp);

gboolean r_gain_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_gain_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean r_gain_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_gain_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_gain_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_gain_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_gain_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_gain_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

void set_b_gain_AW_UE150 (rcp_t *rcp);

gboolean b_gain_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_gain_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean b_gain_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_gain_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_white_frame_AW_UE150 (rcp_t *rcp);


//black.h
void set_r_pedestal (rcp_t *rcp);

gboolean r_pedestal_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_pedestal_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean r_pedestal_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_pedestal_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define r_pedestal_plus_10_button_pressed_AW_HE130 r_pedestal_plus_10_button_pressed
#define r_pedestal_plus_1_button_pressed_AW_HE130 r_pedestal_plus_1_button_pressed
#define r_pedestal_minus_1_button_pressed_AW_HE130 r_pedestal_minus_1_button_pressed
#define r_pedestal_minus_10_button_pressed_AW_HE130 r_pedestal_minus_10_button_pressed

gboolean g_pedestal_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_pedestal_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

void set_b_pedestal (rcp_t *rcp);

gboolean b_pedestal_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_pedestal_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean b_pedestal_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_pedestal_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define b_pedestal_plus_10_button_pressed_AW_HE130 b_pedestal_plus_10_button_pressed
#define b_pedestal_plus_1_button_pressed_AW_HE130 b_pedestal_plus_1_button_pressed
#define b_pedestal_minus_1_button_pressed_AW_HE130 b_pedestal_minus_1_button_pressed
#define b_pedestal_minus_10_button_pressed_AW_HE130 b_pedestal_minus_10_button_pressed

GtkWidget *create_black_frame_AW_HE130 (rcp_t *rcp);


#define r_pedestal_plus_10_button_pressed_AW_UE150 r_pedestal_plus_10_button_pressed
#define r_pedestal_plus_1_button_pressed_AW_UE150 r_pedestal_plus_1_button_pressed
#define r_pedestal_minus_1_button_pressed_AW_UE150 r_pedestal_minus_1_button_pressed
#define r_pedestal_minus_10_button_pressed_AW_UE150 r_pedestal_minus_10_button_pressed

void set_g_pedestal_AW_UE150 (rcp_t *rcp);

gboolean g_pedestal_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_pedestal_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define b_pedestal_plus_10_button_pressed_AW_UE150 b_pedestal_plus_10_button_pressed
#define b_pedestal_plus_1_button_pressed_AW_UE150 b_pedestal_plus_1_button_pressed
#define b_pedestal_minus_1_button_pressed_AW_UE150 b_pedestal_minus_1_button_pressed
#define b_pedestal_minus_10_button_pressed_AW_UE150 b_pedestal_minus_10_button_pressed

GtkWidget *create_black_frame_AW_UE150 (rcp_t *rcp);


//shutter.h
void set_shutter_type_AW_HE130 (rcp_t *rcp);

void populate_shutter_step_combo_box_AW_HE130 (GtkComboBoxText *combo_box);

void set_shutter_step_AW_HE130 (rcp_t *rcp);

void set_shutter_synchro_label_AW_HE130 (rcp_t *rcp);

void set_shutter_synchro_AW_HE130 (rcp_t *rcp);

GtkWidget *create_shutter_frame_AW_HE130 (rcp_t *rcp);


void set_shutter_type_AW_UE150 (rcp_t *rcp);

void populate_shutter_step_combo_box_AW_UE150 (GtkComboBoxText *combo_box);

void update_shutter_step_combo_box_AW_UE150 (rcp_t *rcp);

void set_shutter_step_AW_UE150 (rcp_t *rcp);

void set_shutter_synchro_label_AW_UE150 (rcp_t *rcp);

void set_shutter_synchro_AW_UE150 (rcp_t *rcp);

void create_shutter_synchro_window_AW_UE150 (rcp_t *rcp);

void set_ELC_limit (rcp_t *rcp);

GtkWidget *create_shutter_frame_AW_UE150 (rcp_t *rcp);


//pedestal.h
void set_pedestal_label_AW_HE130 (rcp_t *rcp);

void set_pedestal_AW_HE130 (rcp_t *rcp);

gboolean pedestal_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean pedestal_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean pedestal_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean pedestal_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_pedestal_frame_AW_HE130 (rcp_t *rcp);


void set_pedestal_label_AW_UE150 (rcp_t *rcp);

void set_pedestal_AW_UE150 (rcp_t *rcp);

gboolean pedestal_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean pedestal_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean pedestal_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean pedestal_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_pedestal_frame_AW_UE150 (rcp_t *rcp);


//iris.h
void set_iris (rcp_t *rcp);

gboolean iris_plus_50_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_20_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean iris_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_20_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_50_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define iris_plus_50_button_pressed_AW_HE130 iris_plus_50_button_pressed
#define iris_plus_20_button_pressed_AW_HE130 iris_plus_20_button_pressed
#define iris_plus_10_button_pressed_AW_HE130 iris_plus_10_button_pressed
#define iris_plus_1_button_pressed_AW_HE130 iris_plus_1_button_pressed

#define iris_minus_1_button_pressed_AW_HE130 iris_minus_1_button_pressed
#define iris_minus_10_button_pressed_AW_HE130 iris_minus_10_button_pressed
#define iris_minus_20_button_pressed_AW_HE130 iris_minus_20_button_pressed
#define iris_minus_50_button_pressed_AW_HE130 iris_minus_50_button_pressed

#define iris_plus_50_button_pressed_AW_UE150 iris_plus_50_button_pressed
#define iris_plus_20_button_pressed_AW_UE150 iris_plus_20_button_pressed
#define iris_plus_10_button_pressed_AW_UE150 iris_plus_10_button_pressed
#define iris_plus_1_button_pressed_AW_UE150 iris_plus_1_button_pressed

#define iris_minus_1_button_pressed_AW_UE150 iris_minus_1_button_pressed
#define iris_minus_10_button_pressed_AW_UE150 iris_minus_10_button_pressed
#define iris_minus_20_button_pressed_AW_UE150 iris_minus_20_button_pressed
#define iris_minus_50_button_pressed_AW_UE150 iris_minus_50_button_pressed

void set_iris_auto (rcp_t *rcp);

GtkWidget *create_iris_frame (rcp_t *rcp);


#endif

