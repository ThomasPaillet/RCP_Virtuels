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

#ifndef __SCENES_H
#define __SCENES_H


#include "cameras_set.h"


extern char scenes_label[];

extern GtkWidget *scenes_page;
extern GtkWidget *source_cameras_set_list_box;
extern GtkWidget *source_rcp_list_box_box;
extern GtkWidget *copy_button;
extern GtkWidget *destination_cameras_set_list_box;
extern GtkWidget *destination_rcp_list_box_box;

extern cameras_set_t *source_cameras_set;
extern cameras_set_t *destination_cameras_set;


void source_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_cameras_set_row);
void destination_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_cameras_set_row);

gint rcp_list_box_sort (GtkListBoxRow *row1, GtkListBoxRow *row2, cameras_set_t *cameras_set);

void source_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_rcp_row);
void destination_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_rcp_row);

void create_scenes_page (void);


#endif

