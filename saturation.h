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

#ifndef __SATURATION_H
#define __SATURATION_H


void set_saturation_label (rcp_t *rcp);

void set_saturation (rcp_t *rcp);

gboolean saturation_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean saturation_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_saturation_frame_AW_HE130 (rcp_t *rcp);

gboolean saturation_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean saturation_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean saturation_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

GtkWidget *create_saturation_frame_AW_UE150 (rcp_t *rcp);


#endif

