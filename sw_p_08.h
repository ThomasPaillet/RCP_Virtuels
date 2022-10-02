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

#ifndef __SW_P_08_H
#define __SW_P_08_H


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


#endif

