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

#include <string.h>
#include <stdio.h>


char *cameras_set_label = "Ensembles de caméras";
char *cameras_label = "Caméras";

GMutex cameras_sets_mutex;
GMutex current_cameras_set_mutex;

int number_of_cameras_sets = 0;

cameras_set_t *cameras_sets = NULL;

cameras_set_t *current_cameras_set = NULL;

cameras_set_t *new_cameras_set = NULL;

cameras_set_t *cameras_set_with_error = NULL;

//cameras_set_configuration_window
GtkWidget *cameras_set_configuration_window;
GtkEntryBuffer *cameras_set_configuration_name_entry_buffer;
int new_number_of_cameras;
int old_number_of_cameras;

char *nb[MAX_CAMERAS] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15" };

GtkWidget *cameras_set_configuration_window_grid;

typedef struct {
	GtkWidget *camera_switch;
	GtkEntryBuffer *name_entry_buffer;
	GtkWidget *name_entry;
	GtkEntryBuffer *ip_entry_buffer[4];
	GtkWidget *ip_entry[4];
	GtkWidget *dot[3];
} camera_configuration_widgets_t;

camera_configuration_widgets_t *cameras_configuration_widgets;


void camera_switch_activated (GtkSwitch *camera_switch, GParamSpec *pspec, camera_configuration_widgets_t *camera_configuration_widgets)
{
	gboolean active;

	active = gtk_switch_get_active (camera_switch);

	gtk_widget_set_sensitive (camera_configuration_widgets->ip_entry[0], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->dot[0], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->ip_entry[1], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->dot[1], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->ip_entry[2], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->dot[2], active);
	gtk_widget_set_sensitive (camera_configuration_widgets->ip_entry[3], active);
}

void number_of_cameras_has_changed (GtkComboBoxText *combo_box_text, cameras_set_t *cameras_set)
{
	int i;
	char *active_string;

	active_string = gtk_combo_box_text_get_active_text (combo_box_text);
	sscanf (active_string, "%d", &new_number_of_cameras);
	g_free (active_string);

	if (new_number_of_cameras < old_number_of_cameras) {
		for (i = old_number_of_cameras - 1; i >= new_number_of_cameras; i--) {
			gtk_widget_hide (cameras_configuration_widgets[i].ip_entry[3]);
			gtk_widget_hide (cameras_configuration_widgets[i].dot[2]);
			gtk_widget_hide (cameras_configuration_widgets[i].ip_entry[2]);
			gtk_widget_hide (cameras_configuration_widgets[i].dot[1]);
			gtk_widget_hide (cameras_configuration_widgets[i].ip_entry[1]);
			gtk_widget_hide (cameras_configuration_widgets[i].dot[0]);
			gtk_widget_hide (cameras_configuration_widgets[i].ip_entry[0]);
			gtk_widget_hide (cameras_configuration_widgets[i].name_entry);
			gtk_widget_hide (cameras_configuration_widgets[i].camera_switch);
		}
	} else if (new_number_of_cameras > old_number_of_cameras) {
		for (i = old_number_of_cameras; i < new_number_of_cameras; i++) {
			gtk_widget_show (cameras_configuration_widgets[i].camera_switch);
			gtk_widget_show (cameras_configuration_widgets[i].name_entry);
			gtk_widget_show (cameras_configuration_widgets[i].ip_entry[0]);
			gtk_widget_show (cameras_configuration_widgets[i].dot[0]);
			gtk_widget_show (cameras_configuration_widgets[i].ip_entry[1]);
			gtk_widget_show (cameras_configuration_widgets[i].dot[1]);
			gtk_widget_show (cameras_configuration_widgets[i].ip_entry[2]);
			gtk_widget_show (cameras_configuration_widgets[i].dot[2]);
			gtk_widget_show (cameras_configuration_widgets[i].ip_entry[3]);
		}
	}

	old_number_of_cameras = new_number_of_cameras;

	gtk_window_resize (GTK_WINDOW (cameras_set_configuration_window), 200, 200);
}

void cameras_set_configuration_window_ok (GtkWidget *button, cameras_set_t *cameras_set)
{
	int i, j, ip[4];
	const gchar *entry_buffer_text;
	rcp_t *rcp, *updated_rcp, *other_rcp;
	GtkWidget *box, *widget, *settings_list_box_row;
	gboolean camera_is_active;
	GList *glist_itr, *glist_itr_next;
	GSList *ip_addresss_list, *gslist_itr;
	GtkListBoxRow *source_cameras_set_row, *destination_cameras_set_row;
	const gchar *source_cameras_set_row_name, *destination_cameras_set_row_name;
	gboolean source_cameras_set_is_selected, destination_cameras_set_is_selected;

	g_mutex_lock (&cameras_sets_mutex);

	entry_buffer_text = gtk_entry_buffer_get_text (cameras_set_configuration_name_entry_buffer);
	if (strcmp (cameras_set->name, entry_buffer_text) != 0) {
		strcpy (cameras_set->name, entry_buffer_text);
		if (new_cameras_set == NULL) {
			gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (main_window_notebook), cameras_set->page, entry_buffer_text);
			gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (cameras_set->source_list_box_row))), entry_buffer_text);
			gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (cameras_set->destination_list_box_row))), entry_buffer_text);
			gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (gtk_list_box_get_selected_row (GTK_LIST_BOX (settings_list_box))))), entry_buffer_text);
		}
	}

	source_cameras_set_is_selected = FALSE;
	source_cameras_set_row = gtk_list_box_get_selected_row (GTK_LIST_BOX (source_cameras_set_list_box));
	if (source_cameras_set_row != NULL) {
		source_cameras_set_row_name = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (source_cameras_set_row))));
		if (strcmp (source_cameras_set_row_name, cameras_set->name) == 0) source_cameras_set_is_selected = TRUE;
	}

	destination_cameras_set_is_selected = FALSE;
	destination_cameras_set_row = gtk_list_box_get_selected_row (GTK_LIST_BOX (destination_cameras_set_list_box));
	if (destination_cameras_set_row != NULL) {
		destination_cameras_set_row_name = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (destination_cameras_set_row))));
		if (strcmp (destination_cameras_set_row_name, cameras_set->name) == 0) destination_cameras_set_is_selected = TRUE;
	}

	if (new_cameras_set != NULL) {
		widget = gtk_list_box_new ();
		gtk_list_box_set_sort_func (GTK_LIST_BOX (widget), (GtkListBoxSortFunc)rcp_list_box_sort, cameras_set, NULL);
		g_signal_connect (G_OBJECT (widget), "row-selected", G_CALLBACK (source_rcp_list_box_row_selected), NULL);
		gtk_box_pack_start (GTK_BOX (source_rcp_list_box_box), widget, FALSE, FALSE, 0);
		cameras_set->source_rcp_list_box = widget;

		widget = gtk_list_box_new ();
		gtk_list_box_set_sort_func (GTK_LIST_BOX (widget), (GtkListBoxSortFunc)rcp_list_box_sort, cameras_set, NULL);
		g_signal_connect (G_OBJECT (widget), "row-selected", G_CALLBACK (destination_rcp_list_box_row_selected), NULL);
		gtk_box_pack_start (GTK_BOX (destination_rcp_list_box_box), widget, FALSE, FALSE, 0);
		cameras_set->destination_rcp_list_box = widget;

		for (i = 0; ((i < cameras_set->number_of_cameras) && (i < new_number_of_cameras)); i++) {
			rcp = cameras_set->rcp_ptr_array[i];

			rcp->active = gtk_switch_get_active (GTK_SWITCH (cameras_configuration_widgets[i].camera_switch));

			entry_buffer_text = gtk_entry_buffer_get_text (cameras_configuration_widgets[i].name_entry_buffer);
			strcpy (rcp->name, entry_buffer_text);

			if (rcp->active) {
				create_rcp_widgets (rcp);

				widget = gtk_label_new (rcp->name);
				rcp->source_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->source_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->source_rcp_list_box), rcp->source_rcp_list_box_row);

				widget = gtk_label_new (rcp->name);
				rcp->destination_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->destination_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->destination_rcp_list_box), rcp->destination_rcp_list_box_row);
			} else {
				create_ghost_rcp_widgets (rcp);
			}

			gtk_box_pack_start (GTK_BOX (cameras_set->rcp_box), rcp->root_widget, FALSE, FALSE, 0);
		}
		init_master_rcp (cameras_set);

		settings_list_box_row = gtk_list_box_row_new ();
		widget = gtk_label_new (cameras_set->name);
		gtk_widget_set_margin_start (widget, MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_container_add (GTK_CONTAINER (settings_list_box_row), widget);
		gtk_container_add (GTK_CONTAINER (settings_list_box), settings_list_box_row);
		gtk_widget_show_all (settings_list_box_row);
		gtk_list_box_select_row (GTK_LIST_BOX (settings_list_box), GTK_LIST_BOX_ROW (settings_list_box_row));

		cameras_set->page = gtk_scrolled_window_new (NULL, NULL);
		box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start (GTK_BOX (box), cameras_set->rcp_box, FALSE, FALSE, 0);
		widget = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		widget = gtk_label_new (key_info_1_txt);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (cameras_set->page), box);

		gtk_widget_show_all (cameras_set->page);

		for (i = 0; ((i < cameras_set->number_of_cameras) && (i < new_number_of_cameras)); i++) {
			if (cameras_set->rcp_ptr_array[i]->active) {
				gtk_widget_hide (cameras_set->rcp_ptr_array[i]->scenes_bank_2_box);

				gtk_widget_hide (cameras_set->rcp_ptr_array[i]->shutter_step_combo_box);
				gtk_widget_hide (cameras_set->rcp_ptr_array[i]->shutter_synchro_button);

				if (rcp->model == AW_UE150) {
					gtk_widget_hide (cameras_set->rcp_ptr_array[i]->HLG_knee_frame);
					gtk_widget_hide (cameras_set->rcp_ptr_array[i]->ELC_limit_label);
					gtk_widget_hide (cameras_set->rcp_ptr_array[i]->ELC_limit_combo_box);
				}
			}
		}
		gtk_widget_hide (cameras_set->master_rcp.scenes_bank_2_box);

		if (!show_master_rcp) gtk_widget_hide (cameras_set->master_rcp.root_widget);

		cameras_set->next = cameras_sets;
		cameras_sets = cameras_set;

		widget = gtk_label_new (cameras_set->name);
		cameras_set->page_num = gtk_notebook_insert_page (GTK_NOTEBOOK (main_window_notebook), cameras_set->page, widget, number_of_cameras_sets);
		gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (main_window_notebook), cameras_set->page, TRUE);

		number_of_cameras_sets++;

		widget = gtk_label_new (cameras_set->name);
		cameras_set->source_list_box_row = gtk_list_box_row_new ();
		gtk_container_add (GTK_CONTAINER (cameras_set->source_list_box_row), widget);
		gtk_container_add (GTK_CONTAINER (source_cameras_set_list_box), cameras_set->source_list_box_row);
		gtk_widget_show_all (cameras_set->source_list_box_row);

		widget = gtk_label_new (cameras_set->name);
		cameras_set->destination_list_box_row = gtk_list_box_row_new ();
		gtk_container_add (GTK_CONTAINER (cameras_set->destination_list_box_row), widget);
		gtk_container_add (GTK_CONTAINER (destination_cameras_set_list_box), cameras_set->destination_list_box_row);
		gtk_widget_show_all (cameras_set->destination_list_box_row);
	} else {
		for (i = new_number_of_cameras; i < cameras_set->number_of_cameras; i++) {
			rcp = cameras_set->rcp_ptr_array[i];

			g_mutex_lock (&sw_p_08_mutex);
			if (rcp == rcp_vision) rcp_vision = NULL;
			if (rcp == rcp_pgm) rcp_pgm = NULL;
			if (rcp == rcp_pvw) rcp_pvw = NULL;
			g_mutex_unlock (&sw_p_08_mutex);

			if (rcp->ip_address_is_valid && (rcp->other_rcp == NULL) && (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR)) {
				send_ptz_control_command (rcp, "#LPC0");
				send_update_stop_cmd (rcp);
			}

			glist_itr = rcp_start_glist;
			while (glist_itr != NULL) {
				glist_itr_next = glist_itr->next;

				if (glist_itr->data == rcp) {
					if (((rcp_t*)(glist_itr->data))->other_rcp != NULL) glist_itr->data = ((rcp_t*)(glist_itr->data))->other_rcp;
					else rcp_start_glist = g_list_delete_link (rcp_start_glist, glist_itr);
					break;
				}

				glist_itr = glist_itr_next;
			}

			rcp_glist = g_list_remove (rcp_glist, rcp);

			for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
				other_rcp = (rcp_t*)(glist_itr->data);

				g_mutex_lock (&other_rcp->other_rcp_mutex);
				other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
				g_mutex_unlock (&other_rcp->other_rcp_mutex);
			}

			widget = rcp->root_widget;
			rcp->root_widget = NULL;
			if (widget != NULL) gtk_widget_destroy (widget);
			if (rcp->destination_rcp_list_box_row != NULL) gtk_widget_destroy (rcp->destination_rcp_list_box_row);
			if (rcp->source_rcp_list_box_row != NULL) gtk_widget_destroy (rcp->source_rcp_list_box_row);
			if (rcp->shutter_synchro_window != NULL) gtk_widget_destroy (rcp->shutter_synchro_window);
			if (rcp->detail_window != NULL) gtk_widget_destroy (rcp->detail_window);
			if (rcp->matrix_window != NULL) gtk_widget_destroy (rcp->matrix_window);
			if (rcp->knee_window != NULL) gtk_widget_destroy (rcp->knee_window);

			g_mutex_lock (&rcp->other_rcp_mutex);
			g_slist_free (rcp->other_rcp);
			rcp->other_rcp = NULL;
			g_mutex_unlock (&rcp->other_rcp_mutex);
			g_mutex_clear (&rcp->other_rcp_mutex);

			if (!rcp->camera_is_working) {
				if (rcp->thread != NULL) g_thread_join (rcp->thread);
				g_mutex_clear (&rcp->cmd_mutex);
				g_free (rcp);
			}
		}
	}

	if (new_number_of_cameras > cameras_set->number_of_cameras) {
		cameras_set->rcp_ptr_array = g_realloc (cameras_set->rcp_ptr_array, new_number_of_cameras * sizeof (rcp_t*));

		for (i = cameras_set->number_of_cameras; i < new_number_of_cameras; i++) {
			rcp = g_malloc (sizeof (rcp_t));
			cameras_set->rcp_ptr_array[i] = rcp;
			rcp->cameras_set = cameras_set;

			entry_buffer_text = gtk_entry_buffer_get_text (cameras_configuration_widgets[i].name_entry_buffer);
			strcpy (rcp->name, entry_buffer_text);

			init_rcp (rcp);

			rcp->index = i;

			rcp->active = gtk_switch_get_active (GTK_SWITCH (cameras_configuration_widgets[i].camera_switch));

			if (rcp->active) {
				create_rcp_widgets (rcp);

				widget = gtk_label_new (rcp->name);
				rcp->source_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->source_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->source_rcp_list_box), rcp->source_rcp_list_box_row);
				if (source_cameras_set_is_selected) gtk_widget_show_all (rcp->source_rcp_list_box_row);

				widget = gtk_label_new (rcp->name);
				rcp->destination_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->destination_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->destination_rcp_list_box), rcp->destination_rcp_list_box_row);
				if (destination_cameras_set_is_selected) gtk_widget_show_all (rcp->destination_rcp_list_box_row);

				gtk_widget_show_all (rcp->root_widget);
				gtk_widget_hide (rcp->scenes_bank_2_box);

				gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);

				if (rcp->model == AW_UE150) {
					gtk_widget_hide (rcp->HLG_knee_frame);
					gtk_widget_hide (rcp->ELC_limit_label);
					gtk_widget_hide (rcp->ELC_limit_combo_box);
				}
			} else {
				create_ghost_rcp_widgets (rcp);
				gtk_widget_show_all (rcp->root_widget);
			}

			gtk_box_pack_start (GTK_BOX (cameras_set->rcp_box), rcp->root_widget, FALSE, FALSE, 0);
		}
		gtk_box_reorder_child (GTK_BOX (cameras_set->rcp_box), cameras_set->master_rcp.root_widget, -1);
	}

	ip_addresss_list = NULL;

	for (i = 0; i < new_number_of_cameras; i++) {
		rcp = cameras_set->rcp_ptr_array[i];

		camera_is_active = gtk_switch_get_active (GTK_SWITCH (cameras_configuration_widgets[i].camera_switch));

		if (camera_is_active != rcp->active) {
			widget = rcp->root_widget;
			rcp->root_widget = NULL;
			if (widget != NULL) gtk_widget_destroy (widget);

			if (camera_is_active) {
				rcp->active = TRUE;

				ghost_rcp_glist = g_list_remove (ghost_rcp_glist, rcp);

				create_rcp_widgets (rcp);

				widget = gtk_label_new (rcp->name);
				rcp->source_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->source_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->source_rcp_list_box), rcp->source_rcp_list_box_row);
				if (source_cameras_set_is_selected) gtk_widget_show_all (rcp->source_rcp_list_box_row);

				widget = gtk_label_new (rcp->name);
				rcp->destination_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->destination_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set->destination_rcp_list_box), rcp->destination_rcp_list_box_row);
				if (destination_cameras_set_is_selected) gtk_widget_show_all (rcp->destination_rcp_list_box_row);

				gtk_widget_show_all (rcp->root_widget);
				gtk_widget_hide (rcp->scenes_bank_2_box);

				gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);

				if (rcp->model == AW_UE150) {
					gtk_widget_hide (rcp->HLG_knee_frame);
					gtk_widget_hide (rcp->ELC_limit_label);
					gtk_widget_hide (rcp->ELC_limit_combo_box);
				}
			} else {
				rcp->active = FALSE;

				if (rcp->ip_address_is_valid && (rcp->other_rcp == NULL) && (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR)) {
					send_ptz_control_command (rcp, "#LPC0");
					send_update_stop_cmd (rcp);
				}

				glist_itr = rcp_start_glist;
				while (glist_itr != NULL) {
					glist_itr_next = glist_itr->next;

					if (glist_itr->data == rcp) {
						if (((rcp_t*)(glist_itr->data))->other_rcp != NULL) glist_itr->data = ((rcp_t*)(glist_itr->data))->other_rcp;
						else rcp_start_glist = g_list_delete_link (rcp_start_glist, glist_itr);
						break;
					}

					glist_itr = glist_itr_next;
				}

				rcp_glist = g_list_remove (rcp_glist, rcp);

				for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					other_rcp = (rcp_t*)(glist_itr->data);

					g_mutex_lock (&other_rcp->other_rcp_mutex);
					other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
					g_mutex_unlock (&other_rcp->other_rcp_mutex);
				}

				if (rcp->destination_rcp_list_box_row != NULL) {
					gtk_widget_destroy (rcp->destination_rcp_list_box_row);
					rcp->destination_rcp_list_box_row = NULL;
				}
				if (rcp->source_rcp_list_box_row != NULL) {
					gtk_widget_destroy (rcp->source_rcp_list_box_row);
					rcp->source_rcp_list_box_row = NULL;
				}
				if (rcp->shutter_synchro_window != NULL) {
					gtk_widget_destroy (rcp->shutter_synchro_window);
					rcp->shutter_synchro_window = NULL;
				}
				if (rcp->detail_window != NULL) {
					gtk_widget_destroy (rcp->detail_window);
					rcp->detail_window = NULL;
				}
				if (rcp->matrix_window != NULL) {
					gtk_widget_destroy (rcp->matrix_window);
					rcp->matrix_window = NULL;
				}
				if (rcp->knee_window != NULL) {
					gtk_widget_destroy (rcp->knee_window);
					rcp->knee_window = NULL;
				}

				g_mutex_lock (&rcp->other_rcp_mutex);
				g_slist_free (rcp->other_rcp);
				rcp->other_rcp = NULL;
				g_mutex_unlock (&rcp->other_rcp_mutex);

				rcp->ip_address[0] = '\0';
				rcp->ip_address_is_valid = FALSE;
				rcp->camera_is_on = FALSE;

				rcp->selected_color = nothing;

				create_ghost_rcp_widgets (rcp);
				gtk_widget_show_all (rcp->root_widget);
			}

			gtk_box_pack_start (GTK_BOX (cameras_set->rcp_box), rcp->root_widget, FALSE, FALSE, 0);
			gtk_box_reorder_child (GTK_BOX (cameras_set->rcp_box), rcp->root_widget, i);
		}

		entry_buffer_text = gtk_entry_buffer_get_text (cameras_configuration_widgets[i].name_entry_buffer);
		if (strcmp (rcp->name, entry_buffer_text) != 0) {
			strcpy (rcp->name, entry_buffer_text);
			gtk_widget_queue_draw (rcp->name_drawing_area);
			if (camera_is_active) {
				gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (rcp->source_rcp_list_box_row))), rcp->name);
				gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (rcp->destination_rcp_list_box_row))), rcp->name);
			}
		}

		if (!camera_is_active) continue;

		for (j = 0; j < 4; j++)
		{
			if (sscanf (gtk_entry_buffer_get_text (cameras_configuration_widgets[i].ip_entry_buffer[j]), "%d", &ip[j]) != 1) break;
			else if (ip[j] < 0) break;
			else if (ip[j] > 254) break;
		}

		if (rcp->thread != NULL) g_thread_join (rcp->thread);
		rcp->thread = NULL;

		if (j == 4) {
			sprintf (rcp->new_ip_address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

			for (gslist_itr = ip_addresss_list; gslist_itr != NULL; gslist_itr = gslist_itr->next)
				if (strcmp ((char*)(gslist_itr->data), rcp->new_ip_address) == 0) break;

			if (gslist_itr != NULL) {
				rcp->ip_address_is_valid = FALSE;
				rcp->ip_address[0] = '\0';

				for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					other_rcp = (rcp_t*)(glist_itr->data);

					g_mutex_lock (&other_rcp->other_rcp_mutex);
					other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
					g_mutex_unlock (&other_rcp->other_rcp_mutex);
				}

				g_mutex_lock (&rcp->other_rcp_mutex);
				g_slist_free (rcp->other_rcp);
				g_mutex_unlock (&rcp->other_rcp_mutex);
				rcp->other_rcp = NULL;

				set_rcp_off (rcp);
				gtk_widget_set_sensitive (rcp->on_standby_switch, FALSE);
			} else {
				rcp->ip_address_is_valid = TRUE;
				gtk_widget_set_sensitive (rcp->on_standby_switch, TRUE);

				ip_addresss_list = g_slist_prepend (ip_addresss_list, rcp->new_ip_address);

				rcp->address.sin_addr.s_addr = inet_addr (rcp->new_ip_address);

				if (strcmp (rcp->new_ip_address, rcp->ip_address) != 0) {
					if (rcp->camera_is_on) {
						if ((rcp->other_rcp == NULL) && (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR)) {
							send_ptz_control_command (rcp, "#LPC0");
							send_update_stop_cmd (rcp);
						}
					}

					glist_itr = rcp_start_glist;
					while (glist_itr != NULL) {
						glist_itr_next = glist_itr->next;

						if (glist_itr->data == rcp) {
							if (((rcp_t*)(glist_itr->data))->other_rcp != NULL) glist_itr->data = ((rcp_t*)(glist_itr->data))->other_rcp;
							else rcp_start_glist = g_list_delete_link (rcp_start_glist, glist_itr);
							break;
						}

						glist_itr = glist_itr_next;
					}

					for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
						other_rcp = (rcp_t*)(glist_itr->data);

						g_mutex_lock (&other_rcp->other_rcp_mutex);
						other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
						g_mutex_unlock (&other_rcp->other_rcp_mutex);
					}

					g_mutex_lock (&rcp->other_rcp_mutex);
					g_slist_free (rcp->other_rcp);
					g_mutex_unlock (&rcp->other_rcp_mutex);
					rcp->other_rcp = NULL;

					for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
						other_rcp = (rcp_t*)(glist_itr->data);

						if (strcmp (other_rcp->ip_address, rcp->new_ip_address) == 0) {
							g_mutex_lock (&rcp->other_rcp_mutex);
							rcp->other_rcp = g_slist_prepend (rcp->other_rcp, other_rcp);
							g_mutex_unlock (&rcp->other_rcp_mutex);

							g_mutex_lock (&other_rcp->other_rcp_mutex);
							other_rcp->other_rcp = g_slist_prepend (other_rcp->other_rcp, rcp);
							g_mutex_unlock (&other_rcp->other_rcp_mutex);
						}
					}

					strcpy (rcp->ip_address, rcp->new_ip_address);

					if (rcp->other_rcp == NULL) {
						rcp_start_glist = g_list_prepend (rcp_start_glist, rcp);

						rcp_start_working (rcp);
						rcp->thread = g_thread_new (NULL, (GThreadFunc)check_if_camera_is_on, rcp);
					} else {
						updated_rcp = (rcp_t*)(rcp->other_rcp->data);

						for (gslist_itr = rcp->other_rcp->next; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
							other_rcp = (rcp_t*)(gslist_itr->data);

							if (other_rcp->last_time.tv_sec > updated_rcp->last_time.tv_sec) updated_rcp = other_rcp;
							else if ((other_rcp->last_time.tv_sec == updated_rcp->last_time.tv_sec) && \
								(other_rcp->last_time.tv_usec > updated_rcp->last_time.tv_usec)) updated_rcp = other_rcp;
						}

						if (rcp->model != updated_rcp->model) {
							if (updated_rcp->model == AW_HE130) {
								init_rcp_AW_HE130 (rcp);
								switch_to_AW_HE130 (rcp);
							} else {
								init_rcp_AW_UE150 (rcp);
								switch_to_AW_UE150 (rcp);
							}
						}

						copy_rcp (rcp, updated_rcp);
					}
				}
			}
		} else {
			rcp->ip_address_is_valid = FALSE;
			rcp->ip_address[0] = '\0';

			for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
				other_rcp = (rcp_t*)(glist_itr->data);

				g_mutex_lock (&other_rcp->other_rcp_mutex);
				other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
				g_mutex_unlock (&other_rcp->other_rcp_mutex);
			}

			g_mutex_lock (&rcp->other_rcp_mutex);
			g_slist_free (rcp->other_rcp);
			rcp->other_rcp = NULL;
			g_mutex_unlock (&rcp->other_rcp_mutex);

			set_rcp_off (rcp);
			gtk_widget_set_sensitive (rcp->on_standby_switch, FALSE);
		}
	}

	g_slist_free (ip_addresss_list);

	gtk_widget_destroy (cameras_set_configuration_window);

	g_free (cameras_configuration_widgets);

	if (new_cameras_set != NULL) {
		for (i = 0; i < new_number_of_cameras; i++) {
			if (cameras_set->rcp_ptr_array[i]->active) rcp_glist = g_list_prepend (rcp_glist, cameras_set->rcp_ptr_array[i]);
			else ghost_rcp_glist = g_list_prepend (ghost_rcp_glist, cameras_set->rcp_ptr_array[i]);
		}

		new_cameras_set = NULL;

		if (number_of_cameras_sets == 1) {
			gtk_list_box_select_row (GTK_LIST_BOX (source_cameras_set_list_box), GTK_LIST_BOX_ROW (cameras_set->source_list_box_row));
			source_cameras_set = cameras_set;
			gtk_list_box_select_row (GTK_LIST_BOX (destination_cameras_set_list_box), GTK_LIST_BOX_ROW (cameras_set->destination_list_box_row));
			destination_cameras_set = cameras_set;
		}

		if (number_of_cameras_sets == MAX_CAMERAS_SET) gtk_widget_set_sensitive (settings_new_button, FALSE);
		if (number_of_cameras_sets == 1) gtk_widget_show (scenes_page);
	} else {
		for (i = cameras_set->number_of_cameras; i < new_number_of_cameras; i++) {
			if (rcp->active) rcp_glist = g_list_prepend (rcp_glist, cameras_set->rcp_ptr_array[i]);
			else ghost_rcp_glist = g_list_prepend (ghost_rcp_glist, cameras_set->rcp_ptr_array[i]);
		}
	}

	cameras_set->number_of_cameras = new_number_of_cameras;

	g_mutex_unlock (&cameras_sets_mutex);

	backup_needed = TRUE;
}

void cameras_set_configuration_window_cancel (void)
{
	int i;

	gtk_widget_destroy (cameras_set_configuration_window);

	g_free (cameras_configuration_widgets);

	if (new_cameras_set != NULL) {
		gtk_widget_destroy (new_cameras_set->rcp_box);

		for (i = 0; i < 5; i++) {
			g_mutex_clear (&new_cameras_set->rcp_ptr_array[i]->other_rcp_mutex);
			g_mutex_clear (&new_cameras_set->rcp_ptr_array[i]->cmd_mutex);
			g_free (new_cameras_set->rcp_ptr_array[i]);
		}
		g_free (new_cameras_set->rcp_ptr_array);
		g_free (new_cameras_set);
		new_cameras_set = NULL;
	}
}

gboolean cameras_set_configuration_window_key_press (GtkWidget *confirmation_window, GdkEventKey *event)
{
	if (event->keyval == GDK_KEY_Escape) cameras_set_configuration_window_cancel ();

	return GDK_EVENT_PROPAGATE;
}

void show_cameras_set_configuration_window (void)
{
	gint index;
	cameras_set_t *cameras_set;
	GtkWidget *box1, *box2, *widget, *frame2, *button;
	int i, j, k, l;
	rcp_t *rcp;
	char camera_name[3];

	if (cameras_set_with_error != NULL) {
		cameras_set = cameras_set_with_error;
		cameras_set_with_error = NULL;
	} else if (new_cameras_set == NULL) {
		index = gtk_list_box_row_get_index (gtk_list_box_get_selected_row (GTK_LIST_BOX (settings_list_box)));

		for (cameras_set = cameras_sets; cameras_set != NULL; cameras_set = cameras_set->next) {
			if (cameras_set->page_num == index) break;
		}
	} else cameras_set = new_cameras_set;

	cameras_set_configuration_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (cameras_set_configuration_window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (cameras_set_configuration_window), "Configuration");
	gtk_window_set_default_size (GTK_WINDOW (cameras_set_configuration_window), 50, 50);
	gtk_window_set_modal (GTK_WINDOW (cameras_set_configuration_window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (cameras_set_configuration_window), GTK_WINDOW (main_window));
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (cameras_set_configuration_window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (cameras_set_configuration_window), FALSE);
	gtk_window_set_position (GTK_WINDOW (cameras_set_configuration_window), GTK_WIN_POS_CENTER_ON_PARENT);
	g_signal_connect (G_OBJECT (cameras_set_configuration_window), "delete-event", G_CALLBACK (cameras_set_configuration_window_cancel), NULL);
	g_signal_connect (G_OBJECT (cameras_set_configuration_window), "key-press-event", G_CALLBACK (cameras_set_configuration_window_key_press), NULL);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box1), WINDOW_MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Nom :");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			gtk_container_add (GTK_CONTAINER (box2), widget);

			cameras_set_configuration_name_entry_buffer = gtk_entry_buffer_new (cameras_set->name, strlen (cameras_set->name));
			widget = gtk_entry_new_with_buffer (cameras_set_configuration_name_entry_buffer);
			gtk_entry_set_max_length (GTK_ENTRY (widget), CAMERAS_SET_NAME_LENGTH);
			gtk_entry_set_width_chars (GTK_ENTRY (widget), CAMERAS_SET_NAME_LENGTH);
			gtk_container_add (GTK_CONTAINER (box2), widget);
		gtk_container_add (GTK_CONTAINER (box1), box2);

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Nombre de caméras :");
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
			gtk_container_add (GTK_CONTAINER (box2), widget);

			widget = gtk_combo_box_text_new ();
			for (i = 0; i < MAX_CAMERAS; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), nb[i]);
			}
			gtk_combo_box_set_active (GTK_COMBO_BOX (widget), cameras_set->number_of_cameras - 1);
			g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (number_of_cameras_has_changed), cameras_set);
			gtk_container_add (GTK_CONTAINER (box2), widget);
		gtk_container_add (GTK_CONTAINER (box1), box2);

		frame2 = gtk_frame_new (cameras_label);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		gtk_widget_set_margin_bottom (frame2, WINDOW_MARGIN_VALUE);

		cameras_set_configuration_window_grid = gtk_grid_new ();
		gtk_widget_set_margin_start (cameras_set_configuration_window_grid, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (cameras_set_configuration_window_grid, WINDOW_MARGIN_VALUE);

		widget = gtk_label_new ("Nom :");
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), widget, 1, 0, 1, 1);

		widget = gtk_label_new ("adresse IP :");
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), widget, 2, 0, 7, 1);

		cameras_configuration_widgets = g_malloc (MAX_CAMERAS * sizeof (camera_configuration_widgets_t));
		new_number_of_cameras = cameras_set->number_of_cameras;
		old_number_of_cameras = cameras_set->number_of_cameras;

		for (i = 0, j = 1; i < cameras_set->number_of_cameras; i++, j++) {
			rcp = cameras_set->rcp_ptr_array[i];

			widget = gtk_switch_new ();
			gtk_switch_set_active (GTK_SWITCH (widget), rcp->active);
			g_signal_connect (widget, "notify::active", G_CALLBACK (camera_switch_activated), &cameras_configuration_widgets[i]);
			gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), widget, 0, j, 1, 1);
			cameras_configuration_widgets[i].camera_switch = widget;

			cameras_configuration_widgets[i].name_entry_buffer = gtk_entry_buffer_new (rcp->name, strlen (rcp->name));
			cameras_configuration_widgets[i].name_entry = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].name_entry_buffer);
			gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 2);
			gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 4);
			gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 0.5);
			gtk_widget_set_margin_start (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_end (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].name_entry, 1, j, 1, 1);

			if (rcp->ip_address_is_valid) {
				l = 1;
				while (rcp->ip_address[l] != '.') l++;
				cameras_configuration_widgets[i].ip_entry_buffer[0] = gtk_entry_buffer_new (rcp->ip_address, l);
				k = l + 1;
			} else cameras_configuration_widgets[i].ip_entry_buffer[0] = gtk_entry_buffer_new (NULL, -1);

			cameras_configuration_widgets[i].ip_entry[0] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[0]);
			g_signal_connect (G_OBJECT (cameras_configuration_widgets[i].ip_entry[0]), "key-press-event", G_CALLBACK (digit_key_press), NULL);
			gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 3);
			gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 3);
			gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 0.5);
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[0], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[0], 2, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].ip_entry[0], FALSE);

			cameras_configuration_widgets[i].dot[0] = gtk_label_new (".");
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[0], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[0], 3, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].dot[0], FALSE);

			if (rcp->ip_address_is_valid) {
				l = 1;
				while (rcp->ip_address[k + l] != '.') l++;
				cameras_configuration_widgets[i].ip_entry_buffer[1] = gtk_entry_buffer_new (rcp->ip_address + k, l);
				k += l + 1;
			} else cameras_configuration_widgets[i].ip_entry_buffer[1] = gtk_entry_buffer_new (NULL, -1);

			cameras_configuration_widgets[i].ip_entry[1] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[1]);
			g_signal_connect (G_OBJECT (cameras_configuration_widgets[i].ip_entry[1]), "key-press-event", G_CALLBACK (digit_key_press), NULL);
			gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 3);
			gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 3);
			gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 0.5);
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[1], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[1], 4, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].ip_entry[1], FALSE);

			cameras_configuration_widgets[i].dot[1] = gtk_label_new (".");
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[1], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[1], 5, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].dot[1], FALSE);

			if (rcp->ip_address_is_valid) {
				l = 1;
				while (rcp->ip_address[k + l] != '.') l++;
				cameras_configuration_widgets[i].ip_entry_buffer[2] = gtk_entry_buffer_new (rcp->ip_address + k, l);
				k += l + 1;
			} else cameras_configuration_widgets[i].ip_entry_buffer[2] = gtk_entry_buffer_new (NULL, -1);

			cameras_configuration_widgets[i].ip_entry[2] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[2]);
			g_signal_connect (G_OBJECT (cameras_configuration_widgets[i].ip_entry[2]), "key-press-event", G_CALLBACK (digit_key_press), NULL);
			gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 3);
			gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 3);
			gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 0.5);
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[2], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[2], 6, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].ip_entry[2], FALSE);

			cameras_configuration_widgets[i].dot[2] = gtk_label_new (".");
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[2], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[2], 7, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].dot[2], FALSE);

			if (rcp->ip_address_is_valid) {
				l = 1;
				while (rcp->ip_address[k + l] != '\0') l++;
				cameras_configuration_widgets[i].ip_entry_buffer[3] = gtk_entry_buffer_new (rcp->ip_address + k, l);
			} else cameras_configuration_widgets[i].ip_entry_buffer[3] = gtk_entry_buffer_new (NULL, -1);

			cameras_configuration_widgets[i].ip_entry[3] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[3]);
			g_signal_connect (G_OBJECT (cameras_configuration_widgets[i].ip_entry[3]), "key-press-event", G_CALLBACK (digit_key_press), NULL);
			gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 3);
			gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 3);
			gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 0.5);
			gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[3], WINDOW_MARGIN_VALUE);
			gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[3], 8, j, 1, 1);
			if (!rcp->active) gtk_widget_set_sensitive (cameras_configuration_widgets[i].ip_entry[3], FALSE);
		}
		gtk_container_add (GTK_CONTAINER (frame2), cameras_set_configuration_window_grid);
		gtk_container_add (GTK_CONTAINER (box1), frame2);

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
			button = gtk_button_new_with_label ("OK");
			gtk_widget_set_margin_end (button, WINDOW_MARGIN_VALUE);
			g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cameras_set_configuration_window_ok), cameras_set);
			gtk_box_pack_start (GTK_BOX (box2), button, FALSE, FALSE, 0);

			button = gtk_button_new_with_label ("Annuler");
			g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cameras_set_configuration_window_cancel), NULL);
			gtk_box_pack_start (GTK_BOX (box2), button, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (box1), box2);
	gtk_container_add (GTK_CONTAINER (cameras_set_configuration_window), box1);

	gtk_window_set_resizable (GTK_WINDOW (cameras_set_configuration_window), FALSE);
	gtk_widget_show_all (cameras_set_configuration_window);

	while (i < MAX_CAMERAS) {
		cameras_configuration_widgets[i].camera_switch = gtk_switch_new ();
		gtk_switch_set_active (GTK_SWITCH (cameras_configuration_widgets[i].camera_switch), TRUE);
		g_signal_connect (cameras_configuration_widgets[i].camera_switch, "notify::active", G_CALLBACK (camera_switch_activated), &cameras_configuration_widgets[i]);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].camera_switch, WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].camera_switch, 0, j, 1, 1);

		sprintf (camera_name, "%d", j);
		cameras_configuration_widgets[i].name_entry_buffer = gtk_entry_buffer_new (camera_name, strlen (camera_name));
		cameras_configuration_widgets[i].name_entry = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].name_entry_buffer);
		gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 2);
		gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 4);
		gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].name_entry), 0.5);
		gtk_widget_set_margin_start (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].name_entry, WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].name_entry, 1, j, 1, 1);

		cameras_configuration_widgets[i].ip_entry_buffer[0] = gtk_entry_buffer_new (NULL, -1);
		cameras_configuration_widgets[i].ip_entry[0] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[0]);
		gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 3);
		gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 3);
		gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[0]), 0.5);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[0], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[0], 2, j, 1, 1);

		cameras_configuration_widgets[i].dot[0] = gtk_label_new (".");
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[0], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[0], 3, j, 1, 1);

		cameras_configuration_widgets[i].ip_entry_buffer[1] = gtk_entry_buffer_new (NULL, -1);
		cameras_configuration_widgets[i].ip_entry[1] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[1]);
		gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 3);
		gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 3);
		gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[1]), 0.5);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[1], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[1], 4, j, 1, 1);

		cameras_configuration_widgets[i].dot[1] = gtk_label_new (".");
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[1], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[1], 5, j, 1, 1);

		cameras_configuration_widgets[i].ip_entry_buffer[2] = gtk_entry_buffer_new (NULL, -1);
		cameras_configuration_widgets[i].ip_entry[2] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[2]);
		gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 3);
		gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 3);
		gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[2]), 0.5);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[2], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[2], 6, j, 1, 1);

		cameras_configuration_widgets[i].dot[2] = gtk_label_new (".");
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].dot[2], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].dot[2], 7, j, 1, 1);

		cameras_configuration_widgets[i].ip_entry_buffer[3] = gtk_entry_buffer_new (NULL, -1);
		cameras_configuration_widgets[i].ip_entry[3] = gtk_entry_new_with_buffer (cameras_configuration_widgets[i].ip_entry_buffer[3]);
		gtk_entry_set_max_length (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 3);
		gtk_entry_set_width_chars (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 3);
		gtk_entry_set_alignment (GTK_ENTRY (cameras_configuration_widgets[i].ip_entry[3]), 0.5);
		gtk_widget_set_margin_bottom (cameras_configuration_widgets[i].ip_entry[3], WINDOW_MARGIN_VALUE);
		gtk_grid_attach (GTK_GRID (cameras_set_configuration_window_grid), cameras_configuration_widgets[i].ip_entry[3], 8, j, 1, 1);

		i++;
		j++;
	}
}

void add_cameras_set (void)
{
	int i;
	rcp_t *rcp;

	new_cameras_set = g_malloc (sizeof (cameras_set_t));
	new_cameras_set->name[0] = '\0';
	new_cameras_set->number_of_cameras = 5;
	new_cameras_set->rcp_ptr_array = g_malloc (5 * sizeof (rcp_t*));

	for (i = 0; i < 5; i++) {
		rcp = g_malloc (sizeof (rcp_t));
		new_cameras_set->rcp_ptr_array[i] = rcp;
		rcp->cameras_set = new_cameras_set;
		sprintf (rcp->name, "%d", i + 1);
		init_rcp (rcp);
		rcp->index = i;
	}

	new_cameras_set->rcp_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	show_cameras_set_configuration_window ();
}

void delete_cameras_set (GtkButton *button, GtkWidget *confirmation_window)
{
	GtkListBoxRow *list_box_row;
	GList *glist_itr, *glist_itr_next;
	const gchar *name;
	cameras_set_t *cameras_set_tmp, *cameras_set_prev;
	int i;
	rcp_t *rcp, *other_rcp;

	list_box_row = gtk_list_box_get_selected_row (GTK_LIST_BOX (settings_list_box));

	if (list_box_row != NULL) {
		name = gtk_label_get_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (list_box_row))));

		if (strcmp (cameras_sets->name, name) == 0) {
			g_mutex_lock (&cameras_sets_mutex);
			cameras_set_tmp = cameras_sets;
			cameras_sets = cameras_sets->next;
			g_mutex_unlock (&cameras_sets_mutex);
		} else {
			g_mutex_lock (&cameras_sets_mutex);
			cameras_set_prev = cameras_sets;
			cameras_set_tmp = cameras_sets->next;

			while (cameras_set_tmp != NULL) {
				if (strcmp (cameras_set_tmp->name, name) == 0) {
					cameras_set_prev->next = cameras_set_tmp->next;
					break;
				}
				cameras_set_prev = cameras_set_tmp;
				cameras_set_tmp = cameras_set_tmp->next;
			}
			g_mutex_unlock (&cameras_sets_mutex);
		}

		for (i = 0; i < cameras_set_tmp->number_of_cameras; i++) {
			rcp = cameras_set_tmp->rcp_ptr_array[i];

			g_mutex_lock (&sw_p_08_mutex);
			if (rcp == rcp_vision) rcp_vision = NULL;
			if (rcp == rcp_pgm) rcp_pgm = NULL;
			if (rcp == rcp_pvw) rcp_pvw = NULL;
			g_mutex_unlock (&sw_p_08_mutex);

			if (rcp->ip_address_is_valid && (rcp->other_rcp == NULL) && (rcp->error_code != CAMERA_IS_UNREACHABLE_ERROR)) {
				send_ptz_control_command (rcp, "#LPC0");
				send_update_stop_cmd (rcp);
			}

			glist_itr = rcp_start_glist;
			while (glist_itr != NULL) {
				glist_itr_next = glist_itr->next;

				if (glist_itr->data == rcp) {
					if (((rcp_t*)(glist_itr->data))->other_rcp != NULL) glist_itr->data = ((rcp_t*)(glist_itr->data))->other_rcp;
					else rcp_start_glist = g_list_delete_link (rcp_start_glist, glist_itr);
					break;
				}

				glist_itr = glist_itr_next;
			}

			rcp_glist = g_list_remove (rcp_glist, rcp);

			for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
				other_rcp = (rcp_t*)(glist_itr->data);

				g_mutex_lock (&other_rcp->other_rcp_mutex);
				other_rcp->other_rcp = g_slist_remove (other_rcp->other_rcp, rcp);
				g_mutex_unlock (&other_rcp->other_rcp_mutex);
			}

			rcp->root_widget = NULL;
			if (rcp->active) {
				gtk_widget_destroy (rcp->shutter_synchro_window);
				gtk_widget_destroy (rcp->detail_window);
				gtk_widget_destroy (rcp->matrix_window);
				gtk_widget_destroy (rcp->knee_window);
			}

			g_mutex_lock (&rcp->other_rcp_mutex);
			g_slist_free (rcp->other_rcp);
			rcp->other_rcp = NULL;
			g_mutex_unlock (&rcp->other_rcp_mutex);
			g_mutex_clear (&rcp->other_rcp_mutex);

			if (!rcp->camera_is_working) {
				if (rcp->thread != NULL) g_thread_join (rcp->thread);
				g_mutex_clear (&rcp->cmd_mutex);
				g_free (rcp);
			}
		}
		g_free (cameras_set_tmp->rcp_ptr_array);

		gtk_notebook_remove_page (GTK_NOTEBOOK (main_window_notebook), cameras_set_tmp->page_num);

		gtk_widget_destroy (cameras_set_tmp->source_list_box_row);
		gtk_widget_destroy (cameras_set_tmp->source_rcp_list_box);

		gtk_widget_destroy (cameras_set_tmp->destination_list_box_row);
		gtk_widget_destroy (cameras_set_tmp->destination_rcp_list_box);

		gtk_list_box_select_row (GTK_LIST_BOX (settings_list_box), NULL);
		gtk_widget_destroy (GTK_WIDGET (list_box_row));

		g_free (cameras_set_tmp);

		if (number_of_cameras_sets == MAX_CAMERAS_SET) gtk_widget_set_sensitive (settings_new_button, TRUE);
		number_of_cameras_sets--;

		for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next)
			cameras_set_tmp->page_num = gtk_notebook_page_num (GTK_NOTEBOOK (main_window_notebook), cameras_set_tmp->page);
	}

	if (number_of_cameras_sets == 0) gtk_widget_hide (scenes_page);

	gtk_widget_destroy (confirmation_window);

	backup_needed = TRUE;
}

