/*
 * copyright (c) 2018-2021 Thomas Paillet <thomas.paillet@net-c.fr>

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
					if (physical_rcp.connected && (rcp == rcp_vision)) { \
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
		g_slist_free (rcp->other_rcp);
		g_mutex_clear (&rcp->other_rcp_mutex);
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

gboolean decrease_on_standby_count (cameras_set_t *cameras_set)
{
	cameras_set->on_standby_count--;

	if (cameras_set->on_standby_count == 0) {
		g_signal_handler_block (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
		gtk_switch_set_active (GTK_SWITCH (cameras_set->master_rcp.on_standby_switch), FALSE);
		g_signal_handler_unblock (cameras_set->master_rcp.on_standby_switch, cameras_set->master_rcp.on_standby_handler_id);
	}

	return G_SOURCE_REMOVE;
}


#ifdef MAIN_SETTINGS_READ_ONLY
#define UPDATE_COMBO_BOX_TEXT_FUNC(l,i) \
gboolean update_##l##_combo_box_text (void) \
{ \
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box)); \
 \
	if ((settings_parameters_indexes_array[i] >= 0) || (settings_parameters_indexes_array[i] < settings_array[i].nb_parameters)) { \
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box), settings_array[i].parameters[settings_parameters_indexes_array[i]].label); \
		gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array[i].combo_box), 0); \
	} \
 \
	return G_SOURCE_REMOVE; \
}


UPDATE_COMBO_BOX_TEXT_FUNC(frequency,0)
UPDATE_COMBO_BOX_TEXT_FUNC(format,1)
UPDATE_COMBO_BOX_TEXT_FUNC(down_conversion_mode,2)
UPDATE_COMBO_BOX_TEXT_FUNC(color_bar_setup_level,3)
UPDATE_COMBO_BOX_TEXT_FUNC(preset_scope,4)
UPDATE_COMBO_BOX_TEXT_FUNC(white_balance_mode,5)
UPDATE_COMBO_BOX_TEXT_FUNC(white_clip,6)
UPDATE_COMBO_BOX_TEXT_FUNC(white_clip_level,7)
UPDATE_COMBO_BOX_TEXT_FUNC(agc_max_gain,8)
UPDATE_COMBO_BOX_TEXT_FUNC(frame_mix,9)
UPDATE_COMBO_BOX_TEXT_FUNC(dnr,10)
UPDATE_COMBO_BOX_TEXT_FUNC(tally_brightness,11)
UPDATE_COMBO_BOX_TEXT_FUNC(osd_off_with_tally,12)

gboolean update_format_parameter (void)
{
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array[1].combo_box));
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array[1].combo_box), settings_array[1].parameters[settings_parameters_indexes_array[1]].label);
	gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array[1].combo_box), 0);

	return G_SOURCE_REMOVE;
}
#endif

gpointer ask_format (rcp_t *rcp)
{
	char response[8];
#ifdef MAIN_SETTINGS_READ_ONLY
	int i;
#endif

	usleep (45000000);

	send_update_start_cmd (rcp);

	send_cam_request_command_string (rcp, settings_array[1].query_cmd, response);

#ifdef MAIN_SETTINGS_READ_ONLY
	for (i = 0; i < settings_array[1].nb_parameters; i++) {
		if (strcmp (settings_array[1].answers[i], response) == 0) {
			settings_parameters_indexes_array[1] = i;
			g_idle_add ((GSourceFunc)update_format_parameter, NULL);

			break;
		}
	}
#else
	if (strcmp (settings_array[1].answers[settings_parameters_indexes_array[1]], response) != 0)
		send_cam_control_command_string (rcp, settings_array[1].control_cmd, settings_array[1].parameters[settings_parameters_indexes_array[1]].value);
#endif

	return NULL;
}

gpointer receive_update_notification (gpointer nothing)
{
#ifdef _WIN32
	int addrlen;
#elif defined (__linux)
	socklen_t addrlen;
#endif
	SOCKET src_socket;
	struct sockaddr_in src_addr;
	char buffer[556];
	int buffer_len;
	cameras_set_t *cameras_set_itr;
	int i;
	rcp_t *rcp;
	int memcmp_rcp_last_ctrl_cmd;
	int data;
	int_widget_t *int_widget;
	time_t last_time;
	time_t current_time;

	time (&last_time);

	while (update_notification_started) {
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (update_notification_socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) break;

		buffer_len = recv (src_socket, buffer, 556, 0);

		closesocket (src_socket);

		time (&current_time);

		if (buffer[30] == 'd') {
			if (buffer[31] == '1') {	//Auto focus
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (buffer[32] == '1') rcp->auto_focus = TRUE;
							else rcp->auto_focus = FALSE;

							if (physical_rcp.connected && (rcp == rcp_vision)) {
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
			} else if (buffer[31] == '6') {	//Option switch Day/Night
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
			if ((buffer[32] == 'B') && (buffer[33] == ':')) {	//Color bar/Camera
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

							if (physical_rcp.connected && (rcp == rcp_vision)) {
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
			} else if ((buffer[32] == 'S') && (buffer[33] == ':')) {	//Color bar setup level
#ifdef MAIN_SETTINGS_READ_ONLY
				for (i = 0; i < settings_array[3].nb_parameters; i++) {
					if (buffer[34] == settings_array[3].answers[i][0]) {
						settings_parameters_indexes_array[3] = i;
						g_idle_add ((GSourceFunc)update_color_bar_setup_level_combo_box_text, NULL);
						break;
					}
				}
#else
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (buffer[34] != settings_array[3].answers[settings_parameters_indexes_array[3]][0])
								send_cam_control_command_string (rcp, settings_array[3].control_cmd, settings_array[3].parameters[settings_parameters_indexes_array[3]].value);

							break;
						}
					}
					if (i < cameras_set_itr->number_of_cameras) break;
				}
				g_mutex_unlock (&cameras_sets_mutex);
#endif
			}
		} else if ((buffer[30] == 'E') && (buffer[31] == 'R')) {
			if ((buffer[32] == '3') && (buffer[33] == ':') && (buffer[34] == 'O') && (buffer[35] == 'A') && (buffer[36] == 'S')) {	//ABB execution failed
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							g_idle_add ((GSourceFunc)ABB_execution_failed, rcp);
							g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

							if (physical_rcp.connected && (rcp == rcp_vision)) {
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
			} else if ((buffer[32] == '2') && (buffer[33] == ':') && (buffer[34] == 'O') && (buffer[35] == 'A') && (buffer[36] == 'S')) {	//ABB execution failed (busy status)
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							g_idle_add ((GSourceFunc)ABB_execution_failed_busy_status, rcp);
							g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

							if (physical_rcp.connected && (rcp == rcp_vision)) {
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
		} else if ((buffer[30] == 'l') && (buffer[31] == 'P') && (buffer[32] == 'I')) { 	//Lens information
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
		} else if (buffer[30] == 'O') {
			if (buffer[31] == 'A') {
				if ((buffer[32] == 'F') && (buffer[33] == ':')) {	//Focus Auto/Manual
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (buffer[34] == '1') rcp->auto_focus = TRUE;
								else rcp->auto_focus = FALSE;

								if (physical_rcp.connected && (rcp == rcp_vision)) {
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
				} else if (buffer[32] == 'S') {	//ABB execution successful
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)ABB_rcp_work_end, rcp);

								if (physical_rcp.connected && (rcp == rcp_vision)) {
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
				} else if ((buffer[32] == 'W') && (buffer[33] == ':')) {	//White Balance Mode
#ifdef MAIN_SETTINGS_READ_ONLY
					for (i = 0; i < settings_array[5].nb_parameters; i++) {
						if (buffer[34] == settings_array[5].answers[i][0]) {
							settings_parameters_indexes_array[5] = i;
							g_idle_add ((GSourceFunc)update_white_balance_mode_combo_box_text, NULL);
							break;
						}
					}
#else
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								if (buffer[34] != settings_array[5].answers[settings_parameters_indexes_array[5]][0])
									send_cam_control_command_string (rcp, settings_array[5].control_cmd, settings_array[5].parameters[settings_parameters_indexes_array[5]].value);

								break;
							}
						}
						if (i < cameras_set_itr->number_of_cameras) break;
					}
					g_mutex_unlock (&cameras_sets_mutex);
#endif
				}
			} else if (buffer[31] == 'B') {
				if ((buffer[32] == 'I') && (buffer[33] == ':')) {	//B gain
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
				} else if ((buffer[32] == 'P') && (buffer[33] == ':')) {	//B pedestal
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
			} else if ((buffer[31] == 'D') && (buffer[32] == 'T') && (buffer[33] == ':')) {	//Detail
				UPDATE_NOTIFICATION_BEGIN

					if (buffer[34] == '0') rcp->current_scene.detail = FALSE;
					else rcp->current_scene.detail = TRUE;
					IHM_UPDATE_TOGGLE_BUTTON(detail,DETAIL_POST_ACTION)

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(detail)
			} else if ((buffer[31] == 'F') && (buffer[32] == 'T') && (buffer[33] == ':')) {	//ND filter
				UPDATE_NOTIFICATION_BEGIN

					if (buffer[34] == '3') rcp->current_scene.ND_filter = 2;
					else if (buffer[34] == '4') rcp->current_scene.ND_filter = 1;
					else rcp->current_scene.ND_filter = 0;
					IHM_UPDATE_COMBO_BOX(ND_filter,NO_POST_ACTION)

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(ND_filter)
			} else if ((buffer[31] == 'G') && (buffer[32] == 'U') && (buffer[33] == ':')) {	//Gain
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 4;
				else data = ((int)(buffer[34] - '7')) << 4;
				if (buffer[35] <= '9') data += buffer[35] - '0';
				else data += buffer[35] - '7';

				UPDATE_NOTIFICATION_BEGIN

					if (data == 0x80) rcp->current_scene.gain = 37;
					else rcp->current_scene.gain = 0x2C - data;
					IHM_UPDATE_COMBO_BOX(gain,NO_POST_ACTION)

				PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(gain)
			} else if ((buffer[31] == 'M') && (buffer[32] == 'S') && (buffer[33] == ':')) {	//Synchro scan
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
				else data = ((int)(buffer[34] - '7')) << 8;
				if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
				else data += ((int)(buffer[35] - '7')) << 4;
				if (buffer[36] <= '9') data += buffer[36] - '0';
				else data += buffer[36] - '7';

				UPDATE_NOTIFICATION_BEGIN

					rcp->current_scene.shutter_synchro = data;
					IHM_UPDATE_SCALE(shutter_synchro,SHUTTER_SYNCHRO_POST_ACTION)

				UPDATE_NOTIFICATION_END
			} else if (buffer[31] == 'R') {
				if ((buffer[32] == 'I') && (buffer[33] == ':')) {	//R gain
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
				} else if ((buffer[32] == 'P') && (buffer[33] == ':')) {	//R pedestal
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
				} else if ((buffer[32] == 'S') && (buffer[33] == ':')) {	//Iris Auto/Manual
					UPDATE_NOTIFICATION_BEGIN

						if (buffer[34] == '1') rcp->current_scene.iris_auto = TRUE;
						else rcp->current_scene.iris_auto = FALSE;
						IHM_UPDATE_TOGGLE_BUTTON(iris_auto,IRIS_AUTO_POST_ACTION)

					PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(iris_auto)
				}
			} else if (buffer[31] == 'S') {
				if ((buffer[32] == 'A') && (buffer[33] == ':')) {
					if (buffer[34] == '2') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//Knee Point
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.knee_point = data;
								IHM_UPDATE_SCALE(knee_point,KNEE_POINT_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '4') && (buffer[36] == ':')) {	//Knee Slope
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.knee_slope = data;
								IHM_UPDATE_SCALE(knee_slope,KNEE_SLOPE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//White Clip Level
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[7].nb_parameters; i++) {
								if ((buffer[37] == settings_array[7].answers[i][0]) && (buffer[38] == settings_array[7].answers[i][1])) {
									settings_parameters_indexes_array[7] = i;
									g_idle_add ((GSourceFunc)update_white_clip_level_combo_box_text, NULL);
									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if ((buffer[37] != settings_array[7].answers[settings_parameters_indexes_array[7]][0]) || (buffer[38] != settings_array[7].answers[settings_parameters_indexes_array[7]][1]))
											send_cam_control_command_string (rcp, settings_array[7].control_cmd, settings_array[7].parameters[settings_parameters_indexes_array[7]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'D') && (buffer[36] == ':')) {	//Knee settings
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.knee_settings = buffer[37] - '0';
								IHM_UPDATE_COMBO_BOX(knee_settings,KNEE_SETTINGS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == 'E') && (buffer[36] == ':')) {	//White Clip settings
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[6].nb_parameters; i++) {
								if (buffer[37] == settings_array[6].answers[i][0]) {
									settings_parameters_indexes_array[6] = i;
									g_idle_add ((GSourceFunc)update_white_clip_combo_box_text, NULL);
									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if (buffer[37] != settings_array[6].answers[settings_parameters_indexes_array[6]][0])
											send_cam_control_command_string (rcp, settings_array[6].control_cmd, settings_array[6].parameters[settings_parameters_indexes_array[6]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						}
					} else if ((buffer[34] == '3') && (buffer[35] == '0') && (buffer[36] == ':')) {	//TOTAL DTL LEVEL
						if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
						else data = ((int)(buffer[37] - '7')) << 4;
						if (buffer[38] <= '9') data += buffer[38] - '0';
						else data += buffer[38] - '7';

						UPDATE_NOTIFICATION_BEGIN_SCALE(7)

							rcp->current_scene.master_detail = data;
							IHM_UPDATE_SCALE(master_detail,MASTER_DETAIL_POST_ACTION)

						PHYSICAL_RCP_AND_UPDATE_NOTIFICATION_END(master_detail)
					} else if (buffer[34] == '6') {
						if ((buffer[35] == '5') && (buffer[36] == ':')) {	//Frame mix
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[9].nb_parameters; i++) {
								if ((buffer[37] == settings_array[9].answers[i][0]) && (buffer[38] == settings_array[9].answers[i][1])) {
									settings_parameters_indexes_array[9] = i;
									g_idle_add ((GSourceFunc)update_frame_mix_combo_box_text, NULL);
									break;
								}
							}
#else
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										if ((buffer[37] != settings_array[9].answers[settings_parameters_indexes_array[9]][0]) || (buffer[38] != settings_array[9].answers[settings_parameters_indexes_array[9]][1]))
											send_cam_control_command_string (rcp, settings_array[9].control_cmd, settings_array[9].parameters[settings_parameters_indexes_array[9]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
#endif
						} else if ((buffer[35] == 'A') && (buffer[36] == ':')) {	//Gamma
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.gamma = 0x94 - data;
								IHM_UPDATE_COMBO_BOX(gamma,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if ((buffer[34] == '8') && (buffer[35] == '7') && (buffer[36] == ':')) {	//Format
#ifdef MAIN_SETTINGS_READ_ONLY
						if (buffer[37] == '1') {
							for (i = 0; i < settings_array[1].nb_parameters; i++) {
								if (settings_array[1].answers[i][0] == '1') {
									if (buffer[38] == settings_array[1].answers[i][1]) {
										settings_parameters_indexes_array[1] = i;
										g_idle_add ((GSourceFunc)update_format_combo_box_text, NULL);
										break;
									}
								}
							}
						} else {
							for (i = 0; i < settings_array[1].nb_parameters; i++) {
								if (buffer[37] == settings_array[1].answers[i][0]) {
									settings_parameters_indexes_array[1] = i;
									g_idle_add ((GSourceFunc)update_format_combo_box_text, NULL);
									break;
								}
							}
						}
#else
						if ((buffer[37] != settings_array[1].answers[settings_parameters_indexes_array[1]][0]) || \
							((buffer[37] == '1') && (buffer[38] != settings_array[1].answers[settings_parameters_indexes_array[1]][1]))) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_string (rcp, settings_array[1].control_cmd, settings_array[1].parameters[settings_parameters_indexes_array[1]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					} else if ((buffer[34] == 'D') && (buffer[35] == '3') && (buffer[36] == ':')) {	//Tally Brightness settings
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[11].nb_parameters; i++) {
							if (buffer[37] == settings_array[11].answers[i][0]) {
								settings_parameters_indexes_array[11] = i;
								g_idle_add ((GSourceFunc)update_tally_brightness_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[37] != settings_array[11].answers[settings_parameters_indexes_array[11]][0]) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_string (rcp, settings_array[11].control_cmd, settings_array[11].parameters[settings_parameters_indexes_array[11]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					}
				} else if ((buffer[32] == 'D') && (buffer[33] == ':')) {
					if ((buffer[34] == '2') && (buffer[35] == '2') && (buffer[36] == ':')) {	//NOISE SUPPRESS/CRISP
						if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
						else data = ((int)(buffer[37] - '7')) << 4;
						if (buffer[38] <= '9') data += buffer[38] - '0';
						else data += buffer[38] - '7';

						UPDATE_NOTIFICATION_BEGIN_SCALE(7)

							rcp->current_scene.noise_suppress = data;
							IHM_UPDATE_SCALE(noise_suppress,NOISE_SUPPRESS_POST_ACTION)

						UPDATE_NOTIFICATION_END
					} else if ((buffer[34] == '3') && (buffer[35] == 'A') && (buffer[36] == ':') && (buffer[37] == '0')) {	//Digital noise reduction
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[10].nb_parameters; i++) {
							if (buffer[38] == settings_array[10].answers[i][1]) {
								settings_parameters_indexes_array[10] = i;
								g_idle_add ((GSourceFunc)update_dnr_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[38] != settings_array[10].answers[settings_parameters_indexes_array[10]][1]) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_string (rcp, settings_array[10].control_cmd, settings_array[10].parameters[settings_parameters_indexes_array[10]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					} else if ((buffer[34] == '4') && (buffer[35] == '8') && (buffer[36] == ':')) {	//Picture level Iris offset
						if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
						else data = ((int)(buffer[37] - '7')) << 4;
						if (buffer[38] <= '9') data += buffer[38] - '0';
						else data += buffer[38] - '7';
#ifdef MAIN_SETTINGS_READ_ONLY
						picture_level = data - 0x32;
						g_idle_add ((GSourceFunc)update_picture_level_scale, NULL);
#else
						if (picture_level != (data - 0x32)) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_2_digits (rcp, "OSD:48:", picture_level + 0x32, TRUE);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					} else if ((buffer[34] == '6') && (buffer[35] == '9') && (buffer[36] == ':') && (buffer[37] == '0')) {	//AGC maximum gain value
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[8].nb_parameters; i++) {
							if (buffer[38] == settings_array[8].answers[i][1]) {
								settings_parameters_indexes_array[8] = i;
								g_idle_add ((GSourceFunc)update_agc_max_gain_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[38] != settings_array[8].answers[settings_parameters_indexes_array[8]][1]) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_string (rcp, settings_array[8].control_cmd, settings_array[8].parameters[settings_parameters_indexes_array[8]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					} else if (buffer[34] == '8') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(B_Mg)
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
						if ((buffer[35] == '0') && (buffer[36] == ':')) IHM_UPDATE_CC_SATURATION(G_Cy)
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
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//V DTL LEVEL
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.v_detail_level = data;
								IHM_UPDATE_SCALE(v_detail_level,V_DETAIL_LEVEL_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//DETAIL BAND
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.detail_band = data;
								IHM_UPDATE_SCALE(detail_band,DETAIL_BAND_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//FLESH TONE NOISE SUPPRESS
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								rcp->current_scene.fleshtone_noisesup = data;
								IHM_UPDATE_SCALE(fleshtone_noisesup,FLESHTONE_NOISESUP_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '4') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_G,LINEAR_MATRIX_R_G_POST_ACTION)
						else if ((buffer[35] == '5') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(R_B,LINEAR_MATRIX_R_B_POST_ACTION)
						else if ((buffer[35] == '6') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_R,LINEAR_MATRIX_G_R_POST_ACTION)
						else if ((buffer[35] == '7') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(G_B,LINEAR_MATRIX_G_B_POST_ACTION)
						else if ((buffer[35] == '8') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_R,LINEAR_MATRIX_B_R_POST_ACTION)
						else if ((buffer[35] == '9') && (buffer[36] == ':')) IHM_UPDATE_LINEAR_MATRIX_SCALE(B_G,LINEAR_MATRIX_B_G_POST_ACTION)
					} else if (buffer[34] == 'B') {
						if ((buffer[35] == '0') && (buffer[36] == ':')) {	//Chroma level
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							UPDATE_NOTIFICATION_BEGIN_SCALE(7)

								if (data == 0x00) rcp->current_scene.saturation = 0x1C;
								else rcp->current_scene.saturation = data;
								IHM_UPDATE_SCALE(saturation,SATURATION_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '1') && (buffer[36] == ':')) {	//Color Temperature
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
						}
					}
				} else if ((buffer[32] == 'E') && (buffer[33] == ':')) {
					if ((buffer[34] == '2') && (buffer[35] == '0') && (buffer[36] == ':')) {	//Down-conversion mode
#ifdef MAIN_SETTINGS_READ_ONLY
						for (i = 0; i < settings_array[2].nb_parameters; i++) {
							if (buffer[37] == settings_array[2].answers[i][0]) {
								settings_parameters_indexes_array[2] = i;
								g_idle_add ((GSourceFunc)update_down_conversion_mode_combo_box_text, NULL);
								break;
							}
						}
#else
						if (buffer[37] != settings_array[2].answers[settings_parameters_indexes_array[2]][0]) {
							g_mutex_lock (&cameras_sets_mutex);
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									rcp = cameras_set_itr->rcp_ptr_array[i];

									if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
										send_cam_control_command_string (rcp, settings_array[2].control_cmd, settings_array[2].parameters[settings_parameters_indexes_array[2]].value);

										break;
									}
								}
								if (i < cameras_set_itr->number_of_cameras) break;
							}
							g_mutex_unlock (&cameras_sets_mutex);
						}
#endif
					} else if (buffer[34] == '3') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//Color matrix
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.matrix_type = buffer[37] - '0';
								IHM_UPDATE_COMBO_BOX(matrix_type,MATRIX_TYPE_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '3') && (buffer[36] == ':')) {	//DRS
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.drs = 3 - (buffer[37] - '0');
								IHM_UPDATE_COMBO_BOX(drs,DRS_POST_ACTION)

							UPDATE_NOTIFICATION_END
						}
					} else if (buffer[34] == '7') {
						if ((buffer[35] == '1') && (buffer[36] == ':')) {	//Preset playback range
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[4].nb_parameters; i++) {
								if (buffer[37] == settings_array[4].answers[i][0]) {
									settings_parameters_indexes_array[4] = i;
									g_idle_add ((GSourceFunc)update_preset_scope_combo_box_text, NULL);
									break;
								}
							}
#else
							if (buffer[37] != settings_array[4].answers[settings_parameters_indexes_array[4]][0]) {
								g_mutex_lock (&cameras_sets_mutex);
								for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
									for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
										rcp = cameras_set_itr->rcp_ptr_array[i];

										if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
											send_cam_control_command_string (rcp, settings_array[4].control_cmd, settings_array[4].parameters[settings_parameters_indexes_array[4]].value);

											break;
										}
									}
									if (i < cameras_set_itr->number_of_cameras) break;
								}
								g_mutex_unlock (&cameras_sets_mutex);
							}
#endif
						} else if ((buffer[35] == '2') && (buffer[36] == ':')) {	//Gamma type
							UPDATE_NOTIFICATION_BEGIN

								rcp->current_scene.gamma_type = buffer[37] - '0';
								IHM_UPDATE_COMBO_BOX(gamma_type,NO_POST_ACTION)

							UPDATE_NOTIFICATION_END
						} else if ((buffer[35] == '5') && (buffer[36] == ':')) {	//OSD Off With TALLY
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[12].nb_parameters; i++) {
								if (buffer[37] == settings_array[12].answers[i][0]) {
									settings_parameters_indexes_array[12] = i;
									g_idle_add ((GSourceFunc)update_osd_off_with_tally_combo_box_text, NULL);
									break;
								}
							}
#else
							if (buffer[37] != settings_array[12].answers[settings_parameters_indexes_array[12]][0]) {
								g_mutex_lock (&cameras_sets_mutex);
								for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
									for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
										rcp = cameras_set_itr->rcp_ptr_array[i];

										if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
											send_cam_control_command_string (rcp, settings_array[12].control_cmd, settings_array[12].parameters[settings_parameters_indexes_array[12]].value);

											break;
										}
									}
									if (i < cameras_set_itr->number_of_cameras) break;
								}
								g_mutex_unlock (&cameras_sets_mutex);
							}
#endif
						} else if ((buffer[35] == '7') && (buffer[36] == ':')) {	//Frequency
#ifdef MAIN_SETTINGS_READ_ONLY
							for (i = 0; i < settings_array[0].nb_parameters; i++) {
								if (buffer[37] == settings_array[0].answers[i][0]) {
									settings_parameters_indexes_array[0] = i;

									if (settings_parameters_indexes_array[0] == 1) {
										format_is_50Hz = TRUE;
										settings_array[1].nb_parameters = 6;
										settings_array[1].parameters = format_parameters_array_pal;
										settings_array[1].answers = format_answers_array_pal;
										if ((settings_parameters_indexes_array[1] == 2) || (settings_parameters_indexes_array[1] == 5)) output_fps = _25fps;
										else output_fps = _50fps;
									} else {
										format_is_50Hz = FALSE;
										settings_array[1].nb_parameters = 8;
										settings_array[1].parameters = format_parameters_array_ntsc;
										settings_array[1].answers = format_answers_array_ntsc;
										if ((settings_parameters_indexes_array[1] == 3) || (settings_parameters_indexes_array[1] == 7)) output_fps = _23_98fps;
										else if ((settings_parameters_indexes_array[1] == 2) || (settings_parameters_indexes_array[1] == 6)) output_fps = _29_97fps;
										else output_fps = _59_94fps;
									}

									g_idle_add ((GSourceFunc)update_frequency_combo_box_text, NULL);
									g_thread_new (NULL, (GThreadFunc)ask_format, rcp);
									break;
								}
							}
#else
							if (buffer[37] != settings_array[0].answers[settings_parameters_indexes_array[0]][0]) {
								g_mutex_lock (&cameras_sets_mutex);
								for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
									for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
										rcp = cameras_set_itr->rcp_ptr_array[i];

										if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
											send_cam_control_command_string (rcp, settings_array[0].control_cmd, settings_array[0].parameters[settings_parameters_indexes_array[0]].value);
											g_thread_new (NULL, (GThreadFunc)ask_format, rcp);

											break;
										}
									}
									if (i < cameras_set_itr->number_of_cameras) break;
								}
								g_mutex_unlock (&cameras_sets_mutex);
							}
#endif
						} else if ((buffer[35] == 'B') && (buffer[36] == ':')) {	//OSD Mix
#ifdef MAIN_SETTINGS_READ_ONLY
							if (buffer[37] <= '9') osd_mix = ((int)(buffer[37] - '0')) << 4;
							else osd_mix = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') osd_mix += buffer[38] - '0';
							else osd_mix += buffer[38] - '7';

							g_idle_add ((GSourceFunc)update_osd_mix, NULL);
#else
							if (buffer[37] <= '9') data = ((int)(buffer[37] - '0')) << 4;
							else data = ((int)(buffer[37] - '7')) << 4;
							if (buffer[38] <= '9') data += buffer[38] - '0';
							else data += buffer[38] - '7';

							if (data != osd_mix) {
								g_mutex_lock (&cameras_sets_mutex);
								for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
									for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
										rcp = cameras_set_itr->rcp_ptr_array[i];

										if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
											send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix, TRUE);

											break;
										}
									}
									if (i < cameras_set_itr->number_of_cameras) break;
								}
								g_mutex_unlock (&cameras_sets_mutex);
							}
#endif
						}
					}
				} else if ((buffer[32] == 'H') && (buffer[33] == ':')) {	//Shutter type & step
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

									if (buffer[34] == '0') rcp->current_scene.shutter_type = 0;
									else if (buffer[34] == 'B') rcp->current_scene.shutter_type = 2;
									else if (buffer[34] == 'C') rcp->current_scene.shutter_type = 3;
									else {
										rcp->current_scene.shutter_type = 1;

										if (buffer[34] == '2') rcp->current_scene.shutter_step = 0;
										else if (buffer[34] == '3') {
											if (output_fps == _59_94fps) rcp->current_scene.shutter_step = 0;
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
									IHM_UPDATE_COMBO_BOX(shutter_type,SHUTTER_TYPE_POST_ACTION)

									if (physical_rcp.connected && (rcp == rcp_vision)) {
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
				}
			} else if ((buffer[31] == 'T') && (buffer[32] == 'P') && (buffer[33] == ':')) {	//Pedestal
				if (buffer[34] <= '9') data = ((int)(buffer[34] - '0')) << 8;
				else data = ((int)(buffer[34] - '7')) << 8;
				if (buffer[35] <= '9') data += ((int)(buffer[35] - '0')) << 4;
				else data += ((int)(buffer[35] - '7')) << 4;
				if (buffer[36] <= '9') data += buffer[36] - '0';
				else data += buffer[36] - '7';

				UPDATE_NOTIFICATION_BEGIN_SCALE(4)

					rcp->current_scene.pedestal = data;
					IHM_UPDATE_SCALE(pedestal,PEDESTAL_POST_ACTION)

				UPDATE_NOTIFICATION_END
			}
		} else if (buffer[30] == 'p') {	//Power On/Standby
			if (buffer[31] == '0') {
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							rcp->ip_tally_is_on = FALSE;
							if ((rcp->error_code != 0x30) && (rcp->error_code != 0x00)) g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
							if (rcp->camera_is_on) {
								g_idle_add ((GSourceFunc)set_rcp_off, rcp);
								g_idle_add ((GSourceFunc)decrease_on_standby_count, rcp->camera_set);
							}

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			} else if (buffer[31] == '1') {
				rcp = NULL;
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (!rcp->camera_is_working) g_idle_add ((GSourceFunc)rcp_work_start, rcp);

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);

				if (rcp != NULL) rcp->thread = g_thread_new (NULL, (GThreadFunc)start_rcp, rcp);

#ifdef MAIN_SETTINGS_READ_ONLY
				if (camera_settings_checked == FALSE) {
					camera_settings_checked = TRUE;
					g_thread_new (NULL, (GThreadFunc)check_camera_settings_ro, rcp);
				}
#endif
			} else if (buffer[31] == '3') {
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (!rcp->camera_is_working) g_idle_add ((GSourceFunc)rcp_work_start, rcp);

							break;
						}
					}
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
		} else if ((buffer[30] == 'q') && (buffer[31] == 'S') && (buffer[32] == 'V')) {	//Version information
			g_mutex_lock (&cameras_sets_mutex);
			for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
				for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
					rcp = cameras_set_itr->rcp_ptr_array[i];

					if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
						rcp->last_version_information_notification_time = current_time;

						break;
					}
				}
			}
			g_mutex_unlock (&cameras_sets_mutex);
		} else if ((buffer[30] == 'r') && (buffer[31] == 'E') && (buffer[32] == 'R')) {	//Error information
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
				} else if (buffer[34] == '4') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)Network_communication_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_Network_communication_Error (rcp);
				} else if (buffer[34] == '5') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)CAMERA_communication_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_CAMERA_communication_Error (rcp);
				} else if (buffer[34] == '6') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)CAMERA_RX_Over_run_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_CAMERA_RX_Over_run_Error (rcp);
				} else if (buffer[34] == '7') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)CAMERA_RX_Framing_Error, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_CAMERA_RX_Framing_Error (rcp);
				} else if (buffer[34] == '8') {
					rcp = NULL;
					g_mutex_lock (&cameras_sets_mutex);
					for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
						for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
							rcp = cameras_set_itr->rcp_ptr_array[i];

							if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
								g_idle_add ((GSourceFunc)CAMERA_RX_Command_Buffer_Overflow, rcp);
								break;
							}
						}
					}
					g_mutex_unlock (&cameras_sets_mutex);

					if (rcp != NULL) log_CAMERA_RX_Command_Buffer_Overflow (rcp);
				}
			}
		} else if ((buffer[30] == 't') && (buffer[31] == 'A') && (buffer[32] == 'E')) {	//Tally input
#ifdef MAIN_SETTINGS_READ_ONLY
			tally_input = buffer[33] - 48;
			g_idle_add ((GSourceFunc)update_tally_input_combo_box_text, NULL);
#else
			if ((buffer[33] - 48) != tally_input) {
				g_mutex_lock (&cameras_sets_mutex);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
						rcp = cameras_set_itr->rcp_ptr_array[i];

						if (rcp->address.sin_addr.s_addr == src_addr.sin_addr.s_addr) {
							if (tally_input == 1) send_ptz_control_command (rcp, "#TAE1");
							else send_ptz_control_command (rcp, "#TAE0");

							break;
						}
					}
					if (i < cameras_set_itr->number_of_cameras) break;
				}
				g_mutex_unlock (&cameras_sets_mutex);
			}
#endif
		} else if ((buffer[30] == 'X') && (buffer[31] == 'S') && (buffer[32] == 'F') && (buffer[33] == ':')) {	//Scene file
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

		if (current_time > (last_time + 60)) {
			g_mutex_lock (&cameras_sets_mutex);
			for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
				for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
					rcp = cameras_set_itr->rcp_ptr_array[i];

					if (rcp->last_version_information_notification_time < current_time - 120) {
						if (rcp->camera_is_on) g_idle_add ((GSourceFunc)set_rcp_off, rcp);
					}
				}
			}
			g_mutex_unlock (&cameras_sets_mutex);

			last_time = current_time;
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
		if (((rcp_t*)(glist_itr->data))->error_code != 0x30) send_update_stop_cmd ((rcp_t*)(glist_itr->data));
	}

	update_notification_started = FALSE;

	shutdown (update_notification_socket, SHUT_RD);
	closesocket (update_notification_socket);

	if (update_notification_thread != NULL) {
		g_thread_join (update_notification_thread);
		update_notification_thread = NULL;
	}
}

