/*
 * copyright (c) 2018-2022 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include "rcp.h"

#include "master_cmd_define.h"


extern int color_temperature_array_AW_HE130[];
extern int color_temperature_array_AW_UE150[];


void on_standby_master_switch_activated (GtkSwitch *on_standby_switch, GParamSpec *pspec, cameras_set_t *cameras_set)
{
	gboolean active;
	int i;
	rcp_t *rcp;

	active = gtk_switch_get_active (on_standby_switch);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if ((rcp->ip_address_is_valid) && (!rcp->camera_is_working)) gtk_switch_set_active (GTK_SWITCH (rcp->on_standby_switch), active);
	}
}

void standard_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_button_clicked (GTK_BUTTON (rcp->standard_button));
	}

	cameras_set->master_rcp.mire = FALSE;
	cameras_set->master_rcp.day_night = FALSE;
	cameras_set->master_rcp.detail = DETAIL_DEFAULT;
	cameras_set->master_rcp.saturation = SATURATION_DEFAULT;
	cameras_set->master_rcp.r_gain = R_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.g_gain = G_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.b_gain = B_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.r_pedestal = R_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.g_pedestal = G_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.b_pedestal = B_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.pedestal = PEDESTAL_DEFAULT_AW_UE150;
	cameras_set->master_rcp.iris = IRIS_DEFAULT;
	cameras_set->master_rcp.iris_auto = IRIS_AUTO_DEFAULT;
#if DETAIL_DEFAULT
	gtk_button_set_label (GTK_BUTTON (cameras_set->master_rcp.detail_button), "On");
#else
	gtk_button_set_label (GTK_BUTTON (cameras_set->master_rcp.detail_button), "Off");
#endif
	g_signal_handler_block (cameras_set->master_rcp.saturation_scale, cameras_set->master_rcp.saturation_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.saturation_scale), SATURATION_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.saturation_scale, cameras_set->master_rcp.saturation_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.r_gain_scale, cameras_set->master_rcp.r_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.r_gain_scale), R_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.r_gain_scale, cameras_set->master_rcp.r_gain_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.g_gain_scale, cameras_set->master_rcp.g_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.g_gain_scale, cameras_set->master_rcp.g_gain_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.b_gain_scale, cameras_set->master_rcp.b_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.b_gain_scale), B_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.b_gain_scale, cameras_set->master_rcp.b_gain_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.r_pedestal_scale), R_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.g_pedestal_scale), G_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.b_pedestal_scale), B_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.pedestal_scale, cameras_set->master_rcp.pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.pedestal_scale), PEDESTAL_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.pedestal_scale, cameras_set->master_rcp.pedestal_handler_id);

	g_signal_handler_block (cameras_set->master_rcp.iris_scale, cameras_set->master_rcp.iris_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.iris_scale), IRIS_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.iris_scale, cameras_set->master_rcp.iris_handler_id);
}

void mire_master_button_clicked (GtkButton *mire_button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	cameras_set->master_rcp.mire = !cameras_set->master_rcp.mire;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->mire_toggle_button), cameras_set->master_rcp.mire);
	}
}

void day_night_master_button_clicked (GtkButton *day_night_button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	cameras_set->master_rcp.day_night = !cameras_set->master_rcp.day_night;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->day_night_toggle_button), cameras_set->master_rcp.day_night);
	}
}

void scene_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	gboolean active;
	const gchar *label;
	int index;
	int i;
	rcp_t *rcp;

	active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cameras_set->master_rcp.store_toggle_button));

	label = gtk_button_get_label (button);
	sscanf (label, "%d", &index);
	index--;

	triggered_by_master_rcp = TRUE;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->store_toggle_button), active);
		gtk_button_clicked (GTK_BUTTON (rcp->scenes_button[index]));
	}

	triggered_by_master_rcp = FALSE;

	if (active) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cameras_set->master_rcp.store_toggle_button), FALSE);
		save_settings_and_cameras_sets_to_config_file ();
	}
}

void store_master_toggle_button_clicked (GtkToggleButton *store_toggle_button, cameras_set_t *cameras_set)
{
	gboolean active;
	int i;
	rcp_t *rcp;

	active = gtk_toggle_button_get_active (store_toggle_button);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->store_toggle_button), active);
	}
}

void ABB_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_button_clicked (GTK_BUTTON (rcp->ABB_button));
	}

	if (cameras_set->master_rcp.r_pedestal != R_PEDESTAL_DEFAULT) {
		cameras_set->master_rcp.r_pedestal = R_PEDESTAL_DEFAULT;
		g_signal_handler_block (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.r_pedestal_scale), R_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);
	}

	if (cameras_set->master_rcp.g_pedestal != G_PEDESTAL_DEFAULT) {
		cameras_set->master_rcp.g_pedestal = G_PEDESTAL_DEFAULT;
		g_signal_handler_block (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.g_pedestal_scale), G_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);
	}

	if (cameras_set->master_rcp.b_pedestal != B_PEDESTAL_DEFAULT) {
		cameras_set->master_rcp.b_pedestal = B_PEDESTAL_DEFAULT;
		g_signal_handler_block (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.b_pedestal_scale), B_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);
	}
}

void ND_filter_master_changed (GtkComboBox *ND_filter_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (ND_filter_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rcp->day_night_toggle_button))) continue;	//ND filter switching is not possible in Night mode

		if (rcp->model == AW_UE150) {
			if (active_item == 0) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 0);
			else if (active_item == 1) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 1);
			else if (active_item == 3) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 2);
			else if (active_item == 4) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 3);
		} else {
			if (active_item == 0) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 0);
			else if (active_item == 2) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 1);
			else if (active_item == 4) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->ND_filter_combo_box), 2);
		}
	}

	g_signal_handler_block (ND_filter_combo_box, cameras_set->master_rcp.ND_filter_handler_id);
	gtk_combo_box_set_active (ND_filter_combo_box, -1);
	g_signal_handler_unblock (ND_filter_combo_box, cameras_set->master_rcp.ND_filter_handler_id);
}

void gain_master_changed (GtkComboBox *gain_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (gain_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if ((rcp->model == AW_UE150) && (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1))
			gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gain_combo_box), active_item);
		else if (active_item >= 6) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gain_combo_box), active_item - 6);
	}

	g_signal_handler_block (gain_combo_box, cameras_set->master_rcp.gain_handler_id);
	gtk_combo_box_set_active (gain_combo_box, -1);
	g_signal_handler_unblock (gain_combo_box, cameras_set->master_rcp.gain_handler_id);
}

void gamma_type_master_changed (GtkComboBox *gamma_type_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (gamma_type_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->model == AW_UE150) {
			if (active_item == 0) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 0);
			else if (active_item == 2) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 1);
			else if (active_item == 3) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 2);
			else if (active_item == 4) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 3);
			else if (active_item == 5) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 4);
			else if (active_item == 6) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 5);
			else if (active_item == 7) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), 6);
		} else {
			if (active_item <= 4) gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), active_item);
		}
	}

	g_signal_handler_block (gamma_type_combo_box, cameras_set->master_rcp.gamma_type_handler_id);
	gtk_combo_box_set_active (gamma_type_combo_box, -1);
	g_signal_handler_unblock (gamma_type_combo_box, cameras_set->master_rcp.gamma_type_handler_id);
}

void gamma_master_changed (GtkComboBox *gamma_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (gamma_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_combo_box), active_item);
	}

	g_signal_handler_block (gamma_combo_box, cameras_set->master_rcp.gamma_handler_id);
	gtk_combo_box_set_active (gamma_combo_box, -1);
	g_signal_handler_unblock (gamma_combo_box, cameras_set->master_rcp.gamma_handler_id);
}

void drs_master_changed (GtkComboBox *drs_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (drs_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->drs_combo_box), active_item);
	}

	g_signal_handler_block (drs_combo_box, cameras_set->master_rcp.drs_handler_id);
	gtk_combo_box_set_active (drs_combo_box, -1);
	g_signal_handler_unblock (drs_combo_box, cameras_set->master_rcp.drs_handler_id);
}

void color_temperature_master_changed (GtkComboBox *color_temperature_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i, j;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (color_temperature_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->model == AW_UE150) {
			j = 0;
			while (color_temperature_array_AW_UE150[j] < color_temperature_array_AW_HE130[120 - active_item]) j++;

			gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), 400 - j);
		} else gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), active_item);
	}

	g_signal_handler_block (color_temperature_combo_box, cameras_set->master_rcp.color_temperature_handler_id);
	gtk_combo_box_set_active (color_temperature_combo_box, -1);
	g_signal_handler_unblock (color_temperature_combo_box, cameras_set->master_rcp.color_temperature_handler_id);
}

void color_temperature_plus_10_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;
	int index;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
	}
}

void color_temperature_plus_1_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;
	int index;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
	}
}

void color_temperature_minus_1_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;
	int index;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		index = rcp->current_scene.color_temperature + 1;
		if (rcp->model == AW_UE150) {
			if (index > 400) index = 400;
		} else {
			if (index > 120) index = 120;
		}

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
	}
}

void color_temperature_minus_10_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;
	int index;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		index = rcp->current_scene.color_temperature + 10;
		if (rcp->model == AW_UE150) {
			if (index > 400) index = 400;
		} else {
			if (index > 120) index = 120;
		}

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
	}
}

void knee_settings_master_changed (GtkComboBox *knee_settings_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (knee_settings_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->current_scene.drs != 3) continue;	//When DRS is set to On, the knee setting is disabled

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->knee_settings_combo_box), active_item);
	}

	g_signal_handler_block (knee_settings_combo_box, cameras_set->master_rcp.knee_settings_handler_id);
	gtk_combo_box_set_active (knee_settings_combo_box, -1);
	g_signal_handler_unblock (knee_settings_combo_box, cameras_set->master_rcp.knee_settings_handler_id);
}

void matrix_type_master_changed (GtkComboBox *matrix_type_combo_box, cameras_set_t *cameras_set)
{
	int active_item;
	int i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (matrix_type_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->matrix_type_combo_box), active_item);
	}

	g_signal_handler_block (matrix_type_combo_box, cameras_set->master_rcp.matrix_type_handler_id);
	gtk_combo_box_set_active (matrix_type_combo_box, -1);
	g_signal_handler_unblock (matrix_type_combo_box, cameras_set->master_rcp.matrix_type_handler_id);
}

void detail_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	cameras_set->master_rcp.detail = !cameras_set->master_rcp.detail;

	if (cameras_set->master_rcp.detail) gtk_button_set_label (button, "On");
	else gtk_button_set_label (button, "Off");

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->detail_toggle_button), cameras_set->master_rcp.detail);
	}
}

#define MIN_VALUE 0x1C
#define MAX_VALUE 0xE3

MASTER_RCP_CMD_FUNCS(saturation)

MASTER_BUTTON_MINUS_FUNCS(saturation,10)
MASTER_BUTTON_MINUS_FUNCS(saturation,1)

void saturation_0_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_range_set_value (GTK_RANGE (rcp->saturation_scale), SATURATION_DEFAULT);
	}

	cameras_set->master_rcp.saturation = SATURATION_DEFAULT;
	g_signal_handler_block (cameras_set->master_rcp.saturation_scale, cameras_set->master_rcp.saturation_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.saturation_scale), SATURATION_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.saturation_scale, cameras_set->master_rcp.saturation_handler_id);
}

MASTER_BUTTON_PLUS_FUNCS(saturation,1)
MASTER_BUTTON_PLUS_FUNCS(saturation,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x738
#define MAX_VALUE 0x8C8

MASTER_RCP_CMD_FUNCS(r_gain)

MASTER_BUTTON_PLUS_FUNCS(r_gain,10)
MASTER_BUTTON_PLUS_FUNCS(r_gain,1)

MASTER_BUTTON_MINUS_FUNCS(r_gain,1)
MASTER_BUTTON_MINUS_FUNCS(r_gain,10)

MASTER_RCP_CMD_FUNCS(g_gain)

MASTER_BUTTON_PLUS_FUNCS(g_gain,10)
MASTER_BUTTON_PLUS_FUNCS(g_gain,1)

MASTER_BUTTON_MINUS_FUNCS(g_gain,1)
MASTER_BUTTON_MINUS_FUNCS(g_gain,10)

MASTER_RCP_CMD_FUNCS(b_gain)

MASTER_BUTTON_PLUS_FUNCS(b_gain,10)
MASTER_BUTTON_PLUS_FUNCS(b_gain,1)

MASTER_BUTTON_MINUS_FUNCS(b_gain,1)
MASTER_BUTTON_MINUS_FUNCS(b_gain,10)

#undef MIN_VALUE
#undef MAX_VALUE

void white_raz_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->model == AW_UE150) {
			if (rcp->current_scene.r_gain != R_GAIN_DEFAULT_AW_UE150) {
				rcp->current_scene.r_gain = R_GAIN_DEFAULT_AW_UE150;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), R_GAIN_DEFAULT_AW_UE150);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				set_r_gain_AW_UE150 (rcp);
			}

			rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;

			g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
			g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
		} else {
			if (rcp->current_scene.r_gain != R_GAIN_DEFAULT_AW_HE130) {
				rcp->current_scene.r_gain = R_GAIN_DEFAULT_AW_HE130;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), R_GAIN_DEFAULT_AW_HE130);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				set_r_gain (rcp);
			}
	
			rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_HE130;

			g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_HE130);
			g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
		}
	}

	cameras_set->master_rcp.r_gain = R_GAIN_DEFAULT_AW_UE150;
	g_signal_handler_block (cameras_set->master_rcp.r_gain_scale, cameras_set->master_rcp.r_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.r_gain_scale), R_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.r_gain_scale, cameras_set->master_rcp.r_gain_handler_id);

	cameras_set->master_rcp.g_gain = G_GAIN_DEFAULT_AW_UE150;
	g_signal_handler_block (cameras_set->master_rcp.g_gain_scale, cameras_set->master_rcp.g_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.g_gain_scale, cameras_set->master_rcp.g_gain_handler_id);

	cameras_set->master_rcp.b_gain = B_GAIN_DEFAULT_AW_UE150;
	g_signal_handler_block (cameras_set->master_rcp.b_gain_scale, cameras_set->master_rcp.b_gain_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.b_gain_scale), B_GAIN_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.b_gain_scale, cameras_set->master_rcp.b_gain_handler_id);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->model == AW_UE150) {
			if (rcp->current_scene.b_gain != B_GAIN_DEFAULT_AW_UE150) {
				rcp->current_scene.b_gain = B_GAIN_DEFAULT_AW_UE150;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), B_GAIN_DEFAULT_AW_UE150);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				set_b_gain_AW_UE150 (rcp);
			}
		} else {
			if (rcp->current_scene.b_gain != B_GAIN_DEFAULT_AW_HE130) {
				rcp->current_scene.b_gain = B_GAIN_DEFAULT_AW_HE130;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), B_GAIN_DEFAULT_AW_HE130);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				set_b_gain (rcp);
			}
		}
	}
}

#define MIN_VALUE 0x032
#define MAX_VALUE 0x0FA

MASTER_RCP_CMD_FUNCS(r_pedestal)

MASTER_BUTTON_PLUS_FUNCS(r_pedestal,10)
MASTER_BUTTON_PLUS_FUNCS(r_pedestal,1)

MASTER_BUTTON_MINUS_FUNCS(r_pedestal,1)
MASTER_BUTTON_MINUS_FUNCS(r_pedestal,10)

MASTER_RCP_CMD_FUNCS(g_pedestal)

MASTER_BUTTON_PLUS_FUNCS(g_pedestal,10)
MASTER_BUTTON_PLUS_FUNCS(g_pedestal,1)

MASTER_BUTTON_MINUS_FUNCS(g_pedestal,1)
MASTER_BUTTON_MINUS_FUNCS(g_pedestal,10)

MASTER_RCP_CMD_FUNCS(b_pedestal)

MASTER_BUTTON_PLUS_FUNCS(b_pedestal,10)
MASTER_BUTTON_PLUS_FUNCS(b_pedestal,1)

MASTER_BUTTON_MINUS_FUNCS(b_pedestal,1)
MASTER_BUTTON_MINUS_FUNCS(b_pedestal,10)

#undef MIN_VALUE
#undef MAX_VALUE

void black_raz_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->current_scene.r_pedestal != R_PEDESTAL_DEFAULT) {
			rcp->current_scene.r_pedestal = R_PEDESTAL_DEFAULT;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), R_PEDESTAL_DEFAULT);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			set_r_pedestal (rcp);
		}

		if (rcp->current_scene.g_pedestal != G_PEDESTAL_DEFAULT) {
			rcp->current_scene.g_pedestal = G_PEDESTAL_DEFAULT;

			g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
			g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);

			if (rcp->model == AW_UE150) set_g_pedestal_AW_UE150 (rcp);
		}
	}

	cameras_set->master_rcp.r_pedestal = R_PEDESTAL_DEFAULT;
	g_signal_handler_block (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.r_pedestal_scale), R_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.r_pedestal_scale, cameras_set->master_rcp.r_pedestal_handler_id);

	cameras_set->master_rcp.g_pedestal = G_PEDESTAL_DEFAULT;
	g_signal_handler_block (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.g_pedestal_scale), G_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.g_pedestal_scale, cameras_set->master_rcp.g_pedestal_handler_id);

	cameras_set->master_rcp.b_pedestal = B_PEDESTAL_DEFAULT;
	g_signal_handler_block (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.b_pedestal_scale), B_PEDESTAL_DEFAULT);
	g_signal_handler_unblock (cameras_set->master_rcp.b_pedestal_scale, cameras_set->master_rcp.b_pedestal_handler_id);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->current_scene.b_pedestal != B_PEDESTAL_DEFAULT) {
			rcp->current_scene.b_pedestal = B_PEDESTAL_DEFAULT;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), B_PEDESTAL_DEFAULT);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			set_b_pedestal (rcp);
		}
	}
}

void shutter_type_master_changed (GtkComboBox *shutter_type_combo_box, cameras_set_t *cameras_set)
{
	int active_item, i;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (shutter_type_combo_box);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->shutter_type_combo_box), active_item);
	}

	g_signal_handler_block (shutter_type_combo_box, cameras_set->master_rcp.shutter_type_handler_id);
	gtk_combo_box_set_active (shutter_type_combo_box, -1);
	g_signal_handler_unblock (shutter_type_combo_box, cameras_set->master_rcp.shutter_type_handler_id);
}

#define MIN_VALUE 0x738
#define MAX_VALUE 0x8C8

MASTER_RCP_CMD_FUNCS(pedestal)

MASTER_BUTTON_PLUS_FUNCS(pedestal,10)
MASTER_BUTTON_PLUS_FUNCS(pedestal,1)

MASTER_BUTTON_MINUS_FUNCS(pedestal,1)
MASTER_BUTTON_MINUS_FUNCS(pedestal,10)

#undef MIN_VALUE
#undef MAX_VALUE

void pedestal_raz_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	cameras_set->master_rcp.pedestal = PEDESTAL_DEFAULT_AW_UE150;

	g_signal_handler_block (cameras_set->master_rcp.pedestal_scale, cameras_set->master_rcp.pedestal_handler_id);
	gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.pedestal_scale), PEDESTAL_DEFAULT_AW_UE150);
	g_signal_handler_unblock (cameras_set->master_rcp.pedestal_scale, cameras_set->master_rcp.pedestal_handler_id);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		if (rcp->model == AW_UE150) gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_UE150);
		else gtk_range_set_value (GTK_RANGE (rcp->pedestal_scale), PEDESTAL_DEFAULT_AW_HE130);
	}
}

#define MIN_VALUE 0x555
#define MAX_VALUE 0xFFF

MASTER_RCP_CMD_FUNCS(iris)

MASTER_BUTTON_PLUS_FUNCS(iris,50)
MASTER_BUTTON_PLUS_FUNCS(iris,20)
MASTER_BUTTON_PLUS_FUNCS(iris,10)
MASTER_BUTTON_PLUS_FUNCS(iris,1)

MASTER_BUTTON_MINUS_FUNCS(iris,1)
MASTER_BUTTON_MINUS_FUNCS(iris,10)
MASTER_BUTTON_MINUS_FUNCS(iris,20)
MASTER_BUTTON_MINUS_FUNCS(iris,50)

#undef MIN_VALUE
#undef MAX_VALUE

void iris_auto_master_button_clicked (GtkButton *button, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	cameras_set->master_rcp.iris_auto = !cameras_set->master_rcp.iris_auto;

	if (cameras_set->master_rcp.iris_auto) gtk_widget_set_sensitive (cameras_set->master_rcp.iris_sensitive_widgets, FALSE);
	else gtk_widget_set_sensitive (cameras_set->master_rcp.iris_sensitive_widgets, TRUE);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->iris_auto_toggle_button), cameras_set->master_rcp.iris_auto);
	}
}

/*gboolean remove_master_timeout (GtkWidget *button, GdkEventButton *event, cameras_set_t *cameras_set)
{
	gtk_widget_unset_state_flags (button, GTK_STATE_FLAG_ACTIVE);

	if (cameras_set->master_rcp.timeout_id != 0) {
		g_source_remove (cameras_set->master_rcp.timeout_id);
		cameras_set->master_rcp.timeout_id = 0;
	}

	return FALSE;
}*/

/*gboolean remove_master_green_timeout (GtkWidget *button, GdkEventButton *event, cameras_set_t *cameras_set)
{
	int i;
	rcp_t *rcp;

	gtk_widget_unset_state_flags (button, GTK_STATE_FLAG_ACTIVE);

	for (i = 0; i < cameras_set->number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		if (!rcp->camera_is_on) continue;
		if (rcp->camera_is_working) continue;

		remove_green_timeout (NULL, NULL, rcp);
	}

	if (cameras_set->master_rcp.timeout_id != 0) {
		g_source_remove (cameras_set->master_rcp.timeout_id);
		cameras_set->master_rcp.timeout_id = 0;
	}

	return FALSE;
}*/

gboolean master_rcp_button_press_event (GtkWidget *widget, GdkEventButton *event, master_rcp_t *master_rcp)
{
	if (popup_window != NULL) {
		gtk_widget_hide (popup_window);
		popup_window = NULL;

		if (rcp_vision != NULL) gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
		gtk_event_box_set_above_child (GTK_EVENT_BOX (master_rcp->root_widget), FALSE);
	}

	return TRUE;
}

void init_master_rcp (cameras_set_t *cameras_set)
{
	GtkWidget *main_box, *frame, *box1, *box2, *box3, *box4, *widget;
	char label[32];
	int i;
	float value;

	cameras_set->master_rcp.mire = FALSE;
	cameras_set->master_rcp.day_night = FALSE;
	cameras_set->master_rcp.detail = DETAIL_DEFAULT;
	cameras_set->master_rcp.saturation = SATURATION_DEFAULT;
	cameras_set->master_rcp.r_gain = R_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.g_gain = G_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.b_gain = B_GAIN_DEFAULT_AW_UE150;
	cameras_set->master_rcp.r_pedestal = R_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.g_pedestal = G_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.b_pedestal = B_PEDESTAL_DEFAULT;
	cameras_set->master_rcp.pedestal = PEDESTAL_DEFAULT_AW_UE150;
	cameras_set->master_rcp.iris = IRIS_DEFAULT;
	cameras_set->master_rcp.iris_auto = IRIS_AUTO_DEFAULT;

	cameras_set->master_rcp.timeout_id = 0;

	main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (main_box), 4);

	widget = gtk_label_new ("RCP Maître");
	gtk_widget_set_size_request (widget, 60, 40);
	gtk_box_pack_start (GTK_BOX (main_box), widget, FALSE, FALSE, 0);

	widget = gtk_label_new (NULL);
	gtk_box_pack_start (GTK_BOX (main_box), widget, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_top (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		widget = gtk_switch_new ();
		cameras_set->master_rcp.on_standby_handler_id = g_signal_connect (widget, "notify::active", G_CALLBACK (on_standby_master_switch_activated), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		cameras_set->master_rcp.on_standby_switch = widget;

		widget = gtk_button_new_with_label ("Standard");
		g_signal_connect (widget, "clicked", G_CALLBACK (standard_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		cameras_set->master_rcp.standard_button = widget;

		widget = gtk_button_new_with_label ("Mire");
		g_signal_connect (widget, "clicked", G_CALLBACK (mire_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		cameras_set->master_rcp.mire_button = widget;

		widget = gtk_button_new_with_label ("Nuit");
		g_signal_connect (widget, "clicked", G_CALLBACK (day_night_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);
		cameras_set->master_rcp.day_night_button = widget;
	gtk_box_pack_start (GTK_BOX (main_box), box1, FALSE, FALSE, 0);

//Scenes
	frame = gtk_frame_new (scenes_label);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		cameras_set->master_rcp.scenes_bank_1_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		for (i = 1; i <= 5; i++) {
			sprintf (label, "%d", i);
			widget = gtk_button_new_with_label (label);
			g_signal_connect (widget, "clicked", G_CALLBACK (scene_master_button_clicked), cameras_set);
			gtk_box_pack_start (GTK_BOX (cameras_set->master_rcp.scenes_bank_1_box), widget, FALSE, FALSE, 0);
		}
		gtk_box_pack_start (GTK_BOX (box2), cameras_set->master_rcp.scenes_bank_1_box, FALSE, FALSE, 0);

		cameras_set->master_rcp.scenes_bank_2_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		for (i = 6; i <= NB_SCENES; i++) {
			sprintf (label, "%d", i);
			widget = gtk_button_new_with_label (label);
			g_signal_connect (widget, "clicked", G_CALLBACK (scene_master_button_clicked), cameras_set);
			gtk_box_pack_start (GTK_BOX (cameras_set->master_rcp.scenes_bank_2_box), widget, FALSE, FALSE, 0);
		}
		gtk_box_pack_start (GTK_BOX (box2), cameras_set->master_rcp.scenes_bank_2_box, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
		
		widget = gtk_toggle_button_new_with_label ("S");
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_store), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		g_signal_connect (widget, "toggled", G_CALLBACK (store_master_toggle_button_clicked), cameras_set);
		gtk_box_pack_end (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		cameras_set->master_rcp.store_toggle_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
//ABB
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("ABB");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE * 2);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (ABB_master_button_clicked), cameras_set);
			gtk_box_set_center_widget (GTK_BOX (box2), widget);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

//ND_filter
		frame = gtk_frame_new ("Filtre neutre");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Through");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/4*");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/8**");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/16*");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1/64");
			cameras_set->master_rcp.ND_filter_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (ND_filter_master_changed), cameras_set);
		gtk_container_add (GTK_CONTAINER (frame), widget);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);

//Gain
		frame = gtk_frame_new ("Gain");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
			for (i = 0x08; i <= 0x2C; i++) {
				sprintf (label, "%d db", i - 0x08);
				gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
			}
			for (i = 0x2D; i <= 0x32; i++) {
				sprintf (label, "%d db*", i - 0x08);
				gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
			}
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Auto");
			cameras_set->master_rcp.gain_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gain_master_changed), cameras_set);
		gtk_container_add (GTK_CONTAINER (frame), widget);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (main_box), box1, FALSE, FALSE, 0);
	cameras_set->master_rcp.ABB_filter_gain_box = box1;

//Gamma
	frame = gtk_frame_new ("Gamma");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, gamma_type_tooltip);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "SD**");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE1");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE2");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE3");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILM REC*");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "VIDEO REC*");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HLG*");
		cameras_set->master_rcp.gamma_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_type_master_changed), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, gamma_tooltip);
		for (i = 0x67, value = 0.30; i <= 0x94; i++, value += 0.01) {
			sprintf (label, "%4.2f", value);
			gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		cameras_set->master_rcp.gamma_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_master_changed), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, drs_tooltip);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "High");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Mid");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Low");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Off");
		cameras_set->master_rcp.drs_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (drs_master_changed), cameras_set);
		gtk_box_pack_end (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("DRS:");
		gtk_widget_set_tooltip_text (widget, drs_tooltip);
		gtk_box_pack_end (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);
	cameras_set->master_rcp.gamma_frame = frame;

//Color Temperature
	frame = gtk_frame_new ("Température de couleur");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.04, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		for (i = 120; i >= 0; i--) {
			sprintf (label, "%dK", color_temperature_array_AW_HE130[i]);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		cameras_set->master_rcp.color_temperature_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (color_temperature_master_changed), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, TRUE, TRUE, 0);

		widget = gtk_button_new_with_label ("--");
		g_signal_connect (widget, "clicked", G_CALLBACK (color_temperature_minus_10_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("-");
		g_signal_connect (widget, "clicked", G_CALLBACK (color_temperature_minus_1_master_button_clicked), cameras_set);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("+");
		g_signal_connect (widget, "clicked", G_CALLBACK (color_temperature_plus_1_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("++");
		g_signal_connect (widget, "clicked", G_CALLBACK (color_temperature_plus_10_master_button_clicked), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);
	cameras_set->master_rcp.color_temperature_frame = frame;

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing (GTK_BOX (box1), MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, MARGIN_VALUE);
//Knee
		frame = gtk_frame_new ("Knee");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "OFF");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "MANUAL");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "AUTO");
		cameras_set->master_rcp.knee_settings_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (knee_settings_master_changed), cameras_set);
		gtk_container_add (GTK_CONTAINER (frame), widget);
		gtk_box_pack_start (GTK_BOX (box1), frame, FALSE, FALSE, 0);

//Matrix
		frame = gtk_frame_new ("Matriçage");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Normal");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "EBU");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "NTSC");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "User");
		cameras_set->master_rcp.matrix_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (matrix_type_master_changed), cameras_set);
		gtk_container_add (GTK_CONTAINER (frame), widget);
		gtk_box_pack_start (GTK_BOX (box1), frame, FALSE, FALSE, 0);

//Detail
		frame = gtk_frame_new ("Détail");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
#if DETAIL_DEFAULT
		widget = gtk_button_new_with_label ("On");
#else
		widget = gtk_button_new_with_label ("Off");
#endif
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
		g_signal_connect (widget, "clicked", G_CALLBACK (detail_master_button_clicked), cameras_set);
		gtk_container_add (GTK_CONTAINER (frame), widget);
		gtk_box_pack_start (GTK_BOX (box1), frame, TRUE, TRUE, 0);
		cameras_set->master_rcp.detail_button = widget;
	gtk_box_pack_start (GTK_BOX (main_box), box1, FALSE, FALSE, 0);
	cameras_set->master_rcp.knee_matrix_detail_box = box1;

	widget = gtk_drawing_area_new ();
	gtk_widget_set_size_request (widget, 80, 28);
	gtk_box_pack_start (GTK_BOX (main_box), widget, FALSE, FALSE, 0);
	cameras_set->master_rcp.padding = widget;

//Saturation
	frame = gtk_frame_new ("Saturation");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x1C, 0xE3, 1.0);
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
		gtk_scale_set_has_origin (GTK_SCALE (widget), TRUE);
		gtk_range_set_value (GTK_RANGE (widget), SATURATION_DEFAULT);
		cameras_set->master_rcp.saturation_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (saturation_master_value_changed), cameras_set);
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		cameras_set->master_rcp.saturation_scale = widget;

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_top (box2, MARGIN_VALUE);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			widget = gtk_button_new_with_label ("-10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_10_master_button_pressed), cameras_set);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (saturation_minus_10_master_button_released), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("-1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_minus_1_master_button_pressed), cameras_set);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (saturation_minus_1_master_button_released), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("0%");
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_widget_set_margin_end (widget, MARGIN_VALUE);
			g_signal_connect (widget, "clicked", G_CALLBACK (saturation_0_master_button_clicked), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+1%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_1_master_button_pressed), cameras_set);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (saturation_plus_1_master_button_released), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			widget = gtk_button_new_with_label ("+10%");
			g_signal_connect (widget, "button_press_event", G_CALLBACK (saturation_plus_10_master_button_pressed), cameras_set);
			g_signal_connect (widget, "button_release_event", G_CALLBACK (saturation_plus_10_master_button_released), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box1);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);

//White gain
	frame = gtk_frame_new ("Blanc");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_end (box4, MARGIN_VALUE);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box1), TRUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x738, 0x8C8, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), R_GAIN_DEFAULT_AW_UE150);
				cameras_set->master_rcp.r_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_gain_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.r_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_gain_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_gain_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_gain_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_gain_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x738, 0x8C8, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_GAIN_DEFAULT_AW_UE150);
				cameras_set->master_rcp.g_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_gain_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.g_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_gain_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_gain_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_gain_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_gain_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x738, 0x8C8, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_GAIN_DEFAULT_AW_UE150);
				cameras_set->master_rcp.b_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_gain_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.b_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_gain_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_gain_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_gain_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_gain_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (white_raz_master_button_clicked), cameras_set);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);

//Black pedestal
	frame = gtk_frame_new ("Noir");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_end (box4, MARGIN_VALUE);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box1), TRUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x032, 0x0FA, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), R_PEDESTAL_DEFAULT);
				cameras_set->master_rcp.r_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_pedestal_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.r_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_pedestal_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_pedestal_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_pedestal_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (r_pedestal_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x032, 0x0FA, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_PEDESTAL_DEFAULT);
				cameras_set->master_rcp.g_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_pedestal_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.g_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_pedestal_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_pedestal_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_pedestal_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (g_pedestal_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x032, 0x0FA, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_PEDESTAL_DEFAULT);
				cameras_set->master_rcp.b_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_pedestal_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.b_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_pedestal_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_pedestal_plus_1_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_pedestal_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (b_pedestal_minus_10_master_button_released), cameras_set);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (black_raz_master_button_clicked), cameras_set);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);
	gtk_box_pack_start (GTK_BOX (main_box), frame, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
//Shutter
			frame = gtk_frame_new ("Shutter");
			gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box3, MARGIN_VALUE);
			gtk_widget_set_margin_end (box3, MARGIN_VALUE);
			gtk_widget_set_margin_bottom (box3, MARGIN_VALUE);
				widget = gtk_combo_box_text_new ();
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Off");
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Step");
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Synchro");
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "ELC");
				cameras_set->master_rcp.shutter_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (shutter_type_master_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_container_add (GTK_CONTAINER (frame), box3);
			gtk_box_pack_start (GTK_BOX (box2), frame, FALSE, FALSE, 0);
			cameras_set->master_rcp.shutter_frame = frame;

//Pedestal
			frame = gtk_frame_new ("Pedestal");
			gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
			gtk_widget_set_margin_start (box3, MARGIN_VALUE);
			gtk_widget_set_margin_end (box3, MARGIN_VALUE);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x738, 0x8C8, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), PEDESTAL_DEFAULT_AW_UE150);
				cameras_set->master_rcp.pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (pedestal_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.pedestal_scale = widget;

				box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box4, MARGIN_VALUE);
					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (pedestal_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (pedestal_plus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (pedestal_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					g_signal_connect (widget, "button_press_event", G_CALLBACK (pedestal_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (pedestal_minus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box4, FALSE, FALSE, 0);

				box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					widget = gtk_button_new_with_label ("raz");
					gtk_widget_set_margin_start (widget, MARGIN_VALUE);
					g_signal_connect (widget, "clicked", G_CALLBACK (pedestal_raz_master_button_clicked), cameras_set);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_box_pack_end (GTK_BOX (box3), box4, FALSE, FALSE, 0);
			gtk_container_add (GTK_CONTAINER (frame), box3);
			gtk_box_pack_end (GTK_BOX (box2), frame, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

//Iris
		frame = gtk_frame_new ("Iris");
		gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, 0x555, 0xFFF, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), IRIS_DEFAULT);
				cameras_set->master_rcp.iris_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (iris_master_value_changed), cameras_set);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				cameras_set->master_rcp.iris_scale = widget;

				box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box4, MARGIN_VALUE);
					widget = gtk_button_new_with_label ("+50");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_50_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_plus_50_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+20");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_20_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_plus_20_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_plus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+1");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_plus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_plus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_1_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_minus_1_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_10_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_minus_10_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-20");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_20_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_minus_20_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("-50");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					g_signal_connect (widget, "button_press_event", G_CALLBACK (iris_minus_50_master_button_pressed), cameras_set);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (iris_minus_50_master_button_released), cameras_set);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box4, FALSE, FALSE, 0);

			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
			cameras_set->master_rcp.iris_sensitive_widgets = box3;

			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_widget_set_margin_start (box3, MARGIN_VALUE);
				widget = gtk_toggle_button_new_with_label ("Auto");
				cameras_set->master_rcp.iris_auto_handler_id = g_signal_connect (widget, "clicked", G_CALLBACK (iris_auto_master_button_clicked), cameras_set);
				gtk_box_set_center_widget (GTK_BOX (box3), widget);
				cameras_set->master_rcp.iris_auto_toggle_button = widget;
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame), box2);
		gtk_box_pack_end (GTK_BOX (box1), frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (main_box), box1, FALSE, FALSE, 0);

	widget = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget), FALSE);
	g_signal_connect (widget, "button_press_event", G_CALLBACK (master_rcp_button_press_event), &cameras_set->master_rcp);
	gtk_container_add (GTK_CONTAINER (widget), main_box);

	gtk_box_pack_start (GTK_BOX (cameras_set->rcp_box), widget, FALSE, FALSE, 0);
	cameras_set->master_rcp.root_widget = widget;
}

