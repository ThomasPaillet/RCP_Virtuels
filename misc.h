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

#ifndef __MISC_H
#define __MISC_H


void init_rcp (rcp_t *rcp);

gboolean rcp_start_working (rcp_t *rcp);

//gboolean set_rcp_on (rcp_t *rcp);

gboolean set_rcp_off (rcp_t *rcp);

gpointer check_if_camera_is_on (rcp_t *rcp);

gpointer start_rcp (rcp_t *rcp);

void copy_rcp (rcp_t *rcp_dest, rcp_t *rcp_src);

gboolean rcp_work_end (rcp_t *rcp);

gboolean remove_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp);

gboolean remove_green_timeout (GtkWidget *button, GdkEventButton *event, rcp_t *rcp);


#endif

