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


char gamma_type_tooltip[] = "HD: Gamma caractéristique pour la vidéo en haute définition.\nSD: Le gain dans les zones sombres est plus élevé que dans le gamma HD.\nFILMLIKE1: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que le gamma HD.\nFILMLIKE2: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE1].\nFILMLIKE3: Capacité de reproduire de meilleurs dégradés dans les zones\n       fortement éclairées que [FILMLIKE2].";
char gamma_tooltip[] = "Définir des valeurs plus basses adoucit la pente de la courbe gamma pour les zones de faible luminosité et introduit un contraste plus marqué.\nDéfinir des valeurs plus élevées permet d'élargir la gamme de dégradés dans les zones sombres et de produire des images plus lumineuses.\nLa pente de la courbe gamma pour les zones de faible luminosité sera plus forte et le contraste atténué.";
char drs_tooltip[] = "La fonction \"plage dynamique étendue (Dynamic Range Stretch)\" effectue des corrections lorsqu'une vidéo présentant un contraste élevé entre zones très éclairées et sombres est affichée.\nQuand [DRS] est activé, le réglage du \"Knee\" est désactivé.";


void set_gamma_type (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSE:72:", rcp->current_scene.gamma_type);
}

void gamma_type_changed (GtkComboBox *gamma_type_combo_box, rcp_t *rcp)
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
	send_cam_control_command_1_digit (rcp, "OSE:33:", 3 - rcp->current_scene.drs);
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

GtkWidget *create_gamma_frame (rcp_t *rcp)
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
		gtk_widget_set_tooltip_text (widget, gamma_type_tooltip);
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "HD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "SD");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE1");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE2");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "FILMLIKE3");
		rcp->gamma_type_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (gamma_type_changed), rcp);
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

