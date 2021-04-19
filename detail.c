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

#include "cam_cmd_define.h"


extern GtkCssProvider *css_provider_raz;

const char *detail_txt = "Détail";

char *master_detail_label = "Master Detail: ";
char *v_detail_level_label = "V Detail Level: ";
char *detail_band_label = "Detail Band: ";
char *noise_suppress_label = "Noise Suppress: ";
char *fleshtone_noisesup_label = "FleshTone Noise Suppress: ";

char master_detail_tooltip[] = "Cette option permet d'ajuster le niveau de correction des contours.";
char v_detail_level_tooltip[] = "Cette option permet de régler le niveau de correction verticale des contours.";
char detail_band_tooltip[] = "Cette option sert à régler la fréquence d’accentuation (boost frequency) du détail, elle varie de -7 (Basse fréquence) à +7 (Haute fréquence).\nQuand une fréquence élevée est sélectionnée, un effet de détail est ajouté à des sujets qui ont une définition plus élevée.";
char noise_suppress_tooltip[] = "Cette option sert à réduire le bruit qui est généré par l’effet de détail.\nPlus la valeur est élevée, plus le bruit est réduit.";
char fleshtone_noisesup_tooltip[] = "Cette option sert à rendre la peau du sujet plus régulière et attrayante.";


#define MIN_VALUE 0x61
#define MAX_VALUE 0x9F

void set_master_detail_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d", master_detail_label, rcp->current_scene.master_detail - 0x61);

	gtk_frame_set_label (GTK_FRAME (rcp->master_detail_frame), label);
}

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(master_detail,"OSA:30:",2)

void master_detail_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->master_detail_scale), MASTER_DETAIL_DEFAULT);
}

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

void v_detail_level_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->v_detail_level_scale), 0x80);
}

BUTTON_PRESSED_PLUS_FUNC(v_detail_level,"OSD:A1:",2,3)

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x79
#define MAX_VALUE 0x87

void set_detail_band_label (rcp_t *rcp)
{
	char label[32];

	if (rcp->current_scene.detail_band == 0x80) sprintf (label, "%s0", detail_band_label);
	else sprintf (label, "%s%+d", detail_band_label, rcp->current_scene.detail_band - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->detail_band_frame), label);
}

CAM_CMD_FUNCS(detail_band,"OSD:A2:",2)

BUTTON_PRESSED_MINUS_FUNC(detail_band,"OSD:A2:",2,3)

void detail_band_0_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->detail_band_scale), 0x80);
}

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

void noise_suppress_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->noise_suppress_scale), NOISE_SUPPRESS_DEFAULT);
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x80
#define MAX_VALUE 0x9F

void set_fleshtone_noisesup_label (rcp_t *rcp)
{
	char label[32];

	sprintf (label, "%s%d", fleshtone_noisesup_label, rcp->current_scene.fleshtone_noisesup - 0x80);

	gtk_frame_set_label (GTK_FRAME (rcp->fleshtone_noisesup_frame), label);
}

CAM_CMD_FUNCS(fleshtone_noisesup,"OSD:A3:",2)

BUTTON_PRESSED_MINUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,5)

void fleshtone_noisesup_default_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->fleshtone_noisesup_scale), FLESHTONE_NOISESUP_DEFAULT);
}

BUTTON_PRESSED_PLUS_FUNC(fleshtone_noisesup,"OSD:A3:",2,5)

#undef MIN_VALUE
#undef MAX_VALUE

void detail_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	gtk_range_set_value (GTK_RANGE (rcp->master_detail_scale), MASTER_DETAIL_DEFAULT);
	gtk_range_set_value (GTK_RANGE (rcp->v_detail_level_scale), V_DETAIL_LEVEL_DEFAULT);
	gtk_range_set_value (GTK_RANGE (rcp->detail_band_scale), DETAIL_BAND_DEFAULT);
	gtk_range_set_value (GTK_RANGE (rcp->noise_suppress_scale), NOISE_SUPPRESS_DEFAULT);
	gtk_range_set_value (GTK_RANGE (rcp->fleshtone_noisesup_scale), FLESHTONE_NOISESUP_DEFAULT);
}

void create_detail_window (rcp_t *rcp)
{
	GtkWidget *window, *frame1, *frame2, *box1, *box2, *box3, *box4, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), detail_txt);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);

	frame1 = gtk_frame_new (detail_txt);
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_widget_set_margin_start (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame1, WINDOW_MARGIN_VALUE);
	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
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
		g_signal_connect (widget, "clicked", G_CALLBACK (detail_raz_button_clicked), rcp);
		gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
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

	if (physical_rcp.connected && (rcp == rcp_vision)) {
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
	knee_matrix_detail_popup = TRUE;
	gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
	gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->camera_set))->master_rcp.root_widget), TRUE);
	gtk_widget_show_all (rcp->detail_window);
}

GtkWidget *create_detail_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new (detail_txt);
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_toggle_button_new_with_label ("Off");
		rcp->detail_handler_id = g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (detail_toggle_button_clicked), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->detail_toggle_button = widget;

		create_detail_window (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_detail_window), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->detail_settings_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

