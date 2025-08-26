/*
 * copyright (c) 2020 2021 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "logging.h"

#include <stdio.h>
#include "f_sync.h"


gboolean logging = TRUE;

gboolean log_panasonic = TRUE;
gboolean log_sw_p_08 = FALSE;
gboolean log_tsl_umd_v5 = TRUE;

gboolean fsync_log = FALSE;

GMutex logging_mutex;

FILE *main_log_file;

FILE *panasonic_log_file;
FILE *panasonic_errors_log_file;
FILE *sw_p_08_log_file;
FILE *tsl_umd_v5_log_file;

char *log_buffer = NULL;
int log_buffer_size = 0;


void log_int (const char *c_source_filename, int i, FILE *log_file)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] %d\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, i);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (log_file);
	}

	g_date_time_unref (current_time);
}

void log_string (const char *c_source_filename, const char *str, FILE *log_file)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] %s\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, str);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (log_file);
	}

	g_date_time_unref (current_time);
}

void log_2_strings (const char *c_source_filename, const char *str1, const char *str2, FILE *log_file)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] %s%s\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, str1, str2);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (log_file);
	}

	g_date_time_unref (current_time);
}

void log_string_int (const char *c_source_filename, const char *str, int i, FILE *log_file)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] %s%d\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, str, i);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (log_file);
	}

	g_date_time_unref (current_time);
}

void log_rcp_string (const char *c_source_filename, rcp_t *rcp, const char *str)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Camera %s (%s) %s\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, str);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (panasonic_log_file);
	}

	g_date_time_unref (current_time);
}

void log_rcp_command (const char *c_source_filename, rcp_t *rcp, const char *cmd)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Send command to camera %s (%s)       --> %s\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, cmd);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (panasonic_log_file);
	}

	g_date_time_unref (current_time);
}

void log_rcp_ER1 (const char *c_source_filename, GDateTime *current_time, rcp_t *rcp, const char *str)
{
	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Camera %s (%s) ERROR 1 (%s): The command is not supported by the camera.\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, str);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_errors_log_file);

	if (fsync_log) F_SYNC (panasonic_errors_log_file);
}

void log_rcp_ER2 (const char *c_source_filename, GDateTime *current_time, rcp_t *rcp, const char *str)
{
	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Camera %s (%s) ERROR 2 (%s): The camera is in the busy status.\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, str);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_errors_log_file);

	if (fsync_log) F_SYNC (panasonic_errors_log_file);
}

void log_rcp_ER3 (const char *c_source_filename, GDateTime *current_time, rcp_t *rcp, const char *str)
{
	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Camera %s (%s) ERROR 3 (%s): The data value of the command is outside the acceptable range.\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, str);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_errors_log_file);

	if (fsync_log) F_SYNC (panasonic_errors_log_file);
}

void log_rcp_response (const char *c_source_filename, rcp_t *rcp, const char *response, int len)
{
	GDateTime *current_time;
	int index;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Receive response from camera %s (%s) <-- ", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address);

	memcpy (log_buffer + log_buffer_size, response, len);
	log_buffer_size += len;

	log_buffer[log_buffer_size++] = '\n';
	log_buffer[log_buffer_size++] = '\n';

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);

	index = len - 1;
	while (response[index] != '\n') index--;
	index++;

	if (((response[index] == 'e') || (response[index] == 'E')) && (response[index + 1] == 'R')) {
		if (response[index + 2] == '1') log_rcp_ER1 (__FILE__, current_time, rcp, response + index);
		else if (response[index + 2] == '2') log_rcp_ER2 (__FILE__, current_time, rcp, response + index);
		else if (response[index + 2] == '3') log_rcp_ER3 (__FILE__, current_time, rcp, response + index); 
	}

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (panasonic_log_file);
	}

	g_date_time_unref (current_time);
}

void log_rcp_update_notification (const char *c_source_filename, const char *ip_address, const char *update_notification)
{
	GDateTime *current_time;
	int len;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Receive Update Notification from %s <-- ", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, ip_address);

	len = ((((int)((unsigned char *)update_notification)[22])) << 8) + ((unsigned char *)update_notification)[23] - 8;

	memcpy (log_buffer + log_buffer_size, update_notification + 30, len);
	log_buffer_size += len;

	log_buffer[log_buffer_size++] = '\n';
	log_buffer[log_buffer_size++] = '\n';

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (panasonic_log_file);
	}

	g_date_time_unref (current_time);
}

void log_rcp_error (const char *c_source_filename, rcp_t *rcp, const char *str)
{
	GDateTime *current_time;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Camera %s (%s) %s (%02Xh)\n\n", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, rcp->name, rcp->ip_address, str, rcp->error_code);

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_log_file);
	fwrite (log_buffer, log_buffer_size, 1, panasonic_errors_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (panasonic_log_file);
		F_SYNC (panasonic_errors_log_file);
	}

	g_date_time_unref (current_time);
}

void log_sw_p_08_outgoing_message (const char *c_source_filename, const char *ip_address, const char *sw_p_08_message, int len)
{
	GDateTime *current_time;
	int i;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Send SW-P-08 message to %s      -->", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, ip_address);

	for (i = 0; i < len; i++) {
		sprintf (log_buffer + log_buffer_size, " %02X", sw_p_08_message[i]);
		log_buffer_size += 3;
	}

	log_buffer[log_buffer_size++] = '\n';
	log_buffer[log_buffer_size++] = '\n';

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, sw_p_08_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (sw_p_08_log_file);
	}

	g_date_time_unref (current_time);
}

void log_sw_p_08_incomming_message (const char *c_source_filename, const char *ip_address, const char *sw_p_08_message, int len)
{
	GDateTime *current_time;
	int i;

	current_time = g_date_time_new_now_local ();

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [%s] Receive SW-P-08 message from %s <--", g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, c_source_filename, ip_address);

	for (i = 0; i < len; i++) {
		sprintf (log_buffer + log_buffer_size, " %02X", sw_p_08_message[i]);
		log_buffer_size += 3;
	}

	log_buffer[log_buffer_size++] = '\n';
	log_buffer[log_buffer_size++] = '\n';

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, sw_p_08_log_file);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (sw_p_08_log_file);
	}

	g_date_time_unref (current_time);
}

void log_tsl_umd_v5_packet (const char *packet)
{
	GDateTime *current_time;
	guint16 total_byte_count, length;
	int ptr;

	current_time = g_date_time_new_now_local ();

	total_byte_count = *((guint16 *)packet);
	ptr = 6;

	log_buffer_size = sprintf (log_buffer, "%02dh %02dm %02ds %03dms: [tsl_umd_v5.c] Receive TSL UMD V5 packet\n" \
			"\tTotal byte count of following packet: %d\n" \
			"\tMinor version number: %d\n" \
			"\tFlags: 0x%02X\n" \
			"\tPrimary index (screen): %d\n\n", \
			g_date_time_get_hour (current_time), g_date_time_get_minute (current_time), g_date_time_get_second (current_time), g_date_time_get_microsecond (current_time) / 1000, total_byte_count, *((guint8 *)(packet + 2)), *((guint8 *)(packet + 3)), *((guint16 *)(packet + 4)));

	fwrite (log_buffer, log_buffer_size, 1, main_log_file);
	fwrite (log_buffer, log_buffer_size, 1, tsl_umd_v5_log_file);

	do {
		length = *((guint16 *)(packet + ptr + 4));

		log_buffer_size = sprintf (log_buffer, "\tDisplay Message\n" \
				"\tDisplay index: %d\n" \
				"\tTally data: 0x%04X\n" \
				"\tByte count of following text: %d\n" \
				"\tUMD text: ", \
				*((guint16 *)(packet + ptr)), *((guint16 *)(packet + ptr + 2)), length);

		fwrite (log_buffer, log_buffer_size, 1, main_log_file);
		fwrite (log_buffer, log_buffer_size, 1, tsl_umd_v5_log_file);

		if (length > 0) {
			fwrite (packet + ptr + 6, length, 1, main_log_file);
			fwrite (packet + ptr + 6, length, 1, tsl_umd_v5_log_file);
		}

		fwrite ("\n\n", 2, 1, main_log_file);
		fwrite ("\n\n", 2, 1, tsl_umd_v5_log_file);

		ptr += 6 + length;
	} while (ptr <= total_byte_count - 6);

	if (fsync_log) {
		F_SYNC (main_log_file);
		F_SYNC (tsl_umd_v5_log_file);
	}

	g_date_time_unref (current_time);
}

void init_logging (void)
{
	g_mutex_init (&logging_mutex);

	if (logging) start_logging ();
}

void start_logging (void)
{
	GDateTime *current_time;
	int year, month, day;
	char log_file_name[56];

	g_mutex_lock (&logging_mutex);

	log_buffer = g_malloc (2176);

	current_time = g_date_time_new_now_local ();

	year = g_date_time_get_year (current_time);
	month = g_date_time_get_month (current_time);
	day = g_date_time_get_day_of_month (current_time);

	sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels.log", year, month, day);
	main_log_file = fopen (log_file_name, "a");

	if (log_panasonic) {
		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_Panasonic.log", year, month, day);
		panasonic_log_file = fopen (log_file_name, "a");

		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_Panasonic_errors.log", year, month, day);
		panasonic_errors_log_file = fopen (log_file_name, "a");
	}

	if (log_sw_p_08) {
		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_SW-P-08.log", year, month, day);
		sw_p_08_log_file = fopen (log_file_name, "a");
	}

	if (log_tsl_umd_v5) {
		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_TSL-UMD-V5.log", year, month, day);
		tsl_umd_v5_log_file = fopen (log_file_name, "a");
	}

	g_date_time_unref (current_time);

	logging = TRUE;

	g_mutex_unlock (&logging_mutex);
}

void start_panasonic_log (void)
{
	GDateTime *current_time;
	int year, month, day;
	char log_file_name[56];

	g_mutex_lock (&logging_mutex);

	if (logging) {
		current_time = g_date_time_new_now_local ();

		year = g_date_time_get_year (current_time);
		month = g_date_time_get_month (current_time);
		day = g_date_time_get_day_of_month (current_time);

		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_Panasonic.log", year, month, day);
		panasonic_log_file = fopen (log_file_name, "a");

		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_Panasonic_errors.log", year, month, day);
		panasonic_log_file = fopen (log_file_name, "a");

		g_date_time_unref (current_time);
	}

	log_panasonic = TRUE;

	g_mutex_unlock (&logging_mutex);
}

void start_sw_p_08_log (void)
{
	GDateTime *current_time;
	char log_file_name[56];

	g_mutex_lock (&logging_mutex);

	if (logging) {
		current_time = g_date_time_new_now_local ();

		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_SW-P-08.log", g_date_time_get_year (current_time), g_date_time_get_month (current_time), g_date_time_get_day_of_month (current_time));
		sw_p_08_log_file = fopen (log_file_name, "a");

		g_date_time_unref (current_time);
	}

	log_sw_p_08 = TRUE;

	g_mutex_unlock (&logging_mutex);
}

void start_tsl_umd_v5_log (void)
{
	GDateTime *current_time;
	char log_file_name[56];

	g_mutex_lock (&logging_mutex);

	if (logging) {
		current_time = g_date_time_new_now_local ();

		sprintf (log_file_name, "%04d-%02d-%02d_RCP-Virtuels_TSL-UMD-V5.log", g_date_time_get_year (current_time), g_date_time_get_month (current_time), g_date_time_get_day_of_month (current_time));
		tsl_umd_v5_log_file = fopen (log_file_name, "a");

		g_date_time_unref (current_time);
	}

	log_tsl_umd_v5 = TRUE;

	g_mutex_unlock (&logging_mutex);
}

void stop_logging (void)
{
	g_mutex_lock (&logging_mutex);

	logging = FALSE;

	if (log_tsl_umd_v5) fclose (tsl_umd_v5_log_file);
	if (log_sw_p_08) fclose (sw_p_08_log_file);
	if (log_panasonic) {
		fclose (panasonic_errors_log_file);
		fclose (panasonic_log_file);
	}

	fclose (main_log_file);

	g_free (log_buffer);

	g_mutex_unlock (&logging_mutex);
}

void stop_panasonic_log (void)
{
	g_mutex_lock (&logging_mutex);

	log_panasonic = FALSE;

	if (logging) {
		fclose (panasonic_errors_log_file);
		fclose (panasonic_log_file);
	}

	g_mutex_unlock (&logging_mutex);
}

void stop_sw_p_08_log (void)
{
	g_mutex_lock (&logging_mutex);

	log_sw_p_08 = FALSE;

	if (logging) fclose (sw_p_08_log_file);

	g_mutex_unlock (&logging_mutex);
}

void stop_tsl_umd_v5_log (void)
{
	g_mutex_lock (&logging_mutex);

	log_tsl_umd_v5 = FALSE;

	if (logging) fclose (tsl_umd_v5_log_file);

	g_mutex_unlock (&logging_mutex);
}

