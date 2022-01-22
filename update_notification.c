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

#include <unistd.h>


SOCKET update_notification_socket;
struct sockaddr_in update_notification_address;

gboolean update_notification_started = FALSE;

GThread *update_notification_thread;


#define UPDATE_NOTIFICATION_BEGIN \
	g_mutex_lock (&cameras_sets_mutex); \
	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) { \
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) { \
			rcp = cameras_set_itr->rcp_ptr_array[i]; \
 \
			if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) { \
				g_mutex_lock (&rcp->cmd_mutex); \
				if (rcp->last_ctrl_cmd_len == 0) memcmp_rcp_last_ctrl_cmd = 1; \
				else memcmp_rcp_last_ctrl_cmd = memcmp (buffer + 30, rcp->last_ctrl_cmd, rcp->last_ctrl_cmd_len); \
 \
				if (memcmp_rcp_last_ctrl_cmd == 0) { \
					rcp->last_ctrl_cmd[0] = '\0'; \
					rcp->last_ctrl_cmd_len = 0; \
					g_mutex_unlock (&rcp->cmd_mutex); \
				} else { \
					g_mutex_unlock (&rcp->cmd_mutex);

#define UPDATE_NOTIFICATION_BEGIN_SCALE(s) \
	g_mutex_lock (&cameras_sets_mutex); \
	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) { \
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) { \
			rcp = cameras_set_itr->rcp_ptr_array[i]; \
 \
			if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) { \
				g_mutex_lock (&rcp->cmd_mutex); \
				if (rcp->last_ctrl_cmd_len == 0) memcmp_rcp_last_ctrl_cmd = 1; \
				else memcmp_rcp_last_ctrl_cmd = memcmp (buffer + 30, rcp->last_ctrl_cmd, rcp->last_ctrl_cmd_len); \
 \
				if (memcmp_rcp_last_ctrl_cmd == 0) { \
					rcp->last_ctrl_cmd[0] = '\0'; \
					rcp->last_ctrl_cmd_len = 0; \
					g_mutex_unlock (&rcp->cmd_mutex); \
				} else if (memcmp (buffer + 30, rcp->last_ctrl_cmd, s) == 0) { \
					g_mutex_unlock (&rcp->cmd_mutex); \
				} else { \
					g_mutex_unlock (&rcp->cmd_mutex);

#define UPDATE_NOTIFICATION_END \
				} \
 \
				break; \
			} \
		} \
	} \
	g_mutex_unlock (&cameras_sets_mutex);

#define PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(p) \
					if ((rcp == rcp_vision) && physical_rcp.connected) { \
						g_mutex_lock (&physical_rcp.mutex); \
 \
						physical_rcp.p = rcp->current_scene.p; \
 \
						physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); \
 \
						if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) { \
							send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0); \
						} else physical_rcp.connected = FALSE; \
 \
						closesocket (physical_rcp.socket_for_update_notification); \
 \
						g_mutex_unlock (&physical_rcp.mutex); \
					} \
				} \
 \
				break; \
			} \
		} \
	} \
	g_mutex_unlock (&cameras_sets_mutex);

#define PHYSICAL_RCP_R_GAIN_UE_150_AND_UPDATE_NOTIFICATION_END \
					if ((rcp == rcp_vision) && physical_rcp.connected) { \
						g_mutex_lock (&physical_rcp.mutex); \
 \
						physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75; \
 \
						physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); \
 \
						if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) { \
							send_r_gain_update_notification (); \
						} else physical_rcp.connected = FALSE; \
 \
						closesocket (physical_rcp.socket_for_update_notification); \
 \
						g_mutex_unlock (&physical_rcp.mutex); \
					} \
				} \
 \
				break; \
			} \
		} \
	} \
	g_mutex_unlock (&cameras_sets_mutex);

#define PHYSICAL_RCP_B_GAIN_UE_150_AND_UPDATE_NOTIFICATION_END \
					if ((rcp == rcp_vision) && physical_rcp.connected) { \
						g_mutex_lock (&physical_rcp.mutex); \
 \
						physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75; \
 \
						physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); \
 \
						if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) { \
							send_b_gain_update_notification (); \
						} else physical_rcp.connected = FALSE; \
 \
						closesocket (physical_rcp.socket_for_update_notification); \
 \
						g_mutex_unlock (&physical_rcp.mutex); \
					} \
				} \
 \
				break; \
			} \
		} \
	} \
	g_mutex_unlock (&cameras_sets_mutex);

#define PHYSICAL_RCP_SHUTTER_UE_150_AND_UPDATE_NOTIFICATION_END \
					if ((rcp == rcp_vision) && physical_rcp.connected) { \
						g_mutex_lock (&physical_rcp.mutex); \
 \
						physical_rcp.shutter_type = rcp->current_scene.shutter_type; \
						if (rcp->current_scene.shutter_step == 60) physical_rcp.shutter_step = 0; \
						else if (rcp->current_scene.shutter_step == 100) physical_rcp.shutter_step = 0; \
						else if (rcp->current_scene.shutter_step == 120) physical_rcp.shutter_step = 1; \
						else if (rcp->current_scene.shutter_step == 250) physical_rcp.shutter_step = 2; \
						else if (rcp->current_scene.shutter_step == 500) physical_rcp.shutter_step = 3; \
						else if (rcp->current_scene.shutter_step == 1000) physical_rcp.shutter_step = 4; \
						else if (rcp->current_scene.shutter_step == 2000) physical_rcp.shutter_step = 5; \
						else if (rcp->current_scene.shutter_step == 4000) physical_rcp.shutter_step = 6; \
						else if (rcp->current_scene.shutter_step == 8000) physical_rcp.shutter_step = 6; \
						else if (rcp->current_scene.shutter_step == 10000) physical_rcp.shutter_step = 7; \
						else if (rcp->current_scene.shutter_step == 24) physical_rcp.shutter_step = 8; \
						else if (rcp->current_scene.shutter_step == 25) physical_rcp.shutter_step = 8; \
						else if (rcp->current_scene.shutter_step == 30) physical_rcp.shutter_step = 8; \
						else physical_rcp.shutter_step = 0; \
 \
						physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); \
 \
						if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) { \
							send_shutter_update_notification (); \
						} else physical_rcp.connected = FALSE; \
 \
						closesocket (physical_rcp.socket_for_update_notification); \
 \
						g_mutex_unlock (&physical_rcp.mutex); \
					} \
				} \
 \
				break; \
			} \
		} \
	} \
	g_mutex_unlock (&cameras_sets_mutex);

#define IHM_UPDATE_TOGGLE_BUTTON(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_toggle_button; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_toggle_button, int_widget);

#define IHM_UPDATE_TOGGLE_BUTTON_2(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_toggle_button; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_toggle_button, int_widget);

#define IHM_UPDATE_COMBO_BOX(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_combo_box; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_combo_box, int_widget);

#define IHM_UPDATE_SCALE(l,p) \
	int_widget = g_malloc (sizeof (int_widget_t)); \
	int_widget->widget = rcp->l##_scale; \
	int_widget->handler_id = rcp->l##_handler_id; \
	int_widget->value = &rcp->current_scene.l; \
	int_widget->post_action = p; \
	int_widget->rcp = rcp; \
	g_idle_add ((GSourceFunc)update_scale, int_widget);

#define IHM_UPDATE_LINEAR_MATRIX_SCALE(l,p) \
{ \
	if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4; \
	else data = ((int)(buffer[37] - '7')) << 4; \
	if (buffer[38] <= '9') data += buffer[38] - '0'; \
	else data += buffer[38] - '7'; \
 \
	UPDATE_NOTIFICATION_BEGIN_SCALE(7) \
 \
		rcp->current_scene.linear_matrix.l = data; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->widget = rcp->linear_matrix_##l##_scale; \
		int_widget->handler_id = rcp->linear_matrix_##l##_handler_id; \
		int_widget->value = &rcp->current_scene.linear_matrix.l; \
		int_widget->post_action = p; \
		int_widget->rcp = rcp; \
		g_idle_add ((GSourceFunc)update_scale, int_widget); \
 \
	UPDATE_NOTIFICATION_END \
}

#define IHM_UPDATE_CC_SATURATION(l) \
{ \
	if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4; \
	else data = ((int)(buffer[37] - '7')) << 4; \
	if (buffer[38] <= '9') data += buffer[38] - '0' - 0x80; \
	else data += buffer[38] - '7' - 0x80; \
 \
	UPDATE_NOTIFICATION_BEGIN_SCALE(7) \
 \
		rcp->current_scene.cc_saturation[l] = data; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->post_action = l; \
		int_widget->rcp = rcp; \
		g_idle_add ((GSourceFunc)update_saturation_color_correction_frame, int_widget); \
 \
	UPDATE_NOTIFICATION_END \
}

#define IHM_UPDATE_CC_PHASE(l) \
{ \
	if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4; \
	else data = ((int)(buffer[37] - '7')) << 4; \
	if (buffer[38] <= '9') data += buffer[38] - '0' - 0x80; \
	else data += buffer[38] - '7' - 0x80; \
 \
	UPDATE_NOTIFICATION_BEGIN_SCALE(7) \
 \
		rcp->current_scene.cc_phase[l] = data; \
 \
		int_widget = g_malloc (sizeof (int_widget_t)); \
		int_widget->post_action = l; \
		int_widget->rcp = rcp; \
		g_idle_add ((GSourceFunc)update_phase_color_correction_frame, int_widget); \
 \
	UPDATE_NOTIFICATION_END \
}


gboolean ABB_rcp_work_end (rcp_t *rcp)
{
	if (rcp->root_widget == NULL) {
		g_mutex_clear (&rcp->cmd_mutex);
		g_free (rcp);

		return G_SOURCE_REMOVE;
	}

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

gpointer ask_outputs_format_AW_UE150 (rcp_t *rcp)
{
	char response[8];
#ifdef MAIN_SETTINGS_READ_ONLY
	int i;

	send_cam_request_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].query_cmd, response);

	for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
		if ((response[0] == settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[i][0]) && \
			(response[1] == settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[i][1])) {
			settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150] = i;

			g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150));

			break;
		}
	}

	send_cam_request_command_string (rcp, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].query_cmd, response);

	for (i = 0; i < settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
		if ((response[0] == settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[i][0]) && \
			(response[1] == settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[i][1])) {
			settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150] = i;

			g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_3G_FORMAT_INDEX_AW_UE150));

			break;
		}
	}

	send_cam_request_command_string (rcp, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].query_cmd, response);

	for (i = 0; i < settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
		if ((response[0] == settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[i][0]) && \
			(response[1] == settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[i][1])) {
			settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150] = i;

			g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (MONI_FORMAT_INDEX_AW_UE150));

			break;
		}
	}

	send_cam_request_command_string (rcp, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].query_cmd, response);

	for (i = 0; i < settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
		if ((response[0] == settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[i][0]) && \
			(response[1] == settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[i][1])) {
			settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150] = i;

			g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (HDMI_FORMAT_INDEX_AW_UE150));

			break;
		}
	}
#else
	send_cam_request_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].query_cmd, response);

	if ((response[0] != settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150]][0]) || \
		(response[1] != settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150]][1])) 
		send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150]].value);

	send_cam_request_command_string (rcp, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].query_cmd, response);

	if ((response[0] != settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150]][0]) || \
		(response[1] != settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150]][1])) 
		send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150]].value);

	send_cam_request_command_string (rcp, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].query_cmd, response);

	if ((response[0] != settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150]][0]) || \
		(response[1] != settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150]][1])) 
		send_cam_control_command_string (rcp, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150]].value);

	send_cam_request_command_string (rcp, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].query_cmd, response);

	if ((response[0] != settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150]][0]) || \
		(response[1] != settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150]][1])) 
		send_cam_control_command_string (rcp, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150]].value);
#endif

	return NULL;
}

gpointer ask_format (rcp_t *rcp)
{
	char response[8];
#ifdef MAIN_SETTINGS_READ_ONLY
	int i;
#endif

	usleep (45000000);

	send_update_start_cmd (rcp);

	send_ptz_control_command (rcp, "#LPC1");

#ifdef MAIN_SETTINGS_READ_ONLY
	if (rcp->model == AW_UE150) {
		send_cam_request_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].query_cmd, response);

		for (i = 0; i < settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
			if ((response[0] == settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[i][0]) && \
				(response[1] == settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[i][1])) {
				settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = i;

				update_outputs_parameters_AW_UE150 ();

				g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_UE150));

				ask_outputs_format_AW_UE150 (rcp);

				break;
			}
		}
	} else {
		send_cam_request_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].query_cmd, response);

		for (i = 0; i < settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters; i++) {
			if ((response[0] == settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[i][0]) && \
				(response[1] == settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[i][1])) {
				settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = i;

				if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) {
					if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3)) output_fps_AW_HE130 = fps_50p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 5)) output_fps_AW_HE130 = fps_25p;
				} else {
					if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 4)) output_fps_AW_HE130 = fps_59_94p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 6)) output_fps_AW_HE130 = fps_29_97p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 7)) output_fps_AW_HE130 = fps_23_98p;
					else output_fps_AW_HE130 = fps_59_94i;
				}

				g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_HE130));

				break;
			}
		}
	}
#else
	if (rcp->model == AW_UE150) {
		send_cam_request_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].query_cmd, response);

		if ((response[0] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][0]) || \
			(response[1] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][1])) 
			send_cam_control_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]].value);
	} else {
		send_cam_request_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].query_cmd, response);

		if ((response[0] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][0]) || \
			(response[1] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][1]))
			send_cam_control_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]].value);
	}
#endif

	return NULL;
}

gpointer start_rcp_delayed (rcp_t *rcp)
{
	usleep (2000000);

	start_rcp (rcp);

	return NULL;
}

gpointer receive_update_notification (gpointer nothing)
{
	socklen_t addrlen;
	SOCKET src_socket;
	struct sockaddr_in src_addr;
	char buffer[556];
	int buffer_len;
	cameras_set_t *cameras_set_itr;
	int i;
	rcp_t *rcp;
	GSList *gslist_itr;
	rcp_t *other_rcp;
	int memcmp_rcp_last_ctrl_cmd;
	int data;
	int_widget_t *int_widget;
	time_t current_time;
#ifdef MAIN_SETTINGS_READ_ONLY
	int response;
#endif

	while (update_notification_started) {
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (update_notification_socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) break;

		buffer_len = recv (src_socket, buffer, 556, 0);

		closesocket (src_socket);

		time (&current_time);

		if (buffer[30] == 'd') {
			if (buffer[31] == '1') {	//d1	//Auto focus
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (buffer[32] == '1') rcp->auto_focus = TRUE;
							else rcp->auto_focus = FALSE;

							if ((rcp == rcp_vision) && physical_rcp.connected) {
								g_mutex_lock (&physical_rcp.mutex);

								physical_rcp.auto_focus = rcp->auto_focus;

								physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

								if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
									send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
								} else physical_rcp.connected = FALSE;

								closesocket (physical_rcp.socket_for_update_notification);

								g_mutex_unlock (&physical_rcp.mutex);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if (buffer[31] == '6') {	//d6	//Option switch Day/Night
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if ((buffer[32] == '1') && (rcp->day_night == FALSE)) {
								rcp->day_night = TRUE;
								IHM_UPDATE_TOGGLE_BUTTON_2(day_night,NIGHT_POST_ACTION)
							} else if ((buffer[32] == '0') && (rcp->day_night == TRUE)) {
								rcp->day_night = FALSE;
								IHM_UPDATE_TOGGLE_BUTTON_2(day_night,NIGHT_POST_ACTION)
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
		} else if ((buffer[30] == 'D') && (buffer[31] == 'C')) {
			if ((buffer[32] == 'B') && (buffer[33] == ':')) {	//DCB:	//Color bar/Camera
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if ((buffer[34] == '1') && (rcp->mire == FALSE)) {
								rcp->mire = TRUE;
								IHM_UPDATE_TOGGLE_BUTTON_2(mire,NO_POST_ACTION)
							} else if ((buffer[34] == '0') && (rcp->mire == TRUE)) {
								rcp->mire = FALSE;
								IHM_UPDATE_TOGGLE_BUTTON_2(mire,NO_POST_ACTION)
							}

							if ((rcp == rcp_vision) && physical_rcp.connected) {
								g_mutex_lock (&physical_rcp.mutex);

								physical_rcp.mire = rcp->mire;

								physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

								if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
									send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
								} else physical_rcp.connected = FALSE;

								closesocket (physical_rcp.socket_for_update_notification);

								g_mutex_unlock (&physical_rcp.mutex);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if ((buffer[32] == 'S') && (buffer[33] == ':')) {	//DCS:	//Color bar setup level (Only enabled for AW_HE130)
#ifdef MAIN_SETTINGS_READ_ONLY
				for (i = 0; i < settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].nb_parameters; i++) {
					if (buffer[34] == settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].answers[i][0]) {
						settings_parameters_indexes_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130] = i;

						g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130));

						break;
					}
				}
#else
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (buffer[34] != settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130]][0])
								send_cam_control_command_string (rcp, settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130]].value);

							break;
						}
					}
					if (i < cameras_set_itr->number_of_cameras) break;
				}
				g_mutex_unlock (&cameras_sets_mutex);
#endif
			}
		} else if ((buffer[30] == 'E') && (buffer[31] == 'R')) {
			if ((buffer[32] == '2') && (buffer[33] == ':') && (buffer[34] == 'O') && (buffer[35] == 'A') && (buffer[36] == 'S')) {	//ER2:OAS	//ABB execution failed (busy status)
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							g_idle_add ((GSourceFunc)ABB_execution_failed_busy_status, rcp);
							g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

							if ((rcp == rcp_vision) && physical_rcp.connected) {
								g_mutex_lock (&physical_rcp.mutex);

								physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

								if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
									send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
								} else physical_rcp.connected = FALSE;

								closesocket (physical_rcp.socket_for_update_notification);

								g_mutex_unlock (&physical_rcp.mutex);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if ((buffer[32] == '3') && (buffer[33] == ':') && (buffer[34] == 'O') && (buffer[35] == 'A') && (buffer[36] == 'S')) {	//ER3:OAS	//ABB execution failed
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							g_idle_add ((GSourceFunc)ABB_execution_failed, rcp);
							g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

							if ((rcp == rcp_vision) && physical_rcp.connected) {
								g_mutex_lock (&physical_rcp.mutex);

								physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

								if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
									send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
								} else physical_rcp.connected = FALSE;

								closesocket (physical_rcp.socket_for_update_notification);

								g_mutex_unlock (&physical_rcp.mutex);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
		} else if (buffer[30] == 'l') {
			if ((buffer[31] == 'M') && (buffer[32] == 'P')) {	//lMP	//Status Lamp
#ifdef MAIN_SETTINGS_READ_ONLY
				status_lamp = buffer[33] - 48;
				g_idle_add ((GSourceFunc)update_status_lamp_combo_box_text, NULL);
#else
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if ((buffer[33] - 48) != status_lamp) {
								if (status_lamp == 1) send_ptz_control_command (rcp, "#LMP1");
								else send_ptz_control_command (rcp, "#LMP0");
							}

							break;
						}
					}
					if (i < cameras_set_itr->number_of_cameras) break;
				}
				g_mutex_unlock (&cameras_sets_mutex);
#endif
			} else if ((buffer[31] == 'P') && (buffer[32] == 'I')) { 	//lPI	//Lens information
				if (buffer[39] <= '9') data = ((int)(buffer[39] - '0')) << 8;
				else data = ((int)(buffer[39] - '7')) << 8;
				if (buffer[40] <= '9') data += ((int)(buffer[40] - '0')) << 4;
				else data += ((int)(buffer[40] - '7')) << 4;
				if (buffer[41] <= '9') data += buffer[41] - '0';
				else data += buffer[41] - '7';

				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							g_mutex_lock (&rcp->cmd_mutex);
							if ((rcp->last_ctrl_cmd_len != 0) && ((rcp->last_ctrl_cmd[0] == '#') && (rcp->last_ctrl_cmd[1] == 'A') && (rcp->last_ctrl_cmd[2] == 'X') && (rcp->last_ctrl_cmd[3] == 'I'))) {
								if (data == rcp->current_scene.iris) {
									rcp->last_ctrl_cmd[0] = '\0';
									rcp->last_ctrl_cmd_len = 0;
								}

								g_mutex_unlock (&rcp->cmd_mutex);
							} else {
								g_mutex_unlock (&rcp->cmd_mutex);

								rcp->current_scene.iris = data;
								IHM_UPDATE_SCALE(iris,NO_POST_ACTION)
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
		} else if (buffer[30] == 'O') {
			if (buffer[31] == 'A') {
				if ((buffer[32] == 'F') && (buffer[33] == ':')) {	//OAF	//Focus Auto/Manual
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (buffer[34] == '1') rcp->auto_focus = TRUE;
								else rcp->auto_focus = FALSE;

								if ((rcp == rcp_vision) && physical_rcp.connected) {
									g_mutex_lock (&physical_rcp.mutex);

									physical_rcp.auto_focus = rcp->auto_focus;

									physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

									if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
										send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
									} else physical_rcp.connected = FALSE;

									closesocket (physical_rcp.socket_for_update_notification);

									g_mutex_unlock (&physical_rcp.mutex);
								}

								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);
				} else if (buffer[32] == 'S') {	//OAS	//ABB execution successful
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

								if ((rcp == rcp_vision) && physical_rcp.connected) {
									g_mutex_lock (&physical_rcp.mutex);

									physical_rcp.ABB = FALSE;

									physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

									if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
										send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
									} else physical_rcp.connected = FALSE;

									closesocket (physical_rcp.socket_for_update_notification);

									g_mutex_unlock (&physical_rcp.mutex);
								}

								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);
				} else if ((buffer[32] == 'W') && (buffer[33] == ':')) {	//OAW:	//White Balance Mode
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[WHITE_BALANCE_MODE_INDEX].nb_parameters; i++) {
						if (buffer[34] == settings_array[WHITE_BALANCE_MODE_INDEX].answers[i][0]) {
							settings_parameters_indexes_array[WHITE_BALANCE_MODE_INDEX] = i;

							g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (WHITE_BALANCE_MODE_INDEX));

							break;
						}
					}
#else
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (buffer[34] != settings_array[WHITE_BALANCE_MODE_INDEX].answers[settings_parameters_indexes_array[WHITE_BALANCE_MODE_INDEX]][0])
									send_cam_control_command_string (rcp, settings_array[WHITE_BALANCE_MODE_INDEX].control_cmd, settings_array[WHITE_BALANCE_MODE_INDEX].parameters[settings_parameters_indexes_array[WHITE_BALANCE_MODE_INDEX]].value);

								break;
							}
						}
						if (i < cameras_set_itr->number_of_cameras) break;
					}
					g_mutex_unlock (&cameras_sets_mutex);
#endif
				}
			} else if (buffer[31] == 'B') {
				if ((buffer[32] == 'I') && (buffer[33] == ':')) {	//OBI:	//B gain
					if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
					else data = ((int)(buffer[34] - '7')) << 8;
					if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
					else data += ((int)(buffer[35] - '7')) << 4;
					if (buffer[36] <= '9') data += buffer[36] - '0';
					else data += buffer[36] - '7';

					UPDATE_NOTIFICATION_BEGIN_SCALE(4)

						rcp->current_scene.b_gain = data;
						IHM_UPDATE_SCALE(b_gain,NO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(b_gain)
				} else if ((buffer[32] == 'P') && (buffer[33] == ':')) {	//OPB:	//B pedestal
					if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
					else data = ((int)(buffer[34] - '7')) << 8;
					if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
					else data += ((int)(buffer[35] - '7')) << 4;
					if (buffer[36] <= '9') data += buffer[36] - '0';
					else data += buffer[36] - '7';

					UPDATE_NOTIFICATION_BEGIN_SCALE(4)

						rcp->current_scene.b_pedestal = data;
						IHM_UPDATE_SCALE(b_pedestal,NO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(b_pedestal)
				}
			} else if ((buffer[31] == 'D') && (buffer[32] == 'T') && (buffer[33] == ':')) {	//ODT:	//Detail
				UPDATE_NOTIFICATION_BEGIN

					if (buffer[34] == '0') rcp->current_scene.detail = FALSE;
					else rcp->current_scene.detail = TRUE;
					IHM_UPDATE_TOGGLE_BUTTON(detail,DETAIL_POST_ACTION)

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(detail)
			} else if ((buffer[31] == 'F') && (buffer[32] == 'T') && (buffer[33] == ':')) {	//OFT:	//ND filter
				UPDATE_NOTIFICATION_BEGIN

					rcp->current_scene.ND_filter = buffer[34] - '0';
					g_idle_add ((GSourceFunc)update_ND_filter_combo_box, rcp);

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(ND_filter)
			} else if ((buffer[31] == 'G') && (buffer[32] == 'U') && (buffer[33] == ':')) {	//OGU:	//Gain
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 4;
				else data = ((int)(buffer[34] - '7')) << 4;
				if (buffer[35] <= '9') data += buffer[35] - '0';
				else data += buffer[35] - '7';

				UPDATE_NOTIFICATION_BEGIN

					if ((rcp->model == AW_UE150) && (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1)) {
						if (data == 0x80) rcp->current_scene.gain = 43;
						else {
							rcp->current_scene.gain = 0x32 - data;
							if (data > 0x2C) {
								buffer[34] = '2';
								buffer[35] = 'C';
							}
						}
					} else {
						if (data == 0x80) rcp->current_scene.gain = 37;
						else rcp->current_scene.gain = 0x2C - data;
					}
					IHM_UPDATE_COMBO_BOX(gain,NO_POST_ACTION)

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(gain)
			} else if ((buffer[31] == 'M') && (buffer[32] == 'S') && (buffer[33] == ':')) {	//OMS:	//Synchro scan (AW-HE130)
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
				else data = ((int)(buffer[34] - '7')) << 8;
				if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
				else data += ((int)(buffer[35] - '7')) << 4;
				if (buffer[36] <= '9') data += buffer[36] - '0';
				else data += buffer[36] - '7';

				UPDATE_NOTIFICATION_BEGIN

					rcp->current_scene.shutter_synchro = data;
					IHM_UPDATE_SCALE(shutter_synchro,SHUTTER_SYNCHRO_POST_ACTION_AW_HE130)

				UPDATE_NOTIFICATION_END
			} else if (buffer[31] == 'R') {
				if ((buffer[32] == 'I') && (buffer[33] == ':')) {	//ORI:	//R gain
					if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
					else data = ((int)(buffer[34] - '7')) << 8;
					if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
					else data += ((int)(buffer[35] - '7')) << 4;
					if (buffer[36] <= '9') data += buffer[36] - '0';
					else data += buffer[36] - '7';

					UPDATE_NOTIFICATION_BEGIN_SCALE(4)

						rcp->current_scene.r_gain = data;
						IHM_UPDATE_SCALE(r_gain,NO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(r_gain)
				} else if ((buffer[32] == 'P') && (buffer[33] == ':')) {	//ORP:	//R pedestal
					if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
					else data = ((int)(buffer[34] - '7')) << 8;
					if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
					else data += ((int)(buffer[35] - '7')) << 4;
					if (buffer[36] <= '9') data += buffer[36] - '0';
					else data += buffer[36] - '7';

					UPDATE_NOTIFICATION_BEGIN_SCALE(4)

						rcp->current_scene.r_pedestal = data;
						IHM_UPDATE_SCALE(r_pedestal,NO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(r_pedestal)
				} else if ((buffer[32] == 'S') && (buffer[33] == ':')) {	//ORS:	//Iris Auto/Manual
					UPDATE_NOTIFICATION_BEGIN

						if (buffer[34] == '1') rcp->current_scene.iris_auto = TRUE;
						else rcp->current_scene.iris_auto = FALSE;
						IHM_UPDATE_TOGGLE_BUTTON(iris_auto,IRIS_AUTO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(iris_auto)
				}
			} else if (buffer[31] == 'S') {
				if ((buffer[32] == 'A') && (buffer[33] == ':')) {
					if (buffer[34] == '0') {
						if ((buffer[35] == '7') && (buffer[36] == ':')) {	//OSA:07:	//Black Gamma
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.black_gamma = data;
								IHM_UPDATE_SCALE(black_gamma,BLACK_GAMMA_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'F') && (buffer[36] == ':')) {	//OSA:0F:	//F-REC black STR. Level
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.film_rec_black_stretch_level = data;
								IHM_UPDATE_SCALE(film_rec_black_stretch_level,FILM_REC_BLACK_STRETCH_LEVEL_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '1') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSA:10:	//F-REC Dynamic Level
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.film_rec_dynamic_level = 4 - (buffer[37] - '0');
								IHM_UPDATE_COMBO_BOX(film_rec_dynamic_level,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '2') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSA:20:	//Knee Point
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.knee_point = data;
								IHM_UPDATE_SCALE(knee_point,KNEE_POINT_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSA:21: //V-REC Knee Point
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.video_rec_knee_point = data;
								IHM_UPDATE_SCALE(video_rec_knee_point,VIDEO_REC_KNEE_POINT_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '4') && (buffer[36] == ':')) {	//OSA:24:	//Knee Slope
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.knee_slope = data;
								IHM_UPDATE_SCALE(knee_slope,KNEE_SLOPE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '5') && (buffer[36] == ':')) {	//OSA:25:	//V-REC Knee Slope
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.video_rec_knee_slope = 0x83 - data;
								IHM_UPDATE_COMBO_BOX(video_rec_knee_slope,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//OSA:2A:	//White Clip Level
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[WHITE_CLIP_LEVEL_INDEX].nb_parameters; i++) {
								if ((buffer[37] == settings_array[WHITE_CLIP_LEVEL_INDEX].answers[i][0]) && (buffer[38] == settings_array[WHITE_CLIP_LEVEL_INDEX].answers[i][1])) {
									settings_parameters_indexes_array[WHITE_CLIP_LEVEL_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (WHITE_CLIP_LEVEL_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if ((buffer[37] != settings_array[WHITE_CLIP_LEVEL_INDEX].answers[settings_parameters_indexes_array[WHITE_CLIP_LEVEL_INDEX]][0]) || \
											(buffer[38] != settings_array[WHITE_CLIP_LEVEL_INDEX].answers[settings_parameters_indexes_array[WHITE_CLIP_LEVEL_INDEX]][1]))
											send_cam_control_command_string (rcp, settings_array[WHITE_CLIP_LEVEL_INDEX].control_cmd, settings_array[WHITE_CLIP_LEVEL_INDEX].parameters[settings_parameters_indexes_array[WHITE_CLIP_LEVEL_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'D') && (buffer[36] == ':')) {	//OSA:2D:	//Knee settings
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.knee_settings = buffer[37] - '0';
								IHM_UPDATE_COMBO_BOX(knee_settings,KNEE_SETTINGS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'E') && (buffer[36] == ':')) {	//OSA:2E:	//White Clip settings
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[WHITE_CLIP_INDEX].nb_parameters; i++) {
								if (buffer[37] == settings_array[WHITE_CLIP_INDEX].answers[i][0]) {
									settings_parameters_indexes_array[WHITE_CLIP_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (WHITE_CLIP_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array[WHITE_CLIP_INDEX].answers[settings_parameters_indexes_array[WHITE_CLIP_INDEX]][0])
											send_cam_control_command_string (rcp, settings_array[WHITE_CLIP_INDEX].control_cmd, settings_array[WHITE_CLIP_INDEX].parameters[settings_parameters_indexes_array[WHITE_CLIP_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if (buffer[34] == '3') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSA:30:	//TOTAL DTL LEVEL
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.master_detail = data;
								IHM_UPDATE_SCALE(master_detail,MASTER_DETAIL_POST_ACTION)

							PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(master_detail)
						} else if ((buffer[35] == '8') && (buffer[36] == ':')) {	//OSA:38:	//DETAIL (+)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.detail_gain_plus = data;
								IHM_UPDATE_SCALE(detail_gain_plus,DETAIL_GAIN_PLUS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '9') && (buffer[36] == ':')) {	//OSA:39:	//DETAIL (-)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.detail_gain_minus = data;
								IHM_UPDATE_SCALE(detail_gain_minus,DETAIL_GAIN_MINUS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if ((buffer[34] == '4') && (buffer[35] == '0') && (buffer[36] == ':')) {	//OSA:40:	//SKIN TONE DETAIL
						UPDATE_NOTIFICATION_BEGIN

							if (buffer[37] == '0') rcp->current_scene.skin_detail = FALSE;
							else rcp->current_scene.skin_detail = TRUE;
							IHM_UPDATE_TOGGLE_BUTTON(skin_detail,SKIN_DETAIL_POST_ACTION)

						UPDATE_NOTIFICATION_END
					} else if (buffer[34] == '6') {
						if ((buffer[35] == '5') && (buffer[36] == ':')) {	//OSA:65:	//Frame mix
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[FRAME_MIX_INDEX].nb_parameters; i++) {
								if ((buffer[37] == settings_array[FRAME_MIX_INDEX].answers[i][0]) && (buffer[38] == settings_array[FRAME_MIX_INDEX].answers[i][1])) {
									settings_parameters_indexes_array[FRAME_MIX_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (FRAME_MIX_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if ((buffer[37] != settings_array[FRAME_MIX_INDEX].answers[settings_parameters_indexes_array[FRAME_MIX_INDEX]][0]) || \
											(buffer[38] != settings_array[FRAME_MIX_INDEX].answers[settings_parameters_indexes_array[FRAME_MIX_INDEX]][1]))
											send_cam_control_command_string (rcp, settings_array[FRAME_MIX_INDEX].control_cmd, settings_array[FRAME_MIX_INDEX].parameters[settings_parameters_indexes_array[FRAME_MIX_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//OSA:6A:	//Gamma
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.gamma = 0x94 - data;
								IHM_UPDATE_COMBO_BOX(gamma,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '8') {
						if ((buffer[35] == '7') && (buffer[36] == ':')) {	//OSA:87:	//Format
#ifdef MAIN_SETTINGS_READ_ONLY
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											if (buffer[37] >= '1') {
												for (i = 0; i < settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
													if (settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[i][0] == buffer[37]) {
														if (buffer[38] == settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[i][1]) {
															settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = i;

															g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_UE150));

															g_thread_new (NULL, (GThreadFunc)ask_outputs_format_AW_UE150, rcp);

															break;
														}
													}
												}
											} else {
												for (i = 0; i < settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
													if (buffer[37] == settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[i][0]) {
														settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = i;

														g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_UE150));

														g_thread_new (NULL, (GThreadFunc)ask_outputs_format_AW_UE150, rcp);

														break;
													}
												}
											}
										} else {
											if (buffer[37] == '1') {
												for (i = 0; i < settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters; i++) {
													if (settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[i][0] == '1') {
														if (buffer[38] == settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[i][1]) {
															settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = i;

															g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_HE130));

															break;
														}
													}
												}
											} else {
												for (i = 0; i < settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters; i++) {
													if (buffer[37] == settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[i][0]) {
														settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = i;

														g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_HE130));

														break;
													}
												}
											}
										}

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											if ((buffer[37] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][0]) || \
												(buffer[38] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][1])) {
													send_cam_control_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]].value);
											}
										} else {
											if ((buffer[37] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][0]) || \
												(buffer[38] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][1])) {
													send_cam_control_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]].value);
											}
										}

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '8') && (buffer[36] == ':')) {	//OSA:88:	//OSD Status
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[OSD_STATUS_INDEX].nb_parameters; i++) {
								if (buffer[37] == settings_array[OSD_STATUS_INDEX].answers[i][0]) {
									settings_parameters_indexes_array[OSD_STATUS_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (OSD_STATUS_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array[OSD_STATUS_INDEX].answers[settings_parameters_indexes_array[OSD_STATUS_INDEX]][0])
											send_cam_control_command_string (rcp, settings_array[OSD_STATUS_INDEX].control_cmd, settings_array[OSD_STATUS_INDEX].parameters[settings_parameters_indexes_array[OSD_STATUS_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if ((buffer[34] == 'D') && (buffer[35] == '3') && (buffer[36] == ':')) {	//OSA:D3:	//Tally Brightness settings
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[TALLY_BRIGHTNESS_INDEX].nb_parameters; i++) {
							if (buffer[37] == settings_array[TALLY_BRIGHTNESS_INDEX].answers[i][0]) {
								settings_parameters_indexes_array[TALLY_BRIGHTNESS_INDEX] = i;

								g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (TALLY_BRIGHTNESS_INDEX));

								break;
							}
						}
#else
						g_mutex_lock (&cameras_sets_mutex);
						for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
							for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
								rcp = cameras_set_itr->rcp_ptr_array[i];

								if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
									if (buffer[37] != settings_array[TALLY_BRIGHTNESS_INDEX].answers[settings_parameters_indexes_array[TALLY_BRIGHTNESS_INDEX]][0])
										send_cam_control_command_string (rcp, settings_array[TALLY_BRIGHTNESS_INDEX].control_cmd, settings_array[TALLY_BRIGHTNESS_INDEX].parameters[settings_parameters_indexes_array[TALLY_BRIGHTNESS_INDEX]].value);

									break;
								}
							}
							if (i < cameras_set_itr->number_of_cameras) break;
						}
						g_mutex_unlock (&cameras_sets_mutex);
#endif
					}
				} else if ((buffer[32] == 'D') && (buffer[33] == ':')) {
					if ((buffer[34] == '2') && (buffer[35] == '2') && (buffer[36] == ':')) {	//OSD:22:	//NOISE SUPPRESS/CRISP
						if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
						else data = ((int)(buffer[37] - '7')) << 4;
						if (buffer[38] <= '9') data += buffer[38] - '0';
						else data += buffer[38] - '7';

						UPDATE_NOTIFICATION_BEGIN_SCALE(7)

							rcp->current_scene.noise_suppress = data;
							IHM_UPDATE_SCALE(noise_suppress,NOISE_SUPPRESS_POST_ACTION)

						UPDATE_NOTIFICATION_END
					} else if ((buffer[34] == '3') && (buffer[35] == 'A') && (buffer[36] == ':') && (buffer[37] == '0')) {	//OSD:3A:	//Digital noise reduction
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[DNR_INDEX].nb_parameters; i++) {
							if (buffer[38] == settings_array[DNR_INDEX].answers[i][1]) {
								settings_parameters_indexes_array[DNR_INDEX] = i;

								g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (DNR_INDEX));

								break;
							}
						}
#else
						g_mutex_lock (&cameras_sets_mutex);
						for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
							for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
								rcp = cameras_set_itr->rcp_ptr_array[i];

								if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
									if (buffer[38] != settings_array[DNR_INDEX].answers[settings_parameters_indexes_array[DNR_INDEX]][1])
										send_cam_control_command_string (rcp, settings_array[DNR_INDEX].control_cmd, settings_array[DNR_INDEX].parameters[settings_parameters_indexes_array[DNR_INDEX]].value);

									break;
								}
							}
							if (i < cameras_set_itr->number_of_cameras) break;
						}
						g_mutex_unlock (&cameras_sets_mutex);
#endif
					} else if ((buffer[34] == '4') && (buffer[35] == '8') && (buffer[36] == ':')) {	//OSD:48:	//Picture level Iris offset
						if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
						else data = ((int)(buffer[37] - '7')) << 4;
						if (buffer[38] <= '9') data += buffer[38] - '0';
						else data += buffer[38] - '7';
#ifdef MAIN_SETTINGS_READ_ONLY
						picture_level = data - 0x32;
						g_idle_add ((GSourceFunc)update_picture_level_scale, NULL);
#else
						g_mutex_lock (&cameras_sets_mutex);
						for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
							for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
								rcp = cameras_set_itr->rcp_ptr_array[i];

								if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
									if (picture_level != (data - 0x32)) send_cam_control_command_2_digits (rcp, "OSD:48:", picture_level + 0x32, TRUE);

									break;
								}
							}
							if (i < cameras_set_itr->number_of_cameras) break;
						}
						g_mutex_unlock (&cameras_sets_mutex);
#endif
					} else if ((buffer[34] == '6') && (buffer[35] == '9') && (buffer[36] == ':') && (buffer[37] == '0')) {	//OSD:69:	//AGC maximum gain value
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[AGC_MAX_GAIN_INDEX].nb_parameters; i++) {
							if (buffer[38] == settings_array[AGC_MAX_GAIN_INDEX].answers[i][1]) {
								settings_parameters_indexes_array[AGC_MAX_GAIN_INDEX] = i;

								g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (AGC_MAX_GAIN_INDEX));

								break;
							}
						}
#else
						g_mutex_lock (&cameras_sets_mutex);
						for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
							for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
								rcp = cameras_set_itr->rcp_ptr_array[i];

								if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
									if (buffer[38] != settings_array[AGC_MAX_GAIN_INDEX].answers[settings_parameters_indexes_array[AGC_MAX_GAIN_INDEX]][1])
										send_cam_control_command_string (rcp, settings_array[AGC_MAX_GAIN_INDEX].control_cmd, settings_array[AGC_MAX_GAIN_INDEX].parameters[settings_parameters_indexes_array[AGC_MAX_GAIN_INDEX]].value);

									break;
								}
							}
							if (i < cameras_set_itr->number_of_cameras) break;
						}
						g_mutex_unlock (&cameras_sets_mutex);
#endif
					} else if (buffer[34] == '8') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(B_Mg)	//OSD:80:
						else if ((buffer[35] == '1') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(B_Mg)
						else if ((buffer[35] == '2') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Mg)
						else if ((buffer[35] == '3') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Mg)
						else if ((buffer[35] == '4') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Mg_R)
						else if ((buffer[35] == '5') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Mg_R)
						else if ((buffer[35] == '6') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(R)
						else if ((buffer[35] == '7') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(R)
						else if ((buffer[35] == '8') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(R_Yl)
						else if ((buffer[35] == '9') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(R_Yl)
						else if ((buffer[35] == 'A') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Yl)
						else if ((buffer[35] == 'B') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Yl)
						else if ((buffer[35] == 'C') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Yl_G)
						else if ((buffer[35] == 'D') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Yl_G)
						else if ((buffer[35] == 'E') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(G)
						else if ((buffer[35] == 'F') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(G)
					} else if (buffer[34] == '9') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(G_Cy)	//OSD:90:
						else if ((buffer[35] == '1') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(G_Cy)
						else if ((buffer[35] == '2') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Cy)
						else if ((buffer[35] == '3') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Cy)
						else if ((buffer[35] == '4') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Cy_B)
						else if ((buffer[35] == '5') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Cy_B)
						else if ((buffer[35] == '6') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(B)
						else if ((buffer[35] == '7') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(B)
						else if ((buffer[35] == 'A') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(Mg_R_R)
						else if ((buffer[35] == 'B') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(Mg_R_R)
						else if ((buffer[35] == 'C') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(R_R_Yl)
						else if ((buffer[35] == 'D') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(R_R_Yl)
						else if ((buffer[35] == 'E') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(R_Yl_Yl)
						else if ((buffer[35] == 'F') && (buffer[36] == ':')) IHM_UPDATE_CC_PHASE(R_Yl_Yl)
					} else if (buffer[34] == 'A') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSD:A1:	//V DTL LEVEL
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.v_detail_level = data;
								IHM_UPDATE_SCALE(v_detail_level,V_DETAIL_LEVEL_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSD:A2:	//DETAIL BAND
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.detail_band = data;
								IHM_UPDATE_SCALE(detail_band,DETAIL_BAND_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//OSD:A3:	//FLESH TONE NOISE SUPPRESS
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.fleshtone_noisesup = data;
								IHM_UPDATE_SCALE(fleshtone_noisesup,FLESHTONE_NOISESUP_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '4') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_G,LINEAR_MATRIX_R_G_POST_ACTION)	//OSD:A4:	//Linear matrix R-G
						else if ((buffer[35] == '5') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_B,LINEAR_MATRIX_R_B_POST_ACTION)		//OSD:A5:	//Linear matrix R-B
						else if ((buffer[35] == '6') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_R,LINEAR_MATRIX_G_R_POST_ACTION)		//OSD:A6:	//Linear matrix G-R
						else if ((buffer[35] == '7') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_B,LINEAR_MATRIX_G_B_POST_ACTION)		//OSD:A7:	//Linear matrix G-B
						else if ((buffer[35] == '8') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_R,LINEAR_MATRIX_B_R_POST_ACTION)		//OSD:A8:	//Linear matrix B-R
						else if ((buffer[35] == '9') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_G,LINEAR_MATRIX_B_G_POST_ACTION)		//OSD:A9:	//Linear matrix B-G
					} else if (buffer[34] == 'B') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSD:B0:	//Chroma level
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								if (data == 0x00) rcp->current_scene.saturation = 0x1C;
								else rcp->current_scene.saturation = data;
								IHM_UPDATE_SCALE(saturation,SATURATION_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSD:B1:	//Color Temperature (AW-HE130)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 8;
							else data = ((int)(buffer[37] - '7')) << 8;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 4;
							else data += ((int)(buffer[38] - '7')) << 4;
							if (buffer[39] <= '9') data += buffer[39] - '0';
							else data += buffer[39] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.color_temperature = 0x078 - data;
								IHM_UPDATE_COMBO_BOX(color_temperature,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'F') && (buffer[36] == ':')) {	//OSD:BF:	//Auto shutter limit/ELC Limit
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.ELC_limit = buffer[37] - '2';
								IHM_UPDATE_COMBO_BOX(ELC_limit,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					}
				} else if ((buffer[32] == 'E') && (buffer[33] == ':')) {
					if ((buffer[34] == '2') && (buffer[35] == '0') && (buffer[36] == ':')) {	//OSE:20:	//Down-conversion mode (Not supported by AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130].nb_parameters; i++) {
							if (buffer[37] == settings_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130].answers[i][0]) {
								settings_parameters_indexes_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130] = i;

								g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (DOWN_CONVERSION_MODE_INDEX_AW_HE130));

								break;
							}
						}
#else
						g_mutex_lock (&cameras_sets_mutex);
						for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
							for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
								rcp = cameras_set_itr->rcp_ptr_array[i];

								if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
									if (buffer[37] != settings_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130]][0])
										send_cam_control_command_string (rcp, settings_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[DOWN_CONVERSION_MODE_INDEX_AW_HE130]].value);

									break;
								}
							}
							if (i < cameras_set_itr->number_of_cameras) break;
						}
						g_mutex_unlock (&cameras_sets_mutex);
#endif
					} else if (buffer[34] == '3') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSE:31:	//Color matrix
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.matrix_type = buffer[37] - '0';
								if (rcp->model == AW_UE150) {
									IHM_UPDATE_COMBO_BOX(matrix_type,NO_POST_ACTION)
								} else {
									IHM_UPDATE_COMBO_BOX(matrix_type,MATRIX_TYPE_POST_ACTION)
								}

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//OSE:33:	//DRS
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.drs = 3 - (buffer[37] - '0');
								IHM_UPDATE_COMBO_BOX(drs,DRS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '6') {
						} else if ((buffer[35] == '8') && (buffer[36] == ':')) {	//OSE:68:	//HDMI color component (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (HDMI_VIDEO_SAMPLING_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[HDMI_VIDEO_SAMPLING_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
					} else if (buffer[34] == '7') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSE:71:	//Preset playback range
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[PRESET_SCOPE_INDEX].nb_parameters; i++) {
								if (buffer[37] == settings_array[PRESET_SCOPE_INDEX].answers[i][0]) {
									settings_parameters_indexes_array[PRESET_SCOPE_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (PRESET_SCOPE_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array[PRESET_SCOPE_INDEX].answers[settings_parameters_indexes_array[PRESET_SCOPE_INDEX]][0])
											send_cam_control_command_string (rcp, settings_array[PRESET_SCOPE_INDEX].control_cmd, settings_array[PRESET_SCOPE_INDEX].parameters[settings_parameters_indexes_array[PRESET_SCOPE_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSE:72:	//Gamma type
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.gamma_type = buffer[37] - '0';
								g_idle_add ((GSourceFunc)update_gamma_type_combo_box, rcp);

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '5') && (buffer[36] == ':')) {	//OSE:75:	//OSD Off With TALLY
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[OSD_OFF_WITH_TALLY_INDEX].nb_parameters; i++) {
								if (buffer[37] == settings_array[OSD_OFF_WITH_TALLY_INDEX].answers[i][0]) {
									settings_parameters_indexes_array[OSD_OFF_WITH_TALLY_INDEX] = i;

									g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (OSD_OFF_WITH_TALLY_INDEX));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array[OSD_OFF_WITH_TALLY_INDEX].answers[settings_parameters_indexes_array[OSD_OFF_WITH_TALLY_INDEX]][0])
											send_cam_control_command_string (rcp, settings_array[OSD_OFF_WITH_TALLY_INDEX].control_cmd, settings_array[OSD_OFF_WITH_TALLY_INDEX].parameters[settings_parameters_indexes_array[OSD_OFF_WITH_TALLY_INDEX]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '7') && (buffer[36] == ':')) {	//OSE:77:	//Frequency
#ifdef MAIN_SETTINGS_READ_ONLY
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											for (i = 0; i < settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].nb_parameters; i++) {
												if (buffer[37] == settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].answers[i][0]) {
													settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150] = i;

													switch (i) {
														case 0:	//59,94Hz
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 8;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_ntsc_AW_UE150;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_ntsc_AW_UE150;
															break;
														case 1:	//50Hz
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 7;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_pal_AW_UE150;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_pal_AW_UE150;
															break;
														case 2:	//24Hz
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 2;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_24p_AW_UE150;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_24p_AW_UE150;
															break;
														case 3:	//23.98Hz
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 3;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_23_98p_AW_UE150;
															settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_23_98p_AW_UE150;
													}

													g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FREQUENCY_INDEX_AW_UE150));

													g_thread_new (NULL, (GThreadFunc)ask_format, rcp);

													break;
												}
											}

										} else {
											for (i = 0; i < settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].nb_parameters; i++) {
												if (buffer[37] == settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].answers[i][0]) {
													settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] = i;

													if (i == 1) {	//50Hz
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 6;
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_pal_AW_HE130;
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_pal_AW_HE130;
													} else {		//59,94Hz	//23,98Hz
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 8;
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_ntsc_AW_HE130;
														settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_ntsc_AW_HE130;
													}

													g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FREQUENCY_INDEX_AW_HE130));

													g_thread_new (NULL, (GThreadFunc)ask_format, rcp);

													break;
												}
											}
										}

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											if (buffer[37] != settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]][0]) {
												send_cam_control_command_string (rcp, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]].value);

												g_thread_new (NULL, (GThreadFunc)ask_format, rcp);
											}
										} else {
											if (buffer[37] != settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130]][0]) {
												send_cam_control_command_string (rcp, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130]].value);

												g_thread_new (NULL, (GThreadFunc)ask_format, rcp);
											}
										}

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'B') && (buffer[36] == ':')) {	//OSE:7B:	//OSD Mix
#ifdef MAIN_SETTINGS_READ_ONLY
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											if (buffer[37] <= '9') osd_mix_AW_UE150 = ((int)(buffer[37] - '0')) << 4;
											else osd_mix_AW_UE150 = ((int)(buffer[37] - '7')) << 4;
											if (buffer[38] <= '9') osd_mix_AW_UE150 += buffer[38] - '0';
											else osd_mix_AW_UE150 += buffer[38] - '7';

											g_idle_add ((GSourceFunc)update_osd_mix_AW_UE150, NULL);
										} else {
											if (buffer[37] <= '9') osd_mix_AW_HE130 = ((int)(buffer[37] - '0')) << 4;
											else osd_mix_AW_HE130 = ((int)(buffer[37] - '7')) << 4;
											if (buffer[38] <= '9') osd_mix_AW_HE130 += buffer[38] - '0';
											else osd_mix_AW_HE130 += buffer[38] - '7';

											g_idle_add ((GSourceFunc)update_osd_mix_AW_HE130, NULL);
										}

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#else
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (rcp->model == AW_UE150) {
											if (data != osd_mix_AW_UE150) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_UE150, TRUE);
										} else if (data != osd_mix_AW_HE130) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_HE130, TRUE);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					}
				} else if ((buffer[32] == 'G') && (buffer[33] == ':')) {
					if (buffer[34] == '3') {
						if ((buffer[35] == '9') && (buffer[36] == ':')) {	//OSG:39:	//R gain (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 8;
							else data = ((int)(buffer[37] - '7')) << 8;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 4;
							else data += ((int)(buffer[38] - '7')) << 4;
							if (buffer[39] <= '9') data += buffer[39] - '0';
							else data += buffer[39] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.r_gain = data;
								IHM_UPDATE_SCALE(r_gain,NO_POST_ACTION)

							PHYSICAL_RCP_R_GAIN_UE_150_AND_UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//OSG:3A:	//B gain (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 8;
							else data = ((int)(buffer[37] - '7')) << 8;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 4;
							else data += ((int)(buffer[38] - '7')) << 4;
							if (buffer[39] <= '9') data += buffer[39] - '0';
							else data += buffer[39] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.b_gain = data;
								IHM_UPDATE_SCALE(b_gain,NO_POST_ACTION)

							PHYSICAL_RCP_B_GAIN_UE_150_AND_UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'F') && (buffer[36] == ':')) {	//OSG:3F:	//KNEE APERTURE LEVEL
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.knee_aperture_level = data;
								IHM_UPDATE_SCALE(knee_aperture_level,KNEE_APERTURE_LEVEL_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if ((buffer[34] == '9') && (buffer[35] == '7') && (buffer[36] == ':')) {	//OSG:97:	//Auto Knee Response
						UPDATE_NOTIFICATION_BEGIN_SCALE(7)

							rcp->current_scene.auto_knee_response = buffer[37] - '0';
							IHM_UPDATE_SCALE(auto_knee_response,AUTO_KNEE_RESPONSE_POST_ACTION)

						UPDATE_NOTIFICATION_END
					}
				} else if ((buffer[32] == 'H') && (buffer[33] == ':')) {	//OSH:	//Shutter type & step (AW-HE130)
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_mutex_lock (&rcp->cmd_mutex);

								if (rcp->last_ctrl_cmd_len == 0) memcmp_rcp_last_ctrl_cmd = 1;
								else memcmp_rcp_last_ctrl_cmd = memcmp (buffer + 30, rcp->last_ctrl_cmd, rcp->last_ctrl_cmd_len);

								if (memcmp_rcp_last_ctrl_cmd == 0) {
									rcp->last_ctrl_cmd[0] = '\0';
									rcp->last_ctrl_cmd_len = 0;

									g_mutex_unlock (&rcp->cmd_mutex);
								} else {
									g_mutex_unlock (&rcp->cmd_mutex);

									if (buffer[34] == '0') {
										rcp->current_scene.shutter_type = 0;
										rcp->current_scene.shutter_step = -1;
									} else if (buffer[34] == 'B') {
										rcp->current_scene.shutter_type = 2;
										rcp->current_scene.shutter_step = -1;
									} else if (buffer[34] == 'C') {
										rcp->current_scene.shutter_type = 3;
										rcp->current_scene.shutter_step = -1;
									} else {
										rcp->current_scene.shutter_type = 1;

										if (buffer[34] == '2') rcp->current_scene.shutter_step = 0;
										else if (buffer[34] == '3') {
											if ((output_fps_AW_HE130 == fps_59_94p) || (output_fps_AW_HE130 == fps_59_94i)) rcp->current_scene.shutter_step = 0;
											else rcp->current_scene.shutter_step = 1;
										} else if (buffer[34] == '4') rcp->current_scene.shutter_step = 1;
										else if (buffer[34] == '5') rcp->current_scene.shutter_step = 2;
										else if (buffer[34] == '6') rcp->current_scene.shutter_step = 3;
										else if (buffer[34] == '7') rcp->current_scene.shutter_step = 4;
										else if (buffer[34] == '8') rcp->current_scene.shutter_step = 5;
										else if (buffer[34] == '9') rcp->current_scene.shutter_step = 6;
										else if (buffer[34] == 'A') rcp->current_scene.shutter_step = 7;
										else rcp->current_scene.shutter_step = 8;
									}
									IHM_UPDATE_COMBO_BOX(shutter_type,SHUTTER_TYPE_POST_ACTION_AW_HE130)

									if ((rcp == rcp_vision) && physical_rcp.connected) {
										g_mutex_lock (&physical_rcp.mutex);

										physical_rcp.shutter_type = rcp->current_scene.shutter_type;
										physical_rcp.shutter_step = rcp->current_scene.shutter_step;

										physical_rcp.socket_for_update_notification = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

										if (connect (physical_rcp.socket_for_update_notification, (struct sockaddr *)&physical_rcp.address_for_update_notification, sizeof (struct sockaddr_in)) == 0) {
											send (physical_rcp.socket_for_update_notification, buffer, buffer_len, 0);
										} else physical_rcp.connected = FALSE;

										closesocket (physical_rcp.socket_for_update_notification);

										g_mutex_unlock (&physical_rcp.mutex);
									}
								}

								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);
				} else if ((buffer[32] == 'I') && (buffer[33] == ':')) {
					if (buffer[34] == '2') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSI:20:	//Color temperature (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 16;
							else data = ((int)(buffer[37] - '7')) << 16;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 12;
							else data += ((int)(buffer[38] - '7')) << 12;
							if (buffer[39] <= '9') data += ((int)(buffer[39] - '0')) << 8;
							else data += ((int)(buffer[39] - '7')) << 8;
							if (buffer[40] <= '9') data += ((int)(buffer[40] - '0')) << 4;
							else data += ((int)(buffer[40] - '7')) << 4;
							if (buffer[41] <= '9') data += buffer[41] - '0';
							else data += buffer[41] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.color_temperature = find_color_temperature_index (data);
								IHM_UPDATE_COMBO_BOX(color_temperature,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '8') && (buffer[36] == ':')) {	//OSI:28:	//Super Gain (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SUPER_GAIN_INDEX_AW_UE150));

									break;
								}
							}

							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if ((rcp->model == AW_UE150) && rcp->active) {
											g_idle_add ((GSourceFunc)populate_gain_combo_box_AW_UE150, rcp->gain_combo_box);

											if (rcp->camera_is_on) {
												send_cam_request_command (rcp, "QGU", &response);

												if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) {
													if (response == 0x80) rcp->current_scene.gain = 43;
													else rcp->current_scene.gain = 0x32 - response;
												} else {
													if (response == 0x80) rcp->current_scene.gain = 37;
													else rcp->current_scene.gain = 0x2C - response;
												}

												IHM_UPDATE_COMBO_BOX(gain,NO_POST_ACTION)
											}
										}
									}
								}
							}
							g_mutex_unlock (&cameras_sets_mutex);
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '9') && (buffer[36] == ':')) {	//OSI:29:	//3G SDI Out [LevelA, LevelB] (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_3G_3G_SDI_OUT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_3G_3G_SDI_OUT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if (buffer[34] == '3') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSI:30:	//Shooting Mode (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SHOOTING_MODE_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SHOOTING_MODE_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if (buffer[34] == '4') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSI:40:	//HLG Knee
							UPDATE_NOTIFICATION_BEGIN

								if (buffer[37] == '1') rcp->current_scene.HLG_knee = TRUE;
								else rcp->current_scene.HLG_knee = FALSE;
								IHM_UPDATE_TOGGLE_BUTTON(HLG_knee,HLG_KNEE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSI:41:	//HLG Knee Point
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';

							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.HLG_knee_point = data;
								IHM_UPDATE_SCALE(HLG_knee_point,HLG_KNEE_POINT_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSI:42:	//HLG Knee Slope
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.HLG_knee_slope = data;
								IHM_UPDATE_SCALE(HLG_knee_slope,HLG_KNEE_SLOPE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					}
				} else if ((buffer[32] == 'J') && (buffer[33] == ':')) {
					if (buffer[34] == '0') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSJ:01:	//Auto Iris Speed (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (AUTO_IRIS_SPEED_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_SPEED_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSJ:02:	//Auto Iris Window (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (AUTO_IRIS_WINDOW_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[AUTO_IRIS_WINDOW_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//OSJ:03:	//Shutter Type (AW-UE150)
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.shutter_type = buffer[37] - '0';
								IHM_UPDATE_COMBO_BOX(shutter_type,SHUTTER_TYPE_POST_ACTION_AW_UE150)

							PHYSICAL_RCP_SHUTTER_UE_150_AND_UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '6') && (buffer[36] == ':')) {	//OSJ:06:	//Shutter Step (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 12;
							else data = ((int)(buffer[37] - '7')) << 12;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 8;
							else data += ((int)(buffer[38] - '7')) << 8;
							if (buffer[39] <= '9') data += ((int)(buffer[39] - '0')) << 4;
							else data += ((int)(buffer[39] - '7')) << 4;
							if (buffer[40] <= '9') data += buffer[40] - '0';
							else data += buffer[40] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.shutter_step = data;
								IHM_UPDATE_COMBO_BOX(shutter_step,SHUTTER_STEP_POST_ACTION_AW_UE150)

							PHYSICAL_RCP_SHUTTER_UE_150_AND_UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '9') && (buffer[36] == ':')) {	//OSJ:09:	//Shutter Synchro (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 16;
							else data = ((int)(buffer[37] - '7')) << 16;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 12;
							else data += ((int)(buffer[38] - '7')) << 12;
							if (buffer[39] <= '9') data += ((int)(buffer[39] - '0')) << 8;
							else data += ((int)(buffer[39] - '7')) << 8;
							if (buffer[40] <= '9') data += ((int)(buffer[40] - '0')) << 4;
							else data += ((int)(buffer[40] - '7')) << 4;
							if (buffer[41] <= '9') data += buffer[41] - '0';
							else data += buffer[41] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.shutter_synchro = data;
								IHM_UPDATE_SCALE(shutter_synchro,SHUTTER_SYNCHRO_POST_ACTION_AW_UE150)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'B') && (buffer[36] == ':')) {	//OSJ:0B:	//Chroma Phase
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.chroma_phase = data;
								IHM_UPDATE_SCALE(chroma_phase,CHROMA_PHASE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'F') && (buffer[36] == ':')) {	//OSJ:0F:	//Master pedestal (AW-UE150)
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 8;
							else data = ((int)(buffer[37] - '7')) << 8;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 4;
							else data += ((int)(buffer[38] - '7')) << 4;
							if (buffer[39] <= '9') data += buffer[39] - '0';
							else data += buffer[39] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.pedestal = data;
								IHM_UPDATE_SCALE(pedestal,PEDESTAL_POST_ACTION_AW_UE150)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '1') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSJ:10:	//G pedestal
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 8;
							else data = ((int)(buffer[37] - '7')) << 8;
							if (buffer[38] <= '9') data += ((int)(buffer[38] - '0')) << 4;
							else data += ((int)(buffer[38] - '7')) << 4;
							if (buffer[39] <= '9') data += buffer[39] - '0';
							else data += buffer[39] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.g_pedestal = data;
								IHM_UPDATE_SCALE(g_pedestal,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSJ:11:	//Pedestal offset (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (PEDESTAL_OFFSET_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[PEDESTAL_OFFSET_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSJ:12:	//Detail coring
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.detail_coring = data;
								IHM_UPDATE_SCALE(detail_coring,DETAIL_CORING_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//OSJ:13:	//Level depend
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.level_depend = data;
								IHM_UPDATE_SCALE(level_depend,LEVEL_DEPEND_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'B') && (buffer[36] == ':')) {	//OSJ:1B:	//Black Gamma Range
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.black_gamma_range = 3 - (buffer[37] - '0');
								IHM_UPDATE_COMBO_BOX(black_gamma_range,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'E') && (buffer[36] == ':')) {	//OSJ:1E:	//12G SDI/Fiber Out Output Format (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'F') && (buffer[36] == ':')) {	//OSJ:1F:	//12G SDI/Fiber Out HDR Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_HDR_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if (buffer[34] == '2') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//OSJ:20:	//12G SDI/Fiber Out/3G SDI Out [LevelA, LevelB] (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_3G_SDI_OUT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//OSJ:21:	//3G SDI Out Output Format (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_3G_FORMAT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//OSJ:22:	//3G SDI Out HDR Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_3G_HDR_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//OSJ:23:	//MONI Out Output Format (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (MONI_FORMAT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '4') && (buffer[36] == ':')) {	//OSJ:24:	//MONI Out HDR Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[MONI_HDR_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '5') && (buffer[36] == ':')) {	//OSJ:25:	//HDMI Out Output Format (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (HDMI_FORMAT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '6') && (buffer[36] == ':')) {	//OSJ:26:	//HDMI Out HDR Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[HDMI_HDR_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if ((buffer[34] == '4') && (buffer[35] == 'F') && (buffer[36] == ':')) {	//OSJ:4F:	//Adaptive Matrix
						UPDATE_NOTIFICATION_BEGIN

							if (buffer[37] == '1') rcp->current_scene.adaptive_matrix = TRUE;
							else rcp->current_scene.adaptive_matrix = FALSE;

							int_widget = g_malloc (sizeof (int_widget_t));
							int_widget->widget = rcp->adaptive_matrix_toggle_button_1;
							int_widget->handler_id = rcp->adaptive_matrix_handler_id_1;
							int_widget->value = &rcp->current_scene.adaptive_matrix;
							int_widget->post_action = ADAPTIVE_MATRIX_POST_ACTION;
							int_widget->rcp = rcp;
							g_idle_add ((GSourceFunc)update_toggle_button, int_widget);

						UPDATE_NOTIFICATION_END
					} else if (buffer[34] == '5') {
						if ((buffer[35] == '6') && (buffer[36] == ':')) {	//OSJ:56:	//Color Setting (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (COLOR_SETTING_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[COLOR_SETTING_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '7') && (buffer[36] == ':')) {	//OSJ:57:	//12G SDI/Fiber Out V-Log Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '8') && (buffer[36] == ':')) {	//OSJ:58:	//3G SDI Out V-Log Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[SDI_3G_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == '9') && (buffer[36] == ':')) {	//OSJ:59:	//MONI Out V-Log Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[MONI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//OSJ:5A:	//HDMI Out V-Log Output Select (AW-UE150)
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].nb_parameters; i++) {
								if (buffer[37] == settings_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[i][0]) {
									settings_parameters_indexes_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150] = i;

									g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150));

									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]][0])
											send_cam_control_command_string (rcp, settings_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[HDMI_V_LOG_OUTPUT_SELECT_INDEX_AW_UE150]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					}
				}
			} else if ((buffer[31] == 'T') && (buffer[32] == 'P') && (buffer[33] == ':')) {	//OTP:	//Pedestal (AW-HE130)
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
				else data = ((int)(buffer[34] - '7')) << 8;
				if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
				else data += ((int)(buffer[35] - '7')) << 4;
				if (buffer[36] <= '9') data += buffer[36] - '0';
				else data += buffer[36] - '7';

				UPDATE_NOTIFICATION_BEGIN_SCALE(4)

					rcp->current_scene.pedestal = data;
					IHM_UPDATE_SCALE(pedestal,PEDESTAL_POST_ACTION_AW_HE130)

				UPDATE_NOTIFICATION_END
			}
		} else if (buffer[30] == 'p') {
			if (buffer[31] == '0') {	//p0	//Standby
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							rcp->ip_tally_is_on = FALSE;
							if ((rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR) && (rcp->error_code != 0x00)) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
							if (rcp->camera_is_on) g_idle_add ((GSourceFunc)set_rcp_off, rcp);

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if (buffer[31] == '1') {	//p1	//Power On
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (!rcp->camera_is_on) {
								rcp->camera_is_on = TRUE;

								g_idle_add ((GSourceFunc)rcp_start_working, rcp);

								g_mutex_lock (&rcp->other_rcp_mutex);
								for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
									other_rcp = (rcp_t*)(gslist_itr->data);

									if (other_rcp->thread != NULL) break;
								}
								g_mutex_unlock (&rcp->other_rcp_mutex);

								if ((rcp->thread == NULL) && (gslist_itr == NULL)) rcp->thread = g_thread_new (NULL, (GThreadFunc)start_rcp_delayed, rcp);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if (buffer[31] == '3') {	//p3	//Transferring from Standby to On
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (!rcp->camera_is_on) {
								rcp->camera_is_on = TRUE;

								g_idle_add ((GSourceFunc)rcp_start_working, rcp);

								g_mutex_lock (&rcp->other_rcp_mutex);
								for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
									other_rcp = (rcp_t*)(gslist_itr->data);

									if (other_rcp->thread != NULL) break;
								}
								g_mutex_unlock (&rcp->other_rcp_mutex);

								if ((rcp->thread == NULL) && (gslist_itr == NULL)) rcp->thread = g_thread_new (NULL, (GThreadFunc)start_rcp, rcp);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
		} else if ((buffer[30] == 'q') && (buffer[31] == 'S') && (buffer[32] == 'V')) {	//qSV	//Version information
/*			g_mutex_lock (&cameras_sets_mutex);
			for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
				for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
					rcp = cameras_set_itr->rcp_ptr_array[i];

					if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
						rcp->last_version_information_notification_time = current_time;

						break;
					}
				}
			}
			g_mutex_unlock (&cameras_sets_mutex);*/
		} else if ((buffer[30] == 'r') && (buffer[31] == 'E') && (buffer[32] == 'R')) {	//rER[Error Code]	//Error information
			if (buffer[33] == '0') {
				if (buffer[34] == '0') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)clear_rcp_error, rcp); //Normal
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_clear_rcp_error (rcp);
				} else if ((buffer[34] == '1') || (buffer[34] == '2')) {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_clear_rcp_error (rcp);
				} else if (buffer[34] == '3') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Motor_Driver_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Motor_Driver_Error (rcp);
				} else if (buffer[34] == '4') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Pan_Sensor_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Pan_Sensor_Error (rcp);
				} else if (buffer[34] == '5') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Tilt_Sensor_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Tilt_Sensor_Error (rcp);
				} else if (buffer[34] == '6') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Controller_RX_Over_run_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Controller_RX_Over_run_Error (rcp);
				} else if (buffer[34] == '7') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Controller_RX_Framing_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Controller_RX_Framing_Error (rcp);
				} else if (buffer[34] == '8') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Network_RX_Over_run_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Network_RX_Over_run_Error (rcp);
				} else if (buffer[34] == '9') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Network_RX_Framing_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Network_RX_Framing_Error (rcp);
				} else if ((buffer[34] == 'A') || (buffer[34] == 'B')) {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_clear_rcp_error (rcp);
				}
			} else if (buffer[33] == '1') {
				if (buffer[34] == '7') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Controller_RX_Command_Buffer_Overflow, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Controller_RX_Command_Buffer_Overflow (rcp);
				} else if (buffer[34] == '9') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Network_RX_Command_Buffer_Overflow, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Network_RX_Command_Buffer_Overflow (rcp);
				}
			} else if (buffer[33] == '2') {
				if (buffer[34] == '1') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)System_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_System_Error (rcp);
				} else if (buffer[34] == '2') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Spec_Limit_Over, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Spec_Limit_Over (rcp);
				} else if (buffer[34] == '3') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)FPGA_Config_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_FPGA_Config_Error (rcp);
				} else if (buffer[34] == '4') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (rcp->model == AW_UE150) g_idle_add ((GSourceFunc)NET_Life_monitoring_Error, rcp);
								else g_idle_add ((GSourceFunc)Network_communication_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) {
						if (rcp->model == AW_UE150) log_NET_Life_monitoring_Error (rcp);
						else log_Network_communication_Error (rcp);
					}
				} else if (buffer[34] == '5') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (rcp->model == AW_UE150) g_idle_add ((GSourceFunc)BE_Life_monitoring_Error, rcp);
								else g_idle_add ((GSourceFunc)CAMERA_communication_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) {
						if (rcp->model == AW_UE150) log_BE_Life_monitoring_Error (rcp);
						else log_CAMERA_communication_Error (rcp);
					}
				} else if (buffer[34] == '6') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (rcp->model == AW_UE150) g_idle_add ((GSourceFunc)IF_BE_UART_Buffer_Overflow, rcp);
								else g_idle_add ((GSourceFunc)CAMERA_RX_Over_run_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) {
						if (rcp->model == AW_UE150) log_IF_BE_UART_Buffer_Overflow (rcp);
						else log_CAMERA_RX_Over_run_Error (rcp);
					}
				} else if (buffer[34] == '7') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (rcp->model == AW_UE150) g_idle_add ((GSourceFunc)IF_BE_UART_Framing_Error, rcp);
								else g_idle_add ((GSourceFunc)CAMERA_RX_Framing_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) {
						if (rcp->model == AW_UE150) log_IF_BE_UART_Framing_Error (rcp);
						else log_CAMERA_RX_Framing_Error (rcp);
					}
				} else if (buffer[34] == '8') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (rcp->model == AW_UE150) g_idle_add ((GSourceFunc)IF_BE_UART_Buffer_Overflow_2, rcp);
								else g_idle_add ((GSourceFunc)CAMERA_RX_Command_Buffer_Overflow, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) {
						if (rcp->model == AW_UE150) log_IF_BE_UART_Buffer_Overflow_2 (rcp);
						else log_CAMERA_RX_Command_Buffer_Overflow (rcp);
					}
				} else if (buffer[34] == '9') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)CAM_Life_monitoring_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_CAM_Life_monitoring_Error (rcp);
				}
			} else if (buffer[33] == '3') {
				if (buffer[34] == '1') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Fan1_error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Fan1_error (rcp);
				} else if (buffer[34] == '2') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Fan2_error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Fan2_error (rcp);
				} else if (buffer[34] == '3') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)High_Temp, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_High_Temp (rcp);
				} else if (buffer[34] == '6') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Low_Temp, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Low_Temp (rcp);
				}
			} else if (buffer[33] == '4') {
				if (buffer[34] == '0') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Temp_Sensor_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Temp_Sensor_Error (rcp);
				} else if (buffer[34] == '1') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Lens_Initialize_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Lens_Initialize_Error (rcp);
				} else if (buffer[34] == '2') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)PT_Initialize_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_PT_Initialize_Error (rcp);
				}
			} else if (buffer[33] == '5') {
				if (buffer[34] == '0') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)MR_Level_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_MR_Level_Error (rcp);
				} else if (buffer[34] == '2') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)MR_Offset_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_MR_Offset_Error (rcp);
				} else if (buffer[34] == '3') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Origin_Offset_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Origin_Offset_Error (rcp);
				} else if (buffer[34] == '4') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Angle_MR_Sensor_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Angle_MR_Sensor_Error (rcp);
				} else if (buffer[34] == '5') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)PT_Gear_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_PT_Gear_Error (rcp);
				} else if (buffer[34] == '6') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Motor_Disconnect_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Motor_Disconnect_Error (rcp);
				}
			}
		} else if ((buffer[30] == 't') && (buffer[31] == 'A') && (buffer[32] == 'E')) {	//tAE	//Tally input
#ifdef MAIN_SETTINGS_READ_ONLY
			tally_input = buffer[33] - 48;
			g_idle_add ((GSourceFunc)update_tally_input_combo_box_text, NULL);
#else
			g_mutex_lock (&cameras_sets_mutex);
			for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
				for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
					rcp = cameras_set_itr->rcp_ptr_array[i];

					if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
						if ((buffer[33] - 48) != tally_input) {
							if (tally_input == 1) send_ptz_control_command (rcp, "#TAE1");
							else send_ptz_control_command (rcp, "#TAE0");
						}

						break;
					}
				}
				if (i < cameras_set_itr->number_of_cameras) break;
			}
			g_mutex_unlock (&cameras_sets_mutex);
#endif
		} else if ((buffer[30] == 'X') && (buffer[31] == 'S') && (buffer[32] == 'F') && (buffer[33] == ':')) {	//XSF:	//Scene file
			g_mutex_lock (&cameras_sets_mutex);
			for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
				for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
					rcp = cameras_set_itr->rcp_ptr_array[i];

					if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
						send_cam_request_command (rcp, "QAW", &data);
						if (data != 9) send_cam_control_command (rcp, "OAW:9");

						break;
					}
				}
				if (i < cameras_set_itr->number_of_cameras) break;
			}
			g_mutex_unlock (&cameras_sets_mutex);
		}
	}

	return NULL;
}

void init_update_notification (void)
{
	memset (&update_notification_address, 0, sizeof (struct sockaddr_in));
	update_notification_address.sin_family = AF_INET;
	update_notification_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);
	update_notification_address.sin_addr.s_addr = inet_addr (my_ip_address);
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
		if (((rcp_t*)(glist_itr->data))->error_code != CAMERA_IS_UNREACHABLE_ERROR) send_update_stop_cmd ((rcp_t*)(glist_itr->data));
	}

	update_notification_started = FALSE;

	shutdown (update_notification_socket, SHUT_RD);
	closesocket (update_notification_socket);

	if (update_notification_thread != NULL) {
		g_thread_join (update_notification_thread);
		update_notification_thread = NULL;
	}
}

