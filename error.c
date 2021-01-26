/*
 * copyright (c) 2018-2021 Thomas Paillet <thomas.paillet@net-c.fr

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

#include <stdio.h>


#define RCP_ERROR(f,c,s) gboolean f (rcp_t *rcp) \
{ \
	struct timeval current_time; \
	struct tm *time; \
 \
	rcp->error_code = c; \
	gtk_label_set_text (GTK_LABEL (rcp->error_label), s); \
 \
	gettimeofday (&current_time, NULL); \
	time = localtime (&current_time.tv_sec); \
	fprintf (error_log_file, "%02dh %02dm %02ds: Camera %s (%s) -> %s\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address, s); \
 \
	return G_SOURCE_REMOVE; \
}


FILE *error_log_file = NULL;


gboolean clear_rcp_error(rcp_t *rcp)
{
	struct timeval current_time;
	struct tm *time;

	rcp->error_code = 0x00;
	gtk_label_set_text (GTK_LABEL (rcp->error_label), NULL);

	gettimeofday (&current_time, NULL);
	time = localtime (&current_time.tv_sec);
	fprintf (error_log_file, "%02dh %02dm %02ds: Camera %s (%s) -> Normal\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address);

	return G_SOURCE_REMOVE;
}

RCP_ERROR(Motor_Driver_Error,0x03,"Motor Driver Error")
RCP_ERROR(Pan_Sensor_Error,0x04,"Pan Sensor Error")
RCP_ERROR(Tilt_Sensor_Error,0x05,"Tilt Sensor Error")
RCP_ERROR(Controller_RX_Over_run_Error,0x06,"Controller RX Over run Error")
RCP_ERROR(Controller_RX_Framing_Error,0x07,"Controller RX Framing Error")
RCP_ERROR(Network_RX_Over_run_Error,0x08,"Network RX Over run Error")
RCP_ERROR(Network_RX_Framing_Error,0x09,"Network RX Framing Error")
RCP_ERROR(Controller_RX_Command_Buffer_Overflow,0x17,"Controller RX Command Buffer Overflow")
RCP_ERROR(Network_RX_Command_Buffer_Overflow,0x19,"Network RX Command Buffer Overflow")
RCP_ERROR(System_Error,0x21,"System Error")
RCP_ERROR(Spec_Limit_Over,0x22,"Spec Limit Over")
RCP_ERROR(Network_communication_Error,0x24,"Network communication Error")
RCP_ERROR(CAMERA_communication_Error,0x25,"CAMERA communication Error")
RCP_ERROR(CAMERA_RX_Over_run_Error,0x26,"CAMERA RX Over run Error")
RCP_ERROR(CAMERA_RX_Framing_Error,0x27,"CAMERA RX Framing Error")
RCP_ERROR(CAMERA_RX_Command_Buffer_Overflow,0x28,"CAMERA RX Command Buffer Overflow")

gboolean camera_is_unreachable (rcp_t *rcp)	//error_code = 0x30
{
	struct tm *time;

	gtk_label_set_text (GTK_LABEL (rcp->error_label), "La caméra n'est pas connectée au réseau");
	set_rcp_off (rcp);
	gettimeofday (&rcp->last_time, NULL);

	time = localtime (&rcp->last_time.tv_sec);
	fprintf (error_log_file, "%02dh %02dm %02ds: La caméra %s (%s) n'est pas connectée au réseau\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address);

	return G_SOURCE_REMOVE;
}

RCP_ERROR(ABB_execution_failed,0x40,"ABB execution failed")
RCP_ERROR(ABB_execution_failed_busy_status,0x41,"ABB execution failed (busy status)")

void start_error_log (void)
{
	struct timeval current_time;
	struct tm *time;
	char error_log_file_name[24];

	gettimeofday (&current_time, NULL);
	time = localtime (&current_time.tv_sec);
	sprintf (error_log_file_name, "20%02d-%02d-%02d_Errors.log", time->tm_year - 100, time->tm_mon + 1, time->tm_mday);
	error_log_file = fopen (error_log_file_name, "a");
}

void stop_error_log (void)
{
	fclose (error_log_file);
}

