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

#ifndef __ERROR_H
#define __ERROR_H


#define CAMERA_IS_UNREACHABLE_ERROR 0xFFFF


gboolean camera_is_unreachable (rcp_t *rcp);

gboolean clear_rcp_error (rcp_t *rcp);

gboolean Motor_Driver_Error (rcp_t *rcp);
gboolean Pan_Sensor_Error (rcp_t *rcp);
gboolean Tilt_Sensor_Error (rcp_t *rcp);
gboolean Controller_RX_Over_run_Error (rcp_t *rcp);
gboolean Controller_RX_Framing_Error (rcp_t *rcp);
gboolean Network_RX_Over_run_Error (rcp_t *rcp);
gboolean Network_RX_Framing_Error (rcp_t *rcp);
gboolean Controller_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean Network_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean System_Error (rcp_t *rcp);
gboolean Spec_Limit_Over (rcp_t *rcp);
gboolean ABB_execution_failed (rcp_t *rcp);
gboolean ABB_execution_failed_busy_status (rcp_t *rcp);

gboolean Network_communication_Error (rcp_t *rcp);
gboolean CAMERA_communication_Error (rcp_t *rcp);
gboolean CAMERA_RX_Over_run_Error (rcp_t *rcp);
gboolean CAMERA_RX_Framing_Error (rcp_t *rcp);
gboolean CAMERA_RX_Command_Buffer_Overflow (rcp_t *rcp);

gboolean FPGA_Config_Error (rcp_t *rcp);
gboolean NET_Life_monitoring_Error (rcp_t *rcp);
gboolean BE_Life_monitoring_Error (rcp_t *rcp);
gboolean IF_BE_UART_Buffer_Overflow (rcp_t *rcp);
gboolean IF_BE_UART_Framing_Error (rcp_t *rcp);
gboolean IF_BE_UART_Buffer_Overflow_2 (rcp_t *rcp);
gboolean CAM_Life_monitoring_Error (rcp_t *rcp);
gboolean Fan1_error (rcp_t *rcp);
gboolean Fan2_error (rcp_t *rcp);
gboolean High_Temp (rcp_t *rcp);
gboolean Low_Temp (rcp_t *rcp);
gboolean Temp_Sensor_Error (rcp_t *rcp);
gboolean Lens_Initialize_Error (rcp_t *rcp);
gboolean PT_Initialize_Error (rcp_t *rcp);
gboolean MR_Level_Error (rcp_t *rcp);
gboolean MR_Offset_Error (rcp_t *rcp);
gboolean Origin_Offset_Error (rcp_t *rcp);
gboolean Angle_MR_Sensor_Error (rcp_t *rcp);
gboolean PT_Gear_Error (rcp_t *rcp);
gboolean Motor_Disconnect_Error (rcp_t *rcp);

gboolean log_clear_rcp_error (rcp_t *rcp);

gboolean log_Motor_Driver_Error (rcp_t *rcp);
gboolean log_Pan_Sensor_Error (rcp_t *rcp);
gboolean log_Tilt_Sensor_Error (rcp_t *rcp);
gboolean log_Controller_RX_Over_run_Error (rcp_t *rcp);
gboolean log_Controller_RX_Framing_Error (rcp_t *rcp);
gboolean log_Network_RX_Over_run_Error (rcp_t *rcp);
gboolean log_Network_RX_Framing_Error (rcp_t *rcp);
gboolean log_Controller_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean log_Network_RX_Command_Buffer_Overflow (rcp_t *rcp);
gboolean log_System_Error (rcp_t *rcp);
gboolean log_Spec_Limit_Over (rcp_t *rcp);
gboolean log_ABB_execution_failed (rcp_t *rcp);
gboolean log_ABB_execution_failed_busy_status (rcp_t *rcp);

gboolean log_Network_communication_Error (rcp_t *rcp);
gboolean log_CAMERA_communication_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Over_run_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Framing_Error (rcp_t *rcp);
gboolean log_CAMERA_RX_Command_Buffer_Overflow (rcp_t *rcp);

gboolean log_FPGA_Config_Error (rcp_t *rcp);
gboolean log_NET_Life_monitoring_Error (rcp_t *rcp);
gboolean log_BE_Life_monitoring_Error (rcp_t *rcp);
gboolean log_IF_BE_UART_Buffer_Overflow (rcp_t *rcp);
gboolean log_IF_BE_UART_Framing_Error (rcp_t *rcp);
gboolean log_IF_BE_UART_Buffer_Overflow_2 (rcp_t *rcp);
gboolean log_CAM_Life_monitoring_Error (rcp_t *rcp);
gboolean log_Fan1_error (rcp_t *rcp);
gboolean log_Fan2_error (rcp_t *rcp);
gboolean log_High_Temp (rcp_t *rcp);
gboolean log_Low_Temp (rcp_t *rcp);
gboolean log_Temp_Sensor_Error (rcp_t *rcp);
gboolean log_Lens_Initialize_Error (rcp_t *rcp);
gboolean log_PT_Initialize_Error (rcp_t *rcp);
gboolean log_MR_Level_Error (rcp_t *rcp);
gboolean log_MR_Offset_Error (rcp_t *rcp);
gboolean log_Origin_Offset_Error (rcp_t *rcp);
gboolean log_Angle_MR_Sensor_Error (rcp_t *rcp);
gboolean log_PT_Gear_Error (rcp_t *rcp);
gboolean log_Motor_Disconnect_Error (rcp_t *rcp);

void start_error_log (void);

void stop_error_log (void);


#endif

