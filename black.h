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

#ifndef __BLACK_H
#define __BLACK_H


#include "rcp.h"


void set_r_pedestal (rcp_t *rcp);

gboolean r_pedestal_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_pedestal_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean r_pedestal_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean r_pedestal_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define r_pedestal_plus_10_button_pressed_AW_HE130 r_pedestal_plus_10_button_pressed
#define r_pedestal_plus_1_button_pressed_AW_HE130 r_pedestal_plus_1_button_pressed
#define r_pedestal_minus_1_button_pressed_AW_HE130 r_pedestal_minus_1_button_pressed
#define r_pedestal_minus_10_button_pressed_AW_HE130 r_pedestal_minus_10_button_pressed

gboolean g_pedestal_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_pedestal_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

void set_b_pedestal (rcp_t *rcp);

gboolean b_pedestal_plus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_pedestal_plus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean b_pedestal_minus_1_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean b_pedestal_minus_10_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define b_pedestal_plus_10_button_pressed_AW_HE130 b_pedestal_plus_10_button_pressed
#define b_pedestal_plus_1_button_pressed_AW_HE130 b_pedestal_plus_1_button_pressed
#define b_pedestal_minus_1_button_pressed_AW_HE130 b_pedestal_minus_1_button_pressed
#define b_pedestal_minus_10_button_pressed_AW_HE130 b_pedestal_minus_10_button_pressed

GtkWidget *create_black_frame_AW_HE130 (rcp_t *rcp);


#define r_pedestal_plus_10_button_pressed_AW_UE150 r_pedestal_plus_10_button_pressed
#define r_pedestal_plus_1_button_pressed_AW_UE150 r_pedestal_plus_1_button_pressed
#define r_pedestal_minus_1_button_pressed_AW_UE150 r_pedestal_minus_1_button_pressed
#define r_pedestal_minus_10_button_pressed_AW_UE150 r_pedestal_minus_10_button_pressed

void set_g_pedestal_AW_UE150 (rcp_t *rcp);

gboolean g_pedestal_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

gboolean g_pedestal_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);
gboolean g_pedestal_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp);

#define b_pedestal_plus_10_button_pressed_AW_UE150 b_pedestal_plus_10_button_pressed
#define b_pedestal_plus_1_button_pressed_AW_UE150 b_pedestal_plus_1_button_pressed
#define b_pedestal_minus_1_button_pressed_AW_UE150 b_pedestal_minus_1_button_pressed
#define b_pedestal_minus_10_button_pressed_AW_UE150 b_pedestal_minus_10_button_pressed

GtkWidget *create_black_frame_AW_UE150 (rcp_t *rcp);


#endif

