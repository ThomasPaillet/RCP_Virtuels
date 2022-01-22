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


char *master_detail_label = "Master Detail: ";
char *detail_coring_label = "Detail Coring: ";
char *v_detail_level_label = "V Detail Level: ";
char *detail_band_label = "Detail Band: ";
char *detail_frequency_label = "Detail Frequency: ";
char *noise_suppress_label = "Noise Suppress: ";
char *level_depend_label = "Level Depend: ";
char *knee_aperture_level_label = "Knee Aperture Level: ";
char *detail_gain_plus_label = "Detail Gain(+): ";
char *detail_gain_minus_label = "Detail Gain(-): ";
char *fleshtone_noisesup_label = "FleshTone Noise Suppress: ";
char *skin_detail_effect_label = "Skin Detail Effect: ";

char master_detail_tooltip[] = "Cette option permet d'ajuster le niveau de correction des contours.";
char detail_coring_tooltip[] = "Permet de définir le niveau du signal (bruit compris) pour que l’effet de détail ne fonctionne pas.";
char v_detail_level_tooltip[] = "Cette option permet de régler le niveau de correction verticale des contours.";
char detail_band_tooltip[] = "Cette option sert à régler la fréquence d’accentuation (boost frequency) du détail, elle varie de -7 (Basse fréquence) à +7 (Haute fréquence).\nQuand une fréquence élevée est sélectionnée, un effet de détail est ajouté à des sujets qui ont une définition plus élevée.";
char noise_suppress_tooltip[] = "Cette option sert à réduire le bruit qui est généré par l’effet de détail.\nPlus la valeur est élevée, plus le bruit est réduit.";
char level_depend_tooltip[] = "Lorsque les détails des signaux lumineux sont mis en valeur, le détail des parties sombres est compressé.\nPlus le réglage est grand, plus le détail des parties lumineuses est compressé.";
char knee_aperture_level_tooltip[] = "Permet de régler le niveau de détail des parties très lumineuses.";
char detail_gain_plus_tooltip[] = "Permet de régler le niveau de détail de la direction plus (vers le plus lumineux).";
char detail_gain_minus_tooltip[] = "Permet de régler le niveau de détail de la direction moins (vers le plus sombre).";
char fleshtone_noisesup_tooltip[] = "Cette option sert à rendre la peau du sujet plus régulière et attrayante.\nPlus la valeur est grande, plus la peau des sujets humains apparait douce.";


#define MIN_VALUE 0x61
#define MAX_VALUE 0x9F

void set_master_detail_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->model == AW_UE150) {
		if (rcp->current_scene.master_detail == 0x80) sprintf (label, "%s0", master_detail_label);
		else sprintf (label, "%s%+d", master_detail_label, rcp->current_scene.master_detail - 0x80);
	} else sprintf (label, "%s%d", master_detail_label, rcp->current_scene.master_detail - 0x61);

	gtk_frame_set_label (GTK_FRAME (rcp->master_detail_frame), label);
}

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2)

BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2,10)
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2,1)

void master_detail_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->master_detail_scale), MASTER_DETAIL_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2,1)
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x79
#define MAX_VALUE 0x87

void set_v_detail_level_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.v_detail_level == 0x80) sprintf (label, "%s0", v_detail_level_label);
	else sprintf (label, "%s%+d", v_detail_level_label, rcp->current_scene.v_detail_level - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->v_detail_level_frame), label);
}

CAM_CMD_FUNCS(v_detail_level,"OSD:A1:",2)

BUTTON_PRESSED_MINUS_FUNC(v_detail_level,"OSD:A1:",2,3)
BUTTON_PRESSED_MINUS_FUNC(v_detail_level,"OSD:A1:",2,1)

void v_detail_level_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->v_detail_level_scale), 0x80);
}

BUTTON_PRESSED_PLUS_FUNC(v_detail_level,"OSD:A1:",2,1)
BUTTON_PRESSED_PLUS_FUNC(v_detail_level,"OSD:A1:",2,3)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x79
#define MAX_VALUE 0x87

void set_detail_band_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->model == AW_UE150) {
		if (rcp->current_scene.detail_band == 0x80) sprintf (label, "%s0", detail_frequency_label);
		else sprintf (label, "%s%+d", detail_frequency_label, rcp->current_scene.detail_band - 0x80);
	} else {
		if (rcp->current_scene.detail_band == 0x80) sprintf (label, "%s0", detail_band_label);
		else sprintf (label, "%s%+d", detail_band_label, rcp->current_scene.detail_band - 0x80);
	}

	gtk_frame_set_label (GTK_FRAME (rcp->detail_band_frame), label);
}

CAM_CMD_FUNCS(detail_band,"OSD:A2:",2)

BUTTON_PRESSED_MINUS_FUNC(detail_band,"OSD:A2:",2,3)
BUTTON_PRESSED_MINUS_FUNC(detail_band,"OSD:A2:",2,1)

void detail_band_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->detail_band_scale), 0x80);
}

BUTTON_PRESSED_PLUS_FUNC(detail_band,"OSD:A2:",2,1)
BUTTON_PRESSED_PLUS_FUNC(detail_band,"OSD:A2:",2,3)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x00
#define MAX_VALUE 0x3C

void set_noise_suppress_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d", noise_suppress_label, rcp->current_scene.noise_suppress);

	gtk_frame_set_label (GTK_FRAME (rcp->noise_suppress_frame), label);
}

CAM_CMD_FUNCS(noise_suppress,"OSD:22:",2)

BUTTON_PRESSED_MINUS_FUNC(noise_suppress,"OSD:22:",2,10)
BUTTON_PRESSED_MINUS_FUNC(noise_suppress,"OSD:22:",2,1)

void noise_suppress_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->noise_suppress_scale), NOISE_SUPPRESS_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(noise_suppress,"OSD:22:",2,1)
BUTTON_PRESSED_PLUS_FUNC(noise_suppress,"OSD:22:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x80
#define MAX_VALUE 0x9F

void set_fleshtone_noisesup_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->model == AW_UE150) sprintf (label, "%s%d", skin_detail_effect_label, rcp->current_scene.fleshtone_noisesup - 0x80);
	else sprintf (label, "%s%d", fleshtone_noisesup_label, rcp->current_scene.fleshtone_noisesup - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->fleshtone_noisesup_frame), label);
}

CAM_CMD_FUNCS(fleshtone_noisesup,"OSD:A3:",2)

BUTTON_PRESSED_MINUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,5)
BUTTON_PRESSED_MINUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,1)

void fleshtone_noisesup_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	if (rcp->model == AW_UE150) gtk_range_set_value (GTK_RANGE (rcp->fleshtone_noisesup_scale), FLESHTONE_NOISESUP_DEFAULT_AW_UE150);
	else gtk_range_set_value (GTK_RANGE (rcp->fleshtone_noisesup_scale), FLESHTONE_NOISESUP_DEFAULT_AW_HE130);
}

BUTTON_PRESSED_PLUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,1)
BUTTON_PRESSED_PLUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,5)

#undef MIN_VALUE
#undef MAX_VALUE

void detail_raz_button_clicked_AW_HE130 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(master_detail,MASTER_DETAIL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(v_detail_level,V_DETAIL_LEVEL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(detail_band,DETAIL_BAND_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(noise_suppress,NOISE_SUPPRESS_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(fleshtone_noisesup,FLESHTONE_NOISESUP_DEFAULT_AW_HE130)
}

void create_detail_window_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *window, *frame1, *frame2, *box1, *box2, *box3, *box4, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame1 = gtk_frame_new ("Détail");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_widget_set_margin_start (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame1, WINDOW_MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

//Master Detail
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, master_detail_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x61, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->master_detail_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (master_detail_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->master_detail_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (master_detail_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->master_detail_frame = frame2;

//V Detail Level
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, v_detail_level_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x79, 0x87, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->v_detail_level_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (v_detail_level_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->v_detail_level_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (v_detail_level_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->v_detail_level_frame = frame2;

//Detail Band
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, detail_band_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x79, 0x87, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->detail_band_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (detail_band_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->detail_band_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (detail_band_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->detail_band_frame = frame2;

//Noise Suppress
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, noise_suppress_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x00, 0x3C, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->noise_suppress_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (noise_suppress_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->noise_suppress_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (noise_suppress_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (noise_suppress_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (noise_suppress_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (noise_suppress_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (noise_suppress_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->noise_suppress_frame = frame2;

//Fleshtone Noisesup
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, fleshtone_noisesup_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x80, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->fleshtone_noisesup_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (fleshtone_noisesup_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->fleshtone_noisesup_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_minus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (fleshtone_noisesup_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_plus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->fleshtone_noisesup_frame = frame2;
	gtk_box_pack_start (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		widget = gtk_button_new_with_label ("raz");
		gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
		g_signal_connect (widget, "clicked", G_CALLBACK (detail_raz_button_clicked_AW_HE130), rcp);
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (frame1), box4);
	gtk_container_add (GTK_CONTAINER (window), frame1);

	rcp->detail_window = window;
}

#define MIN_VALUE 0x00
#define MAX_VALUE 0x3C

void set_detail_coring_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d", detail_coring_label, rcp->current_scene.detail_coring);

	gtk_frame_set_label (GTK_FRAME (rcp->detail_coring_frame), label);
}

CAM_CMD_FUNCS(detail_coring,"OSJ:12:",2)

BUTTON_PRESSED_MINUS_FUNC(detail_coring,"OSJ:12:",2,10)
BUTTON_PRESSED_MINUS_FUNC(detail_coring,"OSJ:12:",2,1)

void detail_coring_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->detail_coring_scale), DETAIL_CORING_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(detail_coring,"OSJ:12:",2,1)
BUTTON_PRESSED_PLUS_FUNC(detail_coring,"OSJ:12:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x79
#define MAX_VALUE 0x87

void set_level_depend_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.level_depend == 0x80) sprintf (label, "%s0", level_depend_label);
	else sprintf (label, "%s%+d", level_depend_label, rcp->current_scene.level_depend - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->level_depend_frame), label);
}

CAM_CMD_FUNCS(level_depend,"OSJ:13:",2)

BUTTON_PRESSED_MINUS_FUNC(level_depend,"OSJ:13:",2,3)
BUTTON_PRESSED_MINUS_FUNC(level_depend,"OSJ:13:",2,1)

void level_depend_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->level_depend_scale), 0x80);
}

BUTTON_PRESSED_PLUS_FUNC(level_depend,"OSJ:13:",2,1)
BUTTON_PRESSED_PLUS_FUNC(level_depend,"OSJ:13:",2,3)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x00
#define MAX_VALUE 0x05

void set_knee_aperture_level_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d", knee_aperture_level_label, rcp->current_scene.knee_aperture_level);

	gtk_frame_set_label (GTK_FRAME (rcp->knee_aperture_level_frame), label);
}

CAM_CMD_FUNCS(knee_aperture_level,"OSG:3F:",2)

BUTTON_PRESSED_MINUS_FUNC(knee_aperture_level,"OSG:3F:",2,1)

void knee_aperture_level_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->knee_aperture_level_scale), KNEE_APERTURE_LEVEL_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(knee_aperture_level,"OSG:3F:",2,1)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x61
#define MAX_VALUE 0x9F

void set_detail_gain_plus_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.detail_gain_plus == 0x80) sprintf (label, "%s0", detail_gain_plus_label);
	else sprintf (label, "%s%+d", detail_gain_plus_label, rcp->current_scene.detail_gain_plus - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->detail_gain_plus_frame), label);
}

CAM_CMD_FUNCS(detail_gain_plus,"OSA:38:",2)

BUTTON_PRESSED_MINUS_FUNC(detail_gain_plus,"OSA:38:",2,10)
BUTTON_PRESSED_MINUS_FUNC(detail_gain_plus,"OSA:38:",2,1)

void detail_gain_plus_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->detail_gain_plus_scale), DETAIL_GAIN_PLUS_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(detail_gain_plus,"OSA:38:",2,1)
BUTTON_PRESSED_PLUS_FUNC(detail_gain_plus,"OSA:38:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x61
#define MAX_VALUE 0x9F

void set_detail_gain_minus_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.detail_gain_minus == 0x80) sprintf (label, "%s0", detail_gain_minus_label);
	else sprintf (label, "%s%+d", detail_gain_minus_label, rcp->current_scene.detail_gain_minus - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->detail_gain_minus_frame), label);
}

CAM_CMD_FUNCS(detail_gain_minus,"OSA:39:",2)

BUTTON_PRESSED_MINUS_FUNC(detail_gain_minus,"OSA:39:",2,10)
BUTTON_PRESSED_MINUS_FUNC(detail_gain_minus,"OSA:39:",2,1)

void detail_gain_minus_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->detail_gain_minus_scale), DETAIL_GAIN_MINUS_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(detail_gain_minus,"OSA:39:",2,1)
BUTTON_PRESSED_PLUS_FUNC(detail_gain_minus,"OSA:39:",2,10)

#undef MIN_VALUE
#undef MAX_VALUE

void set_skin_detail (rcp_t *rcp)
{
	if (rcp->current_scene.skin_detail) send_cam_control_command (rcp, "OSA:40:1");
	else send_cam_control_command (rcp, "OSA:40:0");
}

void skin_detail_toggle_button_clicked (GtkToggleButton *skin_detail_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (skin_detail_toggle_button)) {
		rcp->current_scene.skin_detail = TRUE;
		gtk_button_set_label (GTK_BUTTON (rcp->skin_detail_toggle_button), "On");
		gtk_widget_set_sensitive (rcp->fleshtone_noisesup_frame, TRUE);
	} else {
		rcp->current_scene.skin_detail = FALSE;
		gtk_widget_set_sensitive (rcp->fleshtone_noisesup_frame, FALSE);
		gtk_button_set_label (GTK_BUTTON (rcp->skin_detail_toggle_button), "Off");
	}

	set_skin_detail (rcp);
}

void detail_raz_button_clicked_AW_UE150 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(master_detail,MASTER_DETAIL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(detail_coring,DETAIL_CORING_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(v_detail_level,V_DETAIL_LEVEL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(detail_band,DETAIL_BAND_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(level_depend,LEVEL_DEPEND_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(knee_aperture_level,KNEE_APERTURE_LEVEL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(detail_gain_plus,DETAIL_GAIN_PLUS_DEFAULT)
	RAZ_IHM_UPDATE_SCALE(detail_gain_minus,DETAIL_GAIN_MINUS_DEFAULT)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->skin_detail_toggle_button), SKIN_DETAIL_DEFAULT);
	RAZ_IHM_UPDATE_SCALE(fleshtone_noisesup,FLESHTONE_NOISESUP_DEFAULT_AW_UE150)
}

void create_detail_window_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *window, *frame1, *frame2, *box1, *box2, *box3, *box4, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame1 = gtk_frame_new ("Détail");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_widget_set_margin_start (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame1, WINDOW_MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

//Master Detail
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, master_detail_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x61, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->master_detail_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (master_detail_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->master_detail_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (master_detail_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (master_detail_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->master_detail_frame = frame2;

//Detail Coring
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, detail_coring_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x00, 0x3C, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->detail_coring_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (detail_coring_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->detail_coring_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_coring_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_coring_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (detail_coring_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_coring_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_coring_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->detail_coring_frame = frame2;

//V Detail Level
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, v_detail_level_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x79, 0x87, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->v_detail_level_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (v_detail_level_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->v_detail_level_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (v_detail_level_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (v_detail_level_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->v_detail_level_frame = frame2;

//Detail Frequency
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, detail_band_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x79, 0x87, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->detail_band_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (detail_band_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->detail_band_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (detail_band_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_band_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->detail_band_frame = frame2;

//Level Depend
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, level_depend_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x79, 0x87, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->level_depend_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (level_depend_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->level_depend_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (level_depend_minus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (level_depend_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("0");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (level_depend_0_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (level_depend_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+3");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (level_depend_plus_3_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->level_depend_frame = frame2;

//Knee Aperture Level
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, knee_aperture_level_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x00, 0x05, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->knee_aperture_level_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (knee_aperture_level_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->knee_aperture_level_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_aperture_level_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (knee_aperture_level_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (knee_aperture_level_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->knee_aperture_level_frame = frame2;

//Detail Gain(+)
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, detail_gain_plus_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x61, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->detail_gain_plus_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (detail_gain_plus_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->detail_gain_plus_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_plus_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_plus_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (detail_gain_plus_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_plus_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_plus_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->detail_gain_plus_frame = frame2;

//Detail Gain(-)
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, detail_gain_minus_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x61, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->detail_gain_minus_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (detail_gain_minus_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->detail_gain_minus_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_minus_minus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_minus_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (detail_gain_minus_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_minus_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+10");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (detail_gain_minus_plus_10_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->detail_gain_minus_frame = frame2;

//Skin Detail
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_container_set_border_width (GTK_CONTAINER (box2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (box2, fleshtone_noisesup_tooltip);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, WINDOW_MARGIN_VALUE);
				widget = gtk_label_new ("Skin Detail");
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_toggle_button_new_with_label ("Off");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				rcp->skin_detail_handler_id = g_signal_connect (widget, "toggled", G_CALLBACK (skin_detail_toggle_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->skin_detail_toggle_button = widget;
			gtk_box_set_center_widget (GTK_BOX (box2), box3);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

//Skin Detail Effect
		frame2 = gtk_frame_new (NULL);
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.02, 0.5);
		gtk_container_set_border_width (GTK_CONTAINER (frame2), WINDOW_MARGIN_VALUE);
		gtk_widget_set_tooltip_text (frame2, fleshtone_noisesup_tooltip);
		box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_start (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_end (box2, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (box2, WINDOW_MARGIN_VALUE);
			widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x80, 0x9F, 1.0);
			gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
			gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
			rcp->fleshtone_noisesup_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (fleshtone_noisesup_value_changed), rcp);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			rcp->fleshtone_noisesup_scale = widget;

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_set_margin_top (box3, MARGIN_VALUE);
			gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
				widget = gtk_button_new_with_label ("-5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_minus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("-1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_minus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("default");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (widget, "clicked", G_CALLBACK (fleshtone_noisesup_default_button_clicked), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+1");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_plus_1_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

				widget = gtk_button_new_with_label ("+5");
				g_signal_connect (widget, "button_press_event", G_CALLBACK (fleshtone_noisesup_plus_5_button_pressed), rcp);
				g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
		rcp->fleshtone_noisesup_frame = frame2;
	gtk_box_pack_start (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		widget = gtk_button_new_with_label ("raz");
		gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
		g_signal_connect (widget, "clicked", G_CALLBACK (detail_raz_button_clicked_AW_UE150), rcp);
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (frame1), box4);
	gtk_container_add (GTK_CONTAINER (window), frame1);

	rcp->detail_window = window;
}

void set_detail (rcp_t *rcp)
{
	if (rcp->current_scene.detail) send_cam_control_command (rcp, "ODT:1");
	else send_cam_control_command (rcp, "ODT:0");

	if ((rcp == rcp_vision) && physical_rcp.connected) {
		g_mutex_lock (&physical_rcp.mutex);
		physical_rcp.detail = rcp->current_scene.detail;
		send_detail_update_notification ();
		g_mutex_unlock (&physical_rcp.mutex);
	}
}

void detail_toggle_button_clicked (GtkToggleButton *detail_toggle_button, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (detail_toggle_button)) {
		gtk_button_set_label (GTK_BUTTON (detail_toggle_button), "On");
		gtk_widget_set_sensitive (rcp->detail_settings_button, TRUE);
		rcp->current_scene.detail = TRUE;
	} else {
		rcp->current_scene.detail = FALSE;
		gtk_widget_set_sensitive (rcp->detail_settings_button, FALSE);
		gtk_button_set_label (GTK_BUTTON (detail_toggle_button), "Off");
	}

	set_detail (rcp);
}

void show_rcp_detail_window (rcp_t *rcp)
{
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

	popup_window = rcp->detail_window;
	gtk_widget_show_all (rcp->detail_window);
}

GtkWidget *create_detail_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("Détail");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_toggle_button_new_with_label ("Off");
		rcp->detail_handler_id = g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (detail_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->detail_toggle_button = widget;

		if (rcp->model == AW_UE150) create_detail_window_AW_UE150 (rcp);
		else create_detail_window_AW_HE130 (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_detail_window), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->detail_settings_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

