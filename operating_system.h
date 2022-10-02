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

#ifndef __OPERATING_SYSTEM_H
#define __OPERATING_SYSTEM_H


#include <gtk/gtk.h>


GSList *list_rs_port (void);

void close_rs_port (void);

int open_rs_port (const char *name);

void send_to_rs_port (const char *buffer, unsigned long buffer_len);

int receive_from_rs_port (char *buffer, unsigned long buffer_len);


#endif

