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

#ifndef __IRIS_H
#define __IRIS_H


#include "rcp.h"


void set_iris (rcp_t *rcp);

gboolean iris_plus_50_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_20_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean iris_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_20_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean iris_minus_50_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define iris_plus_50_button_pressed_AW_HE130 iris_plus_50_button_pressed
#define iris_plus_20_button_pressed_AW_HE130 iris_plus_20_button_pressed
#define iris_plus_10_button_pressed_AW_HE130 iris_plus_10_button_pressed
#define iris_plus_1_button_pressed_AW_HE130 iris_plus_1_button_pressed

#define iris_minus_1_button_pressed_AW_HE130 iris_minus_1_button_pressed
#define iris_minus_10_button_pressed_AW_HE130 iris_minus_10_button_pressed
#define iris_minus_20_button_pressed_AW_HE130 iris_minus_20_button_pressed
#define iris_minus_50_button_pressed_AW_HE130 iris_minus_50_button_pressed

#define iris_plus_50_button_pressed_AW_UE150 iris_plus_50_button_pressed
#define iris_plus_20_button_pressed_AW_UE150 iris_plus_20_button_pressed
#define iris_plus_10_button_pressed_AW_UE150 iris_plus_10_button_pressed
#define iris_plus_1_button_pressed_AW_UE150 iris_plus_1_button_pressed

#define iris_minus_1_button_pressed_AW_UE150 iris_minus_1_button_pressed
#define iris_minus_10_button_pressed_AW_UE150 iris_minus_10_button_pressed
#define iris_minus_20_button_pressed_AW_UE150 iris_minus_20_button_pressed
#define iris_minus_50_button_pressed_AW_UE150 iris_minus_50_button_pressed

void set_iris_auto (rcp_t *rcp);

GtkWidget *create_iris_frame (rcp_t *rcp);


#endif

