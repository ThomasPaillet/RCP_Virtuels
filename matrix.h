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

#ifndef __MATRIX_H
#define __MATRIX_H


extern void (*set_cc_saturation_label_funcs[15]) (rcp_t *rcp);
extern void (*set_cc_phase_label_funcs[15]) (rcp_t *rcp);


void set_linear_matrix_R_G_label (rcp_t *rcp);
void set_linear_matrix_R_B_label (rcp_t *rcp);
void set_linear_matrix_G_R_label (rcp_t *rcp);
void set_linear_matrix_G_B_label (rcp_t *rcp);
void set_linear_matrix_B_R_label (rcp_t *rcp);
void set_linear_matrix_B_G_label (rcp_t *rcp);

void set_cc_saturation_R_label (rcp_t *rcp);
void set_cc_saturation_R_R_Yl_label (rcp_t *rcp);
void set_cc_saturation_R_Yl_label (rcp_t *rcp);
void set_cc_saturation_R_Yl_Yl_label (rcp_t *rcp);
void set_cc_saturation_Yl_label (rcp_t *rcp);
void set_cc_saturation_Yl_G_label (rcp_t *rcp);
void set_cc_saturation_G_label (rcp_t *rcp);
void set_cc_saturation_G_Cy_label (rcp_t *rcp);
void set_cc_saturation_Cy_label (rcp_t *rcp);
void set_cc_saturation_Cy_B_label (rcp_t *rcp);
void set_cc_saturation_B_label (rcp_t *rcp);
void set_cc_saturation_B_Mg_label (rcp_t *rcp);
void set_cc_saturation_Mg_label (rcp_t *rcp);
void set_cc_saturation_Mg_R_label (rcp_t *rcp);
void set_cc_saturation_Mg_R_R_label (rcp_t *rcp);

void set_cc_phase_R_label (rcp_t *rcp);
void set_cc_phase_R_R_Yl_label (rcp_t *rcp);
void set_cc_phase_R_Yl_label (rcp_t *rcp);
void set_cc_phase_R_Yl_Yl_label (rcp_t *rcp);
void set_cc_phase_Yl_label (rcp_t *rcp);
void set_cc_phase_Yl_G_label (rcp_t *rcp);
void set_cc_phase_G_label (rcp_t *rcp);
void set_cc_phase_G_Cy_label (rcp_t *rcp);
void set_cc_phase_Cy_label (rcp_t *rcp);
void set_cc_phase_Cy_B_label (rcp_t *rcp);
void set_cc_phase_B_label (rcp_t *rcp);
void set_cc_phase_B_Mg_label (rcp_t *rcp);
void set_cc_phase_Mg_label (rcp_t *rcp);
void set_cc_phase_Mg_R_label (rcp_t *rcp);
void set_cc_phase_Mg_R_R_label (rcp_t *rcp);

void set_matrix_type (rcp_t *rcp);
void set_adaptive_matrix (rcp_t *rcp);
void set_linear_matrix_R_G (rcp_t *rcp);
void set_linear_matrix_R_B (rcp_t *rcp);
void set_linear_matrix_G_R (rcp_t *rcp);
void set_linear_matrix_G_B (rcp_t *rcp);
void set_linear_matrix_B_R (rcp_t *rcp);
void set_linear_matrix_B_G (rcp_t *rcp);

void set_cc_saturation_R (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_R_Yl_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Yl (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Yl_G (rcp_t *rcp, gboolean wait);
void set_cc_saturation_G (rcp_t *rcp, gboolean wait);
void set_cc_saturation_G_Cy (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Cy (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Cy_B (rcp_t *rcp, gboolean wait);
void set_cc_saturation_B (rcp_t *rcp, gboolean wait);
void set_cc_saturation_B_Mg (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg_R (rcp_t *rcp, gboolean wait);
void set_cc_saturation_Mg_R_R (rcp_t *rcp, gboolean wait);

void set_cc_phase_R (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_R_Yl_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_Yl (rcp_t *rcp, gboolean wait);
void set_cc_phase_Yl_G (rcp_t *rcp, gboolean wait);
void set_cc_phase_G (rcp_t *rcp, gboolean wait);
void set_cc_phase_G_Cy (rcp_t *rcp, gboolean wait);
void set_cc_phase_Cy (rcp_t *rcp, gboolean wait);
void set_cc_phase_Cy_B (rcp_t *rcp, gboolean wait);
void set_cc_phase_B (rcp_t *rcp, gboolean wait);
void set_cc_phase_B_Mg (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg_R (rcp_t *rcp, gboolean wait);
void set_cc_phase_Mg_R_R (rcp_t *rcp, gboolean wait);

GtkWidget *create_matrix_frame (rcp_t *rcp);


#endif

