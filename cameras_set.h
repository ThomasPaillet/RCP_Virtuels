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

#ifndef __CAMERAS_SET_H
#define __CAMERAS_SET_H


#include "master_rcp.h"


#define CAMERAS_SET_NAME_LENGTH 20
#define MAX_CAMERAS_SET 8
#define MAX_CAMERAS 15


typedef struct cameras_set_s {
	char name[CAMERAS_SET_NAME_LENGTH + 1];

	int number_of_cameras;
	rcp_t **rcp_ptr_array;

	master_rcp_t master_rcp;

	GtkWidget *rcp_box;
	GtkWidget *page;
	gint page_num;

	GtkWidget *source_list_box_row;
	GtkWidget *destination_list_box_row;

	GtkWidget *source_rcp_list_box;
	GtkWidget *destination_rcp_list_box;

	struct cameras_set_s *next;
} cameras_set_t;


extern char *cameras_set_label;
extern char *cameras_label;

extern GMutex cameras_sets_mutex;
extern GMutex current_cameras_set_mutex;

extern int number_of_cameras_sets;

extern cameras_set_t *cameras_sets;

extern cameras_set_t *current_cameras_set;

extern cameras_set_t *new_cameras_set;

extern cameras_set_t *cameras_set_with_error;


void init_master_rcp (cameras_set_t *cameras_set);


void show_cameras_set_configuration_window (void);

void add_cameras_set (void);

void delete_cameras_set (GtkButton *button, GtkWidget *confirmation_window);


#endif

