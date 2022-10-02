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

#ifndef __TALLY_H
#define __TALLY_H


#define TSL_UMD_V5_UDP_PORT 8900


extern struct sockaddr_in tsl_umd_v5_address;


gboolean name_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp);

gboolean tally_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp);

gboolean g_source_rcp_queue_draw (rcp_t *rcp);

void init_tally (void);

void start_tally (void);

void stop_tally (void);


#endif

