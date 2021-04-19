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

#include <stdio.h>


char *error_msg[18] = { "Motor Driver Error", \
"Pan Sensor Error", \
"Tilt Sensor Error", \
"Controller RX Over run Error", \
"Controller RX Framing Error", \
"Network RX Over run Error", \
"Network RX Framing Error", \
"Controller RX Command Buffer Overflow", \
"Network RX Command Buffer Overflow", \
"System Error", \
"Spec Limit Over", \
"Network communication Error", \
"CAMERA communication Error", \
"CAMERA RX Over run Error", \
"CAMERA RX Framing Error", \
"CAMERA RX Command Buffer Overflow", \
"ABB execution failed", \
"ABB execution failed (busy status)" };

char *unreachable_msg = "La caméra n'est pas connectée au réseau";


#define RCP_ERROR(f,c,s) gboolean f (rcp_t *rcp) \
{ \
	rcp->error_code = c; \
	gtk_label_set_text (GTK_LABEL (rcp->error_label), error_msg[s]); \
 \
	return G_SOURCE_REMOVE; \
} \
 \
gboolean log_##f (rcp_t *rcp) \
{ \
	struct timeval current_time; \
	struct tm *time; \
 \
	gettimeofday (&current_time, NULL); \
	time = localtime (&current_time.tv_sec); \
	fprintf (error_log_file, "%02dh %02dm %02ds: Camera %s (%s) -> %s\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address, error_msg[s]); \
 \
	return G_SOURCE_REMOVE; \
}


FILE *error_log_file = NULL;


gboolean clear_rcp_error (rcp_t *rcp)
{
	rcp->error_code = 0x00;
	gtk_label_set_text (GTK_LABEL (rcp->error_label), NULL);

	return G_SOURCE_REMOVE;
}

gboolean log_clear_rcp_error (rcp_t *rcp)
{
	struct timeval current_time;
	struct tm *time;

	gettimeofday (&current_time, NULL);
	time = localtime (&current_time.tv_sec);
	fprintf (error_log_file, "%02dh %02dm %02ds: Camera %s (%s) -> Normal\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address);

	return G_SOURCE_REMOVE;
}

RCP_ERROR(Motor_Driver_Error,0x03,0)
RCP_ERROR(Pan_Sensor_Error,0x04,1)
RCP_ERROR(Tilt_Sensor_Error,0x05,2)
RCP_ERROR(Controller_RX_Over_run_Error,0x06,3)
RCP_ERROR(Controller_RX_Framing_Error,0x07,4)
RCP_ERROR(Network_RX_Over_run_Error,0x08,5)
RCP_ERROR(Network_RX_Framing_Error,0x09,6)
RCP_ERROR(Controller_RX_Command_Buffer_Overflow,0x17,7)
RCP_ERROR(Network_RX_Command_Buffer_Overflow,0x19,8)
RCP_ERROR(System_Error,0x21,9)
RCP_ERROR(Spec_Limit_Over,0x22,10)
RCP_ERROR(Network_communication_Error,0x24,11)
RCP_ERROR(CAMERA_communication_Error,0x25,12)
RCP_ERROR(CAMERA_RX_Over_run_Error,0x26,13)
RCP_ERROR(CAMERA_RX_Framing_Error,0x27,14)
RCP_ERROR(CAMERA_RX_Command_Buffer_Overflow,0x28,15)

gboolean camera_is_unreachable (rcp_t *rcp)	//error_code = 0x30
{
	struct tm *time;
	GSList *gslist_itr;
	rcp_t *other_rcp;
	struct timeval current_time;

	if (rcp->root_widget != NULL) gtk_label_set_text (GTK_LABEL (rcp->error_label), unreachable_msg);

	set_rcp_off (rcp);

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
		other_rcp = (rcp_t *)gslist_itr->data;

		other_rcp->error_code = 0x30;
		gtk_label_set_text (GTK_LABEL (other_rcp->error_label), unreachable_msg);
		set_rcp_off (other_rcp);
	}
	g_mutex_unlock (&rcp->other_rcp_mutex);

	gettimeofday (&current_time, NULL);
	time = localtime (&current_time.tv_sec);
	fprintf (error_log_file, "%02dh %02dm %02ds: La caméra %s (%s) n'est pas connectée au réseau\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address);

	return G_SOURCE_REMOVE;
}

RCP_ERROR(ABB_execution_failed,0x40,16)
RCP_ERROR(ABB_execution_failed_busy_status,0x41,17)

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

