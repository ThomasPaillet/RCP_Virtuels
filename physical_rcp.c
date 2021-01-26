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
FILE *debug_file;


/*char *http_ptz_cmd = "GET /cgi-bin/aw_ptz?cmd=";	//strlen = 24
char *http_cam_cmd = "GET /cgi-bin/aw_cam?cmd=";	//strlen = 24*/

char *answer = "HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Type: text/plain\r\nContent-length: 0\r\n\r\n";

SOCKET physical_rcp_socket;
struct sockaddr_in physical_rcp_address;

struct sockaddr_in remote_rcp_address;


gpointer physical_rcp_server (gpointer data)
{
#ifdef _WIN32
	int addrlen;
#elif defined (__linux)
	socklen_t addrlen;
#endif
	char buffer[556];
	SOCKET src_socket;
	struct sockaddr_in src_addr;
/*	GList *glist_itr;
	rcp_t *rcp;*/

int msg_len;

	while (TRUE) {
printf ("%ld\n", strlen (answer));
		addrlen = sizeof (struct sockaddr_in);
		src_socket = accept (physical_rcp_socket, (struct sockaddr *)&src_addr, &addrlen);

		if (src_socket == INVALID_SOCKET) break;

		remote_rcp_address.sin_addr.s_addr = src_addr.sin_addr.s_addr;

		msg_len = recv (src_socket, buffer, 556, 0);

buffer[msg_len] = '\0';
fprintf (debug_file, "%s\n", buffer);
fflush (debug_file);
		send (src_socket, answer, strlen (answer), 0);

		closesocket (src_socket);

	}

	return NULL;
}

void init_physical_rcp (void)
{
	memset (&physical_rcp_address, 0, sizeof (struct sockaddr_in));
	physical_rcp_address.sin_family = AF_INET;
	physical_rcp_address.sin_port = htons (80);
	physical_rcp_address.sin_addr.s_addr = inet_addr (my_ip_address);

	memset (&remote_rcp_address, 0, sizeof (struct sockaddr_in));
	remote_rcp_address.sin_family = AF_INET;
	remote_rcp_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);
	remote_rcp_address.sin_addr.s_addr = INADDR_NONE;

debug_file = fopen ("debug.txt", "w");
}

void start_physical_rcp (void)
{
	physical_rcp_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind (physical_rcp_socket, (struct sockaddr *)&physical_rcp_address, sizeof (struct sockaddr_in)) == 0) {
		if (listen (physical_rcp_socket, 1) == 0) {
			g_thread_new (NULL, physical_rcp_server, NULL);
		}
	}
}

