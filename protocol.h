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

#ifndef __PROTOCOL_H
#define __PROTOCOL_H


#include "rcp.h"


extern char my_ip_address[16];
extern char network_address[3][4];
extern int network_address_len[3];


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


#endif

