/*
 * copyright (c) 2018 2019 2020 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "../rcp.h"

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


int rs_port = -1;
const char *opened_rs_port_name = NULL;
struct termios sauvegarde;


GSList *list_rs_port (void)
{
	DIR *dir;
	struct dirent *entree;
	char *name;
	size_t name_len;
	GSList *gslist;

	dir = opendir ("/dev");
	if (dir == NULL) return NULL;

	gslist = NULL;
	while ((entree = readdir (dir)) != NULL) {
		if (memcmp (entree->d_name, "ttyUSB", 6) == 0) {
//		if (memcmp (entree->d_name, "tty", 3) == 0) {
			name_len = strlen (entree->d_name) + 6;
			name = g_malloc (name_len);
			name[0] = '/';
			name[1] = 'd';
			name[2] = 'e';
			name[3] = 'v';
			name[4] = '/';
			memcpy (name + 5, entree->d_name, name_len);
			gslist = g_slist_prepend (gslist, name);
		}
	}

	return gslist;
}

void close_rs_port (void)
{
	if (rs_port == -1) return;

	close (rs_port);

	if (opened_rs_port_name != NULL) {
		rs_port = open (opened_rs_port_name, O_RDWR | O_NONBLOCK);
		sauvegarde.c_cflag |= CLOCAL;
		tcsetattr (rs_port, TCSANOW, &sauvegarde);
		close (rs_port);
	}

	rs_port = -1;
	opened_rs_port_name = NULL;
}

int open_rs_port (const char *name)
{
	struct termios configuration;

	rs_port = open (name, O_RDWR | O_NONBLOCK);
	if (rs_port == -1) return -1;

	if (tcgetattr (rs_port, &configuration) != 0) { close_rs_port (); return -1; }
	configuration.c_cflag &= ~CLOCAL;
	if (tcsetattr (rs_port, TCSANOW, &configuration) != 0) { close_rs_port (); return -1; }

	rs_port = open (name, O_RDWR);
	if (rs_port == -1) return -1;

	if (tcgetattr (rs_port, &configuration) != 0) { close_rs_port (); return -1; }
	memcpy (&sauvegarde, &configuration, sizeof (struct termios));
	opened_rs_port_name = name;

	cfmakeraw (&configuration);
	cfsetispeed (&configuration, B38400);
	cfsetospeed (&configuration, B38400);
	configuration.c_cflag &= ~PARENB;
	configuration.c_cflag &= ~CSIZE;
	configuration.c_cflag |= CS8;
	configuration.c_cflag &= ~CSTOPB;
	configuration.c_cflag &= ~CLOCAL;
	configuration.c_cflag |= HUPCL;
	configuration.c_cflag |= CRTSCTS;

	if (tcsetattr (rs_port, TCSANOW, &configuration) != 0) { close_rs_port (); return -1; }

	return 0;
}

void send_to_rs_port (const char *buffer, long unsigned int buffer_len)
{
	if (rs_port == -1) return;

	if (write (rs_port, buffer, buffer_len) == -1) g_idle_add ((GSourceFunc)show_rs_connection_error_window, NULL);
}

int receive_from_rs_port (char *buffer, long unsigned int buffer_len)
{
	if (rs_port == -1) return 0;

	if (read (rs_port, buffer, buffer_len) == -1) {
		g_idle_add ((GSourceFunc)show_rs_connection_error_window, NULL);

		return 0;
	}

	return 1;
}

