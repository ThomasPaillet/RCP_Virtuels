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

#ifndef __DETAIL_H
#define __DETAIL_H


extern char master_detail_tooltip[];
extern char detail_coring_tooltip[];
extern char v_detail_level_tooltip[];
extern char detail_band_tooltip[];
extern char noise_suppress_tooltip[];
extern char level_depend_tooltip[];
extern char knee_aperture_level_tooltip[];
extern char detail_gain_plus_tooltip[];
extern char detail_gain_minus_tooltip[];
extern char fleshtone_noisesup_tooltip[];


void set_master_detail_label (rcp_t *rcp);

void set_master_detail (rcp_t *rcp);

void set_v_detail_level_label (rcp_t *rcp);

void set_v_detail_level (rcp_t *rcp);

void set_detail_band_label (rcp_t *rcp);

void set_detail_band (rcp_t *rcp);

void set_noise_suppress_label (rcp_t *rcp);

void set_noise_suppress (rcp_t *rcp);

void set_fleshtone_noisesup_label (rcp_t *rcp);

void set_fleshtone_noisesup (rcp_t *rcp);

void set_detail_coring_label (rcp_t *rcp);

void set_detail_coring (rcp_t *rcp);

void set_level_depend_label (rcp_t *rcp);

void set_level_depend (rcp_t *rcp);

void set_knee_aperture_level_label (rcp_t *rcp);

void set_knee_aperture_level (rcp_t *rcp);

void set_detail_gain_plus_label (rcp_t *rcp);

void set_detail_gain_plus (rcp_t *rcp);

void set_detail_gain_minus_label (rcp_t *rcp);

void set_detail_gain_minus (rcp_t *rcp);

void set_skin_detail (rcp_t *rcp);

void set_detail (rcp_t *rcp);

GtkWidget *create_detail_frame (rcp_t *rcp);


#endif

