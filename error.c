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

#include <stdio.h>


#define RCP_ERROR(f,c,s) \
gboolean f (rcp_t *rcp) \
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

#define RCP_ERROR_S(f,c,s,m) \
gboolean f (rcp_t *rcp) \
{ \
	rcp->error_code = c; \
	gtk_label_set_text (GTK_LABEL (rcp->error_label), error_msg_##m[s]); \
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
	fprintf (error_log_file, "%02dh %02dm %02ds: Camera %s (%s) -> %s\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address, error_msg_##m[s]); \
 \
	return G_SOURCE_REMOVE; \
}


FILE *error_log_file = NULL;

char *error_msg[13] = { \
	"Motor Driver Error", \
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
	"ABB execution failed", \
	"ABB execution failed (busy status)" \
	};

char *error_msg_AW_HE130[5] = { \
	"Network communication Error", \
	"CAMERA communication Error", \
	"CAMERA RX Over run Error", \
	"CAMERA RX Framing Error", \
	"CAMERA RX Command Buffer Overflow", \
	};

char *error_msg_AW_UE150[20] = { \
	"FPGA Config Error", \
	"NET Life-monitoring Error", \
	"BE Life-monitoring Error", \
	"IF/BE UART Buffer Overflow", \
	"IF/BE UART Framing Error", \
	"IF/BE UART Buffer Overflow", \
	"CAM Life-monitoring Error", \
	"Fan1 error", \
	"Fan2 error", \
	"High Temp", \
	"Low Temp", \
	"Temp Sensor Error", \
	"Lens Initialize Error", \
	"PT. Initialize Error", \
	"MR Level Error", \
	"MR Offset Error", \
	"Origin Offset Error", \
	"Angle MR Sensor Error", \
	"PT. Gear Error", \
	"Motor Disconnect Error", \
	};


gboolean camera_is_unreachable (rcp_t *rcp)
{
	struct tm *time;
	GSList *gslist_itr;
	rcp_t *other_rcp;
	struct timeval current_time;

	if (rcp->root_widget != NULL) gtk_label_set_text (GTK_LABEL (rcp->error_label), "La caméra n'est pas connectée au réseau");

	set_rcp_off (rcp);

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
		other_rcp = (rcp_t *)gslist_itr->data;

		other_rcp->error_code = CAMERA_IS_UNREACHABLE_ERROR;
		if (other_rcp->root_widget != NULL) gtk_label_set_text (GTK_LABEL (other_rcp->error_label), "La caméra n'est pas connectée au réseau");

		set_rcp_off (other_rcp);
	}
	g_mutex_unlock (&rcp->other_rcp_mutex);

	gettimeofday (&current_time, NULL);
	time = localtime (&current_time.tv_sec);
	fprintf (error_log_file, "%02dh %02dm %02ds: La caméra %s (%s) n'est pas connectée au réseau\n", time->tm_hour, time->tm_min, time->tm_sec, rcp->name, rcp->ip_address);

	return G_SOURCE_REMOVE;
}

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
RCP_ERROR(ABB_execution_failed,0x60,11)
RCP_ERROR(ABB_execution_failed_busy_status,0x61,12)

RCP_ERROR_S(Network_communication_Error,0x24,0,AW_HE130)
RCP_ERROR_S(CAMERA_communication_Error,0x25,1,AW_HE130)
RCP_ERROR_S(CAMERA_RX_Over_run_Error,0x26,2,AW_HE130)
RCP_ERROR_S(CAMERA_RX_Framing_Error,0x27,3,AW_HE130)
RCP_ERROR_S(CAMERA_RX_Command_Buffer_Overflow,0x28,4,AW_HE130)

RCP_ERROR_S(FPGA_Config_Error,0x23,0,AW_UE150)
RCP_ERROR_S(NET_Life_monitoring_Error,0x24,1,AW_UE150)
RCP_ERROR_S(BE_Life_monitoring_Error,0x25,2,AW_UE150)
RCP_ERROR_S(IF_BE_UART_Buffer_Overflow,0x26,3,AW_UE150)
RCP_ERROR_S(IF_BE_UART_Framing_Error,0x27,4,AW_UE150)
RCP_ERROR_S(IF_BE_UART_Buffer_Overflow_2,0x28,5,AW_UE150)
RCP_ERROR_S(CAM_Life_monitoring_Error,0x29,6,AW_UE150)
RCP_ERROR_S(Fan1_error,0x31,7,AW_UE150)
RCP_ERROR_S(Fan2_error,0x32,8,AW_UE150)
RCP_ERROR_S(High_Temp,0x33,9,AW_UE150)
RCP_ERROR_S(Low_Temp,0x36,10,AW_UE150)
RCP_ERROR_S(Temp_Sensor_Error,0x40,11,AW_UE150)
RCP_ERROR_S(Lens_Initialize_Error,0x41,12,AW_UE150)
RCP_ERROR_S(PT_Initialize_Error,0x42,13,AW_UE150)
RCP_ERROR_S(MR_Level_Error,0x50,14,AW_UE150)
RCP_ERROR_S(MR_Offset_Error,0x52,15,AW_UE150)
RCP_ERROR_S(Origin_Offset_Error,0x53,16,AW_UE150)
RCP_ERROR_S(Angle_MR_Sensor_Error,0x54,17,AW_UE150)
RCP_ERROR_S(PT_Gear_Error,0x55,18,AW_UE150)
RCP_ERROR_S(Motor_Disconnect_Error,0x56,19,AW_UE150)

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

