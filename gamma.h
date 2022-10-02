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

#ifndef __GAMMA_H
#define __GAMMA_H


extern char gamma_type_tooltip[];
extern char gamma_tooltip[];
extern char drs_tooltip[];


void set_gamma_type (rcp_t *rcp);

void set_gamma (rcp_t *rcp);

void set_drs (rcp_t *rcp);

GtkWidget *create_gamma_frame_AW_HE130 (rcp_t *rcp);

void set_film_rec_dynamic_level (rcp_t *rcp);

void set_film_rec_black_stretch_level_label (rcp_t *rcp);

void set_film_rec_black_stretch_level (rcp_t *rcp);

void set_video_rec_knee_slope (rcp_t *rcp);

void set_video_rec_knee_point_label (rcp_t *rcp);

void set_video_rec_knee_point (rcp_t *rcp);

void set_black_gamma_label (rcp_t *rcp);

void set_black_gamma (rcp_t *rcp);

void set_black_gamma_range (rcp_t *rcp);

GtkWidget *create_gamma_frame_AW_UE150 (rcp_t *rcp);

gboolean update_gamma_type_combo_box (rcp_t *rcp);


#endif

