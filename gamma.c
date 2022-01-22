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

#include "cam_cmd_define.h"


char gamma_type_tooltip_AW_HE130[] = "HD: Gamma caractéristique pour la vidéo en haute définition.\nSD: Le gain dans les zones sombres est plus élevé que dans le gamma HD.\nFILMLIKE1: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que le gamma HD.\nFILMLIKE2: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE1].\nFILMLIKE3: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE2].";
char gamma_type_tooltip_AW_UE150[] = "HD: Gamma caractéristique pour la vidéo en haute définition.\nFILMLIKE1: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que le gamma HD.\nFILMLIKE2: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE1].\nFILMLIKE3: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE2].\nFILM REC: Caractéristique gamma pour un film cinéma.\nVIDEO REC: Caractéristique gamma pour une vidéo cinéma.\nHLG: Caractéristique Hybrid Log Gamma.";
char gamma_type_tooltip[] = "HD: Gamma caractéristique pour la vidéo en haute définition.\nSD: Le gain dans les zones sombres est plus élevé que dans le gamma HD.\nFILMLIKE1: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que le gamma HD.\nFILMLIKE2: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE1].\nFILMLIKE3: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE2].\nFILM REC: Caractéristique gamma pour un film cinéma.\nVIDEO REC: Caractéristique gamma pour une vidéo cinéma.\nHLG: Caractéristique Hybrid Log Gamma.";
char gamma_tooltip[] = "Définir des valeurs plus basses adoucit la pente de la courbe gamma pour les zones de faible luminosité et introduit un contraste plus marqué.\nDéfinir des valeurs plus élevées permet d'élargir la gamme de dégradés dans les zones sombres et de produire des images plus lumineuses.\nLa pente de la courbe gamma pour les zones de faible luminosité sera plus forte et le contraste atténué.";
char drs_tooltip[] = "La fonction \"plage dynamique étendue (Dynamic Range Stretch)\" effectue des corrections lorsqu'une vidéo présentant un contraste élevé entre zones très éclairées et sombres est affichée.\nQuand [DRS] est activé, le réglage du \"Knee\" est désactivé.";

char film_rec_black_stretch_level_label[] = "F-REC black STR. Level: ";
char video_rec_knee_point_label[] = "V-REC Knee Point: ";
char black_gamma_label[] = "Black Gamma: ";


void set_gamma_type (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSE:72:", rcp->current_scene.gamma_type, TRUE);
}

void gamma_type_changed_AW_HE130 (GtkComboBox *gamma_type_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gamma_type = gtk_combo_box_get_active (gamma_type_combo_box);

	set_gamma_type (rcp);
}

void set_gamma (rcp_t *rcp)
{
	send_cam_control_command_2_digits (rcp, "OSA:6A:", 0x94 - rcp->current_scene.gamma, TRUE);
}

void gamma_changed (GtkComboBox *gamma_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gamma = gtk_combo_box_get_active (gamma_combo_box);

	set_gamma (rcp);
}

void set_drs (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSE:33:", 3 - rcp->current_scene.drs, TRUE);
}

void drs_changed (GtkComboBox *drs_combo_box, rcp_t *rcp)
{
	rcp->current_scene.drs = gtk_combo_box_get_active (drs_combo_box);

	set_drs (rcp);

	if (rcp->current_scene.drs == 3) {
		gtk_widget_set_sensitive (rcp->knee_settings_combo_box, TRUE);
		if (rcp->current_scene.knee_settings == 1) gtk_widget_set_sensitive (rcp->knee_button, TRUE);
	} else {
		gtk_widget_set_sensitive (rcp->knee_settings_combo_box, FALSE);
		gtk_widget_set_sensitive (rcp->knee_button, FALSE);
	}
}

GtkWidget *create_gamma_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;
	int i;
	float value;
	char label[8];

	frame = gtk_frame_new ("Gamma");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
	gtk_box_set_spacing (GTK_BOX (box), MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, gamma_type_tooltip_AW_HE130);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "SD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE1");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE2");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE3");
		rcp->gamma_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_type_changed_AW_HE130), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->gamma_type_combo_box = widget;

		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, gamma_tooltip);
		for (i = 0x67, value = 0.30; i <= 0x94; i++, value += 0.01) {
			sprintf (label, "%4.2f", value);
			gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		rcp->gamma_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->gamma_combo_box = widget;

		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, drs_tooltip);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "High");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Mid");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Low");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Off");
		rcp->drs_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (drs_changed), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->drs_combo_box = widget;

		widget = gtk_label_new ("DRS:");
		gtk_widget_set_tooltip_text (widget, drs_tooltip);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

void set_film_rec_dynamic_level (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSA:10:", 4 - rcp->current_scene.film_rec_dynamic_level, TRUE);
}

void film_rec_dynamic_level_changed (GtkComboBox *film_rec_dynamic_level_combo_box, rcp_t *rcp)
{
	rcp->current_scene.film_rec_dynamic_level = gtk_combo_box_get_active (film_rec_dynamic_level_combo_box);

	set_film_rec_dynamic_level (rcp);
}

#define MIN_VALUE 0x00
#define MAX_VALUE 0x1E

void set_film_rec_black_stretch_level_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d%%", film_rec_black_stretch_level_label, rcp->current_scene.film_rec_black_stretch_level);

	gtk_frame_set_label (GTK_FRAME (rcp->film_rec_black_stretch_level_frame), label);
}

CAM_CMD_FUNCS(film_rec_black_stretch_level,"OSA:0F:",2)

void film_rec_black_stretch_level_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->film_rec_black_stretch_level_scale), FILM_REC_BLACK_STRETCH_LEVEL_DEFAULT);
}

BUTTON_PRESSED_MINUS_FUNC(film_rec_black_stretch_level,"OSA:0F:",2,5)
BUTTON_PRESSED_MINUS_FUNC(film_rec_black_stretch_level,"OSA:0F:",2,1)

BUTTON_PRESSED_PLUS_FUNC(film_rec_black_stretch_level,"OSA:0F:",2,1)
BUTTON_PRESSED_PLUS_FUNC(film_rec_black_stretch_level,"OSA:0F:",2,5)

#undef MIN_VALUE
#undef MAX_VALUE

void set_video_rec_knee_slope (rcp_t *rcp)
{
	send_cam_control_command_2_digits (rcp, "OSA:25:", 0x83 - rcp->current_scene.video_rec_knee_slope, TRUE);
}

void video_rec_knee_slope_changed (GtkComboBox *video_rec_knee_slope_combo_box, rcp_t *rcp)
{
	rcp->current_scene.video_rec_knee_slope = gtk_combo_box_get_active (video_rec_knee_slope_combo_box);

	set_video_rec_knee_slope (rcp);
}

#define MIN_VALUE 0x62
#define MAX_VALUE 0xAF

void set_video_rec_knee_point_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d%%", video_rec_knee_point_label, rcp->current_scene.video_rec_knee_point - 68);

	gtk_frame_set_label (GTK_FRAME (rcp->video_rec_knee_point_frame), label);
}

CAM_CMD_FUNCS(video_rec_knee_point,"OSA:21:",2)

void video_rec_knee_point_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->video_rec_knee_point_scale), VIDEO_REC_KNEE_POINT_DEFAULT);
}

BUTTON_PRESSED_MINUS_FUNC(video_rec_knee_point,"OSA:21:",2,10)
BUTTON_PRESSED_MINUS_FUNC(video_rec_knee_point,"OSA:21:",2,1)

BUTTON_PRESSED_PLUS_FUNC(video_rec_knee_point,"OSA:21:",2,1)
BUTTON_PRESSED_PLUS_FUNC(video_rec_knee_point,"OSA:21:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x78
#define MAX_VALUE 0x88

void set_black_gamma_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.black_gamma == 0x80) sprintf (label, "%s0", black_gamma_label);
	else sprintf (label, "%s%+d", black_gamma_label, rcp->current_scene.black_gamma - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->black_gamma_frame), label);
}

CAM_CMD_FUNCS(black_gamma,"OSA:07:",2)

BUTTON_PRESSED_MINUS_FUNC(black_gamma,"OSA:07:",2,3)
BUTTON_PRESSED_MINUS_FUNC(black_gamma,"OSA:07:",2,1)

void black_gamma_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->black_gamma_scale), BLACK_GAMMA_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(black_gamma,"OSA:07:",2,1)
BUTTON_PRESSED_PLUS_FUNC(black_gamma,"OSA:07:",2,3)

#undef MIN_VALUE
#undef MAX_VALUE

void set_black_gamma_range (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSJ:1B:", 3 - rcp->current_scene.black_gamma_range, TRUE);
}

void black_gamma_range_changed (GtkComboBox *black_gamma_range_combo_box, rcp_t *rcp)
{
	rcp->current_scene.black_gamma_range = gtk_combo_box_get_active (black_gamma_range_combo_box);

	set_black_gamma_range (rcp);
}

void gamma_raz_button_clicked_AW_UE150 (GtkButton *button, rcp_t *rcp)
{
	if (rcp->current_scene.gamma_type == 7) {
		RAZ_IHM_UPDATE_SCALE(black_gamma,BLACK_GAMMA_DEFAULT)
		RAZ_IHM_UPDATE_COMBO_BOX(black_gamma_range,BLACK_GAMMA_RANGE_DEFAULT);
	} else if (rcp->current_scene.gamma_type == 6) {
		RAZ_IHM_UPDATE_COMBO_BOX(gamma,GAMMA_DEFAULT);
		RAZ_IHM_UPDATE_COMBO_BOX(video_rec_knee_slope,VIDEO_REC_KNEE_SLOPE_DEFAULT);
		RAZ_IHM_UPDATE_SCALE(video_rec_knee_point,VIDEO_REC_KNEE_POINT_DEFAULT);
		RAZ_IHM_UPDATE_SCALE(black_gamma,BLACK_GAMMA_DEFAULT)
		RAZ_IHM_UPDATE_COMBO_BOX(black_gamma_range,BLACK_GAMMA_RANGE_DEFAULT);
		RAZ_IHM_UPDATE_COMBO_BOX(drs,DRS_DEFAULT);
	} else if (rcp->current_scene.gamma_type == 5) {
		RAZ_IHM_UPDATE_COMBO_BOX(gamma,GAMMA_DEFAULT);
		RAZ_IHM_UPDATE_COMBO_BOX(film_rec_dynamic_level,FILM_REC_DYNAMIC_LEVEL_DEFAULT);
		RAZ_IHM_UPDATE_SCALE(film_rec_black_stretch_level,FILM_REC_BLACK_STRETCH_LEVEL_DEFAULT);
		RAZ_IHM_UPDATE_SCALE(black_gamma,BLACK_GAMMA_DEFAULT)
		RAZ_IHM_UPDATE_COMBO_BOX(black_gamma_range,BLACK_GAMMA_RANGE_DEFAULT);
		RAZ_IHM_UPDATE_COMBO_BOX(drs,DRS_DEFAULT);
	} else {
		RAZ_IHM_UPDATE_COMBO_BOX(gamma,GAMMA_DEFAULT);
		RAZ_IHM_UPDATE_SCALE(black_gamma,BLACK_GAMMA_DEFAULT)
		RAZ_IHM_UPDATE_COMBO_BOX(black_gamma_range,BLACK_GAMMA_RANGE_DEFAULT);
		RAZ_IHM_UPDATE_COMBO_BOX(drs,DRS_DEFAULT);
	}
}

void create_gamma_window_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame1, *frame2, *box1, *box2, *box3, *box4, *widget;
	int i;
	float value;
	char label[8];

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
//	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame1 = gtk_frame_new ("Gamma");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_widget_set_margin_start (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame1, WINDOW_MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

//Gamma
		rcp->gamma_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (rcp->gamma_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (rcp->gamma_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (rcp->gamma_box, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (rcp->gamma_box), WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Gamma:");
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_tooltip_text (widget, gamma_tooltip);
			gtk_box_pack_start (GTK_BOX (rcp->gamma_box), widget, FALSE, FALSE, 0);

			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_tooltip_text (widget, gamma_tooltip);
			for (i = 0x67, value = 0.30; i <= 0x94; i++, value += 0.01) {
				sprintf (label, "%4.2f", value);
				gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT (widget), label);
			}
			rcp->gamma_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_changed), rcp);
			gtk_box_pack_end (GTK_BOX (rcp->gamma_box), widget, FALSE, FALSE, 0);
			rcp->gamma_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), rcp->gamma_box, FALSE, FALSE, 0);

//F-REC Dynamic Level
		rcp->film_rec_dynamic_level_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (rcp->film_rec_dynamic_level_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (rcp->film_rec_dynamic_level_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (rcp->film_rec_dynamic_level_box, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (rcp->film_rec_dynamic_level_box), WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("F-REC Dynamic Level:");
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_box_pack_start (GTK_BOX (rcp->film_rec_dynamic_level_box), widget, FALSE, FALSE, 0);

			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "600%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "500%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "400%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "300%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "200%");
			rcp->film_rec_dynamic_level_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (film_rec_dynamic_level_changed), rcp);
			gtk_box_pack_end (GTK_BOX (rcp->film_rec_dynamic_level_box), widget, FALSE, FALSE, 0);
			rcp->film_rec_dynamic_level_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), rcp->film_rec_dynamic_level_box, FALSE, FALSE, 0);

//F-REC black STR. Level
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_widget_set_margin_start (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x00, 0x1E, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->film_rec_black_stretch_level_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (film_rec_black_stretch_level_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->film_rec_black_stretch_level_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (film_rec_black_stretch_level_minus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (film_rec_black_stretch_level_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (film_rec_black_stretch_level_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (film_rec_black_stretch_level_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (film_rec_black_stretch_level_plus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->film_rec_black_stretch_level_frame = frame2;

//V-REC Knee Slope
		rcp->video_rec_knee_slope_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (rcp->video_rec_knee_slope_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (rcp->video_rec_knee_slope_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (rcp->video_rec_knee_slope_box, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (rcp->video_rec_knee_slope_box), WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("V-REC Knee Slope:");
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_box_pack_start (GTK_BOX (rcp->video_rec_knee_slope_box), widget, FALSE, FALSE, 0);

			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "500%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "450%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "400%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "350%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "300%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "250%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "200%");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "150%");
			rcp->video_rec_knee_slope_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (video_rec_knee_slope_changed), rcp);
			gtk_box_pack_end (GTK_BOX (rcp->video_rec_knee_slope_box), widget, FALSE, FALSE, 0);
			rcp->video_rec_knee_slope_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), rcp->video_rec_knee_slope_box, FALSE, FALSE, 0);

//V-REC Knee Point
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_widget_set_margin_start (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, WINDOW_MARGIN_VALUE);
//gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x62, 0xAF, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->video_rec_knee_point_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (video_rec_knee_point_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->video_rec_knee_point_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (video_rec_knee_point_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (video_rec_knee_point_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (video_rec_knee_point_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "button_press_event", G_CALLBACK (video_rec_knee_point_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (video_rec_knee_point_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->video_rec_knee_point_frame = frame2;

//Black Gamma
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_widget_set_margin_start (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (frame2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (frame2, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x78, 0x88, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->black_gamma_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (black_gamma_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->black_gamma_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (black_gamma_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (black_gamma_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (black_gamma_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (black_gamma_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (black_gamma_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->black_gamma_frame = frame2;

//Black Gamma Range
		rcp->black_gamma_range_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (rcp->black_gamma_range_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (rcp->black_gamma_range_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (rcp->black_gamma_range_box, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (rcp->black_gamma_range_box), WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("Black Gamma Range:");
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_box_pack_start (GTK_BOX (rcp->black_gamma_range_box), widget, FALSE, FALSE, 0);

			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "3 (~40%)");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "2 (~30%)");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "1 (~20%)");
			rcp->black_gamma_range_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (black_gamma_range_changed), rcp);
			gtk_box_pack_end (GTK_BOX (rcp->black_gamma_range_box), widget, FALSE, FALSE, 0);
			rcp->black_gamma_range_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), rcp->black_gamma_range_box, FALSE, FALSE, 0);

//DRS
		rcp->drs_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (rcp->drs_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (rcp->drs_box, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (rcp->drs_box, WINDOW_MARGIN_VALUE);
//		gtk_container_set_border_width (GTK_CONTAINER (rcp->drs_box), WINDOW_MARGIN_VALUE);
			widget = gtk_label_new ("DRS:");
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_tooltip_text (widget, drs_tooltip);
			gtk_box_pack_start (GTK_BOX (rcp->drs_box), widget, FALSE, FALSE, 0);

			widget = gtk_combo_box_text_new ();
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
			gtk_widget_set_tooltip_text (widget, drs_tooltip);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "High");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Mid");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Low");
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Off");
			rcp->drs_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (drs_changed), rcp);
			gtk_box_pack_end (GTK_BOX (rcp->drs_box), widget, FALSE, FALSE, 0);
			rcp->drs_combo_box = widget;
		gtk_box_pack_start (GTK_BOX (box1), rcp->drs_box, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		widget = gtk_button_new_with_label ("raz");
		gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
		g_signal_connect (widget, "clicked", G_CALLBACK (gamma_raz_button_clicked_AW_UE150), rcp);
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (frame1), box4);
	gtk_container_add (GTK_CONTAINER (window), frame1);

	gtk_widget_show_all (frame1);

	rcp->gamma_window = window;
}

void gamma_type_changed_AW_UE150 (GtkComboBox *gamma_type_combo_box, rcp_t *rcp)
{
	rcp->current_scene.gamma_type = gtk_combo_box_get_active (gamma_type_combo_box);

	if (rcp->current_scene.gamma_type) rcp->current_scene.gamma_type++;

	set_gamma_type (rcp);

	if (rcp->current_scene.gamma_type == 7) {
		gtk_widget_hide (rcp->knee_frame);
		gtk_widget_show (rcp->HLG_knee_frame);
	} else {
		gtk_widget_hide (rcp->HLG_knee_frame);
		gtk_widget_show (rcp->knee_frame);
	}
}

void show_rcp_gamma_window_AW_UE150 (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	if (rcp->current_scene.gamma_type == 7) {
		gtk_widget_hide (rcp->gamma_box);
		gtk_widget_hide (rcp->film_rec_dynamic_level_box);
		gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
		gtk_widget_hide (rcp->video_rec_knee_slope_box);
		gtk_widget_hide (rcp->video_rec_knee_point_frame);
		gtk_widget_show (rcp->black_gamma_frame);
		gtk_widget_show (rcp->black_gamma_range_box);
		gtk_widget_hide (rcp->drs_box);
	} else if (rcp->current_scene.gamma_type == 6) {
		gtk_widget_show (rcp->gamma_box);
		gtk_widget_hide (rcp->film_rec_dynamic_level_box);
		gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
		gtk_widget_show (rcp->video_rec_knee_slope_box);
		gtk_widget_show (rcp->video_rec_knee_point_frame);
		gtk_widget_show (rcp->black_gamma_frame);
		gtk_widget_show (rcp->black_gamma_range_box);
		gtk_widget_show (rcp->drs_box);
	} else if (rcp->current_scene.gamma_type == 5) {
		gtk_widget_show (rcp->gamma_box);
		gtk_widget_show (rcp->film_rec_dynamic_level_box);
		gtk_widget_show (rcp->film_rec_black_stretch_level_frame);
		gtk_widget_hide (rcp->video_rec_knee_slope_box);
		gtk_widget_hide (rcp->video_rec_knee_point_frame);
		gtk_widget_show (rcp->black_gamma_frame);
		gtk_widget_show (rcp->black_gamma_range_box);
		gtk_widget_show (rcp->drs_box);
	} else {
		gtk_widget_show (rcp->gamma_box);
		gtk_widget_hide (rcp->film_rec_dynamic_level_box);
		gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
		gtk_widget_hide (rcp->video_rec_knee_slope_box);
		gtk_widget_hide (rcp->video_rec_knee_point_frame);
		gtk_widget_show (rcp->black_gamma_frame);
		gtk_widget_show (rcp->black_gamma_range_box);
		gtk_widget_show (rcp->drs_box);
	}

	gtk_window_resize (GTK_WINDOW (rcp->gamma_window), 100, 100);

	popup_window = rcp->gamma_window;
	gtk_widget_show (rcp->gamma_window);
}

GtkWidget *create_gamma_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("Gamma");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
	gtk_box_set_spacing (GTK_BOX (box), MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_tooltip_text (widget, gamma_type_tooltip_AW_UE150);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE1");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE2");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE3");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILM REC");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "VIDEO REC");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HLG");
		rcp->gamma_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_type_changed_AW_UE150), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->gamma_type_combo_box = widget;

		create_gamma_window_AW_UE150 (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_gamma_window_AW_UE150), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->gamma_settings_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

gboolean update_gamma_type_combo_box (rcp_t *rcp)
{
	int index;

	if ((rcp->model == AW_UE150) && rcp->current_scene.gamma_type) index = rcp->current_scene.gamma_type - 1;
	else index = rcp->current_scene.gamma_type;

	g_signal_handler_block (rcp->gamma_type_combo_box, rcp->gamma_type_handler_id);
	gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gamma_type_combo_box), index);
	g_signal_handler_unblock (rcp->gamma_type_combo_box, rcp->gamma_type_handler_id);

	if (rcp->model == AW_UE150) {
		if (rcp->current_scene.gamma_type == 7) {
			if (popup_window == rcp->gamma_window) {
				gtk_widget_hide (rcp->gamma_box);
				gtk_widget_hide (rcp->film_rec_dynamic_level_box);
				gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
				gtk_widget_hide (rcp->video_rec_knee_slope_box);
				gtk_widget_hide (rcp->video_rec_knee_point_frame);
				gtk_widget_show (rcp->black_gamma_frame);
				gtk_widget_show (rcp->black_gamma_range_box);
				gtk_widget_hide (rcp->drs_box);

				gtk_window_resize (GTK_WINDOW (rcp->gamma_window), 100, 100);
			}

			gtk_widget_hide (rcp->knee_frame);
			gtk_widget_show (rcp->HLG_knee_frame);
		} else if (rcp->current_scene.gamma_type == 6) {
			if (popup_window == rcp->gamma_window) {
				gtk_widget_show (rcp->gamma_box);
				gtk_widget_hide (rcp->film_rec_dynamic_level_box);
				gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
				gtk_widget_show (rcp->video_rec_knee_slope_box);
				gtk_widget_show (rcp->video_rec_knee_point_frame);
				gtk_widget_show (rcp->black_gamma_frame);
				gtk_widget_show (rcp->black_gamma_range_box);
				gtk_widget_show (rcp->drs_box);

				gtk_window_resize (GTK_WINDOW (rcp->gamma_window), 100, 100);
			}

			gtk_widget_hide (rcp->HLG_knee_frame);
			gtk_widget_show (rcp->knee_frame);
		} else if (rcp->current_scene.gamma_type == 5) {
			if (popup_window == rcp->gamma_window) {
				gtk_widget_show (rcp->gamma_box);
				gtk_widget_show (rcp->film_rec_dynamic_level_box);
				gtk_widget_show (rcp->film_rec_black_stretch_level_frame);
				gtk_widget_hide (rcp->video_rec_knee_slope_box);
				gtk_widget_hide (rcp->video_rec_knee_point_frame);
				gtk_widget_show (rcp->black_gamma_frame);
				gtk_widget_show (rcp->black_gamma_range_box);
				gtk_widget_show (rcp->drs_box);

				gtk_window_resize (GTK_WINDOW (rcp->gamma_window), 100, 100);
			}

			gtk_widget_hide (rcp->HLG_knee_frame);
			gtk_widget_show (rcp->knee_frame);
		} else {
			if (popup_window == rcp->gamma_window) {
				gtk_widget_show (rcp->gamma_box);
				gtk_widget_hide (rcp->film_rec_dynamic_level_box);
				gtk_widget_hide (rcp->film_rec_black_stretch_level_frame);
				gtk_widget_hide (rcp->video_rec_knee_slope_box);
				gtk_widget_hide (rcp->video_rec_knee_point_frame);
				gtk_widget_show (rcp->black_gamma_frame);
				gtk_widget_show (rcp->black_gamma_range_box);
				gtk_widget_show (rcp->drs_box);

				gtk_window_resize (GTK_WINDOW (rcp->gamma_window), 100, 100);
			}

			gtk_widget_hide (rcp->HLG_knee_frame);
			gtk_widget_show (rcp->knee_frame);
		}
	}

	return G_SOURCE_REMOVE;
}

