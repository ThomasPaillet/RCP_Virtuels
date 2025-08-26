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

#include "error.h"

#include "cameras_set.h"
#include "logging.h"
#include "misc.h"

#include <stdio.h>


#define RCP_ERROR(f,c,s) gboolean f (struct in_addr *src_in_addr) \
{ \
	rcp_t *rcp; \
	cameras_set_t *cameras_set_itr; \
	int i; \
 \
	rcp = NULL; \
 \
	g_mutex_lock (&cameras_sets_mutex); \
 \
	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) { \
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) { \
			if (cameras_set_itr->cameras[i]->address.sin_addr.s_addr == src_in_addr->s_addr) { \
				if (rcp == NULL) { \
					rcp = cameras_set_itr->cameras[i]; \
 \
					LOG_RCP_ERROR(s); \
				} else rcp = cameras_set_itr->cameras[i]; \
 \
				rcp->error_code = c; \
				gtk_label_set_text (GTK_LABEL (rcp->error_label), s); \
 \
				break; \
			} \
		} \
	} \
 \
	g_mutex_unlock (&cameras_sets_mutex); \
 \
	g_free (src_in_addr); \
 \
	return G_SOURCE_REMOVE; \
}


gboolean camera_is_unreachable (rcp_t *rcp)
{
	GSList *gslist_itr;
	rcp_t *other_rcp;

	if (rcp->root_widget != NULL) gtk_label_set_text (GTK_LABEL (rcp->error_label), "La caméra n'est pas connectée au réseau");

	set_rcp_off (rcp);

	LOG_RCP_STRING("is not connected physically")

	g_mutex_lock (&rcp->other_rcp_mutex);
	for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
		other_rcp = (rcp_t *)gslist_itr->data;

		other_rcp->error_code = CAMERA_IS_UNREACHABLE_ERROR;
		if (other_rcp->root_widget != NULL) gtk_label_set_text (GTK_LABEL (other_rcp->error_label), "La caméra n'est pas connectée au réseau");

		set_rcp_off (other_rcp);
	}
	g_mutex_unlock (&rcp->other_rcp_mutex);

	return G_SOURCE_REMOVE;
}

gboolean clear_rcp_error (struct in_addr *src_in_addr)
{
	rcp_t *rcp;
	cameras_set_t *cameras_set_itr;
	int i;

	rcp = NULL;

	g_mutex_lock (&cameras_sets_mutex);

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
			if (cameras_set_itr->cameras[i]->address.sin_addr.s_addr == src_in_addr->s_addr) {
				if (rcp == NULL) {
					rcp = cameras_set_itr->cameras[i];

					LOG_RCP_STRING("Normal");
				} else rcp = cameras_set_itr->cameras[i];

				rcp->error_code = 0x00;
				gtk_label_set_text (GTK_LABEL (rcp->error_label), NULL);

				break;
			}
		}
	}

	g_mutex_unlock (&cameras_sets_mutex);

	g_free (src_in_addr);

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
RCP_ERROR(ABB_execution_failed,0x60,"ABB execution failed")
RCP_ERROR(ABB_execution_failed_busy_status,0x61,"ABB execution failed (busy status)")

RCP_ERROR(Network_communication_Error,0x24,"Network communication Error")
RCP_ERROR(CAMERA_communication_Error,0x25,"CAMERA communication Error")
RCP_ERROR(CAMERA_RX_Over_run_Error,0x26,"CAMERA RX Over run Error")
RCP_ERROR(CAMERA_RX_Framing_Error,0x27,"CAMERA RX Framing Error")
RCP_ERROR(CAMERA_RX_Command_Buffer_Overflow,0x28,"CAMERA RX Command Buffer Overflow")

RCP_ERROR(FPGA_Config_Error,0x23,"FPGA Config Error")
RCP_ERROR(NET_Life_monitoring_Error,0x24,"NET Life-monitoring Error")
RCP_ERROR(BE_Life_monitoring_Error,0x25,"BE Life-monitoring Error")
RCP_ERROR(IF_BE_UART_Buffer_Overflow,0x26,"IF/BE UART Buffer Overflow")
RCP_ERROR(IF_BE_UART_Framing_Error,0x27,"IF/BE UART Framing Error")
RCP_ERROR(IF_BE_UART_Buffer_Overflow_2,0x28,"IF/BE UART Buffer Overflow")

RCP_ERROR(CAM_Life_monitoring_Error,0x29,"CAM Life-monitoring Error")
RCP_ERROR(NET_Life_monitoring_Error_2,0x30,"NET Life-monitoring Error")
RCP_ERROR(Fan1_error,0x31,"Fan1 error")
RCP_ERROR(Fan2_error,0x32,"Fan2 error")
RCP_ERROR(High_Temp,0x33,"High Temp")
RCP_ERROR(Low_Temp,0x36,"Low Temp")
RCP_ERROR(Wiper_Error,0x39,"Wiper Error")
RCP_ERROR(Temp_Sensor_Error,0x40,"Temp Sensor Error")
RCP_ERROR(Lens_Initialize_Error,0x41,"Lens Initialize Error")
RCP_ERROR(PT_Initialize_Error,0x42,"PT. Initialize Error")
RCP_ERROR(PoEpp_Software_auth_Timeout,0x43,"PoE++ Software auth. Timeout")
RCP_ERROR(PoEp_Software_auth_Timeout,0x45,"PoE+ Software auth. Timeout")
RCP_ERROR(USB_Streaming_Error,0x47,"USB Streaming Error")
RCP_ERROR(MR_Level_Error,0x50,"MR Level Error")
RCP_ERROR(MR_Offset_Error,0x52,"MR Offset Error")
RCP_ERROR(Origin_Offset_Error,0x53,"Origin Offset Error")
RCP_ERROR(Angle_MR_Sensor_Error,0x54,"Angle MR Sensor Error")
RCP_ERROR(PT_Gear_Error,0x55,"PT. Gear Error")
RCP_ERROR(Motor_Disconnect_Error,0x56,"Motor Disconnect Error")
RCP_ERROR(Gyro_Error,0x57,"Gyro Error")
RCP_ERROR(PT_Initialize_Error_2,0x58,"PT. Initialize Error")
RCP_ERROR(Update_Firmware_Error,0x60,"Update Firmware Error")
RCP_ERROR(Update_Hardware_Error,0x61,"Update Hardware Error")
RCP_ERROR(Update_Error,0x62,"Update Error")
RCP_ERROR(Update_Fan_Error,0x63,"Update Fan Error")

