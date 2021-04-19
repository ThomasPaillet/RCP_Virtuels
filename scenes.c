/*
 * copyright (c) 2018-2021 Thomas Paillet <thomas.paillet@net-c.fr>

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

#include <string.h>


char *scenes_label = "Scènes";

GtkWidget *scenes_page;
GtkWidget *source_cameras_set_list_box;
GtkWidget *source_rcp_list_box_box;
GtkWidget *copy_button;
GtkWidget *destination_cameras_set_list_box;
GtkWidget *destination_rcp_list_box_box;

int source_scene_index;
int destination_scene_index;

cameras_set_t *source_cameras_set;
cameras_set_t *destination_cameras_set;

rcp_t *source_rcp;
rcp_t *destination_rcp;

GtkWidget *ND_filter_check_button;
GtkWidget *gain_check_button;

GtkWidget *all_gamma_check_button;
GtkWidget *gamma_type_check_button;
GtkWidget *gamma_check_button;
GtkWidget *drs_check_button;

GtkWidget *color_temperature_check_button;

GtkWidget *knee_check_button;
GtkWidget *knee_settings_check_button;
GtkWidget *knee_point_check_button;
GtkWidget *knee_slope_check_button;

GtkWidget *matrix_check_button;
GtkWidget *matrix_type_check_button;
GtkWidget *linear_matrix_check_button;
GtkWidget *cc_saturation_check_button;
GtkWidget *cc_phase_check_button;

GtkWidget *all_detail_check_button;
GtkWidget *detail_check_button;
GtkWidget *master_detail_check_button;
GtkWidget *v_detail_level_check_button;
GtkWidget *detail_band_check_button;
GtkWidget *noise_suppress_check_button;
GtkWidget *fleshtone_noisesup_check_button;

GtkWidget *saturation_check_button;

GtkWidget *r_gain_check_button;
GtkWidget *b_gain_check_button;

GtkWidget *r_pedestal_check_button;
GtkWidget *b_pedestal_check_button;

GtkWidget *shutter_check_button;
GtkWidget *shutter_type_check_button;
GtkWidget *shutter_step_check_button;
GtkWidget *shutter_synchro_check_button;

GtkWidget *pedestal_check_button;

GtkWidget *iris_check_button;
GtkWidget *iris_auto_check_button;


void source_scenes_radio_button_toggled (GtkToggleButton *button, gpointer index)
{
	if (!gtk_toggle_button_get_active (button)) return;

	source_scene_index = GPOINTER_TO_INT (index);

	if ((source_rcp != NULL) && (destination_rcp != NULL)) {
		if ((source_rcp == destination_rcp) && (source_scene_index == destination_scene_index)) gtk_widget_set_sensitive (copy_button, FALSE);
		else gtk_widget_set_sensitive (copy_button, TRUE);
	}
}

void destination_scenes_radio_button_toggled (GtkToggleButton *button, gpointer index)
{
	if (!gtk_toggle_button_get_active (button)) return;

	destination_scene_index = GPOINTER_TO_INT (index);

	if ((destination_rcp != NULL) && (source_rcp != NULL)) {
		if ((destination_rcp == source_rcp) && (destination_scene_index == source_scene_index)) gtk_widget_set_sensitive (copy_button, FALSE);
		else gtk_widget_set_sensitive (copy_button, TRUE);
	}
}

void source_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_cameras_set_row)
{
	gint index;
	cameras_set_t *cameras_set_tmp;

	if (source_cameras_set_row == NULL) {
		gtk_widget_hide (source_cameras_set->source_rcp_list_box);
		gtk_list_box_select_row (GTK_LIST_BOX (source_cameras_set->source_rcp_list_box), NULL);
		source_cameras_set = NULL;
		return;
	}

	if (source_cameras_set != NULL) {
		gtk_widget_hide (source_cameras_set->source_rcp_list_box);
		gtk_list_box_select_row (GTK_LIST_BOX (source_cameras_set->source_rcp_list_box), NULL);
	}

	index = gtk_list_box_row_get_index (source_cameras_set_row);

	for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
		if (cameras_set_tmp->page_num == index) {
			source_cameras_set = cameras_set_tmp;
			break;
		}
	}

	gtk_widget_show_all (source_cameras_set->source_rcp_list_box);
}

void destination_cameras_set_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_cameras_set_row)
{
	gint index;
	cameras_set_t *cameras_set_tmp;

	if (destination_cameras_set_row == NULL) {
		gtk_widget_hide (destination_cameras_set->destination_rcp_list_box);
		gtk_list_box_select_row (GTK_LIST_BOX (destination_cameras_set->destination_rcp_list_box), NULL);
		destination_cameras_set = NULL;
		return;
	}

	if (destination_cameras_set != NULL) {
		gtk_widget_hide (destination_cameras_set->destination_rcp_list_box);
		gtk_list_box_select_row (GTK_LIST_BOX (destination_cameras_set->destination_rcp_list_box), NULL);
	}

	index = gtk_list_box_row_get_index (destination_cameras_set_row);

	for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
		if (cameras_set_tmp->page_num == index) {
			destination_cameras_set = cameras_set_tmp;
			break;
		}
	}

	gtk_widget_show_all (destination_cameras_set->destination_rcp_list_box);
}

gint cameras_set_source_list_box_sort (GtkWidget *row1, GtkWidget *row2)	//GtkListBoxRow
{
	cameras_set_t *cameras_set_1, *cameras_set_2;

	cameras_set_1 = cameras_sets;
	while (cameras_set_1->source_list_box_row != row1) cameras_set_1 = cameras_set_1->next;

	cameras_set_2 = cameras_sets;
	while (cameras_set_2->source_list_box_row != row2) cameras_set_2 = cameras_set_2->next;

	return cameras_set_1->page_num - cameras_set_2->page_num;
}

gint cameras_set_destination_list_box_sort (GtkWidget *row1, GtkWidget *row2)
{
	cameras_set_t *cameras_set_1, *cameras_set_2;

	cameras_set_1 = cameras_sets;
	while (cameras_set_1->destination_list_box_row != row1) cameras_set_1 = cameras_set_1->next;

	cameras_set_2 = cameras_sets;
	while (cameras_set_2->destination_list_box_row != row2) cameras_set_2 = cameras_set_2->next;

	return cameras_set_1->page_num - cameras_set_2->page_num;
}

gint rcp_list_box_sort (GtkListBoxRow *row1, GtkListBoxRow *row2, cameras_set_t *cameras_set)	//< 0 if row1 should be before row2 , 0 if they are equal and > 0 otherwise
{
	const gchar *label1, *label2;
	int index1, index2;

	label1 = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (row1))));
	label2 = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (row2))));

	for (index1 = 0; index1 < cameras_set->number_of_cameras; index1++)
	{
		if (strcmp (cameras_set->rcp_ptr_array[index1]->name, label1) == 0) break;
	}

	for (index2 = 0; index2 < cameras_set->number_of_cameras; index2++)
	{
		if (strcmp (cameras_set->rcp_ptr_array[index2]->name, label2) == 0) break;
	}

	return index1 - index2;
}

void source_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *source_rcp_row)
{
	const gchar *label;
	int i;

	if (source_rcp_row == NULL) {
		source_rcp = NULL;
		gtk_widget_set_sensitive (copy_button, FALSE);
		return;
	}

	label = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (source_rcp_row))));

	for (i = 0; i < source_cameras_set->number_of_cameras; i++) {
		if (strcmp (source_cameras_set->rcp_ptr_array[i]->name, label) == 0) {
			source_rcp = source_cameras_set->rcp_ptr_array[i];
			break;
		}
	}

	if (destination_rcp != NULL) {
		if ((source_rcp == destination_rcp) && (source_scene_index == destination_scene_index)) gtk_widget_set_sensitive (copy_button, FALSE);
		else gtk_widget_set_sensitive (copy_button, TRUE);
	}
}

void destination_rcp_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *destination_rcp_row)
{
	const gchar *label;
	int i;

	if (destination_rcp_row == NULL) {
		destination_rcp = NULL;
		gtk_widget_set_sensitive (copy_button, FALSE);
		return;
	}

	label = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (destination_rcp_row))));

	for (i = 0; i < destination_cameras_set->number_of_cameras; i++) {
		if (strcmp (destination_cameras_set->rcp_ptr_array[i]->name, label) == 0) {
			destination_rcp = destination_cameras_set->rcp_ptr_array[i];
			break;
		}
	}

	if (source_rcp != NULL) {
		if ((destination_rcp == source_rcp) && (destination_scene_index == source_scene_index)) gtk_widget_set_sensitive (copy_button, FALSE);
		else gtk_widget_set_sensitive (copy_button, TRUE);
	}
}

#define COPY_SCENE_PARAMETER(l) \
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (l##_check_button))) \
		destination_rcp->scenes[destination_scene_index].l = source_rcp->scenes[source_scene_index].l;

void copy_button_clicked (GtkButton *button)
{
	COPY_SCENE_PARAMETER(ND_filter)
	COPY_SCENE_PARAMETER(gain)

	COPY_SCENE_PARAMETER(gamma_type)
	COPY_SCENE_PARAMETER(gamma)
	COPY_SCENE_PARAMETER(drs)

	COPY_SCENE_PARAMETER(color_temperature)

	COPY_SCENE_PARAMETER(knee_settings)
	COPY_SCENE_PARAMETER(knee_point)
	COPY_SCENE_PARAMETER(knee_slope)

	COPY_SCENE_PARAMETER(matrix_type)
	COPY_SCENE_PARAMETER(linear_matrix)
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cc_saturation_check_button)))
		memcpy (destination_rcp->scenes[destination_scene_index].cc_saturation, source_rcp->scenes[source_scene_index].cc_saturation, 15);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cc_phase_check_button)))
		memcpy (destination_rcp->scenes[destination_scene_index].cc_phase, source_rcp->scenes[source_scene_index].cc_phase, 15);

	COPY_SCENE_PARAMETER(detail)
	COPY_SCENE_PARAMETER(master_detail)
	COPY_SCENE_PARAMETER(v_detail_level)
	COPY_SCENE_PARAMETER(detail_band)
	COPY_SCENE_PARAMETER(noise_suppress)
	COPY_SCENE_PARAMETER(fleshtone_noisesup)

	COPY_SCENE_PARAMETER(saturation)

	COPY_SCENE_PARAMETER(r_gain)
	COPY_SCENE_PARAMETER(b_gain)

	COPY_SCENE_PARAMETER(r_pedestal)
	COPY_SCENE_PARAMETER(b_pedestal)

	COPY_SCENE_PARAMETER(shutter_type)
	COPY_SCENE_PARAMETER(shutter_step)
	COPY_SCENE_PARAMETER(shutter_synchro)

	COPY_SCENE_PARAMETER(pedestal)

	COPY_SCENE_PARAMETER(iris)
	COPY_SCENE_PARAMETER(iris_auto)

	backup_needed = TRUE;
}

void all_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ND_filter_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gain_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (all_gamma_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (color_temperature_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (knee_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (matrix_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (all_detail_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (saturation_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (r_gain_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (b_gain_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (r_pedestal_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (b_pedestal_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shutter_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (pedestal_check_button), active);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (iris_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (iris_auto_check_button), active);
}

void all_gamma_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gamma_type_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gamma_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (drs_check_button), active);
}

void knee_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (knee_settings_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (knee_point_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (knee_slope_check_button), active);
}

void matrix_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (matrix_type_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (linear_matrix_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cc_saturation_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cc_phase_check_button), active);
}

void all_detail_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (detail_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (master_detail_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (v_detail_level_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (detail_band_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (noise_suppress_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fleshtone_noisesup_check_button), active);
}

void shutter_check_button_toggled (GtkToggleButton *toggle_button)
{
	gboolean active;

	active = gtk_toggle_button_get_active (toggle_button);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shutter_type_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shutter_step_check_button), active);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shutter_synchro_check_button), active);
}

#define SCENE_PARAMETER_CHECK_BUTTON(l,s,x,w) \
	l##_check_button = gtk_check_button_new (); \
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (l##_check_button), TRUE); \
	gtk_grid_attach (GTK_GRID (grid), l##_check_button, x, i, 1, 1); \
	widget = gtk_label_new (s); \
	gtk_widget_set_halign (widget, GTK_ALIGN_START); \
	gtk_widget_set_margin_start (widget, MARGIN_VALUE); \
	gtk_grid_attach (GTK_GRID (grid), widget, x + 1, i, w, 1); \
	i++;

#undef MARGIN_VALUE
#define MARGIN_VALUE 4

void create_scenes_page (void)
{
	GtkWidget *frame1, *frame2, *box1, *box2, *box3, *box4, *box5, *grid, *first_radio_button, *widget;
	char label[4];
	int i;

	source_scene_index = 0;
	destination_scene_index = 0;

	source_cameras_set = NULL;
	destination_cameras_set = NULL;

	source_rcp = NULL;
	destination_rcp = NULL;

	scenes_page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	frame1 = gtk_frame_new ("Source");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_container_set_border_width (GTK_CONTAINER (frame1), MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		frame2 = gtk_frame_new (cameras_set_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
			source_cameras_set_list_box = gtk_list_box_new ();
			gtk_list_box_set_sort_func (GTK_LIST_BOX (source_cameras_set_list_box), (GtkListBoxSortFunc)cameras_set_source_list_box_sort, NULL, NULL);
			gtk_container_add (GTK_CONTAINER (box2), source_cameras_set_list_box);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);

		frame2 = gtk_frame_new (cameras_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		source_rcp_list_box_box = box2;

		frame2 = gtk_frame_new (scenes_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
			box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				first_radio_button = gtk_radio_button_new_with_label (NULL, "1");
				gtk_widget_set_margin_start (first_radio_button, MARGIN_VALUE);
				g_signal_connect (first_radio_button, "toggled", G_CALLBACK (source_scenes_radio_button_toggled), GINT_TO_POINTER (0));
				gtk_box_pack_start (GTK_BOX (box5), first_radio_button, FALSE, FALSE, 0);
				for (i = 1; i < 4; i++) {
					sprintf (label, "%d", i + 1);
					widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), label);
					gtk_widget_set_margin_start (widget, MARGIN_VALUE);
					g_signal_connect (widget, "toggled", G_CALLBACK (source_scenes_radio_button_toggled), GINT_TO_POINTER (i));
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
				}
				widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), "5");
				gtk_widget_set_margin_start (widget, MARGIN_VALUE);
				g_signal_connect (widget, "toggled", G_CALLBACK (destination_scenes_radio_button_toggled), GINT_TO_POINTER (5));
				gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box5, FALSE, FALSE, 0);

			box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box5, MARGIN_VALUE);
				for (i = 5; i < NB_SCENES; i++) {
					sprintf (label, "%d", i + 1);
					widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), label);
					gtk_widget_set_margin_start (widget, MARGIN_VALUE);
					g_signal_connect (widget, "toggled", G_CALLBACK (source_scenes_radio_button_toggled), GINT_TO_POINTER (i));
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
				}
			gtk_box_pack_start (GTK_BOX (box2), box5, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame1), box1);
	gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (box4), box3, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			copy_button = gtk_button_new_with_label ("--->");
			gtk_container_set_border_width (GTK_CONTAINER (copy_button), MARGIN_VALUE);
			gtk_widget_set_sensitive (copy_button, FALSE);
			g_signal_connect (copy_button, "clicked", G_CALLBACK (copy_button_clicked), NULL);
			gtk_box_set_center_widget (GTK_BOX (box2), copy_button);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		grid = gtk_grid_new ();
		gtk_container_set_border_width (GTK_CONTAINER (copy_button), MARGIN_VALUE);
			widget = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
			g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (all_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), widget, 0, 0, 1, 1);
			widget = gtk_label_new ("Tout");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 1, 0, 3, 1);

			i = 1;

			SCENE_PARAMETER_CHECK_BUTTON(ND_filter,"Filtre neutre (Through, 1/8, 1/64)",1,2)
			SCENE_PARAMETER_CHECK_BUTTON(gain,"Gain (Auto, 0dB ~ 36dB)",1,2)

			all_gamma_check_button = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (all_gamma_check_button), TRUE);
			g_signal_connect (G_OBJECT (all_gamma_check_button), "toggled", G_CALLBACK (all_gamma_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), all_gamma_check_button, 1, i, 1, 1);
			widget = gtk_label_new ("Gamma");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 2, i, 2, 1);
			i++;

			SCENE_PARAMETER_CHECK_BUTTON(gamma_type,"Gamma type (HD, SD, FILMLIKE1, FILMLIKE2, FILMLIKE3)",2,1)
			gtk_widget_set_tooltip_text (gamma_type_check_button, gamma_type_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(gamma,"Gamma (0,30 ~ 0,75)",2,1)
			gtk_widget_set_tooltip_text (gamma_check_button, gamma_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(drs,"DRS (Off, Low, Mid, High)",2,1)
			gtk_widget_set_tooltip_text (drs_check_button, drs_tooltip);

			SCENE_PARAMETER_CHECK_BUTTON(color_temperature,"Température de couleur (2000K ~ 15000K)",1,2)

			knee_check_button = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (knee_check_button), TRUE);
			g_signal_connect (G_OBJECT (knee_check_button), "toggled", G_CALLBACK (knee_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), knee_check_button, 1, i, 1, 1);
			widget = gtk_label_new ("Knee");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 2, i, 2, 1);
			i++;

			SCENE_PARAMETER_CHECK_BUTTON(knee_settings,"Knee (Off/Manual/Auto)",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(knee_point,"Knee point",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(knee_slope,"Knee slope",2,1)

			matrix_check_button = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (matrix_check_button), TRUE);
			g_signal_connect (G_OBJECT (matrix_check_button), "toggled", G_CALLBACK (matrix_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), matrix_check_button, 1, i, 1, 1);
			widget = gtk_label_new ("Matrix");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 2, i, 2, 1);
			i++;

			SCENE_PARAMETER_CHECK_BUTTON(matrix_type,"Matrix type (Normal, EBU, NTSC, User)",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(linear_matrix,"Linear matrix",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(cc_saturation,"Color correction saturation",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(cc_phase,"Color correction phase",2,1)

			all_detail_check_button = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (all_detail_check_button), TRUE);
			g_signal_connect (G_OBJECT (all_detail_check_button), "toggled", G_CALLBACK (all_detail_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), all_detail_check_button, 1, i, 1, 1);
			widget = gtk_label_new ("Détail");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 2, i, 2, 1);
			i++;

			SCENE_PARAMETER_CHECK_BUTTON(detail,"Détail (On/Off)",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(master_detail,"Master detail",2,1)
			gtk_widget_set_tooltip_text (master_detail_check_button, master_detail_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(v_detail_level,"V detail level",2,1)
			gtk_widget_set_tooltip_text (v_detail_level_check_button, v_detail_level_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(detail_band,"Detail band",2,1)
			gtk_widget_set_tooltip_text (detail_band_check_button, detail_band_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(noise_suppress,"Noise suppress",2,1)
			gtk_widget_set_tooltip_text (noise_suppress_check_button, noise_suppress_tooltip);
			SCENE_PARAMETER_CHECK_BUTTON(fleshtone_noisesup,"FleshTone noise suppress",2,1)
			gtk_widget_set_tooltip_text (fleshtone_noisesup_check_button, fleshtone_noisesup_tooltip);

			SCENE_PARAMETER_CHECK_BUTTON(saturation,"Saturation",1,2)

			SCENE_PARAMETER_CHECK_BUTTON(r_gain,"Gain rouge",1,2)
			SCENE_PARAMETER_CHECK_BUTTON(b_gain,"Gain bleu",1,2)

			SCENE_PARAMETER_CHECK_BUTTON(r_pedestal,"Piédestal rouge",1,2)
			SCENE_PARAMETER_CHECK_BUTTON(b_pedestal,"Piédestal bleu",1,2)

			shutter_check_button = gtk_check_button_new ();
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shutter_check_button), TRUE);
			g_signal_connect (G_OBJECT (shutter_check_button), "toggled", G_CALLBACK (shutter_check_button_toggled), NULL);
			gtk_grid_attach (GTK_GRID (grid), shutter_check_button, 1, i, 1, 1);
			widget = gtk_label_new ("Shutter");
			gtk_widget_set_halign (widget, GTK_ALIGN_START);
			gtk_widget_set_margin_start (widget, MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (grid), widget, 2, i, 2, 1);
			i++;

			SCENE_PARAMETER_CHECK_BUTTON(shutter_type,"Shutter mode (Off, Step, Synchro, ELC)",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(shutter_step,"Shutter step",2,1)
			SCENE_PARAMETER_CHECK_BUTTON(shutter_synchro,"Shutter synchro",2,1)

			SCENE_PARAMETER_CHECK_BUTTON(pedestal,"Piédestal",1,2)

			SCENE_PARAMETER_CHECK_BUTTON(iris,"Iris",1,2)
			SCENE_PARAMETER_CHECK_BUTTON(iris_auto,"Iris auto (On/Off)",1,2)
		gtk_box_pack_start (GTK_BOX (box1), grid, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	frame1 = gtk_frame_new ("Destination");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_container_set_border_width (GTK_CONTAINER (frame1), MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		frame2 = gtk_frame_new (cameras_set_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
			destination_cameras_set_list_box = gtk_list_box_new ();
			gtk_list_box_set_sort_func (GTK_LIST_BOX (destination_cameras_set_list_box), (GtkListBoxSortFunc)cameras_set_destination_list_box_sort, NULL, NULL);
			gtk_container_add (GTK_CONTAINER (box2), destination_cameras_set_list_box);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);

		frame2 = gtk_frame_new (cameras_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		destination_rcp_list_box_box = box2;

		frame2 = gtk_frame_new (scenes_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_start (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, MARGIN_VALUE);
			box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			first_radio_button = gtk_radio_button_new_with_label (NULL, "1");
			gtk_widget_set_margin_start (first_radio_button, MARGIN_VALUE);
			g_signal_connect (first_radio_button, "toggled", G_CALLBACK (destination_scenes_radio_button_toggled), GINT_TO_POINTER (0));
			gtk_box_pack_start (GTK_BOX (box5), first_radio_button, FALSE, FALSE, 0);
			for (i = 1; i < 4; i++) {
				sprintf (label, "%d", i + 1);
				widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), label);
				gtk_widget_set_margin_start (widget, MARGIN_VALUE);
				g_signal_connect (widget, "toggled", G_CALLBACK (destination_scenes_radio_button_toggled), GINT_TO_POINTER (i));
				gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
			}
				widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), "5 ");
				gtk_widget_set_margin_start (widget, MARGIN_VALUE);
				g_signal_connect (widget, "toggled", G_CALLBACK (destination_scenes_radio_button_toggled), GINT_TO_POINTER (5));
				gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box5, FALSE, FALSE, 0);

			box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box5, MARGIN_VALUE);
			for (i = 5; i < NB_SCENES; i++) {
				sprintf (label, "%d", i + 1);
				widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (first_radio_button), label);
				gtk_widget_set_margin_start (widget, MARGIN_VALUE);
				g_signal_connect (widget, "toggled", G_CALLBACK (destination_scenes_radio_button_toggled), GINT_TO_POINTER (i));
				gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
			}
			gtk_box_pack_start (GTK_BOX (box2), box5, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame1), box1);
	gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (box4), box3, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (scenes_page), box4, FALSE, FALSE, 0);

	widget = gtk_label_new (key_info_2_txt);
	gtk_box_pack_end (GTK_BOX (scenes_page), widget, FALSE, FALSE, 0);

	widget = gtk_label_new (scenes_label);
	gtk_notebook_append_page (GTK_NOTEBOOK (main_window_notebook), scenes_page, widget);
	gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (main_window_notebook), scenes_page, FALSE);
}

