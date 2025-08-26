/*
 * copyright (c) 2018-2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "sw_p_08.h"

#include "cameras_set.h"
#include "logging.h"
#include "main_window.h"
#include "misc.h"
#include "operating_system.h"
#include "physical_rcp.h"
#include "protocol.h"
#include "rcp_AW_HE130.h"
#include "rcp_AW_UE150.h"
#include "settings.h"
#include "tally.h"

#include <string.h>


#define DLE 0x10
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15


GMutex sw_p_08_mutex;

GThread *sw_p_08_thread = NULL;

char OK[2] = { DLE, ACK };
char NOK[2] = { DLE, NAK };

char full_sw_p_08_buffer[14] = { DLE, ACK, DLE, STX, 0x04, 0x00, 0x00, 0x00, 0x00, 0x05, -0x09, DLE, ETX, ETX };
char *sw_p_08_buffer = &full_sw_p_08_buffer[2];
int sw_p_08_buffer_len = 11;
int rs_try = 0;

gboolean ip_rs = TRUE;

SOCKET sw_p_08_socket;
struct sockaddr_in sw_p_08_address;

gboolean sw_p_08_tcp_server_started = FALSE;

remote_device_t remote_devices[2];

char rs_port_name[16] = { ' ', '\0' };

rcp_t *rcp_vision = NULL;
rcp_t *rcp_pgm = NULL;
rcp_t *rcp_pvw = NULL;


char tally_camera_set = MAX_CAMERAS + 1;
char tally_rcp = MAX_CAMERAS + 1;
char tally_memory = MAX_CAMERAS + 1;
char tally_pgm = MAX_CAMERAS + 1;
char tally_pvw = MAX_CAMERAS + 1;


gboolean g_source_select_cameras_set_page (gpointer page_num)
{
	cameras_set_t *cameras_set_itr;

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		if (cameras_set_itr->page_num == GPOINTER_TO_INT (page_num)) {
			gtk_notebook_set_current_page (GTK_NOTEBOOK (main_window_notebook), cameras_set_itr->page_num);
			break;
		}
	}

	return G_SOURCE_REMOVE;
}

gboolean g_source_rcp_event_box_set_above_child (rcp_t *rcp)
{
	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
		gtk_widget_unset_state_flags (rcp->iris_plus_10_button, GTK_STATE_FLAG_ACTIVE);
		gtk_widget_unset_state_flags (rcp->iris_plus_1_button, GTK_STATE_FLAG_ACTIVE);
		gtk_widget_unset_state_flags (rcp->iris_minus_1_button, GTK_STATE_FLAG_ACTIVE);
		gtk_widget_unset_state_flags (rcp->iris_minus_10_button, GTK_STATE_FLAG_ACTIVE);
	}

	if (popup_window != NULL) {
		gtk_widget_hide (popup_window);
		popup_window = NULL;
	}

	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);

	return G_SOURCE_REMOVE;
}

gboolean g_source_rcp_event_box_set_below_child (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), FALSE);

	return G_SOURCE_REMOVE;
}

gboolean g_source_recall_memories (gpointer index)
{
	int i;
	rcp_t *rcp;
	int scene_selected = GPOINTER_TO_INT (index);

	if (current_cameras_set != NULL) {
		for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
			rcp = current_cameras_set->cameras[i];

			if (!rcp->camera_is_on) continue;
			if (rcp->camera_is_working) continue;

			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->store_toggle_button), FALSE);

			if ((rcp->scene_selected != -1) && (rcp->scene_selected != scene_selected)) gtk_style_context_remove_provider (gtk_widget_get_style_context (rcp->scenes_button[rcp->scene_selected]), GTK_STYLE_PROVIDER (css_provider_scene_selected));

			if (rcp->scene_selected != scene_selected) {
				rcp->scene_selected = scene_selected;

				gtk_style_context_add_provider (gtk_widget_get_style_context (rcp->scenes_button[scene_selected]), GTK_STYLE_PROVIDER (css_provider_scene_selected), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			}

			rcp_start_working (rcp);

			if (rcp->model == AW_UE150) rcp->thread = g_thread_new (NULL, (GThreadFunc)load_scene_AW_UE150, rcp);
			else rcp->thread = g_thread_new (NULL, (GThreadFunc)load_scene_AW_HE130, rcp);
		}
	}

	return G_SOURCE_REMOVE;
}

void tell_camera_set_is_selected (gint page_num)
{
	tally_camera_set = page_num;

	sw_p_08_buffer[2] = 0x04;		//CROSSPOINT CONNECTED Message
	sw_p_08_buffer[3] = 0;
	sw_p_08_buffer[4] = 0;
	sw_p_08_buffer[5] = 0;			//Destination: "Ensemble de caméras"

	if (tally_camera_set == DLE) {
		sw_p_08_buffer[6] = DLE;	//Source: camera_set->page_num
		sw_p_08_buffer[7] = DLE;
		sw_p_08_buffer[8] = 5;
		sw_p_08_buffer[9] = -25;
		sw_p_08_buffer[10] = DLE;
//		sw_p_08_buffer[11] = ETX;
		sw_p_08_buffer_len = 12;

		if (ip_rs) {
			if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 12, 0);
			if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 12, 0);
		} else if (sw_p_08_thread != NULL) {
			send_to_rs_port (sw_p_08_buffer, 12);
			rs_try = 5;
		}
	} else {
		sw_p_08_buffer[6] = tally_camera_set;	//Source: camera_set->page_num
		sw_p_08_buffer[7] = 5;
		sw_p_08_buffer[8] = -9 - tally_camera_set;
		sw_p_08_buffer[9] = DLE;
		sw_p_08_buffer[10] = ETX;
		sw_p_08_buffer_len = 11;

		if (ip_rs) {
			if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 11, 0);
			if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 11, 0);
		} else if (sw_p_08_thread != NULL) {
			send_to_rs_port (sw_p_08_buffer, 11);
			rs_try = 5;
		}
	}
}

void ask_to_connect_pgm_to_ctrl_vision (void)
{
	g_mutex_lock (&sw_p_08_mutex);

	tally_rcp = MAX_CAMERAS;

	sw_p_08_buffer[2] = 0x04;	//CROSSPOINT CONNECTED Message
	sw_p_08_buffer[3] = 0;
	sw_p_08_buffer[4] = 0;
	sw_p_08_buffer[5] = 1;		//Destination: "RCP" (CTRL VISION)
#if MAX_CAMERAS == DLE
	sw_p_08_buffer[6] = DLE;	//Source: "PGM"
	sw_p_08_buffer[7] = DLE;
	sw_p_08_buffer[8] = 5;
	sw_p_08_buffer[9] = -26;
	sw_p_08_buffer[10] = DLE;
//	sw_p_08_buffer[11] = ETX;
	sw_p_08_buffer_len = 12;

	if (ip_rs) {
		if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 12, 0);
		if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 12, 0);
	} else if (sw_p_08_thread != NULL) {
		send_to_rs_port (sw_p_08_buffer, 12);
		rs_try = 5;
	}
#else
	sw_p_08_buffer[6] = MAX_CAMERAS;	//Source: "PGM"
	sw_p_08_buffer[7] = 5;
	sw_p_08_buffer[8] = -10 - MAX_CAMERAS;
	sw_p_08_buffer[9] = DLE;
	sw_p_08_buffer[10] = ETX;
	sw_p_08_buffer_len = 11;

	if (ip_rs) {
		if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 11, 0);
		if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 11, 0);
	} else if (sw_p_08_thread != NULL) {
		send_to_rs_port (sw_p_08_buffer, 11);
		rs_try = 5;
	}
#endif
	if (rcp_vision != NULL) {
		g_source_rcp_queue_draw (rcp_vision);
		if (rcp_vision->active) g_source_rcp_event_box_set_above_child (rcp_vision);
		rcp_vision = NULL;
	}

	g_mutex_unlock (&sw_p_08_mutex);
}

void ask_to_connect_camera_to_ctrl_vision (rcp_t *rcp)
{
	g_mutex_lock (&sw_p_08_mutex);

	tally_rcp = rcp->index;

	sw_p_08_buffer[2] = 0x04;		//CROSSPOINT CONNECTED Message
	sw_p_08_buffer[3] = 0;
	sw_p_08_buffer[4] = 0;
	sw_p_08_buffer[5] = 1;			//Destination: RCP (CTRL VISION)

	if (tally_rcp == DLE) {
		sw_p_08_buffer[6] = DLE;	//Source: rcp->index
		sw_p_08_buffer[7] = DLE;
		sw_p_08_buffer[8] = 5;
		sw_p_08_buffer[9] = -26;
		sw_p_08_buffer[10] = DLE;
//		sw_p_08_buffer[11] = ETX;
		sw_p_08_buffer_len = 12;

		if (ip_rs) {
			if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 12, 0);
			if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 12, 0);
		} else if (sw_p_08_thread != NULL) {
			send_to_rs_port (sw_p_08_buffer, 12);
			rs_try = 5;
		}
	} else {
		sw_p_08_buffer[6] = tally_rcp;	//Source: rcp->index
		sw_p_08_buffer[7] = 5;
		sw_p_08_buffer[8] = -10 - tally_rcp;
		sw_p_08_buffer[9] = DLE;
		sw_p_08_buffer[10] = ETX;
		sw_p_08_buffer_len = 11;

		if (ip_rs) {
			if (remote_devices[0].src_socket != INVALID_SOCKET) send (remote_devices[0].src_socket, sw_p_08_buffer, 11, 0);
			if (remote_devices[1].src_socket != INVALID_SOCKET) send (remote_devices[1].src_socket, sw_p_08_buffer, 11, 0);
		} else if (sw_p_08_thread != NULL) {
			send_to_rs_port (sw_p_08_buffer, 11);
			rs_try = 5;
		}
	}

	if (rcp_vision != NULL) {
		g_source_rcp_queue_draw (rcp_vision);
		if (rcp_vision->active) g_source_rcp_event_box_set_above_child (rcp_vision);
	}

	rcp_vision = rcp;

	g_source_rcp_queue_draw (rcp);
	if (rcp->active) gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), FALSE);

	g_mutex_unlock (&sw_p_08_mutex);
}

void send_ok_plus_crosspoint_tally_message_to_rs (char dest)
{
	char src;

	if (dest == 0) src = tally_camera_set;
	else if (dest == 1) src = tally_rcp;
	else if (dest == 2) src = tally_memory;
	else if (dest == 3) src = tally_pgm;
	else if (dest == 4) src = tally_pvw;
	else src = MAX_CAMERAS + 1;

	full_sw_p_08_buffer[4] = 0x03;
	full_sw_p_08_buffer[5] = 0x00;
	full_sw_p_08_buffer[6] = 0x00;
	full_sw_p_08_buffer[7] = dest;

	if (src == DLE) {
		full_sw_p_08_buffer[8] = DLE;
		full_sw_p_08_buffer[9] = DLE;
		full_sw_p_08_buffer[10] = 5;
		full_sw_p_08_buffer[11] = -(dest + 24);
		full_sw_p_08_buffer[12] = DLE;
//		full_sw_p_08_buffer[13] = ETX;
		sw_p_08_buffer_len = 12;

		send_to_rs_port (full_sw_p_08_buffer, 14);
		rs_try = 5;
	} else {
		full_sw_p_08_buffer[8] = src;
		full_sw_p_08_buffer[9] = 5;
		full_sw_p_08_buffer[10] = -(dest + src + 8);
		full_sw_p_08_buffer[11] = DLE;
		full_sw_p_08_buffer[12] = ETX;
		sw_p_08_buffer_len = 11;

		send_to_rs_port (full_sw_p_08_buffer, 13);
		rs_try = 5;
	}
}

void send_crosspoint_message_to_rs (char dest, char src)
{
	sw_p_08_buffer[2] = 0x04;
	sw_p_08_buffer[3] = 0x00;
	sw_p_08_buffer[4] = 0x00;
	sw_p_08_buffer[5] = dest;

	if (src == DLE) {
		sw_p_08_buffer[6] = DLE;
		sw_p_08_buffer[7] = DLE;
		sw_p_08_buffer[8] = 5;
		sw_p_08_buffer[9] = -(dest + 25);
		sw_p_08_buffer[10] = DLE;
//		sw_p_08_buffer[11] = ETX;
		sw_p_08_buffer_len = 12;

		send_to_rs_port (full_sw_p_08_buffer, 12);
		rs_try = 5;
	} else {
		sw_p_08_buffer[6] = src;
		sw_p_08_buffer[7] = 5;
		sw_p_08_buffer[8] = -(dest + src + 9);
		sw_p_08_buffer[9] = DLE;
		sw_p_08_buffer[10] = ETX;
		sw_p_08_buffer_len = 11;

		send_to_rs_port (full_sw_p_08_buffer, 11);
		rs_try = 5;
	}
}

gpointer listen_to_rs_port (void)
{
	char buffer[8];
	rcp_t *rcp;

	while (receive_from_rs_port (buffer, 2)) {
		g_mutex_lock (&sw_p_08_mutex);

		if (buffer[0] == DLE) {
			if (buffer[1] == ACK) {
				rs_try = 0;
			} else if (buffer[1] == NAK) {
				if (rs_try-- > 0) send_to_rs_port (sw_p_08_buffer, sw_p_08_buffer_len);
			} else if (buffer[1] == STX) {
				if (!receive_from_rs_port (buffer, 1)) { g_mutex_unlock (&sw_p_08_mutex); break; }

				if (buffer[0] == 0x01) {	//Crosspoint Interrogate Message
					if (!receive_from_rs_port (buffer, 7)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x01 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4]) == 0) {
						send_ok_plus_crosspoint_tally_message_to_rs (buffer[2]);
					} else send_to_rs_port (NOK, 2);
				} else if (buffer[0] == 0x02) {	//Crosspoint Connect Message
					if (!receive_from_rs_port (buffer, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if (buffer[3] == DLE) {
						if (!receive_from_rs_port (buffer + 3, 5)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					} else if (!receive_from_rs_port (buffer + 4, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x02 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5]) == 0) {
						send_to_rs_port (OK, 2);

						if (buffer[2] == 0) {
							if (buffer[3] < number_of_cameras_sets) {
								g_idle_add ((GSourceFunc)g_source_select_cameras_set_page, GINT_TO_POINTER (buffer[3]));

								tally_camera_set = buffer[3];
							} else tally_camera_set = MAX_CAMERAS + 1;
						} else if (buffer[2] == 1) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if ((rcp_vision != NULL) && (rcp_vision != rcp)) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);

								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_above_child, rcp_vision);
							}

							rcp_vision = rcp;

							if (rcp_vision != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);

								if (rcp_vision->active) {
									g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_below_child, rcp_vision);

									if (physical_rcp.connected) g_idle_add ((GSourceFunc)update_physical_rcp, rcp_vision);
								}

								tally_rcp = buffer[3];
							} else tally_rcp = MAX_CAMERAS + 1;
						} else if (buffer[2] == 2) {
							if (buffer[3] < NB_SCENES) {
								g_idle_add ((GSourceFunc)g_source_recall_memories, GINT_TO_POINTER (buffer[3]));

								tally_memory = buffer[3];
							} else tally_memory = MAX_CAMERAS + 1;
						} else if (buffer[2] == 3) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if (rcp_pgm != NULL) {
								if (send_ip_tally && rcp_pgm->ip_tally_is_on && rcp_pgm->camera_is_on) send_tally_off_control_command (rcp_pgm);

								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);
							}

							rcp_pgm = rcp;

							if (rcp_pgm != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);

								if (send_ip_tally && !rcp_pgm->ip_tally_is_on && rcp_pgm->camera_is_on) send_tally_on_control_command (rcp_pgm);

								tally_pgm = buffer[3];
							} else tally_pgm = MAX_CAMERAS + 1;
						} else if (buffer[2] == 4) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if (rcp_pvw != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

							rcp_pvw = rcp;

							if (rcp_pvw != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

								tally_pvw = buffer[3];
							} else tally_pvw = MAX_CAMERAS + 1;
						}

						send_crosspoint_message_to_rs (buffer[2], buffer[3]);
					} else send_to_rs_port (NOK, 2);
				} else {
					do {
						if (buffer[0] == DLE) {
							if (!receive_from_rs_port (buffer, 1)) break;
							if (buffer[0] == ETX) break;
						}
						if (!receive_from_rs_port (buffer, 1)) break;
					} while (TRUE);
				}
			}
		}

		g_mutex_unlock (&sw_p_08_mutex);
	}

	return NULL;
}

gboolean sw_p_08_tcp_remote_device_connect (remote_device_t *remote_device)
{
	char label[32];

	if (remote_device->src_socket != INVALID_SOCKET) {
		gtk_widget_hide (ip_waiting_label);

		sprintf (label, "%s est connecté", inet_ntoa (remote_device->src_addr.sin_addr));
		gtk_label_set_text (GTK_LABEL (remote_device->connected_label), label);
		gtk_widget_show (remote_device->connected_label);
	}

	return G_SOURCE_REMOVE;
}

gboolean sw_p_08_tcp_remote_device_disconnect (remote_device_t *remote_device)
{
	if (remote_device->thread != NULL) {
		g_thread_join (remote_device->thread);
		remote_device->thread = NULL;
	}

	gtk_widget_hide (remote_device->connected_label);
	gtk_widget_show (ip_waiting_label);

	return G_SOURCE_REMOVE;
}

void send_ok_plus_crosspoint_tally_message_to_socket (SOCKET sock, char dest)
{
	char src;

	if (dest == 0) src = tally_camera_set;
	else if (dest == 1) src = tally_rcp;
	else if (dest == 2) src = tally_memory;
	else if (dest == 3) src = tally_pgm;
	else if (dest == 4) src = tally_pvw;
	else src = MAX_CAMERAS + 1;

	full_sw_p_08_buffer[4] = 0x03;
	full_sw_p_08_buffer[5] = 0x00;
	full_sw_p_08_buffer[6] = 0x00;
	full_sw_p_08_buffer[7] = dest;

	if (src == DLE) {
		full_sw_p_08_buffer[8] = DLE;
		full_sw_p_08_buffer[9] = DLE;
		full_sw_p_08_buffer[10] = 5;
		full_sw_p_08_buffer[11] = -(dest + 24);
		full_sw_p_08_buffer[12] = DLE;
//		full_sw_p_08_buffer[13] = ETX;
		sw_p_08_buffer_len = 12;

		send (sock, full_sw_p_08_buffer, 14, 0);
	} else {
		full_sw_p_08_buffer[8] = src;
		full_sw_p_08_buffer[9] = 5;
		full_sw_p_08_buffer[10] = -(dest + src + 8);
		full_sw_p_08_buffer[11] = DLE;
		full_sw_p_08_buffer[12] = ETX;
		sw_p_08_buffer_len = 11;

		send (sock, full_sw_p_08_buffer, 13, 0);
	}
}

void send_crosspoint_message_to_socket (SOCKET sock, char dest, char src)
{
	sw_p_08_buffer[2] = 0x04;
	sw_p_08_buffer[3] = 0x00;
	sw_p_08_buffer[4] = 0x00;
	sw_p_08_buffer[5] = dest;

	if (src == DLE) {
		sw_p_08_buffer[6] = DLE;
		sw_p_08_buffer[7] = DLE;
		sw_p_08_buffer[8] = 5;
		sw_p_08_buffer[9] = -(dest + 25);
		sw_p_08_buffer[10] = DLE;
//		sw_p_08_buffer[11] = ETX;
		sw_p_08_buffer_len = 12;

		send (sock, sw_p_08_buffer, 12, 0);
	} else {
		sw_p_08_buffer[6] = src;
		sw_p_08_buffer[7] = 5;
		sw_p_08_buffer[8] = -(dest + src + 9);
		sw_p_08_buffer[9] = DLE;
		sw_p_08_buffer[10] = ETX;
		sw_p_08_buffer_len = 11;

		send (sock, sw_p_08_buffer, 11, 0);
	}
}

gboolean recv_from_remote_device_socket (remote_device_t *remote_device, char* buffer, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (remote_device->index < remote_device->recv_len) {
			buffer[i] = remote_device->buffer[remote_device->index++];
		} else {
			if ((remote_device->recv_len = recv (remote_device->src_socket, remote_device->buffer, 256, 0)) <= 0) return FALSE;
			buffer[i] = remote_device->buffer[0];
			remote_device->index = 1;
		}
	}

	return TRUE;
}

gpointer receive_message_from_remote_device (remote_device_t *remote_device)
{
	char buffer[8];
	rcp_t *rcp;

	while (recv_from_remote_device_socket (remote_device, buffer, 2)) {
		g_mutex_lock (&sw_p_08_mutex);

		if (buffer[0] == DLE) {
			if (buffer[1] == STX) {
				if (!recv_from_remote_device_socket (remote_device, buffer, 1)) { g_mutex_unlock (&sw_p_08_mutex); break; }

				if (buffer[0] == 0x01) {	//Crosspoint Interrogate Message
					if (!recv_from_remote_device_socket (remote_device, buffer, 7)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if ((0x01 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4]) == 0) {
						send_ok_plus_crosspoint_tally_message_to_socket (remote_device->src_socket, buffer[2]);
					} else send (remote_device->src_socket, NOK, 2, 0);
				} else if (buffer[0] == 0x02) {	//Crosspoint Connect Message
					if (!recv_from_remote_device_socket (remote_device, buffer, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if (buffer[3] == DLE) {
						if (!recv_from_remote_device_socket (remote_device, buffer + 3, 5)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					} else if (!recv_from_remote_device_socket (remote_device, buffer + 4, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x02 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5]) == 0) {
						send (remote_device->src_socket, OK, 2, 0);

						if (buffer[2] == 0) {
							if (buffer[3] < number_of_cameras_sets) {
								g_idle_add ((GSourceFunc)g_source_select_cameras_set_page, GINT_TO_POINTER ((int)buffer[3]));

								tally_camera_set = buffer[3];
							} else tally_camera_set = MAX_CAMERAS + 1;
						} else if (buffer[2] == 1) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if ((rcp_vision != NULL) && (rcp_vision != rcp)) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);
								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_above_child, rcp_vision);
							}

							rcp_vision = rcp;

							if (rcp_vision != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);

								if (rcp_vision->active) {
									g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_below_child, rcp_vision);

									if (physical_rcp.connected) g_idle_add ((GSourceFunc)update_physical_rcp, rcp_vision);
								}

								tally_rcp = buffer[3];
							} else tally_rcp = MAX_CAMERAS + 1;
						} else if (buffer[2] == 2) {
							if (buffer[3] < NB_SCENES) {
								g_idle_add ((GSourceFunc)g_source_recall_memories, GINT_TO_POINTER ((int)buffer[3]));

								tally_memory = buffer[3];
							} else tally_memory = MAX_CAMERAS + 1;
						} else if (buffer[2] == 3) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if (rcp_pgm != NULL) {
								if (send_ip_tally && rcp_pgm->ip_tally_is_on && rcp_pgm->camera_is_on) send_tally_off_control_command (rcp_pgm);

								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);
							}

							rcp_pgm = rcp;

							if (rcp_pgm != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);

								if (send_ip_tally && !rcp_pgm->ip_tally_is_on && rcp_pgm->camera_is_on) send_tally_on_control_command (rcp_pgm);

								tally_pgm = buffer[3];
							} else tally_pgm = MAX_CAMERAS + 1;
						} else if (buffer[2] == 4) {
							g_mutex_lock (&current_cameras_set_mutex);

							if ((current_cameras_set != NULL) && (buffer[3] < current_cameras_set->number_of_cameras))
								rcp = current_cameras_set->cameras[(int)buffer[3]];
							else rcp = NULL;

							g_mutex_unlock (&current_cameras_set_mutex);

							if (rcp_pvw != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

							rcp_pvw = rcp;

							if (rcp_pvw != NULL) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

								tally_pvw = buffer[3];
							} else tally_pvw = MAX_CAMERAS + 1;
						}

						send_crosspoint_message_to_socket (remote_device->src_socket, buffer[2], buffer[3]);
					} else send (remote_device->src_socket, NOK, 2, 0);
				} else {
					send (remote_device->src_socket, OK, 2, 0);

					if ((remote_device->recv_len = recv (remote_device->src_socket, remote_device->buffer, 256, 0)) <= 0) {
						g_mutex_unlock (&sw_p_08_mutex);
						break;
					}

					remote_device->index = 0;
				}
			}
		}

		g_mutex_unlock (&sw_p_08_mutex);
	}

	closesocket (remote_device->src_socket);
	remote_device->src_socket = INVALID_SOCKET;

	g_idle_add ((GSourceFunc)sw_p_08_tcp_remote_device_disconnect, remote_device);

	return NULL;
}

gpointer sw_p_08_tcp_server (void)
{
	SOCKET src_socket;
	struct sockaddr_in src_addr;
	socklen_t addrlen;

	sw_p_08_tcp_server_started = TRUE;

	while (sw_p_08_tcp_server_started) {
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (sw_p_08_socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) continue;

		if (remote_devices[0].src_socket == INVALID_SOCKET) {
			if (remote_devices[0].thread != NULL) {
				g_thread_join (remote_devices[0].thread);
				remote_devices[0].thread = NULL;
			}

			if ((remote_devices[0].recv_len = recv (src_socket, remote_devices[0].buffer, 256, 0)) <= 0) {
				closesocket (src_socket);
				continue;
			}

			LOG_SW_P_08_INCOMMING_MESSAGE(inet_ntoa (src_addr.sin_addr),remote_devices[0].buffer,remote_devices[0].recv_len)

			remote_devices[0].src_socket = src_socket;
			memcpy (&remote_devices[0].src_addr, &src_addr, sizeof (struct sockaddr_in));
			remote_devices[0].index = 0;

			g_idle_add ((GSourceFunc)sw_p_08_tcp_remote_device_connect, remote_devices);

			remote_devices[0].thread = g_thread_new (NULL, (GThreadFunc)receive_message_from_remote_device, remote_devices);
		} else if (remote_devices[1].src_socket == INVALID_SOCKET) {
			if (remote_devices[1].thread != NULL) {
				g_thread_join (remote_devices[1].thread);
				remote_devices[1].thread = NULL;
			}

			if ((remote_devices[1].recv_len = recv (src_socket, remote_devices[1].buffer, 256, 0)) <= 0) {
				closesocket (src_socket);
				continue;
			}

			LOG_SW_P_08_INCOMMING_MESSAGE(inet_ntoa (src_addr.sin_addr),remote_devices[1].buffer,remote_devices[1].recv_len)

			remote_devices[1].src_socket = src_socket;
			memcpy (&remote_devices[1].src_addr, &src_addr, sizeof (struct sockaddr_in));
			remote_devices[1].index = 0;

			g_idle_add ((GSourceFunc)sw_p_08_tcp_remote_device_connect, remote_devices + 1);

			remote_devices[1].thread = g_thread_new (NULL, (GThreadFunc)receive_message_from_remote_device, remote_devices + 1);
		} else closesocket (src_socket);
	}

	return NULL;
}

void init_sw_p_08 (void)
{
	g_mutex_init (&sw_p_08_mutex);

	memset (&sw_p_08_address, 0, sizeof (struct sockaddr_in));
	sw_p_08_address.sin_family = AF_INET;
	sw_p_08_address.sin_port = htons (SW_P_08_TCP_PORT);
	sw_p_08_address.sin_addr.s_addr = inet_addr (my_ip_address);

	remote_devices[0].src_socket = INVALID_SOCKET;
	remote_devices[0].connected_label = NULL;
	remote_devices[0].thread = NULL;

	remote_devices[1].src_socket = INVALID_SOCKET;
	remote_devices[1].connected_label = NULL;
	remote_devices[1].thread = NULL;
}

void start_sw_p_08 (void)
{
	gtk_widget_hide (remote_devices[0].connected_label);
	gtk_widget_hide (remote_devices[1].connected_label);

	if (ip_rs) {
		gtk_widget_hide (rs_ok_label);

		if (!start_sw_p_08_tcp_server ()) gtk_widget_hide (ip_waiting_label);
	} else {
		gtk_widget_hide (ip_waiting_label);

		if (!start_rs_communication ()) gtk_widget_hide (rs_ok_label);
	}
}

gboolean start_sw_p_08_tcp_server (void)
{
	sw_p_08_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind (sw_p_08_socket, (struct sockaddr *)&sw_p_08_address, sizeof (struct sockaddr_in)) == 0) {
		if (listen (sw_p_08_socket, 2) == 0) {
			sw_p_08_thread = g_thread_new (NULL, (GThreadFunc)sw_p_08_tcp_server, NULL);

			return TRUE;
		}
	}

	return FALSE;
}

gboolean start_rs_communication (void)
{
	if (open_rs_port (rs_port_name) != -1) {
		sw_p_08_thread = g_thread_new (NULL, (GThreadFunc)listen_to_rs_port, NULL);

		return TRUE;
	} else return FALSE;
}

void stop_sw_p_08_tcp_server (void)
{
	sw_p_08_tcp_server_started = FALSE;

	gtk_widget_hide (ip_waiting_label);
	gtk_widget_hide (remote_devices[0].connected_label);
	gtk_widget_hide (remote_devices[1].connected_label);

	if (rcp_pgm != NULL) {
		g_source_rcp_queue_draw (rcp_pgm);
		rcp_pgm = NULL;
	}

	if (rcp_pvw != NULL) {
		g_source_rcp_queue_draw (rcp_pvw);
		rcp_pvw = NULL;
	}

	if (remote_devices[0].src_socket != INVALID_SOCKET) {
		shutdown (remote_devices[0].src_socket, SHUT_RD);
		closesocket (remote_devices[0].src_socket);
		remote_devices[0].src_socket = INVALID_SOCKET;
	}

	if (remote_devices[1].src_socket != INVALID_SOCKET) {
		shutdown (remote_devices[1].src_socket, SHUT_RD);
		closesocket (remote_devices[1].src_socket);
		remote_devices[1].src_socket = INVALID_SOCKET;
	}

	shutdown (sw_p_08_socket, SHUT_RD);
	closesocket (sw_p_08_socket);

	tally_camera_set = MAX_CAMERAS + 1;
	tally_rcp = MAX_CAMERAS + 1;
	tally_memory = MAX_CAMERAS + 1;
	tally_pgm = MAX_CAMERAS + 1;
	tally_pvw = MAX_CAMERAS + 1;

	if (remote_devices[0].thread != NULL) {
		g_thread_join (remote_devices[0].thread);
		remote_devices[0].thread = NULL;
	}

	if (remote_devices[1].thread != NULL) {
		g_thread_join (remote_devices[1].thread);
		remote_devices[1].thread = NULL;
	}

	if (sw_p_08_thread != NULL) {
		g_thread_join (sw_p_08_thread);
		sw_p_08_thread = NULL;
	}
}

void stop_rs_communication (void)
{
	gtk_widget_hide (rs_ok_label);

	if (rcp_pgm != NULL) {
		g_source_rcp_queue_draw (rcp_pgm);
		rcp_pgm = NULL;
	}

	if (rcp_pvw != NULL) {
		g_source_rcp_queue_draw (rcp_pvw);
		rcp_pvw = NULL;
	}

	if (sw_p_08_thread != NULL) {
		close_rs_port ();
		g_thread_join (sw_p_08_thread);
		sw_p_08_thread = NULL;
	}
}

