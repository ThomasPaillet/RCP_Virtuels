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

#ifndef __PHYSICAL_RCP_H
#define __PHYSICAL_RCP_H


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


#endif

