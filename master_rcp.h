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

#ifndef __MASTER_RCP_H
#define __MASTER_RCP_H


typedef struct {
	GtkWidget *on_standby_switch;
	int on_standby_handler_id;
	GtkWidget *standard_button;
	GtkWidget *mire_button;
	GtkWidget *day_night_button;
	gboolean mire;
	gboolean day_night;

	GtkWidget *scenes_bank_1_box;
	GtkWidget *scenes_bank_2_box;
	GtkWidget *store_toggle_button;

	GtkWidget *ABB_filter_gain_box;
	int ND_filter_handler_id;
	int gain_handler_id;

	GtkWidget *gamma_frame;
	int gamma_type_handler_id;
	int gamma_handler_id;
	int drs_handler_id;

	GtkWidget *color_temperature_frame;
	int color_temperature_handler_id;

	GtkWidget *knee_matrix_detail_box;
	int knee_settings_handler_id;
	int matrix_type_handler_id;
	GtkWidget *detail_button;
	gboolean detail;

	GtkWidget *padding;

	GtkWidget *saturation_scale;
	int saturation_handler_id;
	int saturation;

	GtkWidget *r_gain_scale;
	GtkWidget *g_gain_scale;
	GtkWidget *b_gain_scale;
	int r_gain_handler_id;
	int g_gain_handler_id;
	int b_gain_handler_id;
	int r_gain;
	int g_gain;
	int b_gain;

	GtkWidget *r_pedestal_scale;
	GtkWidget *g_pedestal_scale;
	GtkWidget *b_pedestal_scale;
	int r_pedestal_handler_id;
	int g_pedestal_handler_id;
	int b_pedestal_handler_id;
	int r_pedestal;
	int g_pedestal;
	int b_pedestal;

	GtkWidget *shutter_frame;
	int shutter_type_handler_id;

	GtkWidget *pedestal_scale;
	int pedestal_handler_id;
	int pedestal;

	GtkWidget *iris_scale;
	GtkWidget *iris_auto_toggle_button;
	int iris_handler_id;
	int iris_auto_handler_id;
	GtkWidget *iris_sensitive_widgets;
	int iris;
	gboolean iris_auto;

	guint timeout_id;
	int timeout_value;

	GtkWidget *root_widget;
} master_rcp_t;


extern gboolean triggered_by_master_rcp;


#endif

