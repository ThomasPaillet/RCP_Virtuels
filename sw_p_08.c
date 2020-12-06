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

#include "rcp.h"

#include <string.h>


#define DLE 0x10
#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x15


GMutex sw_p_08_mutex;

GThread *sw_p_08_thread = NULL;

char OK[2] = {DLE, ACK};
char NOK[2] = {DLE, NAK};

char full_sw_p_08_buffer[14] = {DLE, ACK, DLE, STX, 0x04, 0x00, 0x00, 0x00, 0x00, 0x05, -0x09, DLE, ETX, ETX};
char *sw_p_08_buffer = &full_sw_p_08_buffer[2];
int sw_p_08_buffer_len = 11;
int rs_try = 0;

gboolean ip_rs = TRUE;

SOCKET sw_p_08_socket;
struct sockaddr_in sw_p_08_adresse;

gboolean sw_p_08_tcp_server_started = FALSE;

remote_device_t remote_devices[2];

char rs_port_name[16] = { ' ', '\0'};

int number_of_matrix_source = 2;

rcp_t *rcp_vision = NULL;
rcp_t *rcp_pgm = NULL;
rcp_t *rcp_pvw = NULL;

char tally_vision = 0;
char tally_pgm = 0;
char tally_pvw = 0;

gboolean knee_matrix_detail_popup = FALSE;


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
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);

	return G_SOURCE_REMOVE;
}

gboolean g_source_rcp_event_box_set_below_child (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), FALSE);

	return G_SOURCE_REMOVE;
}

void ask_to_connect_pgm_to_ctrl_vision (void)
{
	g_mutex_lock (&sw_p_08_mutex);

	tally_vision = 1;

	sw_p_08_buffer[2] = 0x04;	//8.2.2. CROSSPOINT CONNECTED Message page 24
	sw_p_08_buffer[3] = 0;
	sw_p_08_buffer[4] = 0;
	sw_p_08_buffer[5] = 0;	//CTRL VISION/CTRL OPV
	sw_p_08_buffer[6] = 1;	//Source PGM
	sw_p_08_buffer[7] = 5;
	sw_p_08_buffer[8] = -10;
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

	if (rcp_vision != NULL) {
		g_source_rcp_queue_draw (rcp_vision);
		if (rcp_vision->active) g_source_rcp_event_box_set_above_child (rcp_vision);
		rcp_vision = NULL;
	}

	g_mutex_unlock (&sw_p_08_mutex);
}

gboolean rcp_button_press_event (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp)
{
	g_mutex_lock (&sw_p_08_mutex);

	if (knee_matrix_detail_popup) {
		knee_matrix_detail_popup = FALSE;
		if (rcp_vision != NULL) {
			if (rcp_vision->active) gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
		}
		gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->camera_set))->master_rcp.root_widget), FALSE);

		g_mutex_unlock (&sw_p_08_mutex);

		return GDK_EVENT_PROPAGATE;
	}

	if (rcp == rcp_vision) {
		g_mutex_unlock (&sw_p_08_mutex);

		return GDK_EVENT_PROPAGATE;
	}

	tally_vision = rcp->matrix_source_number;

	sw_p_08_buffer[2] = 0x04;	//8.2.2. CROSSPOINT CONNECTED Message page 24
	sw_p_08_buffer[3] = 0;
	sw_p_08_buffer[4] = 0;
	sw_p_08_buffer[5] = 0;	//CTRL VISION

	if (tally_vision == DLE) {
		sw_p_08_buffer[6] = DLE;
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
		sw_p_08_buffer[6] = tally_vision;
		sw_p_08_buffer[7] = 5;
		sw_p_08_buffer[8] = -(tally_vision + 9);
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

	return GDK_EVENT_PROPAGATE;
}

void send_ok_plus_crosspoint_tally_message_to_rs (char dest)
{
	char src;

	if (dest == 0) src = tally_vision;
	else if (dest == 1) src = tally_pgm;
	else if (dest == 2) src = tally_pvw;
	else src = 0;

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
	int i;
	cameras_set_t *cameras_set_itr;
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

				if (buffer[0] == 0x01) {	//8.1.1. Crosspoint Interrogate Message page 20
					if (!receive_from_rs_port (buffer, 7)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x01 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4]) == 0) {
						send_ok_plus_crosspoint_tally_message_to_rs (buffer[2]);
					} else send_to_rs_port (NOK, 2);
				} else if (buffer[0] == 0x02) {	//8.1.2. Crosspoint Connect Message page 21
					if (!receive_from_rs_port (buffer, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if (buffer[3] == DLE) {
						if (!receive_from_rs_port (buffer + 3, 5)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					} else if (!receive_from_rs_port (buffer + 4, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x02 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5]) == 0) {
						send_to_rs_port (OK, 2);

						rcp = NULL;
						if (buffer[3] > 1) {
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									if (cameras_set_itr->rcp_ptr_array[i]->matrix_source_number == buffer[3]) {
										rcp = cameras_set_itr->rcp_ptr_array[i];
										break;
									}
								}
								if (rcp != NULL) break;
							}
						}

						if (buffer[2] == 0) {
							if ((rcp_vision != NULL) && (rcp_vision != rcp)) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);
								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_above_child, rcp_vision);
							}

							rcp_vision = rcp;

							if (rcp_vision != NULL) {
								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_below_child, rcp_vision);
								tally_vision = rcp_vision->matrix_source_number;
							} else tally_vision = 0;
						} else if (buffer[2] == 1) {
							if (rcp_pgm != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);

							rcp_pgm = rcp;

							if (rcp_pgm != NULL) tally_pgm = rcp_pgm->matrix_source_number;
							else tally_pgm = 0;
						} else if (buffer[2] == 2) {
							if (rcp_pvw != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

							rcp_pvw = rcp;

							if (rcp_pvw != NULL) tally_pvw = rcp_pvw->matrix_source_number;
							else tally_pvw = 0;
						}

						if (rcp != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp);

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
	gtk_widget_hide (remote_device->connected_label);
	gtk_widget_show (ip_waiting_label);

	return G_SOURCE_REMOVE;
}

void send_ok_plus_crosspoint_tally_message_to_socket (SOCKET sock, char dest)
{
	char src;

	if (dest == 0) src = tally_vision;
	else if (dest == 1) src = tally_pgm;
	else if (dest == 2) src = tally_pvw;
	else src = 0;

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
	cameras_set_t *cameras_set_itr;
	rcp_t *rcp;
	int i;

	while (recv_from_remote_device_socket (remote_device, buffer, 2)) {
		g_mutex_lock (&sw_p_08_mutex);

		if (buffer[0] == DLE) {
			/*if (buffer[1] == ACK) {
			} else if (buffer[1] == NAK) {
			} else*/ if (buffer[1] == STX) {
				if (!recv_from_remote_device_socket (remote_device, buffer, 1)) { g_mutex_unlock (&sw_p_08_mutex); break; }

				if (buffer[0] == 0x01) {	//8.1.1. Crosspoint Interrogate Message page 20
					if (!recv_from_remote_device_socket (remote_device, buffer, 7)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if ((0x01 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4]) == 0) {
						send_ok_plus_crosspoint_tally_message_to_socket (remote_device->src_socket, buffer[2]);
					} else send (remote_device->src_socket, NOK, 2, 0);
				} else if (buffer[0] == 0x02) {	//8.1.2. Crosspoint Connect Message page 21
					if (!recv_from_remote_device_socket (remote_device, buffer, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					if (buffer[3] == DLE) {
						if (!recv_from_remote_device_socket (remote_device, buffer + 3, 5)) { g_mutex_unlock (&sw_p_08_mutex); break; }
					} else if (!recv_from_remote_device_socket (remote_device, buffer + 4, 4)) { g_mutex_unlock (&sw_p_08_mutex); break; }

					if ((0x02 + buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5]) == 0) {
						send (remote_device->src_socket, OK, 2, 0);

						rcp = NULL;
						if (buffer[3] > 1) {
							for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
								for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
									if (cameras_set_itr->rcp_ptr_array[i]->matrix_source_number == buffer[3]) {
										rcp = cameras_set_itr->rcp_ptr_array[i];
										break;
									}
								}
								if (rcp != NULL) break;
							}
						}

						if (buffer[2] == 0) {
							if ((rcp_vision != NULL) && (rcp_vision != rcp)) {
								g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_vision);
								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_above_child, rcp_vision);
							}

							rcp_vision = rcp;

							if (rcp_vision != NULL) {
								if (rcp_vision->active) g_idle_add ((GSourceFunc)g_source_rcp_event_box_set_below_child, rcp_vision);
								tally_vision = rcp_vision->matrix_source_number;
							} else tally_vision = 0;
						} else if (buffer[2] == 1) {
							if (rcp_pgm != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pgm);

							rcp_pgm = rcp;

							if (rcp_pgm != NULL) tally_pgm = rcp_pgm->matrix_source_number;
							else tally_pgm = 0;
						} else if (buffer[2] == 2) {
							if (rcp_pvw != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp_pvw);

							rcp_pvw = rcp;

							if (rcp_pvw != NULL) tally_pvw = rcp_pvw->matrix_source_number;
							else tally_pvw = 0;
						}

						if (rcp != NULL) g_idle_add ((GSourceFunc)g_source_rcp_queue_draw, rcp);

						send_crosspoint_message_to_socket (remote_device->src_socket, buffer[2], buffer[3]);
					} else send (remote_device->src_socket, NOK, 2, 0);
				} else {
					send (remote_device->src_socket, OK, 2, 0);

					if ((remote_device->recv_len = recv (remote_device->src_socket, remote_device->buffer, 256, 0)) <= 0) { g_mutex_unlock (&sw_p_08_mutex); break; }
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
#ifdef _WIN32
	int addrlen;
#elif defined (__linux)
	socklen_t addrlen;
#endif

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

	memset (&sw_p_08_adresse, 0, sizeof (struct sockaddr_in));
	sw_p_08_adresse.sin_family = AF_INET;
	sw_p_08_adresse.sin_port = htons (SW_P_08_TCP_PORT);
	sw_p_08_adresse.sin_addr.s_addr = inet_addr (my_ip_adresse);

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

	if (bind (sw_p_08_socket, (struct sockaddr *)&sw_p_08_adresse, sizeof (struct sockaddr_in)) == 0) {
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
		closesocket (remote_devices[0].src_socket);
		remote_devices[0].src_socket = INVALID_SOCKET;
	}

	if (remote_devices[1].src_socket != INVALID_SOCKET) {
		closesocket (remote_devices[1].src_socket);
		remote_devices[1].src_socket = INVALID_SOCKET;
	}

	closesocket (sw_p_08_socket);

	tally_vision = 0;
	tally_pgm = 0;
	tally_pvw = 0;

/*	if (remote_devices[0].thread != NULL) {
		g_thread_join (remote_devices[0].thread);
		remote_devices[0].thread = NULL;
	}

	if (remote_devices[1].thread != NULL) {
		g_thread_join (remote_devices[1].thread);
		remote_devices[1].thread = NULL;
	}*/

	if (sw_p_08_thread != NULL) {
//		g_thread_join (sw_p_08_thread);
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

