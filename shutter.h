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

#ifndef __SHUTTER_H
#define __SHUTTER_H


#include "rcp.h"


void set_shutter_type_AW_HE130 (rcp_t *rcp);

void populate_shutter_step_combo_box_AW_HE130 (GtkComboBoxText *combo_box);

void set_shutter_step_AW_HE130 (rcp_t *rcp);

void set_shutter_synchro_label_AW_HE130 (rcp_t *rcp);

void set_shutter_synchro_AW_HE130 (rcp_t *rcp);

GtkWidget *create_shutter_frame_AW_HE130 (rcp_t *rcp);


void set_shutter_type_AW_UE150 (rcp_t *rcp);

void populate_shutter_step_combo_box_AW_UE150 (GtkComboBoxText *combo_box);

void update_shutter_step_combo_box_AW_UE150 (rcp_t *rcp);

void set_shutter_step_AW_UE150 (rcp_t *rcp);

void set_shutter_synchro_label_AW_UE150 (rcp_t *rcp);

void set_shutter_synchro_AW_UE150 (rcp_t *rcp);

void create_shutter_synchro_window_AW_UE150 (rcp_t *rcp);

void set_ELC_limit (rcp_t *rcp);

GtkWidget *create_shutter_frame_AW_UE150 (rcp_t *rcp);


#endif

