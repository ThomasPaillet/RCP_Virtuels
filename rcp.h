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
#define COLOR_TEMPERATURE_DEFAULT_AW_UE150 80		//5600K

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
	int scene_selected;

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
	int color_temperature_handler_id_2;
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


#define MARGIN_VALUE 3


GtkWidget *create_scenes_frame (rcp_t *rcp);

void deselect_scene (rcp_t *rcp);

void ABB_button_clicked (GtkButton *button, rcp_t *rcp);

void create_ghost_rcp_widgets (rcp_t *rcp);

void create_rcp_widgets (rcp_t *rcp);


#endif

