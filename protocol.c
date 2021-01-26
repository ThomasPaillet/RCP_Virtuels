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

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>	//gethostname usleep

#if defined (__linux)
#include <sys/ioctl.h>
#include <net/if.h>
#endif


char *http_ptz_cmd = "GET /cgi-bin/aw_ptz?cmd=";	//strlen = 24
char *http_cam_cmd = "GET /cgi-bin/aw_cam?cmd=";	//strlen = 24

char *http_update_start_cmd = "GET /cgi-bin/event?connect=start&my_port=";	//strlen = 41
char *http_update_stop_cmd = "GET /cgi-bin/event?connect=stop&my_port=";	//strlen = 40

char *http_abb_cmd = "GET /cgi-bin/aw_cam?cmd=OAS&res=0";	//strlen = 33

char *http_camera_cmd = "GET /cgi-bin/camera?resolution=1920&quality=1&page=";	//strlen = 51

char *http_getuid_cmd = "GET /cgi-bin/getuid?FILE=2&vcodec=h264_2&reply=info";	//strlen = 51

char *http_h264_start_cmd = "GET /cgi-bin/h264?connect=start&protocol=rtp&stream=2&my_port=40000&UID=";	//strlen = 72
char *http_h264_stop_cmd = "GET /cgi-bin/h264?connect=stop&protocol=rtp&my_port=40000&UID=";	//strlen = 62

char *http_keep_alive_cmd = "GET /cgi-bin/keep_alive?mode=h264&protocol=rtp&UID=";	//strlen = 51

char *http_cam_ptz_header = "&res=1";	//strlen = 6
char *http_update_header = "&uid=0";	//strlen = 6
char *http_header_1 = " HTTP/1.1\r\nAccept: image/gif, ... (omitted) ... , */*\r\nReferer: http://";	//strlen = 71
char *http_header_2 = "/\r\nAccept-Language: en\r\nAccept-Encoding: gzip, deflate\r\nUser-Agent: AW-Cam Controller\r\nHost: ";	//strlen = 93
char *http_header_3 = "\r\nConnection: Keep-Alive\r\n\r\n";	//strlen = 28

char *http_header;
int http_header_size;
int full_http_header_size;

char my_ip_address[16];

char user_id[16];
int user_id_len = 0;


#define COMMAND_FUNC_END \
		gettimeofday (&rcp->last_time, NULL); \
 \
	} else { \
		rcp->error_code = 0x30; \
		g_idle_add ((GSourceFunc)camera_is_unreachable, rcp); \
	} \
 \
	closesocket (sock); \
 \
g_mutex_unlock (&rcp->cmd_mutex); \
}


void init_protocol (void)
{
#ifdef _WIN32
	char host_name[256];
	char **pp;
	struct hostent *host;

	gethostname (host_name, 256);
	host = gethostbyname (host_name);

	for (pp = host->h_addr_list; *pp != NULL; pp++) {
		strcpy (my_ip_address, inet_ntoa (*(struct in_addr *)*pp));
	}
#elif defined (__linux)
	SOCKET sock;
	struct ifconf ip_interfaces;
	int i;

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ip_interfaces.ifc_len = 0;
	ip_interfaces.ifc_req = NULL;
	ioctl (sock, SIOCGIFCONF, &ip_interfaces);
	ip_interfaces.ifc_req = g_malloc (ip_interfaces.ifc_len);
	ioctl (sock, SIOCGIFCONF, &ip_interfaces);

	for (i = 0; i < ip_interfaces.ifc_len / sizeof (struct ifreq); i++) {
		strcpy (my_ip_address, inet_ntoa (((struct sockaddr_in *)&ip_interfaces.ifc_req[i].ifr_addr)->sin_addr));
	}
#endif

	http_header = g_malloc (152);
	http_header_size = sprintf (http_header, "%s%s%s%s", my_ip_address, http_header_2, my_ip_address, http_header_3);
	full_http_header_size = http_header_size + 101;
}

void init_rcp_cmd_buffer (rcp_t *rcp)
{
	memcpy (rcp->cmd_buffer + 39, http_cam_ptz_header, 6);
	memcpy (rcp->cmd_buffer + 45, http_header_1, 71);
	memcpy (rcp->cmd_buffer + 116, http_header, http_header_size);
	rcp->cmd_buffer[116 + http_header_size] = '\0';

	rcp->last_cmd = rcp->cmd_buffer + 13;
	memcpy (rcp->last_cmd, http_ptz_cmd, 24);
	rcp->cam_ptz = FALSE;
}

void send_cam_request_command (rcp_t *rcp, char* cmd, int *response)
{
	int size, index;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;
	char buffer[264];

g_mutex_lock (&rcp->cmd_mutex);

	size = 3;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

		index = 0;
		size = recv (sock, buffer, sizeof (buffer), 0);

		while (size > 0) {
			index += size;
			size = recv (sock, buffer + index, sizeof (buffer) - index, 0);
		}
		buffer[index] = '\0';

		index--;
		while ((buffer[index] != ':') && (buffer[index] != '\n')) index--;
		index++;

		sscanf (buffer + index, "%x", response);

COMMAND_FUNC_END

void send_cam_request_command_string (rcp_t *rcp, char* cmd, char *response)
{
	int size, index;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;
	char buffer[264];

g_mutex_lock (&rcp->cmd_mutex);

	size = 3;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

		index = 0;
		size = recv (sock, buffer, sizeof (buffer), 0);

		while (size > 0) {
			index += size;
			size = recv (sock, buffer + index, sizeof (buffer) - index, 0);
		}
		buffer[index] = '\0';

		index--;
		while ((buffer[index] != ':') && (buffer[index] != '\n')) index--;
		index++;

		strcpy (response, buffer + index);

COMMAND_FUNC_END

void send_cam_control_command (rcp_t *rcp, char* cmd)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	size = 5;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size]= '\0';
	rcp->last_ctrl_cmd_len = size;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

COMMAND_FUNC_END

void send_cam_control_command_string (rcp_t *rcp, char* cmd, char* value)
{
	int size, index;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	size = 1;
	while (value[size] != '\0') size++;

	memcpy (rcp->cmd_buffer + 39 - size, value, size);

	index = 4;
	size += 4;
	while (cmd[index] != '\0') { index++; size++; }

	memcpy (rcp->last_ctrl_cmd, cmd, index);
	memcpy (rcp->last_ctrl_cmd + index, value, size - index);
	rcp->last_ctrl_cmd[size]= '\0';
	rcp->last_ctrl_cmd_len = size;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, index);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

COMMAND_FUNC_END

void send_cam_control_command_1_digit (rcp_t *rcp, char* cmd, int value)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	rcp->cmd_buffer[38] = (char)(value + 48);

	size = 4;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size] = (char)(value + 48);
	rcp->last_ctrl_cmd[size + 1] = '\0';
	rcp->last_ctrl_cmd_len = size + 1;

	memcpy (rcp->cmd_buffer + 38 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 14 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + 1 + full_http_header_size, 0);

COMMAND_FUNC_END

void send_cam_control_command_2_digits (rcp_t *rcp, char* cmd, int value, gboolean wait)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	sprintf (rcp->cmd_buffer + 37, "%02X", value);
	rcp->cmd_buffer[39] = '&';

	size = 4;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size] = rcp->cmd_buffer[37];
	rcp->last_ctrl_cmd[size + 1] = rcp->cmd_buffer[38];
	rcp->last_ctrl_cmd[size + 2] = '\0';
	rcp->last_ctrl_cmd_len = size + 2;

	memcpy (rcp->cmd_buffer + 37 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 13 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (wait) {
		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);
	}

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + 2 + full_http_header_size, 0);

COMMAND_FUNC_END

void send_cam_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	sprintf (rcp->cmd_buffer + 36, "%03X", value);
	rcp->cmd_buffer[39] = '&';

	size = 4;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size] = rcp->cmd_buffer[36];
	rcp->last_ctrl_cmd[size + 1] = rcp->cmd_buffer[37];
	rcp->last_ctrl_cmd[size + 2] = rcp->cmd_buffer[38];
	rcp->last_ctrl_cmd[size + 3] = '\0';
	rcp->last_ctrl_cmd_len = size + 3;

	memcpy (rcp->cmd_buffer + 36 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 12 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == FALSE)) {
		memcpy (http_cmd, http_cam_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = TRUE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (wait) {
		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);
	}

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + 3 + full_http_header_size, 0);

COMMAND_FUNC_END

void send_ptz_request_command (rcp_t *rcp, char* cmd, int *response)
{
	int size, index;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;
	char buffer[264];

g_mutex_lock (&rcp->cmd_mutex);

	size = 2;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == TRUE)) {
		memcpy (http_cmd, http_ptz_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = FALSE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

		index = 0;
		size = recv (sock, buffer, sizeof (buffer), 0);

		while (size > 0) {
			index += size;
			size = recv (sock, buffer + index, sizeof (buffer) - index, 0);
		}
		buffer[index] = '\0';

		*response = buffer[--index] - 48;

		gettimeofday (&rcp->last_time, NULL);
	} else {
		*response = 0;
		rcp->error_code = 0x30;
		g_idle_add ((GSourceFunc)camera_is_unreachable, rcp);
	}

	closesocket (sock);

g_mutex_unlock (&rcp->cmd_mutex);
}

void send_ptz_request_command_string (rcp_t *rcp, char* cmd, char *response)
{
	int size, index;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;
	char buffer[264];

g_mutex_lock (&rcp->cmd_mutex);

	size = 3;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == TRUE)) {
		memcpy (http_cmd, http_ptz_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = FALSE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

		index = 0;
		size = recv (sock, buffer, sizeof (buffer), 0);

		while (size > 0) {
			index += size;
			size = recv (sock, buffer + index, sizeof (buffer) - index, 0);
		}
		buffer[index] = '\0';

		index--;
		while (buffer[index] != '\n') index--;
		index++;

		strcpy (response, buffer + index);

COMMAND_FUNC_END

void send_ptz_control_command (rcp_t *rcp, char* cmd)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	size = 3;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size] = '\0';
	rcp->last_ctrl_cmd_len = size;

	memcpy (rcp->cmd_buffer + 39 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 15 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == TRUE)) {
		memcpy (http_cmd, http_ptz_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = FALSE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + full_http_header_size, 0);

COMMAND_FUNC_END

void send_ptz_control_command_3_digits (rcp_t *rcp, char* cmd, int value, gboolean wait)
{
	int size;
	char *http_cmd;
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	sprintf (rcp->cmd_buffer + 36, "%03X", value);
	rcp->cmd_buffer[39] = '&';

	size = 4;
	while (cmd[size] != '\0') size++;

	memcpy (rcp->last_ctrl_cmd, cmd, size);
	rcp->last_ctrl_cmd[size] = rcp->cmd_buffer[36];
	rcp->last_ctrl_cmd[size + 1] = rcp->cmd_buffer[37];
	rcp->last_ctrl_cmd[size + 2] = rcp->cmd_buffer[38];
	rcp->last_ctrl_cmd[size + 3] = '\0';
	rcp->last_ctrl_cmd_len = size + 3;

	memcpy (rcp->cmd_buffer + 36 - size, cmd, size);
	http_cmd = rcp->cmd_buffer + 12 - size;
	if ((http_cmd != rcp->last_cmd) || (rcp->cam_ptz == TRUE)) {
		memcpy (http_cmd, http_ptz_cmd, 24);
		rcp->last_cmd = http_cmd;
		rcp->cam_ptz = FALSE;
	}

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (wait) {
		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);
	}

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, http_cmd, size + 3 + full_http_header_size, 0);

COMMAND_FUNC_END

gboolean send_ABB_execution_control_command (rcp_t *rcp)
{
	char buffer[264];
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	memcpy (buffer, http_abb_cmd, 33);
	memcpy (buffer + 33, http_header_1, 71);
	memcpy (buffer + 104, http_header, http_header_size);
	buffer[104 + http_header_size] = '\0';

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, buffer, 104 + http_header_size, 0);

		gettimeofday (&rcp->last_time, NULL);
		closesocket (sock);
g_mutex_unlock (&rcp->cmd_mutex);

		return TRUE;
	} else {
		closesocket (sock);
g_mutex_unlock (&rcp->cmd_mutex);

		return FALSE;
	}
}

void send_update_start_cmd (rcp_t *rcp)
{
	int size, index;
	char buffer[280];
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	memcpy (buffer, http_update_start_cmd, 41);
	index = 41 + sprintf (buffer + 41, "%hu", ntohs (update_notification_address.sin_port));
	memcpy (buffer + index, http_update_header, 6);
	index += 6;
	memcpy (buffer + index, http_header_1, 71);
	index += 71;
	memcpy (buffer + index, http_header, http_header_size);
	size = index + http_header_size;
	buffer[size] = '\0';

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, buffer, size, 0);

		if (rcp->error_code == 0x30) {
			rcp->error_code = 0x00;
			g_idle_add ((GSourceFunc)clear_rcp_error, rcp);
		}

COMMAND_FUNC_END

void send_update_stop_cmd (rcp_t *rcp)
{
	int size, index;
	char buffer[280];
	SOCKET sock;
	struct timeval current_time, elapsed_time;

g_mutex_lock (&rcp->cmd_mutex);

	memcpy (buffer, http_update_stop_cmd, 40);
	index = 40 + sprintf (buffer + 40, "%hu", ntohs (update_notification_address.sin_port));
	memcpy (buffer + index, http_update_header, 6);
	index += 6;
	memcpy (buffer + index, http_header_1, 71);
	index += 71;
	memcpy (buffer + index, http_header, http_header_size);
	size = index + http_header_size;
	buffer[size] = '\0';

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, buffer, size, 0);

COMMAND_FUNC_END

gpointer send_jpeg_image_request_cmd (rcp_t *rcp)
{
	int size, index;
	char buffer[8192];
	SOCKET sock;
	struct timeval current_time, elapsed_time;
	struct tm *time;
	FILE *jpeg_file;
	char jpeg_file_name[40];

g_mutex_lock (&rcp->cmd_mutex);

	memcpy (buffer, http_camera_cmd, 51);
	size = sprintf (buffer + 51, "%d", rcp->jpeg_page);
	rcp->jpeg_page++;
	memcpy (buffer + 51 + size, http_header_1, 71);
	memcpy (buffer + 122 + size, http_header, http_header_size);
	buffer[122 + size + http_header_size] = '\0';

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);
	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) usleep (130000 - elapsed_time.tv_usec);

	if (connect (sock, (struct sockaddr *) &rcp->address, sizeof (struct sockaddr_in)) == 0) {
		send (sock, buffer, 122 + size + http_header_size, 0);

		time = localtime (&current_time.tv_sec);
		sprintf (jpeg_file_name, "20%02d-%02d-%02d %02d_%02d_%02d Camera %s.jpg", time->tm_year - 100, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec, rcp->name);
		jpeg_file = fopen (jpeg_file_name, "wb");

		recv (sock, buffer, sizeof (buffer), 0);
		size = recv (sock, buffer, sizeof (buffer), 0);
		size += recv (sock, buffer + size, sizeof (buffer) - size, 0);

		index = 0;
		do {
			while (buffer[index] != '\n') index++;
			index++;
		} while (buffer[index] != '\r');
		index++;
		index++;
		fwrite (buffer + index, 1, size - index, jpeg_file);

		size = recv (sock, buffer, sizeof (buffer), 0);
		while (size > 0) {
			fwrite (buffer, 1, size, jpeg_file);
			size = recv (sock, buffer, sizeof (buffer), 0);
		}
		fclose (jpeg_file);

		gettimeofday (&rcp->last_time, NULL);

	} else {
		rcp->error_code = 0x30;
		g_idle_add ((GSourceFunc)camera_is_unreachable, rcp);
	}

	closesocket (sock);

g_mutex_unlock (&rcp->cmd_mutex);

	return NULL;
}

