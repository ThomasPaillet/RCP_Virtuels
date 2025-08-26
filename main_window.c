/*
 * copyright (c) 2018-2022 2025 Thomas Paillet <thomas.paillet@net-c.fr>

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
#include "main_window.h"

#include "logging.h"
#include "protocol.h"
#include "misc.h"
#include "error.h"
#include "update_notification.h"

#include "iris.h"

#include "cameras_set.h"
#include "scenes.h"
#include "settings.h"
#include "sw_p_08.h"
#include "tally.h"
#include "physical_rcp.h"

#ifdef _WIN32
extern GdkPixbuf *pixbuf_icon;

void load_pixbufs (void);
#elif defined (__linux)
#include "Linux/gresources.h"
#endif


GdkScreen *screen;

GtkCssProvider *light_css_provider;
GtkCssProvider *dark_css_provider;

GtkCssProvider *css_provider_gain_red, *css_provider_gain_green, *css_provider_gain_blue;
GtkCssProvider *css_provider_pedestal_red, *css_provider_pedestal_green, *css_provider_pedestal_blue;
GtkCssProvider *css_provider_white, *css_provider_black, *css_provider_raz, *css_provider_store, *css_provider_scene_selected;

GtkWidget *main_window;
GtkWidget *main_window_notebook;
GtkWidget *popup_window = NULL;

gboolean fullscreen = TRUE;
gboolean simple_ihm = FALSE;

const char *application_name_txt = "RCP virtuels pour caméras Panasonic AW-HE130 et AW-UE150";
const char *key_info_1_txt = "PGM -> CTRL VISION (Echap) | CAM 1 ... x -> CTRL VISION (F1 ... Fx) | Mémoires 6 à 10 (Shift) | Plein écran (F) | Affichage simplifié (S, Espace) | Instantané(s) (I) | Quitter (Q, ALT + F4)";
const char *key_info_2_txt = "Plein écran (F) | Quitter (Q, ALT + F4)";
const char *warning_txt = "Attention !";


gboolean digit_key_press (GtkEntry *entry, GdkEventKey *event)
{
	if ((event->keyval >= GDK_KEY_0) && (event->keyval <= GDK_KEY_9)) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval >= GDK_KEY_KP_0) && (event->keyval <= GDK_KEY_KP_9)) return GDK_EVENT_PROPAGATE;
	else if (event->state & GDK_CONTROL_MASK) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval == GDK_KEY_BackSpace) || (event->keyval == GDK_KEY_Tab) || (event->keyval == GDK_KEY_Clear) || (event->keyval == GDK_KEY_Return) || \
		(event->keyval == GDK_KEY_Escape) || (event->keyval == GDK_KEY_Delete) || (event->keyval == GDK_KEY_Cancel)) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval >= GDK_KEY_Home) && (event->keyval <= GDK_KEY_Select)) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval >= GDK_KEY_Execute) && (event->keyval <= GDK_KEY_Redo)) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval == GDK_KEY_KP_Tab) || (event->keyval == GDK_KEY_KP_Enter)) return GDK_EVENT_PROPAGATE;
	else if ((event->keyval >= GDK_KEY_KP_Home) && (event->keyval <= GDK_KEY_KP_Delete)) return GDK_EVENT_PROPAGATE;
	else return GDK_EVENT_STOP;
}

gboolean hide_popup_window (GtkWidget *window)
{
	gtk_widget_hide (window);
	popup_window = NULL;

	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp_vision->cameras_set))->master_rcp.root_widget), FALSE);

	return GDK_EVENT_STOP;
}

gboolean popup_window_key_press (GtkWidget *window, GdkEventKey *event)
{
	if (event->keyval == GDK_KEY_Escape) hide_popup_window (window);

	return GDK_EVENT_STOP;
}

void show_rs_connection_error_window (void)
{
	GtkWidget *window, *box, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), warning_txt);
	gtk_window_set_modal (GTK_WINDOW (window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), WINDOW_MARGIN_VALUE);
		widget = gtk_label_new ("Erreur de connection série avec la régie !");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("OK");
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_halign (widget, GTK_ALIGN_CENTER);
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (gtk_widget_destroy), window);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box);

	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_show_all (window);

	stop_rs_communication ();
}

void main_window_notebook_switch_page (GtkNotebook *notebook, GtkWidget *page, guint page_num)
{
	int i;
	rcp_t *rcp;
	GList *list, *glist_itr;

	if (popup_window != NULL) {
		gtk_widget_hide (popup_window);
		popup_window = NULL;

		if (rcp_vision != NULL) {
			gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
			gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp_vision->cameras_set))->master_rcp.root_widget), FALSE);
		}
	}

	if (backup_needed) save_settings_and_cameras_sets_to_config_file ();

	g_mutex_lock (&sw_p_08_mutex);
	rcp_pgm = NULL;
	rcp_pvw = NULL;
	g_mutex_unlock (&sw_p_08_mutex);

	if ((send_ip_tally) && (current_cameras_set != NULL)) {
		for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
			rcp = current_cameras_set->cameras[i];

			if (rcp->camera_is_on && rcp->ip_tally_is_on) send_tally_off_control_command (rcp);
		}
	}

	g_mutex_lock (&current_cameras_set_mutex);

	for (current_cameras_set = cameras_sets; current_cameras_set != NULL; current_cameras_set = current_cameras_set->next) {
		if (current_cameras_set->page == page) break;
	}

	g_mutex_unlock (&current_cameras_set_mutex);

	g_mutex_lock (&sw_p_08_mutex);

	if (current_cameras_set == NULL) {
		tell_camera_set_is_selected (MAX_CAMERAS + 1);
		g_mutex_unlock (&sw_p_08_mutex);

		return;
	}

	if ((int)tally_pgm < current_cameras_set->number_of_cameras) rcp_pgm = current_cameras_set->cameras[(int)tally_pgm];
	if ((int)tally_pvw < current_cameras_set->number_of_cameras) rcp_pvw = current_cameras_set->cameras[(int)tally_pvw];

	tell_camera_set_is_selected (page_num);

	g_mutex_unlock (&sw_p_08_mutex);

	if (send_ip_tally) {
		for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
			rcp = current_cameras_set->cameras[i];

			if ((rcp->camera_is_on) && ((rcp->tally_data & 0x30) || (rcp == rcp_pgm))) send_tally_on_control_command (rcp);
		}
	}

	list = gtk_container_get_children (GTK_CONTAINER (settings_list_box));

	for (glist_itr = list; page_num != 0; page_num--) glist_itr = glist_itr->next;

	gtk_list_box_select_row (GTK_LIST_BOX (settings_list_box), GTK_LIST_BOX_ROW (glist_itr->data));

	g_list_free (list);
}

void main_window_notebook_page_reordered (GtkNotebook *notebook)
{
	GList *list_tmp, *list_box_row_list;
	cameras_set_t *cameras_set_itr;
	int i;
	gint current_page;

	if (popup_window != NULL) {
		gtk_widget_hide (popup_window);
		popup_window = NULL;

		if (rcp_vision != NULL) {
			gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
			gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp_vision->cameras_set))->master_rcp.root_widget), FALSE);
		}
	}

	list_box_row_list = gtk_container_get_children (GTK_CONTAINER (settings_list_box));

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		cameras_set_itr->page_num = gtk_notebook_page_num (GTK_NOTEBOOK (main_window_notebook), cameras_set_itr->page);

		i = cameras_set_itr->page_num;
		list_tmp = list_box_row_list;
		while (i != 0) { i--; list_tmp = list_tmp->next; }
		gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN (list_tmp->data))), cameras_set_itr->name);
	}

	g_list_free (list_box_row_list);

	gtk_list_box_invalidate_sort (GTK_LIST_BOX (source_cameras_set_list_box));
	gtk_list_box_invalidate_sort (GTK_LIST_BOX (destination_cameras_set_list_box));

	current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK (main_window_notebook));
	main_window_notebook_switch_page (NULL, gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_window_notebook), current_page), current_page);

	backup_needed = TRUE;
}

void rcp_main_quit (GtkWidget *confirmation_window)
{
	gtk_widget_destroy (confirmation_window);

	gtk_widget_hide (main_window);

	gtk_main_quit ();
}

gboolean quit_confirmation_window_key_press (GtkWidget *confirmation_window, GdkEventKey *event)
{
	if ((event->keyval == GDK_KEY_n) || (event->keyval == GDK_KEY_N) || (event->keyval == GDK_KEY_Escape)) gtk_widget_destroy (confirmation_window);
	else if ((event->keyval == GDK_KEY_o) || (event->keyval == GDK_KEY_O)) rcp_main_quit (confirmation_window);

	return GDK_EVENT_PROPAGATE;
}

gboolean show_quit_confirmation_window (void)
{
	GtkWidget *confirmation_window, *box1, *box2, *widget;

	confirmation_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (confirmation_window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (confirmation_window), warning_txt);
	gtk_window_set_modal (GTK_WINDOW (confirmation_window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (confirmation_window), GTK_WINDOW (main_window));
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (confirmation_window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (confirmation_window), FALSE);
	gtk_window_set_position (GTK_WINDOW (confirmation_window), GTK_WIN_POS_CENTER_ON_PARENT);
	g_signal_connect (G_OBJECT (confirmation_window), "key-press-event", G_CALLBACK (quit_confirmation_window_key_press), NULL);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box1), WINDOW_MARGIN_VALUE);
		widget = gtk_label_new ("Etes-vous sûr de vouloir quitter l'application ?");
		gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_top (box2, WINDOW_MARGIN_VALUE);
		gtk_box_set_spacing (GTK_BOX (box2), WINDOW_MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box2), TRUE);
			widget = gtk_button_new_with_label ("OUI");
			g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (rcp_main_quit), confirmation_window);
			gtk_box_pack_start (GTK_BOX (box2), widget, TRUE, TRUE, 0);

			widget = gtk_button_new_with_label ("NON");
			g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (gtk_widget_destroy), confirmation_window);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (confirmation_window), box1);

	gtk_window_set_resizable (GTK_WINDOW (confirmation_window), FALSE);
	gtk_widget_show_all (confirmation_window);

	return GDK_EVENT_STOP;
}

gboolean main_window_key_press (GtkWidget *widget, GdkEventKey *event)
{
	int rcp_num, i;
	GList *glist_itr;
	rcp_t *rcp;
	cameras_set_t *cameras_set_itr;

	static GdkEventButton event_button = { GDK_BUTTON_PRESS, NULL, TRUE, 0, 0.0, 0.0, NULL, GDK_BUTTON1_MASK, GDK_BUTTON_PRIMARY, NULL, 0.0, 0.0 };

	if ((event->keyval == GDK_KEY_q) || (event->keyval == GDK_KEY_Q)) show_quit_confirmation_window ();
	else if ((event->keyval == GDK_KEY_f) || (event->keyval == GDK_KEY_F)) {
		if (fullscreen) {
			gtk_window_unfullscreen (GTK_WINDOW (main_window));
			fullscreen = FALSE;
		} else {
			gtk_window_fullscreen (GTK_WINDOW (main_window));
			fullscreen = TRUE;
		}
	} else if (current_cameras_set != NULL) {
		if (event->keyval == GDK_KEY_Escape) ask_to_connect_pgm_to_ctrl_vision ();
		else if ((GDK_KEY_F1 <= event->keyval) && (event->keyval <= GDK_KEY_F15)) {
			rcp_num = event->keyval - GDK_KEY_F1;
			if (rcp_num >= current_cameras_set->number_of_cameras) rcp_num = current_cameras_set->number_of_cameras - 1;
			rcp_button_press_event (NULL, NULL, current_cameras_set->cameras[rcp_num]);
		} else if ((event->keyval == GDK_KEY_Shift_L) || (event->keyval == GDK_KEY_Shift_R)) {
			for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
				if (current_cameras_set->cameras[i]->active) {
					gtk_widget_hide (current_cameras_set->cameras[i]->scenes_bank_1_box);
					gtk_widget_show (current_cameras_set->cameras[i]->scenes_bank_2_box);
				}
			}
			gtk_widget_hide (current_cameras_set->master_rcp.scenes_bank_1_box);
			gtk_widget_show (current_cameras_set->master_rcp.scenes_bank_2_box);
		} else if ((event->keyval == GDK_KEY_s) || (event->keyval == GDK_KEY_S) || (event->keyval == GDK_KEY_space)) {
			if (simple_ihm) {
				for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					rcp = (rcp_t*)(glist_itr->data);

					if (rcp->active) {
						gtk_widget_show (rcp->standard_button);
						gtk_widget_show (rcp->mire_toggle_button);
						gtk_widget_show (rcp->day_night_toggle_button);
						gtk_widget_show (rcp->ABB_filter_gain_box);
						gtk_widget_show (rcp->gamma_frame);
						if (rcp->model == AW_UE150) gtk_widget_show (rcp->chroma_phase_frame);
						gtk_widget_show (rcp->color_temperature_frame);
						gtk_widget_show (rcp->knee_matrix_detail_box);
						gtk_widget_show (rcp->shutter_frame);
					}
				}
				for (glist_itr = ghost_rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					rcp = (rcp_t*)(glist_itr->data);

					if (!rcp->active) gtk_widget_show (rcp->ghost_rcp_body);
				}
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					gtk_widget_show (cameras_set_itr->master_rcp.standard_button);
					gtk_widget_show (cameras_set_itr->master_rcp.mire_button);
					gtk_widget_show (cameras_set_itr->master_rcp.day_night_button);
					gtk_widget_show (cameras_set_itr->master_rcp.ABB_filter_gain_box);
					gtk_widget_show (cameras_set_itr->master_rcp.gamma_frame);
					gtk_widget_show (cameras_set_itr->master_rcp.color_temperature_frame);
					gtk_widget_show (cameras_set_itr->master_rcp.knee_matrix_detail_box);
					gtk_widget_show (cameras_set_itr->master_rcp.padding);
					gtk_widget_show (cameras_set_itr->master_rcp.shutter_frame);
				}
				gtk_widget_show (scenes_page);
				gtk_widget_show (settings_page);
				simple_ihm = FALSE;
			} else {
				gtk_widget_hide (settings_page);
				gtk_widget_hide (scenes_page);
				for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
					gtk_widget_hide (cameras_set_itr->master_rcp.standard_button);
					gtk_widget_hide (cameras_set_itr->master_rcp.mire_button);
					gtk_widget_hide (cameras_set_itr->master_rcp.day_night_button);
					gtk_widget_hide (cameras_set_itr->master_rcp.ABB_filter_gain_box);
					gtk_widget_hide (cameras_set_itr->master_rcp.gamma_frame);
					gtk_widget_hide (cameras_set_itr->master_rcp.color_temperature_frame);
					gtk_widget_hide (cameras_set_itr->master_rcp.knee_matrix_detail_box);
					gtk_widget_hide (cameras_set_itr->master_rcp.padding);
					gtk_widget_hide (cameras_set_itr->master_rcp.shutter_frame);
				}
				for (glist_itr = ghost_rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					rcp = (rcp_t*)(glist_itr->data);

					if (!rcp->active) gtk_widget_hide (rcp->ghost_rcp_body);
				}
				for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
					rcp = (rcp_t*)(glist_itr->data);

					if (rcp->active) {
						gtk_widget_hide (rcp->standard_button);
						gtk_widget_hide (rcp->mire_toggle_button);
						gtk_widget_hide (rcp->day_night_toggle_button);
						gtk_widget_hide (rcp->ABB_filter_gain_box);
						gtk_widget_hide (rcp->gamma_frame);
						if (rcp->model == AW_UE150) gtk_widget_hide (rcp->chroma_phase_frame);
						gtk_widget_hide (rcp->color_temperature_frame);
						gtk_widget_hide (rcp->knee_matrix_detail_box);
						gtk_widget_hide (rcp->shutter_frame);
					}
				}
				simple_ihm = TRUE;
			}
		} else if ((event->keyval == GDK_KEY_i) || (event->keyval == GDK_KEY_I)) {
			if (rcp_vision != NULL) {
				if ((rcp_vision->active) && (rcp_vision->camera_is_on) && (!rcp_vision->camera_is_working)) {
					if (rcp_vision->thread != NULL) g_thread_join (rcp_vision->thread);
					rcp_vision->thread = g_thread_new (NULL, (GThreadFunc)send_jpeg_image_request_cmd, rcp_vision);
				}
			} else {
				for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
					rcp = current_cameras_set->cameras[i];

					if ((rcp->active) && (rcp->camera_is_on) && (!rcp->camera_is_working)) {
						if (rcp->thread != NULL) g_thread_join (rcp->thread);
						rcp->thread = g_thread_new (NULL, (GThreadFunc)send_jpeg_image_request_cmd, rcp);
					}
				}
			}
		} else if ((event->keyval == GDK_KEY_Up) || (event->keyval == GDK_KEY_KP_Up)) {
			if (rcp_vision != NULL) {
				if ((rcp_vision->camera_is_on) && (!rcp_vision->camera_is_working) && (rcp_vision->current_scene.iris_auto == FALSE)) {
					gtk_widget_set_state_flags (rcp_vision->iris_plus_10_button, GTK_STATE_FLAG_ACTIVE, FALSE);
					if (rcp_vision->timeout_id == 0) iris_plus_10_button_pressed (GTK_BUTTON (rcp_vision->iris_plus_10_button), &event_button, rcp_vision);
				}
			}
		} else if ((event->keyval == GDK_KEY_Down) || (event->keyval == GDK_KEY_KP_Down)) {
			if (rcp_vision != NULL) {
				if ((rcp_vision->camera_is_on) && (!rcp_vision->camera_is_working) && (rcp_vision->current_scene.iris_auto == FALSE)) {
					gtk_widget_set_state_flags (rcp_vision->iris_minus_10_button, GTK_STATE_FLAG_ACTIVE, FALSE);
					if (rcp_vision->timeout_id == 0) iris_minus_10_button_pressed (GTK_BUTTON (rcp_vision->iris_minus_10_button), &event_button, rcp_vision);
				}
			}
		} else if ((event->keyval == GDK_KEY_Right) || (event->keyval == GDK_KEY_KP_Right)) {
			if (rcp_vision != NULL) {
				if ((rcp_vision->camera_is_on) && (!rcp_vision->camera_is_working) && (rcp_vision->current_scene.iris_auto == FALSE)) {
					gtk_widget_set_state_flags (rcp_vision->iris_plus_1_button, GTK_STATE_FLAG_ACTIVE, FALSE);
					if (rcp_vision->timeout_id == 0) iris_plus_1_button_pressed (GTK_BUTTON (rcp_vision->iris_plus_1_button), &event_button, rcp_vision);
				}
			}
		} else if ((event->keyval == GDK_KEY_Left) || (event->keyval == GDK_KEY_KP_Left)) {
			if (rcp_vision != NULL) {
				if ((rcp_vision->camera_is_on) && (!rcp_vision->camera_is_working) && (rcp_vision->current_scene.iris_auto == FALSE)) {
					gtk_widget_set_state_flags (rcp_vision->iris_minus_1_button, GTK_STATE_FLAG_ACTIVE, FALSE);
					if (rcp_vision->timeout_id == 0) iris_minus_1_button_pressed (GTK_BUTTON (rcp_vision->iris_minus_1_button), &event_button, rcp_vision);
				}
			}
		} else return GDK_EVENT_PROPAGATE;
	} else return GDK_EVENT_PROPAGATE;

	return GDK_EVENT_STOP;
}

gboolean main_window_key_release (GtkWidget *widget, GdkEventKey *event)
{
	int i;

	if (current_cameras_set != NULL) {
		if ((event->keyval == GDK_KEY_Shift_L) || (event->keyval == GDK_KEY_Shift_R)) {
			for (i = 0; i < current_cameras_set->number_of_cameras; i++) {
				if (current_cameras_set->cameras[i]->active) {
					gtk_widget_hide (current_cameras_set->cameras[i]->scenes_bank_2_box);
					gtk_widget_show (current_cameras_set->cameras[i]->scenes_bank_1_box);
				}
			}
			gtk_widget_hide (current_cameras_set->master_rcp.scenes_bank_2_box);
			gtk_widget_show (current_cameras_set->master_rcp.scenes_bank_1_box);
		} else if (rcp_vision != NULL) {
			if ((event->keyval == GDK_KEY_Up) || (event->keyval == GDK_KEY_KP_Up)) remove_timeout (rcp_vision->iris_plus_10_button, NULL, rcp_vision);
			else if ((event->keyval == GDK_KEY_Down) || (event->keyval == GDK_KEY_KP_Down)) remove_timeout (rcp_vision->iris_minus_10_button, NULL, rcp_vision);
			else if ((event->keyval == GDK_KEY_Right) || (event->keyval == GDK_KEY_KP_Right)) remove_timeout (rcp_vision->iris_plus_1_button, NULL, rcp_vision);
			else if ((event->keyval == GDK_KEY_Left) || (event->keyval == GDK_KEY_KP_Left)) remove_timeout (rcp_vision->iris_minus_1_button, NULL, rcp_vision);
		} else return GDK_EVENT_PROPAGATE;
	} else return GDK_EVENT_PROPAGATE;

	return GDK_EVENT_STOP;
}

void create_main_window (void)
{
	main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (main_window), application_name_txt);
	gtk_window_set_default_size (GTK_WINDOW (main_window), 1200, 1000);
	gtk_window_fullscreen (GTK_WINDOW (main_window));
	g_signal_connect (G_OBJECT (main_window), "key-press-event", G_CALLBACK (main_window_key_press), NULL);
	g_signal_connect (G_OBJECT (main_window), "key-release-event", G_CALLBACK (main_window_key_release), NULL);
	g_signal_connect (G_OBJECT (main_window), "delete-event", G_CALLBACK (show_quit_confirmation_window), NULL);

#ifdef _WIN32
	gtk_window_set_icon (GTK_WINDOW (main_window), pixbuf_icon);
#elif defined (__linux)
	GtkWidget *widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/icon.png");
	gtk_window_set_icon (GTK_WINDOW (main_window), gtk_image_get_pixbuf (GTK_IMAGE (widget)));
#endif

	main_window_notebook = gtk_notebook_new ();
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (main_window_notebook), TRUE);
	g_signal_connect (G_OBJECT (main_window_notebook), "switch-page", G_CALLBACK (main_window_notebook_switch_page), NULL);
	g_signal_connect (G_OBJECT (main_window_notebook), "page-reordered", G_CALLBACK (main_window_notebook_page_reordered), NULL);

	gtk_container_add (GTK_CONTAINER (main_window), main_window_notebook);
}

#ifdef _WIN32
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
#elif defined (__linux)
int main (int argc, char** argv)
#endif
{
	GFile *file;
	cameras_set_t *cameras_set_itr;
	int i;
	rcp_t *rcp;
	GList *glist_itr;

	WSAInit ();	//_WIN32

#ifdef _WIN32
	gtk_init (NULL, NULL);

	load_pixbufs ();

	FreeConsole ();
#elif defined (__linux)
	gtk_init (&argc, &argv);

	g_resources_register (gresources_get_resource ());
#endif

	init_logging ();

	init_protocol ();

	init_update_notification ();

	init_tally ();

	init_sw_p_08 ();

	init_physical_rcp ();

	screen = gdk_screen_get_default ();

	light_css_provider = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "light" G_DIR_SEPARATOR_S "main_window.css");
	gtk_css_provider_load_from_file (light_css_provider, file, NULL);
	g_object_unref (file);

	dark_css_provider = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "dark" G_DIR_SEPARATOR_S "Adwaita-dark.css");
	gtk_css_provider_load_from_file (dark_css_provider, file, NULL);
	g_object_unref (file);

	css_provider_gain_red = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "gain" G_DIR_SEPARATOR_S "red.css");
	gtk_css_provider_load_from_file (css_provider_gain_red, file, NULL);
	g_object_unref (file);

	css_provider_gain_green = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "gain" G_DIR_SEPARATOR_S "green.css");
	gtk_css_provider_load_from_file (css_provider_gain_green, file, NULL);
	g_object_unref (file);

	css_provider_gain_blue = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "gain" G_DIR_SEPARATOR_S "blue.css");
	gtk_css_provider_load_from_file (css_provider_gain_blue, file, NULL);
	g_object_unref (file);

	css_provider_pedestal_red = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "pedestal" G_DIR_SEPARATOR_S "red.css");
	gtk_css_provider_load_from_file (css_provider_pedestal_red, file, NULL);
	g_object_unref (file);

	css_provider_pedestal_green = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "pedestal" G_DIR_SEPARATOR_S "green.css");
	gtk_css_provider_load_from_file (css_provider_pedestal_green, file, NULL);
	g_object_unref (file);

	css_provider_pedestal_blue = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "pedestal" G_DIR_SEPARATOR_S "blue.css");
	gtk_css_provider_load_from_file (css_provider_pedestal_blue, file, NULL);
	g_object_unref (file);

	css_provider_white = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "white.css");
	gtk_css_provider_load_from_file (css_provider_white, file, NULL);
	g_object_unref (file);

	css_provider_black = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "black.css");
	gtk_css_provider_load_from_file (css_provider_black, file, NULL);
	g_object_unref (file);

	css_provider_raz = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "raz.css");
	gtk_css_provider_load_from_file (css_provider_raz, file, NULL);
	g_object_unref (file);

	css_provider_store = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "store.css");
	gtk_css_provider_load_from_file (css_provider_store, file, NULL);
	g_object_unref (file);

	css_provider_scene_selected = gtk_css_provider_new ();
	file = g_file_new_for_path ("resources" G_DIR_SEPARATOR_S "scene-selected.css");
	gtk_css_provider_load_from_file (css_provider_scene_selected, file, NULL);
	g_object_unref (file);

	create_main_window ();

	load_settings_from_config_file ();

	if (theme) gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (light_css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	else gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (dark_css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	create_scenes_page ();

	create_settings_page ();

	g_mutex_init (&cameras_sets_mutex);
	g_mutex_init (&current_cameras_set_mutex);

	load_cameras_sets_from_config_file ();

	g_signal_connect (G_OBJECT (source_cameras_set_list_box), "row-selected", G_CALLBACK (source_cameras_set_list_box_row_selected), NULL);
	g_signal_connect (G_OBJECT (destination_cameras_set_list_box), "row-selected", G_CALLBACK (destination_cameras_set_list_box_row_selected), NULL);

	gtk_widget_show_all (main_window);

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		for (i = 0; i < cameras_set_itr->number_of_cameras; i++) {
			rcp = cameras_set_itr->cameras[i];

			if (rcp->active) {
				gtk_widget_hide (rcp->scenes_bank_2_box);

				gtk_widget_hide (rcp->shutter_step_combo_box);
				gtk_widget_hide (rcp->shutter_synchro_button);

				if (rcp->model == AW_UE150) {
					gtk_widget_set_size_request (rcp->HLG_knee_frame, gtk_widget_get_allocated_width (rcp->knee_frame), gtk_widget_get_allocated_height (rcp->knee_frame));

					gtk_widget_hide (rcp->HLG_knee_frame);
					gtk_widget_hide (rcp->ELC_limit_label);
					gtk_widget_hide (rcp->ELC_limit_combo_box);
				}

				if (rcp->ip_address_is_valid) rcp_start_working (rcp);
			}
		}

		gtk_widget_hide (cameras_set_itr->master_rcp.scenes_bank_2_box);

		if (!show_master_rcp) gtk_widget_hide (cameras_set_itr->master_rcp.root_widget);

		if (cameras_set_itr->next != NULL) {
			gtk_widget_hide (cameras_set_itr->source_rcp_list_box);
			gtk_widget_hide (cameras_set_itr->destination_rcp_list_box);
		}
	}

	if (number_of_cameras_sets == 0) {
		gtk_widget_hide (scenes_page);
		add_cameras_set ();
	} else if (cameras_set_with_error != NULL) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK (main_window_notebook), cameras_set_with_error->page_num);
		show_cameras_set_configuration_window ();
	}

	start_update_notification ();

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		((rcp_t*)(glist_itr->data))->thread = g_thread_new (NULL, (GThreadFunc)check_if_camera_is_on, glist_itr->data);
	}

	start_physical_rcp ();

	start_tally ();

	start_sw_p_08 ();


	gtk_main ();


	if (backup_needed || physical_rcp.connected) save_settings_and_cameras_sets_to_config_file ();

	stop_sw_p_08_tcp_server ();
	stop_rs_communication ();

	stop_tally ();

	stop_physical_rcp ();

	stop_update_notification ();

	if (logging) stop_logging ();

	g_list_free (rcp_start_glist);

	for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		gtk_widget_destroy (rcp->shutter_synchro_window);
		gtk_widget_destroy (rcp->detail_window);
		gtk_widget_destroy (rcp->matrix_window);
		gtk_widget_destroy (rcp->knee_window);
		g_slist_free (rcp->other_rcp);
	}
	g_list_free (rcp_glist);

	g_list_free (ghost_rcp_glist);

	for (cameras_set_itr = cameras_sets; cameras_set_itr != NULL; cameras_set_itr = cameras_set_itr->next) {
		if (source_cameras_set_list_box != NULL) gtk_list_box_select_row (GTK_LIST_BOX (source_cameras_set_list_box), NULL);
		if (destination_cameras_set_list_box != NULL) gtk_list_box_select_row (GTK_LIST_BOX (destination_cameras_set_list_box), NULL);
	}

	gtk_widget_destroy (main_window);

	WSACleanup ();	//_WIN32

	return 0;
}

