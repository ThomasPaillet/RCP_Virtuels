/*
 * copyright (c) 2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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

#ifndef __KNEE_H
#define __KNEE_H


#include "rcp.h"


extern char auto_knee_response_tooltip[];


void set_knee_point_label (rcp_t *rcp);

void set_knee_point (rcp_t *rcp);

void set_knee_slope_label (rcp_t *rcp);

void set_knee_slope (rcp_t *rcp);

void set_auto_knee_response_label (rcp_t *rcp);

void set_auto_knee_response (rcp_t *rcp);

void set_knee_settings (rcp_t *rcp);

GtkWidget *create_knee_frame (rcp_t *rcp);

void set_HLG_knee_point_label (rcp_t *rcp);

void set_HLG_knee_point (rcp_t *rcp);

void set_HLG_knee_slope_label (rcp_t *rcp);

void set_HLG_knee_slope (rcp_t *rcp);

void set_HLG_knee (rcp_t *rcp);

GtkWidget *create_HLG_knee_frame (rcp_t *rcp);


#endif

