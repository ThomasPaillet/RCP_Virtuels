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

#ifndef __RCP_H
#define __RCP_H


//#define RCP_ELECTRO


#ifdef _WIN32
	#include <winsock2.h>

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


#include <gtk/gtk.h>
#include <sys/time.h>


typedef enum {_59_94fps, _29_97fps, _23_98fps, _50fps, _25fps} fps_t;


#define ND_FILTER_DEFAULT 0
#define GAIN_DEFAULT 36

#define GAMMA_TYPE_DEFAULT 0
#define GAMMA_DEFAULT 30
#define DRS_DEFAULT 3

#define COLOR_TEMPERATURE_DEFAULT 28	//5600K

#define KNEE_SETTINGS_DEFAULT 0
#define KNEE_POINT_DEFAULT 0x80
#define KNEE_SLOPE_DEFAULT 0x55

#define MATRIX_TYPE_DEFAULT 0
#define LINEAR_MATRIX_DEFAULT 0x80
#define COLOR_CORRECTION_DEFAULT 0x00

#define DETAIL_DEFAULT TRUE			//On
#define MASTER_DETAIL_DEFAULT 0x80		//31
#define V_DETAIL_LEVEL_DEFAULT 0x80	//0
#define DETAIL_BAND_DEFAULT 0x80		//0
#define NOISE_SUPPRESS_DEFAULT 0x01	//1
#define FLESHTONE_NOISESUP_DEFAULT 0x80	//0	//0x90	//16

#define SATURATION_DEFAULT 0x80

#define R_GAIN_DEFAULT 0x096
#define G_GAIN_DEFAULT 0x096
#define B_GAIN_DEFAULT 0x096

#define R_PEDESTAL_DEFAULT 0x096
#define G_PEDESTAL_DEFAULT 0x096
#define B_PEDESTAL_DEFAULT 0x096

#define SHUTTER_TYPE_DEFAULT 0
#define SHUTTER_SYNCHRO_DEFAULT 0x001

#define PEDESTAL_DEFAULT 0x096

#define IRIS_DEFAULT 0xAAA
#define IRIS_AUTO_DEFAULT FALSE


typedef struct {
	int R_G;
	int R_B;
	int G_R;
	int G_B;
	int B_R;
	int B_G;
} linear_matrix_t;

typedef enum {R, R_R_Yl, R_Yl, R_Yl_Yl, Yl, Yl_G, G, G_Cy, Cy, Cy_B, B, B_Mg, Mg, Mg_R, Mg_R_R, nothing} color_t;

typedef struct {
	int ND_filter;
	int gain;

	int gamma_type;
	int gamma;
	int drs;

	int color_temperature;

	int knee_settings;
	int knee_point;
	int knee_slope;

	int matrix_type;
	linear_matrix_t linear_matrix;
	int cc_saturation[15];
	int cc_phase[15];

	gboolean detail;
	int master_detail;
	int v_detail_level;
	int detail_band;
	int noise_suppress;
	int fleshtone_noisesup;

	int saturation;

	int r_gain;
	int b_gain;

	int r_pedestal;
	int b_pedestal;

	int shutter_type;
	int shutter_step;
	int shutter_synchro;

	int pedestal;

	int iris;
	gboolean iris_auto;
} scene_t;


#define NB_SCENES 10


typedef struct {
	char name[3];
	gboolean active;
	char ip_adresse[16];
	char new_ip_adresse[16];
	gboolean ip_adresse_is_valid;

	int matrix_source_number;

	char cmd_buffer[272];
	char *last_cmd;
	gboolean cam_ptz;

	char last_ctrl_cmd[16];
	int last_ctrl_cmd_len;
	GMutex cmd_mutex;

	struct sockaddr_in adresse;

	GMutex other_rcp_mutex;
	GSList *other_rcp;	//with_the_same_ip_adresse;

	gboolean camera_is_on;
	gboolean camera_is_working;

	GThread *thread;
	struct timeval last_time;
	guint timeout_id;
	gboolean r_b;
	gboolean need_last_call;
	int timeout_value;
	gboolean green_is_ahead;
	gboolean green_timeout_return_value;

	int jpeg_page;

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
	GtkWidget *gamma_type_combo_box;
	GtkWidget *gamma_combo_box;
	GtkWidget *drs_combo_box;			//When any setting except Off is used for DRS and any setting except Normal is used for the gamma type or when digital zooming is valid, the setting is accepted but it is not reflected in the images. The setting is reflected in the images when the above restrictions are released.
	int gamma_type_handler_id;
	int gamma_handler_id;
	int drs_handler_id;

	GtkWidget *color_temperature_frame;
	GtkWidget *color_temperature_combo_box;
	int color_temperature_handler_id;

	GtkWidget *knee_matrix_detail_box;
	GtkWidget *knee_settings_combo_box;	//When DRS is set to On, the knee setting is disabled
	GtkWidget *knee_button;
	GtkWidget *knee_point_slope_window;
	GtkWidget *knee_point_scale;
	GtkWidget *knee_slope_scale;
	int knee_settings_handler_id;
	int knee_point_handler_id;
	int knee_slope_handler_id;
	GtkWidget *knee_point_value;
	GtkWidget *knee_slope_value;

	GtkWidget *matrix_type_combo_box;
	GtkWidget *matrix_color_button;
	GtkWidget *matrix_window;
	GtkWidget *linear_matrix_R_G_scale;
	GtkWidget *linear_matrix_R_B_scale;
	GtkWidget *linear_matrix_G_R_scale;
	GtkWidget *linear_matrix_G_B_scale;
	GtkWidget *linear_matrix_B_R_scale;
	GtkWidget *linear_matrix_B_G_scale;
	int matrix_type_handler_id;
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
	GtkWidget *v_detail_level_scale;
	GtkWidget *detail_band_scale;
	GtkWidget *noise_suppress_scale;
	GtkWidget *fleshtone_noisesup_scale;
	int detail_handler_id;
	int master_detail_handler_id;
	int v_detail_level_handler_id;
	int detail_band_handler_id;
	int noise_suppress_handler_id;
	int fleshtone_noisesup_handler_id;
	GtkWidget *master_detail_frame;
	GtkWidget *v_detail_level_frame;
	GtkWidget *detail_band_frame;
	GtkWidget *noise_suppress_frame;
	GtkWidget *fleshtone_noisesup_frame;

	GtkWidget *saturation_scale;
	int saturation_handler_id;
	GtkWidget *saturation_frame;

	GtkWidget *r_gain_scale;
	gboolean r_gain_need_update;
	GtkWidget *g_gain_scale;
	int g_gain;
	GtkWidget *b_gain_scale;
	gboolean b_gain_need_update;
	int r_gain_handler_id;
	int g_gain_handler_id;
	int b_gain_handler_id;

	GtkWidget *r_pedestal_scale;
	gboolean r_pedestal_need_update;
	GtkWidget *g_pedestal_scale;
	int g_pedestal;
	GtkWidget *b_pedestal_scale;
	gboolean b_pedestal_need_update;
	int r_pedestal_handler_id;
	int g_pedestal_handler_id;
	int b_pedestal_handler_id;

	GtkWidget *shutter_frame;
	GtkWidget *shutter_type_combo_box;
	GtkWidget *shutter_step_combo_box;
	GtkWidget *shutter_synchro_button;
	GtkWidget *shutter_synchro_window;
	GtkWidget *shutter_synchro_scale;
	int shutter_type_handler_id;
	int shutter_step_handler_id;
	int shutter_synchro_handler_id;
	GtkWidget *shutter_synchro_frame;

	GtkWidget *pedestal_scale;
	int pedestal_handler_id;
	GtkWidget *pedestal_frame;

	GtkWidget *iris_scale;
	GtkWidget *iris_auto_toggle_button;
	int iris_handler_id;
	int iris_auto_handler_id;
	GtkWidget *iris_plus_10_button;
	GtkWidget *iris_plus_1_button;
	GtkWidget *iris_minus_1_button;
	GtkWidget *iris_minus_10_button;
	GtkWidget *iris_sensitive_widgets;

	GtkWidget *tally_widgets[4];
	guint16 tally_data;
	double tally_brightness;
	gboolean tally_1_is_on;

	GtkWidget *sensitive_widgets;
	GtkWidget *event_box;

	GtkWidget *ghost_rcp_body;

	GtkWidget *root_widget;

	gpointer camera_set;

	GtkWidget *source_rcp_list_box_row;
	GtkWidget *destination_rcp_list_box_row;
} rcp_t;


void set_ND_filter (rcp_t *rcp);
void set_gain (rcp_t *rcp);

void set_gamma_type (rcp_t *rcp);
void set_gamma (rcp_t *rcp);
void set_drs (rcp_t *rcp);

void set_color_temperature (rcp_t *rcp);

void set_knee_settings (rcp_t *rcp);
void set_knee_point (rcp_t *rcp);
void set_knee_slope (rcp_t *rcp);

void set_matrix_type (rcp_t *rcp);
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

void set_detail (rcp_t *rcp);
void set_master_detail (rcp_t *rcp);
void set_v_detail_level (rcp_t *rcp);
void set_detail_band (rcp_t *rcp);
void set_noise_suppress (rcp_t *rcp);
void set_fleshtone_noisesup (rcp_t *rcp);

void set_saturation (rcp_t *rcp);

void set_r_gain (rcp_t *rcp);
void set_b_gain (rcp_t *rcp);

void set_r_pedestal (rcp_t *rcp);
void set_b_pedestal (rcp_t *rcp);

void set_shutter_type (rcp_t *rcp);
void set_shutter_step (rcp_t *rcp);
void set_shutter_synchro (rcp_t *rcp);

void set_pedestal (rcp_t *rcp);

void set_iris (rcp_t *rcp);
gboolean iris_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
void set_iris_auto (rcp_t *rcp);


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
	int shutter_type;
	GtkWidget *shutter_type_combo_box;
	GtkWidget *shutter_step_combo_box;
	int shutter_type_handler_id;
	int shutter_step_handler_id;

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

	int number_of_ghost_cameras;
	int on_standby_count;

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

extern int number_of_cameras_sets;

extern cameras_set_t *cameras_sets;

extern cameras_set_t *new_cameras_set;

extern cameras_set_t *cameras_set_with_error;


void show_cameras_set_configuration_window (void);

void add_cameras_set (void);

void delete_cameras_set (GtkButton *button, GtkWidget *confirmation_window);


//settings.h
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


#define NB_SETTINGS 13


extern gboolean backup_needed;

extern gboolean format_is_50Hz;
extern fps_t output_fps;

extern GList *rcp_glist;
extern GMutex rcp_start_glist_mutex;
extern GList *rcp_start_glist;
extern GList *ghost_rcp_glist;

#ifdef RCP_ELECTRO
extern parameter_t format_parameters_array_pal[6];
extern char *format_answers_array_pal[6];
extern parameter_t format_parameters_array_ntsc[8];
extern char *format_answers_array_ntsc[8];
#endif

extern setting_t settings_array[NB_SETTINGS];
extern int settings_parameters_indexes_array[NB_SETTINGS];

extern int picture_level;
extern int tally_input;
extern int osd_mix;

extern gboolean theme;
extern gboolean show_master_rcp;

extern GtkWidget *settings_page;
extern GtkWidget *settings_list_box;
extern GtkWidget *settings_configuration_button;
extern GtkWidget *settings_new_button;
extern GtkWidget *settings_delete_button;

extern GtkWidget *ip_waiting_label;
extern GtkWidget *ip_connected_label_1;
extern GtkWidget *ip_connected_label_2;
extern GtkWidget *rs_ok_label;


#ifdef RCP_ELECTRO
extern gboolean camera_settings_checked;

gboolean update_osd_mix (void);
gboolean update_picture_level_scale (void);
gboolean update_tally_input_combo_box_text (void);

void check_camera_settings_ro (rcp_t *rcp);
gpointer check_cameras_settings_ro (void);
#else
void check_camera_settings (rcp_t *rcp);
#endif

void create_settings_page (void);

void load_settings_from_config_file (void);

void load_cameras_set_from_config_file (void);

void save_settings_and_cameras_sets_to_config_file (void);


//scenes.h
extern char *scenes_label;

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
extern char my_ip_adresse[16];


void init_protocol (void);

void init_rcp_cmd_buffer (rcp_t *rcp);

void send_cam_request_command (rcp_t *rcp, char* cmd, int *response);

void send_cam_request_command_string (rcp_t *rcp, char* cmd, char *response);

void send_cam_control_command (rcp_t *rcp, char* cmd);

void send_cam_control_command_string (rcp_t *rcp, char* cmd, char* value);

void send_cam_control_command_1_digit (rcp_t *rcp, char* cmd, int value);

void send_cam_control_command_2_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_cam_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

void send_ptz_request_command (rcp_t *rcp, char* cmd, int *response);

void send_ptz_request_command_string (rcp_t *rcp, char* cmd, char *response);

void send_ptz_control_command (rcp_t *rcp, char* cmd);

void send_ptz_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait);

gboolean send_ABB_execution_control_command (rcp_t *rcp);

void send_update_start_cmd (rcp_t *rcp);

void send_update_stop_cmd (rcp_t *rcp);

gpointer send_jpeg_image_request_cmd (rcp_t *rcp);


//update_notification.h
#define UPDATE_NOTIFICATION_TCP_PORT 31004


extern struct sockaddr_in update_notification_adresse;


void init_update_notification (void);

void start_update_notification (void);

void stop_update_notification (void);


//misc.h
void init_rcp (rcp_t *rcp);

gboolean set_rcp_on (rcp_t *rcp);

gboolean set_rcp_off (rcp_t *rcp);

gpointer check_if_camera_is_on (rcp_t *rcp);

gpointer start_rcp (rcp_t *rcp);

gpointer update_rcp (rcp_t *rcp);

void copy_rcp (rcp_t *rcp_dest, rcp_t *rcp_src);

void rcp_work_start (rcp_t *rcp, GThreadFunc thread_func);

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

extern GtkWidget *main_window;
extern GtkWidget *main_window_notebook;

extern gboolean triggered_by_master_rcp;

extern const char *application_name_txt;
extern const char *key_info_1_txt;
extern const char *key_info_2_txt;
extern const char *warning_txt;


gboolean digit_key_press (GtkEntry *entry, GdkEventKey *event);

void show_rs_connection_error_window (void);


//{operating system}.h
GSList *list_rs_port (void);

void close_rs_port (void);

int open_rs_port (const char *name);

void send_to_rs_port (const char *buffer, unsigned long buffer_len);

int receive_from_rs_port (char *buffer, unsigned long buffer_len);


//tally.h
#define TSL_UMD_V5_UDP_PORT 8900


extern struct sockaddr_in tsl_umd_v5_adresse;


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

extern struct sockaddr_in sw_p_08_adresse;

extern remote_device_t remote_devices[2];

extern char rs_port_name[16];

extern int number_of_matrix_source;

extern rcp_t *rcp_vision;
extern rcp_t *rcp_pgm;
extern rcp_t *rcp_pvw;

extern gboolean knee_matrix_detail_popup;


void ask_to_connect_pgm_to_ctrl_vision (void);

gboolean rcp_button_press_event (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp);

void init_sw_p_08 (void);

void start_sw_p_08 (void);

gboolean start_sw_p_08_tcp_server (void);

gboolean start_rs_communication (void);

void stop_sw_p_08_tcp_server (void);

void stop_rs_communication (void);


//error.h
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
gboolean Network_communication_Error (rcp_t *rcp);
gboolean CAMERA_communication_Error (rcp_t *rcp);
gboolean CAMERA_RX_Over_run_Error (rcp_t *rcp);
gboolean CAMERA_RX_Framing_Error (rcp_t *rcp);
gboolean CAMERA_RX_Command_Buffer_Overflow (rcp_t *rcp);

gboolean camera_is_unreachable (rcp_t *rcp);

gboolean ABB_execution_failed (rcp_t *rcp);
gboolean ABB_execution_failed_busy_status (rcp_t *rcp);

void start_error_log (void);

void stop_error_log (void);


//rcp.h
#define MARGIN_VALUE 3


extern char gamma_type_tooltip[];
extern char gamma_tooltip[];
extern char drs_tooltip[];


void create_ghost_rcp_widgets (rcp_t *rcp);

void create_rcp_widgets (rcp_t *rcp);

GtkWidget *create_gamma_frame (rcp_t *rcp);

GtkWidget *create_color_temperature_frame (rcp_t *rcp);

GtkWidget *create_knee_frame (rcp_t *rcp);
void set_knee_point_label (rcp_t *rcp);
void set_knee_slope_label (rcp_t *rcp);

GtkWidget *create_matrix_frame (rcp_t *rcp);
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

extern void (*set_cc_saturation_label_funcs[15]) (rcp_t *rcp);
extern void (*set_cc_phase_label_funcs[15]) (rcp_t *rcp);

extern char master_detail_tooltip[];
extern char v_detail_level_tooltip[];
extern char detail_band_tooltip[];
extern char noise_suppress_tooltip[];
extern char fleshtone_noisesup_tooltip[];

GtkWidget *create_detail_frame (rcp_t *rcp);
void set_master_detail_label (rcp_t *rcp);
void set_v_detail_level_label (rcp_t *rcp);
void set_detail_band_label (rcp_t *rcp);
void set_noise_suppress_label (rcp_t *rcp);
void set_fleshtone_noisesup_label (rcp_t *rcp);

GtkWidget *create_saturation_frame (rcp_t *rcp);
void set_saturation_label (rcp_t *rcp);

GtkWidget *create_white_frame (rcp_t *rcp);

GtkWidget *create_black_frame (rcp_t *rcp);

void populate_shutter_step_combo_box (GtkComboBoxText *combo_box);

GtkWidget *create_shutter_frame (rcp_t *rcp);
void set_shutter_synchro_label (rcp_t *rcp);

GtkWidget *create_pedestal_frame (rcp_t *rcp);
void set_pedestal_label (rcp_t *rcp);

GtkWidget *create_iris_frame (rcp_t *rcp);


#endif

