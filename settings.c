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

#include "version.h"

#include "settings.h"

#include "protocol.h"
#include "misc.h"
#include "error.h"
#include "update_notification.h"
#include "operating_system.h"

#include "gain.h"
#include "shutter.h"

#include "cameras_set.h"
#include "scenes.h"
#include "sw_p_08.h"
#include "tally.h"
#include "physical_rcp.h"

#include "main_window.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>


#ifdef _WIN32
extern GdkPixbuf *pixbuf_grille_1_light, *pixbuf_grille_2_light, *pixbuf_grille_3_light, *pixbuf_grille_4_light;
extern GdkPixbuf *pixbuf_grille_1_dark, *pixbuf_grille_2_dark, *pixbuf_grille_3_dark, *pixbuf_grille_4_dark;

extern GdkPixbuf *pixbuf_logo_light;
extern GdkPixbuf *pixbuf_logo_dark;
#endif


char *config_file_name = "RCP-Virtuels-" RCP_VIRTUELS_VERSION ".dat";
FILE *config_file = NULL;

gboolean backup_needed = FALSE;


parameter_t white_balance_mode_parameters_array[6] = { { "0", "ATW" }, { "1", "AWB A" }, { "2", "AWB B" }, { "4", "3200K" }, { "5", "5600K" }, { "9", "VAR" } };
char *white_balance_mode_answers_array[6] = { "0", "1", "2", "4", "5", "9" };

char white_balance_mode_tooltip[] = "ATW: La balance des blancs est compensée automatiquement en continu, même si la\n       source de lumière ou la température de couleur changent.\nAWB A: Lorsque [AWB A] ou [AWB B] est sélectionné et que la balance des blancs est\n       effectuée, les résultats du réglage sont stockés dans la mémoire sélectionnée.\n       Lorsque [AWB A] ou [AWB B] est sélectionné par la suite, la balance des blancs\n       sauvegardée dans la mémoire sélectionnée peut être rappelée.\n3200K: C’est le mode de balance des blancs idéal quand la source de lumière est un\n       éclairage à halogène à 3200 K.\n5600K: C’est le mode de balance des blancs idéal quand la source de lumière est un\n       éclairage fluorescent ou la lumière du soleil à 5600 K.\nVAR: Vous pouvez définir une température de couleur comprise entre 2000 K et\n       15000 K. C'est le seul mode utilisable avec ce programme.";
//char white_balance_mode_tooltip[] = "ATW: La balance des blancs est compensée automatiquement en continu,\n       même si la source de lumière ou la température de couleur changent.\nAWB A/B: Lorsque [AWB A] ou [AWB B] est sélectionné et que la balance\n       des blancs est effectuée, les résultats du réglage sont stockés dans la\n       mémoire sélectionnée (A ou B).\n       Lorsque [AWB A] ou [AWB B] est sélectionné par la suite, la balance des\n       blancs sauvegardée dans la mémoire sélectionnée est rappelée.\n3200K: C’est le mode de balance des blancs idéal quand la source de lumière\n       est un éclairage à halogène à 3200 K.\n5600K: C’est le mode de balance des blancs idéal quand la source de lumière\n       est un éclairage fluorescent ou la lumière du soleil à 5600 K.\nVAR: Vous pouvez définir une température de couleur comprise entre 2000 K et\n       15000 K. C'est le seul mode utilisable avec ce programme.";

parameter_t preset_scope_parameters_array[3] = { { "0", "Mode A" }, { "1", "Mode B" }, { "2", "Mode C" } };
char *preset_scope_answers_array[3] = { "0", "1", "2" };

char preset_scope_tooltip[] = "Etendue des modifications lorsqu'une mémoire Panasonic est rappelée:\nMode A: Pan, Tilt, Zoom (y compris zoom numérique), Focus, Iris, Gain, valeur de réglage de la balance des blancs.\nMode B: Pan, Tilt, Zoom (y compris zoom numérique), Focus, Iris.\nMode C: Pan, Tilt, Zoom (y compris zoom numérique), Focus.";

parameter_t white_clip_parameters_array[2] = { { "0", "Off" }, { "1", "On" } };
char *white_clip_answers_array[2] = { "0", "1" };

parameter_t white_clip_level_parameters_array[20] = { { "13", "109%" }, { "12", "108%" }, { "11", "107%" }, { "10", "106%" }, { "0F", "105%" }, { "0E", "104%" }, \
	{ "0D", "103%" }, { "0C", "102%" }, { "0B", "101%" }, { "0A", "100%" }, { "09", "99%" }, { "08", "98%" }, { "07", "97%" }, { "06", "96%" }, \
	{ "05", "95%" }, { "04", "94%" }, { "03", "93%" }, { "02", "92%" }, { "01", "91%" }, { "00", "90%" } };
char *white_clip_level_answers_array[20] = { "13", "12", "11", "10", "0F", "0E", "0D", "0C", "0B", "0A", "09", "08", "07", "06", "05", "04", "03", "02", "01", "00" };

parameter_t agc_max_gain_parameters_array[3] = { { "03", "18 dB" }, { "02", "12 dB" }, { "01", "6 dB" } };
char *agc_max_gain_answers_array[3] = { "03", "02", "01" };

char agc_max_gain_tooltip[] = "Quantité maximum d’augmentation du gain lorsque\nle paramètre [Gain] des caméras est réglé sur [Auto].";

parameter_t frame_mix_parameters_array[5] = { { "00", "Off" }, { "06", "6 dB" }, { "0C", "12 dB" }, { "12", "18 dB" }, { "18", "24 dB" } };
char *frame_mix_answers_array[5] = { "00", "06", "0C", "12", "18" };

char frame_mix_tooltip[] = "Sélectionner la quantité d’adjonction d’images (augmentation du gain grâce à la mémoire de capteur).\nQuand l’adjonction d’images est réalisée, la vidéo apparaît comme s’il manquait quelques images.\nSon réglage prend effet si le réglage sélectionné à [Shutter Mode] est [Off].\nCeci ne peut pas être configuré lorsque le format est 2160/29.97p, 2160/23.98p, 2160/24p, 2160/25p, 1080/29.97p, 1080/23.98p, 1080/24p, 1080/29.97PsF, 1080/23.98PsF, 1080/25p ou 1080/25PsF.\n[Frame Mix] ne peut pas être réglé sur [18dB] ou [24dB] lorsque [Iris Mode], [Gain] ou [Focus Mode] est réglé sur [Auto].\n(Avec [Frame Mix] sur [18dB] ou [24dB], régler [Iris Mode] et [Focus Mode] sur [Manual] et régler [Gain] sur un autre réglage que [Auto].)";

parameter_t dnr_parameters_array[3] = { { "00", "Off" }, { "01", "Low" }, { "02", "High" } };
char *dnr_answers_array[3] = { "00", "01", "02" };

char dnr_tooltip[] = "La fonction \"réduction du bruit numérique (Digital Noise Reduction)\"\npermet de pouvoir sortir des images lumineuses, claires et sans bruit,\nmême la nuit et dans des conditions de faible éclairage.\nNéanmoins, un décalage d'image peut se produire.";

parameter_t osd_off_with_tally_parameters_array[2] = { { "0", "Off" }, { "1", "On" } };
char *osd_off_with_tally_answers_array[2] = { "0", "1" };

char osd_off_with_tally_tooltip[] = "Cette option permet de désactiver l'affichage du menu de la caméra (OSD)\nlorsque des signaux tally sont reçus via des commandes ou des contacts.\nQuand le signal tally est libéré, l'affichage du menu de la caméra revient.";

parameter_t osd_status_parameters_array[2] = { { "0", "Off" }, { "1", "On" } };
char *osd_status_answers_array[2] = { "0", "1" };

char osd_status_tooltip[] = "Cette option permet d'activer/désactiver l'affichage d'état\npendant la balance automatique des blancs (AWB) et des noirs (ABB),\nou l'affichage de l'erreur si une erreur se produit.";

parameter_t tally_brightness_parameters_array[3] = { { "2", "High" }, { "1", "Mid" }, { "0", "Low" } };
char *tally_brightness_answers_array[3] = { "2", "1", "0" };

setting_t settings_array[NB_SETTINGS] = { \
	{ "White Balance Mode :", "OAW:", "QAW", 6, white_balance_mode_parameters_array, white_balance_mode_answers_array, NULL, white_balance_mode_tooltip, 5, FALSE }, \
	{ "Preset Scope :", "OSE:71:", "QSE:71", 3, preset_scope_parameters_array, preset_scope_answers_array, NULL, preset_scope_tooltip, 2, TRUE }, \
	{ "White Clip :", "OSA:2E:", "QSA:2E", 2, white_clip_parameters_array, white_clip_answers_array, NULL, NULL, 1, TRUE }, \
	{ "White Clip Level :", "OSA:2A:", "QSA:2A", 20, white_clip_level_parameters_array, white_clip_level_answers_array, NULL, NULL, 9, TRUE }, \
	{ "AGC Max Gain :", "OSD:69:", "QSD:69", 3, agc_max_gain_parameters_array, agc_max_gain_answers_array, NULL, agc_max_gain_tooltip, 0, TRUE }, \
	{ "Frame Mix :", "OSA:65:", "QSA:65", 5, frame_mix_parameters_array, frame_mix_answers_array, NULL, frame_mix_tooltip, 0, FALSE }, \
	{ "Digital Noise Reduction (DNR) :", "OSD:3A:", "QSD:3A", 3, dnr_parameters_array, dnr_answers_array, NULL, dnr_tooltip, 0, FALSE }, \
	{ "OSD Off With TALLY :", "OSE:75:", "QSE:75", 2, osd_off_with_tally_parameters_array, osd_off_with_tally_answers_array, NULL, osd_off_with_tally_tooltip, 1, TRUE }, \
	{ "OSD Status :", "OSA:88:", "QSA:88", 2, osd_status_parameters_array, osd_status_answers_array, NULL, osd_status_tooltip, 1, TRUE }, \
	{ "Tally Brightness :", "OSA:D3:", "QSA:D3", 3, tally_brightness_parameters_array, tally_brightness_answers_array, NULL, NULL, 2, TRUE } \
	};

int settings_parameters_indexes_array[NB_SETTINGS];

int status_lamp = 1;

int tally_input = 1;

gboolean send_ip_tally = FALSE;

int picture_level = 0;

char picture_level_tooltip[] = "Détermine la luminosité de l’image à atteindre avec la compensation automatique de l’exposition si [Iris Mode] est réglé sur [Auto] ou [Shutter Mode] est réglé sur [ELC] ou [Gain] est réglé sur [Auto].\nEn gros, c'est la luminosité quand on est en \"diaph auto\".";


//AW_HE130
parameter_t frequency_parameters_array_AW_HE130[2] = { { "0", "59,94Hz" }, { "1", "50Hz" } };
char *frequency_answers_array_AW_HE130[2] = { "0", "1" };

parameter_t format_parameters_array_ntsc_AW_HE130[8] = { { "01", "720/59.94p" }, { "04", "1080/59.94i" }, { "07", "1080/29.97PsF" }, { "0A", "1080/23.98PsF" }, \
	{ "10", "1080/59.94p" }, { "12", "480/59.94p(i)" }, { "14", "1080/29.97p" }, { "16", "1080/23.98p" } };
char *format_answers_array_ntsc_AW_HE130[8] = { "01", "04", "07", "0A", "10", "12", "14", "16" };

parameter_t format_parameters_array_pal_AW_HE130[6] = { { "02", "720/50p" }, { "05", "1080/50i" }, { "08", "1080/25PsF" }, { "11", "1080/50p" }, { "13", "576/50p(i)" }, \
	{ "15", "1080/25p" } };
char *format_answers_array_pal_AW_HE130[6] = { "02", "05", "08", "11", "13", "15" };

//char format_tooltip[] = "Dans le cas des formats 576/50p(i) et 480/59.94p(i),\nle signal progressif (p) est envoyé vers le connecteur HDMI et\nle signal entrelacé (i) est envoyé vers les connecteurs SDI et \"vidéo composite\".";
char format_tooltip[] = "Dans le cas des formats 576/50p(i) et 480/59.94p(i), le signal progressif (p) est envoyé vers le connecteur HDMI et le signal entrelacé (i) est envoyé vers les connecteurs SDI et \"vidéo composite\".";

parameter_t down_conversion_mode_parameters_array[3] = { { "0", "SideCut" }, { "1", "Squeeze" }, { "2", "LetterBOX" } };
char *down_conversion_mode_answers_array[3] = { "0", "1", "2" };

parameter_t color_bar_setup_level_parameters_array[2] = { { "0", "0.0 IRE" }, { "1", "7.5 IRE" } };
char *color_bar_setup_level_answers_array[2] = { "0", "1" };

char color_bar_setup_level_tooltip[] = "Cette option est activée uniquement lorsque [Frequency] est réglé sur [59,94Hz]. Elle permet de sélectionner le niveau de la mire de couleur sur la sortie \"vidéo composite\".";

#define COLOR_BAR_SETUP_LEVEL_AW_HE130_COMBO_BOX settings_array_AW_HE130[COLOR_BAR_SETUP_LEVEL_INDEX_AW_HE130].combo_box

setting_t settings_array_AW_HE130[NB_SETTINGS_AW_HE130] = { \
	{ "Frequency :", "OSE:77:", "QSE:77", 2, frequency_parameters_array_AW_HE130, frequency_answers_array_AW_HE130, NULL, NULL, 1, TRUE }, \
	{ "Format :", "OSA:87:", "QSA:87", 6, format_parameters_array_pal_AW_HE130, format_answers_array_pal_AW_HE130, NULL, format_tooltip, 1, TRUE }, \
	{ "Down-conversion Mode :", "OSE:20:", "QSE:20", 3, down_conversion_mode_parameters_array, down_conversion_mode_answers_array, NULL, NULL, 1, TRUE }, \
	{ "Color Bar Setup Level :", "DCS:", "QCS", 2, color_bar_setup_level_parameters_array, color_bar_setup_level_answers_array, NULL, color_bar_setup_level_tooltip, 0, TRUE } \
	};

int settings_parameters_indexes_array_AW_HE130[NB_SETTINGS_AW_HE130];

int osd_mix_AW_HE130 = 0x1A;		//SDI Off, HDMI On, Video On, IP On


//AW_UE150
parameter_t frequency_parameters_array_AW_UE150[4] = { { "0", "59,94Hz" }, { "1", "50Hz" }, { "2", "24Hz" }, { "3", "23.98Hz" } };
char *frequency_answers_array_AW_UE150[4] = { "0", "1", "2", "3" };

parameter_t format_parameters_array_ntsc_AW_UE150[8] = { { "01", "720/59.94p" }, { "04", "1080/59.94i" }, { "07", "1080/29.97PsF" }, { "10", "1080/59.94p" }, \
	{ "14", "1080/29.97p" }, { "16", "1080/23.98p (over 59.94i/p)" }, { "17", "2160/29.97p" }, { "19", "2160/59.94p" } };
char *format_answers_array_ntsc_AW_UE150[8] = { "01", "04", "07", "10", "14", "16", "17", "19" };

parameter_t format_parameters_array_pal_AW_UE150[7] = { { "02", "720/50p" }, { "05", "1080/50i" }, { "08", "1080/25PsF" }, { "11", "1080/50p" }, { "15", "1080/25p" }, \
	{ "18", "2160/25p" }, { "1A", "2160/50p" } };
char *format_answers_array_pal_AW_UE150[7] = { "02", "05", "08", "11", "15", "18", "1A" };

parameter_t format_parameters_array_24p_AW_UE150[2] = { { "21", "2160/24p" }, { "22", "1080/24p" } };
char *format_answers_array_24p_AW_UE150[2] = { "21", "22" };

parameter_t format_parameters_array_23_98p_AW_UE150[3] = { { "1B", "2160/23.98p" }, { "23", "1080/23.98p" }, { "0A", "1080/23.98PsF" } };
char *format_answers_array_23_98p_AW_UE150[3] = { "1B", "23", "0A" };

parameter_t pedestal_offset_parameters_array[2] = { { "0", "Off" }, { "1", "On" } };
char *pedestal_offset_answers_array[2] = { "0", "1" };

parameter_t auto_iris_speed_parameters_array[3] = { { "0", "Slow" }, { "1", "Normal" }, { "2", "Fast" } };
char *auto_iris_speed_answers_array[3] = { "0", "1", "2" };

parameter_t auto_iris_window_parameters_array[3] = { { "0", "Normal1" }, { "1", "Normal2" }, { "2", "Center" } };
char *auto_iris_window_answers_array[3] = { "0", "1", "2" };

char auto_iris_window_tooltip[] = "Sélectionne la fenêtre de détection du diaphragme auto :\nNormal1 : Fenêtre vers le centre de l'écran.\nNormal2 : Fenêtre vers le bas de l'écran.\nCenter : Fenêtre en pointillé au centre de l'écran.";

parameter_t super_gain_parameters_array[2] = { { "0", "Off" }, { "1", "On" } };
char *super_gain_answers_array[2] = { "0", "1" };

char super_gain_tooltip[] = "Permet d'augmenter le gain jusqu'à 42dB (au lieu de 36dB).";

parameter_t shooting_mode_parameters_array[2] = { { "0", "Normal" }, { "1", "High Sensitivity" } };
char *shooting_mode_answers_array[2] = { "0", "1" };

char shooting_mode_tooltip[] = "Sélectionne le mode de prise de vues en fonction de l'environnement ambiant :\nNormal : Prises de vues dans un endroit où le niveau de luminosité est normal.\nHigh Sensitivity : Prises de vues dans un environnement sombre.";

parameter_t color_setting_parameters_array[2] = { { "0", "Normal" }, { "1", "V-Log" } };
char *color_setting_answers_array[2] = { "0", "1" };

char color_setting_tooltip[] = "Définissez la couleur (gamma et gamut) pour le système dans son ensemble :\nNormal : Ce paramètre permet des réglages détaillés de la qualité de l'image sur la caméra.\nV-Log : Définissez une courbe gamma qui fournit des tons et une large plage de latitude (plage d'exposition).";

//12G SDI/OPTICAL
parameter_t sdi_12g_4k_59_94p_format_parameters_array[2] = { { "10", "1080/59.94p" }, { "19", "2160/59.94p" } };
char *sdi_12g_4k_59_94p_format_answers_array[2] = { "10", "19" };

parameter_t sdi_12g_4k_29_97p_format_parameters_array[3] = { { "07", "1080/29.97PsF" }, { "14", "1080/29.97p" }, { "17", "2160/29.97p" } };
char *sdi_12g_4k_29_97p_format_answers_array[3] = { "07", "14", "17" };

parameter_t sdi_12g_1080_59_94p_format_parameters_array[1] = { { "10", "1080/59.94p" } };
char *sdi_12g_1080_59_94p_format_answers_array[1] = { "10" };

parameter_t sdi_12g_1080_59_94i_format_parameters_array[1] = { { "04", "1080/59.94i" } };
char *sdi_12g_1080_59_94i_format_answers_array[1] = { "04" };

parameter_t sdi_12g_1080_29_97p_format_parameters_array[1] = { { "14", "1080/29.97p" } };
char *sdi_12g_1080_29_97p_format_answers_array[1] = { "14" };

parameter_t sdi_12g_1080_29_97psf_format_parameters_array[1] = { { "07", "1080/29.97PsF" } };
char *sdi_12g_1080_29_97psf_format_answers_array[1] = { "07" };

parameter_t sdi_12g_1080_23_98p_over_59_94i_format_parameters_array[1] = { { "16", "1080/23.98p(over 59.94i/p)" } };
char *sdi_12g_1080_23_98p_over_59_94i_format_answers_array[1] = { "16" };

parameter_t sdi_12g_720_59_94p_format_parameters_array[1] = { { "01", "720/59.94p" } };
char *sdi_12g_720_59_94p_format_answers_array[1] = { "01" };

parameter_t sdi_12g_4k_50p_format_parameters_array[2] = { { "11", "1080/50p" }, { "1A", "2160/50p" } };
char *sdi_12g_4k_50p_format_answers_array[2] = { "11", "1A" };

parameter_t sdi_12g_4k_25p_format_parameters_array[3] = { { "08", "1080/25PsF" }, { "15", "1080/25p" }, { "18", "2160/25p" } };
char *sdi_12g_4k_25p_format_answers_array[3] = { "08", "15", "18" };

parameter_t sdi_12g_1080_50p_format_parameters_array[1] = { { "11", "1080/50p" } };
char *sdi_12g_1080_50p_format_answers_array[1] = { "11" };

parameter_t sdi_12g_1080_50i_format_parameters_array[1] = { { "05", "1080/50i" } };
char *sdi_12g_1080_50i_format_answers_array[1] = { "05" };

parameter_t sdi_12g_1080_25p_format_parameters_array[1] = { { "15", "1080/25p" } };
char *sdi_12g_1080_25p_format_answers_array[1] = { "15" };

parameter_t sdi_12g_1080_25psf_format_parameters_array[1] = { { "08", "1080/25PsF" } };
char *sdi_12g_1080_25psf_format_answers_array[1] = { "08" };

parameter_t sdi_12g_720_50p_format_parameters_array[1] = { { "02", "720/50p" } };
char *sdi_12g_720_50p_format_answers_array[1] = { "02" };

parameter_t sdi_12g_4k_24p_format_parameters_array[2] = { { "21", "2160/24p" }, { "22", "1080/24p" } };
char *sdi_12g_4k_24p_format_answers_array[2] = { "21", "22" };

parameter_t sdi_12g_1080_24p_format_parameters_array[1] = { { "22", "1080/24p" } };
char *sdi_12g_1080_24p_format_answers_array[1] = { "22" };

parameter_t sdi_12g_4k_23_98p_format_parameters_array[3] = { { "1B", "2160/23.98p" }, { "23", "1080/23.98p" }, { "0A", "1080/23.98PsF" } };
char *sdi_12g_4k_23_98p_format_answers_array[3] = { "1B", "23", "0A" };

parameter_t sdi_12g_1080_23_98p_format_parameters_array[1] = { { "23", "1080/23.98p" } };
char *sdi_12g_1080_23_98p_format_answers_array[1] = { "23" };

parameter_t sdi_12g_1080_23_98psf_format_parameters_array[1] = { { "0A", "1080/23.98PsF" } };
char *sdi_12g_1080_23_98psf_format_answers_array[1] = { "0A" };

parameter_t hdr_output_select_parameters_array[3] = { { "0", "SDR" }, { "1", "HDR (2020)" }, { "2", "HDR (709)" } };
char *hdr_output_select_answers_array[3] = { "0", "1", "2" };

char hdr_output_select_tooltip[] = "Select the signal to be output when [Gamma Mode] is [HLG] :\nSDR : SDR output.\nHDR(2020) : HDR output (BT.2020 equivalent color gamut).\nHDR(709) : HDR output (BT.709 equivalent color gamut).";

parameter_t v_log_output_select_parameters_array[2] = { { "0", "V-Log" }, { "1", "V-709" } };
char *v_log_output_select_answers_array[2] = { "0", "1" };

parameter_t sdi_3g_out_parameters_array[2] = { { "0", "Level A" }, { "1", "Level B" } };
char *sdi_3g_out_answers_array[2] = { "0", "1" };

//3G SDI
parameter_t sdi_3g_4k_59_94p_format_parameters_array[2] = { { "04", "1080/59.94i" }, { "10", "1080/59.94p" } };
char *sdi_3g_4k_59_94p_format_answers_array[2] = { "04", "10" };

parameter_t sdi_3g_4k_29_97p_format_parameters_array[2] = { { "07", "1080/29.97PsF" }, { "14", "1080/29.97p" } };
char *sdi_3g_4k_29_97p_format_answers_array[2] = { "07", "14" };

parameter_t sdi_3g_1080_59_94p_format_parameters_array[2] = { { "04", "1080/59.94i" }, { "10", "1080/59.94p" } };
char *sdi_3g_1080_59_94p_format_answers_array[2] = { "04", "10" };

parameter_t sdi_3g_1080_59_94i_format_parameters_array[1] = { { "04", "1080/59.94i" } };
char *sdi_3g_1080_59_94i_format_answers_array[1] = { "04" };

parameter_t sdi_3g_1080_29_97p_format_parameters_array[1] = { { "14", "1080/29.97p" } };
char *sdi_3g_1080_29_97p_format_answers_array[1] = { "14" };

parameter_t sdi_3g_1080_29_97psf_format_parameters_array[1] = { { "07", "1080/29.97PsF" } };
char *sdi_3g_1080_29_97psf_format_answers_array[1] = { "07" };

parameter_t sdi_3g_1080_23_98p_over_59_94i_format_parameters_array[1] = { { "16", "1080/23.98p (over 59.94i/p)" } };
char *sdi_3g_1080_23_98p_over_59_94i_format_answers_array[1] = { "16" };

parameter_t sdi_3g_720_59_94p_format_parameters_array[1] = { { "01", "720/59.94p" } };
char *sdi_3g_720_59_94p_format_answers_array[1] = { "01" };

parameter_t sdi_3g_4k_50p_format_parameters_array[2] = { { "05", "1080/50i" }, { "11", "1080/50p" } };
char *sdi_3g_4k_50p_format_answers_array[2] = { "05", "11" };

parameter_t sdi_3g_4k_25p_format_parameters_array[2] = { { "08", "1080/25PsF" }, { "15", "1080/25p" } };
char *sdi_3g_4k_25p_format_answers_array[2] = { "08", "15" };

parameter_t sdi_3g_1080_50p_format_parameters_array[2] = { { "05", "1080/50i" }, { "11", "1080/50p" } };
char *sdi_3g_1080_50p_format_answers_array[2] = { "05", "11" };

parameter_t sdi_3g_1080_50i_format_parameters_array[1] = { { "05", "1080/50i" } };
char *sdi_3g_1080_50i_format_answers_array[1] = { "05" };

parameter_t sdi_3g_1080_25p_format_parameters_array[1] = { { "15", "1080/25p" } };
char *sdi_3g_1080_25p_format_answers_array[1] = { "15" };

parameter_t sdi_3g_1080_25psf_format_parameters_array[1] = { { "08", "1080/25PsF" } };
char *sdi_3g_1080_25psf_format_answers_array[1] = { "08" };

parameter_t sdi_3g_720_50p_format_parameters_array[1] = { { "02", "720/50p" } };
char *sdi_3g_720_50p_format_answers_array[1] = { "02" };

parameter_t sdi_3g_24p_format_parameters_array[1] = { { "22", "1080/24p" } };
char *sdi_3g_24p_format_answers_array[1] = { "22" };

parameter_t sdi_3g_4k_23_98p_format_parameters_array[2] = { { "23", "1080/23.98p" }, { "0A", "1080/23.98PsF" } };
char *sdi_3g_4k_23_98p_format_answers_array[2] = { "23", "0A" };

parameter_t sdi_3g_1080_23_98p_format_parameters_array[1] = { { "23", "1080/23.98p" } };
char *sdi_3g_1080_23_98p_format_answers_array[1] = { "23" };

parameter_t sdi_3g_1080_23_98psf_format_parameters_array[1] = { { "0A", "1080/23.98PsF" } };
char *sdi_3g_1080_23_98psf_format_answers_array[1] = { "0A" };

//MONI
parameter_t moni_4k_59_94p_format_parameters_array[1] = { { "04", "1080/59.94i" }/*, { "10", "1080/59.94p" }*/ };
char *moni_4k_59_94p_format_answers_array[1] = { "04"/*, "10"*/ };

parameter_t moni_4k_29_97p_format_parameters_array[2] = { { "07", "1080/29.97PsF" }, { "14", "1080/29.97p" } };
char *moni_4k_29_97p_format_answers_array[2] = { "07", "14" };

parameter_t moni_1080_59_94p_format_parameters_array[1] = { { "04", "1080/59.94i" }/*, { "10", "1080/59.94p" }*/ };
char *moni_1080_59_94p_format_answers_array[1] = { "04"/*, "10"*/ };

parameter_t moni_1080_59_94i_format_parameters_array[1] = { { "04", "1080/59.94i" } };
char *moni_1080_59_94i_format_answers_array[1] = { "04" };

parameter_t moni_1080_29_97p_format_parameters_array[1] = { { "14", "1080/29.97p" } };
char *moni_1080_29_97p_format_answers_array[1] = { "14" };

parameter_t moni_1080_29_97psf_format_parameters_array[1] = { { "07", "1080/29.97PsF" } };
char *moni_1080_29_97psf_format_answers_array[1] = { "07" };

parameter_t moni_1080_23_98p_over_59_94i_format_parameters_array[1] = { { "16", "1080/23.98p (over 59.94i/p)" } };
char *moni_1080_23_98p_over_59_94i_format_answers_array[1] = { "16" };

parameter_t moni_720_59_94p_format_parameters_array[1] = { { "01", "720/59.94p" } };
char *moni_720_59_94p_format_answers_array[1] = { "01" };

parameter_t moni_4k_50p_format_parameters_array[1] = { { "05", "1080/50i" }/*, { "11", "1080/50p" }*/ };
char *moni_4k_50p_format_answers_array[1] = { "05"/*, "11"*/ };

parameter_t moni_4k_25p_format_parameters_array[2] = { { "08", "1080/25PsF" }, { "15", "1080/25p" } };
char *moni_4k_25p_format_answers_array[2] = { "08", "15" };

parameter_t moni_1080_50p_format_parameters_array[1] = { { "05", "1080/50i" }/*, { "11", "1080/50p" }*/ };
char *moni_1080_50p_format_answers_array[1] = { "05"/*, "11"*/ };

parameter_t moni_1080_50i_format_parameters_array[1] = { { "05", "1080/50i" } };
char *moni_1080_50i_format_answers_array[1] = { "05" };

parameter_t moni_1080_25p_format_parameters_array[1] = { { "15", "1080/25p" } };
char *moni_1080_25p_format_answers_array[1] = { "15" };

parameter_t moni_1080_25psf_format_parameters_array[1] = { { "08", "1080/25PsF" } };
char *moni_1080_25psf_format_answers_array[1] = { "08" };

parameter_t moni_720_50p_format_parameters_array[1] = { { "02", "720/50p" } };
char *moni_720_50p_format_answers_array[1] = { "02" };

parameter_t moni_24p_format_parameters_array[1] = { { "22", "1080/24p" } };
char *moni_24p_format_answers_array[1] = { "22" };

parameter_t moni_4k_23_98p_format_parameters_array[2] = { { "23", "1080/23.98p" }, { "0A", "1080/23.98PsF" } };
char *moni_4k_23_98p_format_answers_array[2] = { "23", "0A" };

parameter_t moni_1080_23_98p_format_parameters_array[1] = { { "23", "1080/23.98p" } };
char *moni_1080_23_98p_format_answers_array[1] = { "23" };

parameter_t moni_1080_23_98psf_format_parameters_array[1] = { { "0A", "1080/23.98PsF" } };
char *moni_1080_23_98psf_format_answers_array[1] = { "0A" };

//HDMI
parameter_t hdmi_4k_59_94p_format_parameters_array[2] = { { "10", "1080/59.94p" }, { "19", "2160/59.94p" } };
char *hdmi_4k_59_94p_format_answers_array[2] = { "10", "19" };

parameter_t hdmi_4k_29_97p_format_parameters_array[2] = { { "14", "1080/29.97p" }, { "17", "2160/29.97p" } };
char *hdmi_4k_29_97p_format_answers_array[2] = { "14", "17" };

parameter_t hdmi_1080_59_94p_format_parameters_array[1] = { { "10", "1080/59.94p" } };
char *hdmi_1080_59_94p_format_answers_array[1] = { "10" };

parameter_t hdmi_1080_59_94i_format_parameters_array[1] = { { "04", "1080/59.94i" } };
char *hdmi_1080_59_94i_format_answers_array[1] = { "04" };

parameter_t hdmi_1080_29_97p_format_parameters_array[1] = { { "14", "1080/29.97p" } };
char *hdmi_1080_29_97p_format_answers_array[1] = { "14" };

parameter_t hdmi_1080_29_97psf_format_parameters_array[1] = { { "14", "1080/29.97p" } };
char *hdmi_1080_29_97psf_format_answers_array[1] = { "14" };

parameter_t hdmi_1080_23_98p_over_59_94i_format_parameters_array[1] = { { "16", "1080/23.98p(over 59.94i/p)" } };
char *hdmi_1080_23_98p_over_59_94i_format_answers_array[1] = { "16" };

parameter_t hdmi_720_59_94p_format_parameters_array[1] = { { "01", "720/59.94p" } };
char *hdmi_720_59_94p_format_answers_array[1] = { "01" };

parameter_t hdmi_4k_50p_format_parameters_array[2] = { { "11", "1080/50p" }, { "1A", "2160/50p" } };
char *hdmi_4k_50p_format_answers_array[2] = { "11", "1A" };

parameter_t hdmi_4k_25p_format_parameters_array[2] = { { "15", "1080/25p" }, { "18", "2160/25p" } };
char *hdmi_4k_25p_format_answers_array[2] = { "15", "18" };

parameter_t hdmi_1080_50p_format_parameters_array[1] = { { "11", "1080/50p" } };
char *hdmi_1080_50p_format_answers_array[1] = { "11" };

parameter_t hdmi_1080_50i_format_parameters_array[1] = { { "05", "1080/50i" } };
char *hdmi_1080_50i_format_answers_array[1] = { "05" };

parameter_t hdmi_1080_25p_format_parameters_array[1] = { { "15", "1080/25p" } };
char *hdmi_1080_25p_format_answers_array[1] = { "15" };

parameter_t hdmi_1080_25psf_format_parameters_array[1] = { { "15", "1080/25p" } };
char *hdmi_1080_25psf_format_answers_array[1] = { "15" };

parameter_t hdmi_720_50p_format_parameters_array[1] = { { "02", "720/50p" } };
char *hdmi_720_50p_format_answers_array[1] = { "02" };

parameter_t hdmi_4k_24p_format_parameters_array[2] = { { "21", "2160/24p" }, { "22", "1080/24p" } };
char *hdmi_4k_24p_format_answers_array[2] = { "21", "22" };

parameter_t hdmi_1080_24p_format_parameters_array[1] = { { "22", "1080/24p" } };
char *hdmi_1080_24p_format_answers_array[1] = { "22" };

parameter_t hdmi_4k_23_98p_format_parameters_array[2] = { { "1B", "2160/23.98p" }, { "23", "1080/23.98p" } };
char *hdmi_4k_23_98p_format_answers_array[2] = { "1B", "23" };

parameter_t hdmi_1080_23_98p_format_parameters_array[1] = { { "23", "1080/23.98p" } };
char *hdmi_1080_23_98p_format_answers_array[1] = { "23" };

parameter_t hdmi_1080_23_98psf_format_parameters_array[1] = { { "23", "1080/23.98p" } };
char *hdmi_1080_23_98psf_format_answers_array[1] = { "23" };

parameter_t hdmi_color_component_parameters_array[2] = { { "2", "4:2:2/10bit" }, { "4", "4:2:0/8bit" } };
char *hdmi_color_component_answers_array[2] = { "2", "4" };

setting_t settings_array_AW_UE150[NB_SETTINGS_AW_UE150] = { \
	{ "Frequency :", "OSE:77:", "QSE:77", 4, frequency_parameters_array_AW_UE150, frequency_answers_array_AW_UE150, NULL, NULL, 1, TRUE }, \
	{ "Format :", "OSA:87:", "QSA:87", 7, format_parameters_array_pal_AW_UE150, format_answers_array_pal_AW_UE150, NULL, NULL, 1, TRUE }, \
	{ "Pedestal Offset :", "OSJ:11:", "QSJ:11", 2, pedestal_offset_parameters_array, pedestal_offset_answers_array, NULL, NULL, 1, TRUE }, \
	{ "Auto Iris Speed :", "OSJ:01:", "QSJ:01", 3, auto_iris_speed_parameters_array, auto_iris_speed_answers_array, NULL, NULL, 1, TRUE }, \
	{ "Auto Iris Window :", "OSJ:02:", "QSJ:02", 3, auto_iris_window_parameters_array, auto_iris_window_answers_array, NULL, auto_iris_window_tooltip, 0, TRUE }, \
	{ "Super Gain :", "OSI:28:", "QSI:28", 2, super_gain_parameters_array, super_gain_answers_array, NULL, super_gain_tooltip, 0, TRUE }, \
	{ "Shooting Mode :", "OSI:30:", "QSI:30", 2, shooting_mode_parameters_array, shooting_mode_answers_array, NULL, shooting_mode_tooltip, 0, TRUE }, \
	{ "Color Setting :", "OSJ:56:", "QSJ:56", 2, color_setting_parameters_array, color_setting_answers_array, NULL, color_setting_tooltip, 0, FALSE }, \
/*12G SDI/OPTICAL*/ \
	{ "Format :", "OSJ:1E:", "QSJ:1E", 1, sdi_12g_1080_50i_format_parameters_array, sdi_12g_1080_50i_format_answers_array, NULL, NULL, 0, TRUE }, \
	{ "HDR Output Select :", "OSJ:1F:", "QSJ:1F", 3, hdr_output_select_parameters_array, hdr_output_select_answers_array, NULL, hdr_output_select_tooltip, 2, FALSE }, \
	{ "V-Log Output Select :", "OSJ:57:", "QSJ:57", 2, v_log_output_select_parameters_array, v_log_output_select_answers_array, NULL, NULL, 1, FALSE }, \
	{ "3G SDI Out :", "OSJ:20:", "QSJ:20", 2, sdi_3g_out_parameters_array, sdi_3g_out_answers_array, NULL, NULL, 0, FALSE }, \
/*3G SDI*/ \
	{ "Format :", "OSJ:21:", "QSJ:21", 1, sdi_3g_1080_50i_format_parameters_array, sdi_3g_1080_50i_format_answers_array, NULL, NULL, 0, TRUE }, \
	{ "HDR Output Select :", "OSJ:22:", "QSJ:22", 3, hdr_output_select_parameters_array, hdr_output_select_answers_array, NULL, hdr_output_select_tooltip, 2, FALSE }, \
	{ "V-Log Output Select :", "OSJ:58:", "QSJ:58", 2, v_log_output_select_parameters_array, v_log_output_select_answers_array, NULL, NULL, 1, FALSE }, \
	{ "3G SDI Out", "OSI:29:", "QSI:29", 2, sdi_3g_out_parameters_array, sdi_3g_out_answers_array, NULL, NULL, 0, FALSE }, \
/*MONI*/ \
	{ "Format :", "OSJ:23:", "QSJ:23", 1, moni_1080_50i_format_parameters_array, moni_1080_50i_format_answers_array, NULL, NULL, 0, TRUE }, \
	{ "HDR Output Select :", "OSJ:24:", "QSJ:24", 3, hdr_output_select_parameters_array, hdr_output_select_answers_array, NULL, hdr_output_select_tooltip, 2, FALSE }, \
	{ "V-Log Output Select :", "OSJ:59:", "QSJ:59", 2, v_log_output_select_parameters_array, v_log_output_select_answers_array, NULL, NULL, 1, FALSE }, \
/*HDMI*/ \
	{ "Format :", "OSJ:25:", "QSJ:25", 1, hdmi_1080_50i_format_parameters_array, hdmi_1080_50i_format_answers_array, NULL, NULL, 0, TRUE }, \
	{ "HDR Output Select :", "OSJ:26:", "QSJ:26", 3, hdr_output_select_parameters_array, hdr_output_select_answers_array, NULL, hdr_output_select_tooltip, 2, FALSE }, \
	{ "V-Log Output Select :", "OSJ:5A:", "QSJ:5A", 2, v_log_output_select_parameters_array, v_log_output_select_answers_array, NULL, NULL, 1, FALSE }, \
	{ "Video Sampling :", "OSE:68:", "QSE:68", 2, hdmi_color_component_parameters_array, hdmi_color_component_answers_array,  NULL, NULL, 0, FALSE }, \
	};

int settings_parameters_indexes_array_AW_UE150[NB_SETTINGS_AW_UE150];

int osd_mix_AW_UE150 = 0x52;		//3G SDI Off, HDMI On, IP On, 12G SDI/OPTICAL Off, MONI On


gboolean theme = FALSE;

gboolean show_master_rcp = TRUE;

fps_t output_fps_AW_HE130 = fps_50i;
fps_t output_fps_AW_UE150 = fps_50i;

GList *rcp_glist = NULL;
GList *rcp_start_glist = NULL;
GList *ghost_rcp_glist = NULL;

//settings page
GtkWidget *settings_page;
GtkWidget *settings_list_box;
GtkWidget *settings_configuration_button;
GtkWidget *settings_new_button;
GtkWidget *settings_delete_button;

GtkWidget *picture_level_value_label;

GtkWidget *tcp_port_entry;
GtkWidget *rs_combo_box_text;
GtkWidget *ip_waiting_label;
GtkWidget *rs_ok_label;


#ifdef MAIN_SETTINGS_READ_ONLY
GtkWidget *status_lamp_combo_box_text;
GtkWidget *tally_input_combo_box_text;
GtkWidget *picture_level_scale;
GtkWidget *osd_mix_sdi_toggle_button_AW_HE130, *osd_mix_hdmi_toggle_button_AW_HE130, *osd_mix_video_toggle_button_AW_HE130, *osd_mix_ip_toggle_button_AW_HE130;
GtkWidget *osd_mix_3g_sdi_toggle_button_AW_UE150, *osd_mix_hdmi_toggle_button_AW_UE150, *osd_mix_ip_toggle_button_AW_UE150, *osd_mix_12g_sdi_optical_toggle_button_AW_UE150, *osd_mix_moni_toggle_button_AW_UE150;

gboolean camera_settings_checked = FALSE;
gboolean camera_settings_AW_HE130_checked = FALSE;
gboolean camera_settings_AW_UE150_checked = FALSE;


gboolean update_settings_array_combo_box_text (gpointer index)
{
	int i = GPOINTER_TO_INT(index);

	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box));

	if ((settings_parameters_indexes_array[i] >= 0) || (settings_parameters_indexes_array[i] < settings_array[i].nb_parameters)) {
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array[i].combo_box), settings_array[i].parameters[settings_parameters_indexes_array[i]].label);
		gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array[i].combo_box), 0);
	}

	return G_SOURCE_REMOVE;
}

gboolean update_settings_array_AW_HE130_combo_box_text (gpointer index)
{
	int i = GPOINTER_TO_INT(index);

	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_HE130[i].combo_box));

	if ((settings_parameters_indexes_array_AW_HE130[i] >= 0) || (settings_parameters_indexes_array_AW_HE130[i] < settings_array_AW_HE130[i].nb_parameters)) {
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_HE130[i].combo_box), settings_array_AW_HE130[i].parameters[settings_parameters_indexes_array_AW_HE130[i]].label);
		gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array_AW_HE130[i].combo_box), 0);
	}

	return G_SOURCE_REMOVE;
}

gboolean update_settings_array_AW_UE150_combo_box_text (gpointer index)
{
	int i = GPOINTER_TO_INT(index);

	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[i].combo_box));

	if ((settings_parameters_indexes_array_AW_UE150[i] >= 0) || (settings_parameters_indexes_array_AW_UE150[i] < settings_array_AW_UE150[i].nb_parameters)) {
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[i].combo_box), settings_array_AW_UE150[i].parameters[settings_parameters_indexes_array_AW_UE150[i]].label);
		gtk_combo_box_set_active (GTK_COMBO_BOX (settings_array_AW_UE150[i].combo_box), 0);
	}

	return G_SOURCE_REMOVE;
}

gboolean update_status_lamp_combo_box_text (void)
{
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (status_lamp_combo_box_text));

	if (status_lamp == 1) gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (status_lamp_combo_box_text), "Enable");
	else gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (status_lamp_combo_box_text), "Disable");

	gtk_combo_box_set_active (GTK_COMBO_BOX (status_lamp_combo_box_text), 0);

	return G_SOURCE_REMOVE;
}

gboolean update_tally_input_combo_box_text (void)
{
	gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (tally_input_combo_box_text));

	if (tally_input == 1) gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (tally_input_combo_box_text), "Enable");
	else gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (tally_input_combo_box_text), "Disable");

	gtk_combo_box_set_active (GTK_COMBO_BOX (tally_input_combo_box_text), 0);

	return G_SOURCE_REMOVE;
}

gboolean update_picture_level_scale (void)
{
	char label[8];

	gtk_range_set_value (GTK_RANGE (picture_level_scale), picture_level);

	if (picture_level == 0) gtk_label_set_text (GTK_LABEL (picture_level_value_label), "0");
	else {
		sprintf (label, "%+d", picture_level);
		gtk_label_set_text (GTK_LABEL (picture_level_value_label), label);
	}

	return G_SOURCE_REMOVE;
}

gboolean update_osd_mix_AW_HE130 (void)
{
	if (osd_mix_AW_HE130 & 0x01) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_sdi_toggle_button_AW_HE130), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_sdi_toggle_button_AW_HE130), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_sdi_toggle_button_AW_HE130), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_sdi_toggle_button_AW_HE130), FALSE);
	}

	if (osd_mix_AW_HE130 & 0x02) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_hdmi_toggle_button_AW_HE130), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_hdmi_toggle_button_AW_HE130), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_hdmi_toggle_button_AW_HE130), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_hdmi_toggle_button_AW_HE130), FALSE);
	}

	if (osd_mix_AW_HE130 & 0x08) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_video_toggle_button_AW_HE130), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_video_toggle_button_AW_HE130), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_video_toggle_button_AW_HE130), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_video_toggle_button_AW_HE130), FALSE);
	}

	if (osd_mix_AW_HE130 & 0x10) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_ip_toggle_button_AW_HE130), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_ip_toggle_button_AW_HE130), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_ip_toggle_button_AW_HE130), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_ip_toggle_button_AW_HE130), FALSE);
	}

	return G_SOURCE_REMOVE;
}

gboolean update_osd_mix_AW_UE150 (void)
{
	if (osd_mix_AW_UE150 & 0x01) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_3g_sdi_toggle_button_AW_UE150), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_3g_sdi_toggle_button_AW_UE150), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_3g_sdi_toggle_button_AW_UE150), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_3g_sdi_toggle_button_AW_UE150), FALSE);
	}

	if (osd_mix_AW_UE150 & 0x02) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_hdmi_toggle_button_AW_UE150), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_hdmi_toggle_button_AW_UE150), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_hdmi_toggle_button_AW_UE150), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_hdmi_toggle_button_AW_UE150), FALSE);
	}

	if (osd_mix_AW_UE150 & 0x10) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_ip_toggle_button_AW_UE150), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_ip_toggle_button_AW_UE150), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_ip_toggle_button_AW_UE150), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_ip_toggle_button_AW_UE150), FALSE);
	}

	if (osd_mix_AW_UE150 & 0x20) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_12g_sdi_optical_toggle_button_AW_UE150), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_12g_sdi_optical_toggle_button_AW_UE150), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_12g_sdi_optical_toggle_button_AW_UE150), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_12g_sdi_optical_toggle_button_AW_UE150), FALSE);
	}

	if (osd_mix_AW_UE150 & 0x40) {
		gtk_button_set_label (GTK_BUTTON (osd_mix_moni_toggle_button_AW_UE150), "On");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_moni_toggle_button_AW_UE150), TRUE);
	} else {
		gtk_button_set_label (GTK_BUTTON (osd_mix_moni_toggle_button_AW_UE150), "Off");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (osd_mix_moni_toggle_button_AW_UE150), FALSE);
	}

	return G_SOURCE_REMOVE;
}

#endif

void update_outputs_parameters_AW_UE150 (void)
{
	switch (settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]) {
		case 0:	//59,94Hz
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 8;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_ntsc_AW_UE150;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_ntsc_AW_UE150;

			switch (settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]) {
				case 0:	//720/59.94p
					output_fps_AW_UE150 = fps_59_94p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_720_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_720_59_94p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_720_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_720_59_94p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_720_59_94p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_720_59_94p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_720_59_94p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_720_59_94p_format_answers_array;
					break;
				case 1:	//1080/59.94i
					output_fps_AW_UE150 = fps_59_94i;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_59_94i_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_59_94i_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_59_94i_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_59_94i_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_59_94i_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_59_94i_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_59_94i_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_59_94i_format_answers_array;
					break;
				case 2:	//1080/29.97PsF
					output_fps_AW_UE150 = fps_29_97p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_29_97psf_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_29_97psf_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_29_97psf_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_29_97psf_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_29_97psf_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_29_97psf_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_29_97p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_29_97p_format_answers_array;
					break;
				case 3:	//1080/59.94p
					output_fps_AW_UE150 = fps_59_94p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_59_94p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_59_94p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_59_94p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_59_94p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_59_94p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_59_94p_format_answers_array;
					break;
				case 4:	//1080/29.97p
					output_fps_AW_UE150 = fps_29_97p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_29_97p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_29_97p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_29_97p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_29_97p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_29_97p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_29_97p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_29_97p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_29_97p_format_answers_array;
					break;
				case 5:	//1080/23.98p (over 59.94i/p)
					output_fps_AW_UE150 = fps_23_98p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_23_98p_over_59_94i_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_23_98p_over_59_94i_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_23_98p_over_59_94i_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_23_98p_over_59_94i_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_23_98p_over_59_94i_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_23_98p_over_59_94i_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_23_98p_over_59_94i_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_23_98p_over_59_94i_format_answers_array;
					break;
				case 6:	//2160/29.97p
					output_fps_AW_UE150 = fps_29_97p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 3;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_29_97p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_29_97p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_4k_29_97p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_4k_29_97p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_4k_29_97p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_4k_29_97p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_29_97p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_29_97p_format_answers_array;
					break;
				case 7:	//2160/59.94p
					output_fps_AW_UE150 = fps_59_94p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_59_94p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_4k_59_94p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_4k_59_94p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_4k_59_94p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_4k_59_94p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_59_94p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_59_94p_format_answers_array;
			}

			break;
		case 1:	//50Hz
			switch (settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]) {
				case 0:	//720/50p
					output_fps_AW_UE150 = fps_50p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_720_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_720_50p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_720_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_720_50p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_720_50p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_720_50p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_720_50p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_720_50p_format_answers_array;
					break;
				case 1:	//1080/50i
					output_fps_AW_UE150 = fps_50i;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_50i_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_50i_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_50i_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_50i_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_50i_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_50i_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_50i_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_50i_format_answers_array;
					break;
				case 2:	//1080/25PsF
					output_fps_AW_UE150 = fps_25p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_25psf_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_25psf_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_25psf_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_25psf_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_25psf_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_25psf_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_25p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_25p_format_answers_array;
					break;
				case 3:	//1080/50p
					output_fps_AW_UE150 = fps_50p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_50p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_50p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_50p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_50p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_50p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_50p_format_answers_array;
					break;
				case 4:	//1080/25p
					output_fps_AW_UE150 = fps_25p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_25p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_25p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_25p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_25p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_25p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_25p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_25p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_25p_format_answers_array;
					break;
				case 5:	//2160/25p
					output_fps_AW_UE150 = fps_25p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 3;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_25p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_25p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_4k_25p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_4k_25p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_4k_25p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_4k_25p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_25p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_25p_format_answers_array;
					break;
				case 6:	//2160/50p
					output_fps_AW_UE150 = fps_50p;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_50p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_4k_50p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_4k_50p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_4k_50p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_4k_50p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_50p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_50p_format_answers_array;
			}
			
			break;
		case 2:	//24Hz
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 2;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_24p_AW_UE150;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_24p_AW_UE150;

			output_fps_AW_UE150 = fps_24p;

			switch (settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]) {
				case 0:	//2160/24p
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_24p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_24p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_24p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_24p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_24p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_24p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_24p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_24p_format_answers_array;
					break;
				case 1:	//1080/24p
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_24p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_24p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_24p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_24p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_24p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_24p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_24p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_24p_format_answers_array;
			}

			break;
		case 3:	//23.98Hz
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 3;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_23_98p_AW_UE150;
			settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_23_98p_AW_UE150;

			output_fps_AW_UE150 = fps_23_98p;

			switch (settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]) {
				case 0:	//2160/23.98p
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 3;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_4k_23_98p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_4k_23_98p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_4k_23_98p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_4k_23_98p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_4k_23_98p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_4k_23_98p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_4k_23_98p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_4k_23_98p_format_answers_array;
					break;
				case 1:	//1080/23.98p
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_23_98p_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_23_98p_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_23_98p_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_23_98p_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_23_98p_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_23_98p_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_23_98p_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_23_98p_format_answers_array;
					break;
				case 2:	//1080/23.98PsF
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters = sdi_12g_1080_23_98psf_format_parameters_array;
					settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].answers = sdi_12g_1080_23_98psf_format_answers_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters = sdi_3g_1080_23_98psf_format_parameters_array;
					settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].answers = sdi_3g_1080_23_98psf_format_answers_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters = moni_1080_23_98psf_format_parameters_array;
					settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].answers = moni_1080_23_98psf_format_answers_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters = 1;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters = hdmi_1080_23_98psf_format_parameters_array;
					settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].answers = hdmi_1080_23_98psf_format_answers_array;
			}
	}
}

void check_camera_settings_AW_HE130 (rcp_t *rcp)
{
	char response_string[8];
	int i;

#ifdef MAIN_SETTINGS_READ_ONLY
	int j;

	if (!camera_settings_AW_HE130_checked) {
		camera_settings_AW_HE130_checked = TRUE;

		send_cam_request_command_string (rcp, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].query_cmd, response_string);

		for (j = 0; j < settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].nb_parameters; j++) {
			if (strcmp (response_string, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].answers[j]) == 0) {
				settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] = j;

				if (j == 1) {	//50Hz
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 6;
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_pal_AW_HE130;
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_pal_AW_HE130;
				} else {		//59,94Hz	//23,98Hz
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 8;
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_ntsc_AW_HE130;
					settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_ntsc_AW_HE130;
				}

				g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FREQUENCY_INDEX_AW_HE130));

				break;
			} else settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] = -1;
		}

		send_cam_request_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].query_cmd, response_string);

		for (j = 0; j < settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters; j++) {
			if (strcmp (response_string, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[j]) == 0) {
				settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = j;

				if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) {	//50Hz
					if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3)) output_fps_AW_HE130 = fps_50p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 5)) output_fps_AW_HE130 = fps_25p;
					else output_fps_AW_HE130 = fps_50i;
				} else {																			//59,94Hz	//23,98Hz
					if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 4)) output_fps_AW_HE130 = fps_59_94p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 6)) output_fps_AW_HE130 = fps_29_97p;
					else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 7)) output_fps_AW_HE130 = fps_23_98p;
					else output_fps_AW_HE130 = fps_59_94i;
				}

				g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_HE130));

				break;
			} else settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = -1;
		}

		for (i = 2; i < NB_SETTINGS_AW_HE130; i++) {
			send_cam_request_command_string (rcp, settings_array_AW_HE130[i].query_cmd, response_string);

			for (j = 0; j < settings_array_AW_HE130[i].nb_parameters; j++) {
				if (strcmp (response_string, settings_array_AW_HE130[i].answers[j]) == 0) {
					settings_parameters_indexes_array_AW_HE130[i] = j;

					g_idle_add ((GSourceFunc)update_settings_array_AW_HE130_combo_box_text, GINT_TO_POINTER (i));

					break;
				} else settings_parameters_indexes_array_AW_HE130[i] = -1;
			}
		}

		send_cam_request_command (rcp, "QSE:7B", &osd_mix_AW_HE130);
		g_idle_add ((GSourceFunc)update_osd_mix_AW_HE130, NULL);
	}
#else
	int osd_mix;

	send_cam_request_command_string (rcp, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].query_cmd, response_string);

	if (response_string[0] != settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130]][0]) {
		send_cam_control_command_string (rcp, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[FREQUENCY_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130]].value);

		usleep (45000000);

		send_update_start_cmd (rcp);

		send_ptz_control_command (rcp, "#LPC1");
	}

	if (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] != -1) {
		send_cam_request_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].query_cmd, response_string);

		if ((response_string[0] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][0]) || \
			(response_string[1] != settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]][1])) {
			send_cam_control_command_string (rcp, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].control_cmd, settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters[settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130]].value);

			usleep (30000000);
		}
	}

	for (i = 2; i < NB_SETTINGS_AW_HE130; i++) {
		if (settings_parameters_indexes_array_AW_HE130[i] == -1) continue;

		send_cam_request_command_string (rcp, settings_array_AW_HE130[i].query_cmd, response_string);

		if (strcmp (response_string, settings_array_AW_HE130[i].answers[settings_parameters_indexes_array_AW_HE130[i]]) != 0)
			send_cam_control_command_string (rcp, settings_array_AW_HE130[i].control_cmd, settings_array_AW_HE130[i].parameters[settings_parameters_indexes_array_AW_HE130[i]].value);
	}

	send_cam_request_command (rcp, "QSE:7B", &osd_mix);

	if (osd_mix != osd_mix_AW_HE130) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_HE130, TRUE);
#endif
}

void check_camera_settings_AW_UE150 (rcp_t *rcp)
{
	char response_string[8];
	int i;

#ifdef MAIN_SETTINGS_READ_ONLY
	int j;

	if (!camera_settings_AW_UE150_checked) {
		camera_settings_AW_UE150_checked = TRUE;

		send_cam_request_command_string (rcp, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].query_cmd, response_string);

		for (j = 0; j < settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].nb_parameters; j++) {
			if (strcmp (response_string, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].answers[j]) == 0) {
				settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150] = j;

				switch (j) {
					case 0:	//59,94Hz
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 8;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_ntsc_AW_UE150;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_ntsc_AW_UE150;
						break;
					case 1:	//50Hz
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 7;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_pal_AW_UE150;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_pal_AW_UE150;
						break;
					case 2:	//24Hz
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 2;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_24p_AW_UE150;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_24p_AW_UE150;
						break;
					case 3:	//23.98Hz
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 3;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_23_98p_AW_UE150;
						settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_23_98p_AW_UE150;
				}

				g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FREQUENCY_INDEX_AW_UE150));

				break;
			} else settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150] = -1;
		}

		send_cam_request_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].query_cmd, response_string);

		for (j = 0; j < settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters; j++) {
			if (strcmp (response_string, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[j]) == 0) {
				settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = j;

				update_outputs_parameters_AW_UE150 ();

				g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (FORMAT_INDEX_AW_UE150));

				break;
			} else settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = -1;
		}

		for (i = 2; i < NB_SETTINGS_AW_UE150; i++) {
			send_cam_request_command_string (rcp, settings_array_AW_UE150[i].query_cmd, response_string);

			for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++) {
				if (strcmp (response_string, settings_array_AW_UE150[i].answers[j]) == 0) {
					settings_parameters_indexes_array_AW_UE150[i] = j;

					g_idle_add ((GSourceFunc)update_settings_array_AW_UE150_combo_box_text, GINT_TO_POINTER (i));

					break;
				} else settings_parameters_indexes_array_AW_UE150[i] = -1;
			}
		}

		send_cam_request_command (rcp, "QSE:7B", &osd_mix_AW_UE150);
		g_idle_add ((GSourceFunc)update_osd_mix_AW_UE150, NULL);
	}
#else
	int osd_mix;

	send_cam_request_command_string (rcp, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].query_cmd, response_string);

	if (response_string[0] != settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]][0]) {
		send_cam_control_command_string (rcp, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[FREQUENCY_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]].value);

		usleep (45000000);

		send_update_start_cmd (rcp);

		send_ptz_control_command (rcp, "#LPC1");
	}

	if (settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] != -1) {
		send_cam_request_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].query_cmd, response_string);

		if ((response_string[0] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][0]) || \
			(response_string[1] != settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]][1])) {
			send_cam_control_command_string (rcp, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].control_cmd, settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters[settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150]].value);

			usleep (30000000);
		}
	}

	for (i = 2; i < 8 /*NB_SETTINGS_AW_UE150*/; i++) {
		if (settings_parameters_indexes_array_AW_UE150[i] == -1) continue;

		send_cam_request_command_string (rcp, settings_array_AW_UE150[i].query_cmd, response_string);

		if (strcmp (response_string, settings_array_AW_UE150[i].answers[settings_parameters_indexes_array_AW_UE150[i]]) != 0)
			send_cam_control_command_string (rcp, settings_array_AW_UE150[i].control_cmd, settings_array_AW_UE150[i].parameters[settings_parameters_indexes_array_AW_UE150[i]].value);
	}

	send_cam_request_command (rcp, "QSE:7B", &osd_mix);

	if (osd_mix != osd_mix_AW_UE150) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_UE150, TRUE);
#endif
}

void check_camera_settings (rcp_t *rcp)
{
	int i;
	char response_string[8];

#ifdef MAIN_SETTINGS_READ_ONLY
	int j;

	if (!camera_settings_checked) {
		camera_settings_checked = TRUE;

		for (i = 0; i < NB_SETTINGS; i++) {
			send_cam_request_command_string (rcp, settings_array[i].query_cmd, response_string);

			for (j = 0; j < settings_array[i].nb_parameters; j++) {
				if (strcmp (response_string, settings_array[i].answers[j]) == 0) {
					settings_parameters_indexes_array[i] = j;

					g_idle_add ((GSourceFunc)update_settings_array_combo_box_text, GINT_TO_POINTER (i));

					break;
				} else settings_parameters_indexes_array[i] = -1;
			}
		}

		send_ptz_request_command (rcp, "#LMP", &status_lamp);
		g_idle_add ((GSourceFunc)update_status_lamp_combo_box_text, NULL);

		send_ptz_request_command (rcp, "#TAE", &tally_input);
		g_idle_add ((GSourceFunc)update_tally_input_combo_box_text, NULL);

		send_cam_request_command (rcp, "QSD:48", &picture_level);
		picture_level -= 0x32;
		g_idle_add ((GSourceFunc)update_picture_level_scale, NULL);
	}
#else
	int response;

	for (i = 0; i < NB_SETTINGS; i++) {
		if (settings_parameters_indexes_array[i] == -1) continue;

		send_cam_request_command_string (rcp, settings_array[i].query_cmd, response_string);

		if (strcmp (response_string, settings_array[i].answers[settings_parameters_indexes_array[i]]) != 0)
			send_cam_control_command_string (rcp, settings_array[i].control_cmd, settings_array[i].parameters[settings_parameters_indexes_array[i]].value);
	}

	send_ptz_request_command (rcp, "#LMP", &response);
	if (response != status_lamp) {
		if (status_lamp == 1) send_ptz_control_command (rcp, "#LMP1");
		else send_ptz_control_command (rcp, "#LMP0");
	}

	send_ptz_request_command (rcp, "#TAE", &response);
	if (response != tally_input) {
		if (tally_input == 1) send_ptz_control_command (rcp, "#TAE1");
		else send_ptz_control_command (rcp, "#TAE0");
	}

	send_cam_request_command (rcp, "QSD:48", &response);
	if ((response - 0x32) != picture_level) send_cam_control_command_2_digits (rcp, "OSD:48:", picture_level + 0x32, TRUE);
#endif
}


#ifndef MAIN_SETTINGS_READ_ONLY
void parameter_changed (GtkComboBox *combo_box, int *parameter)
{
	int active_item, offset;
	GList *glist_itr;
	rcp_t *rcp;

	active_item = gtk_combo_box_get_active (combo_box);

	if (active_item == -1) return;

	offset = parameter - settings_parameters_indexes_array;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if (rcp->camera_is_on) send_cam_control_command_string (rcp, settings_array[offset].control_cmd, settings_array[offset].parameters[active_item].value);
	}

	*parameter = active_item;

	if (offset == WHITE_CLIP_INDEX) {
		if (settings_parameters_indexes_array[WHITE_CLIP_INDEX] == 1) gtk_widget_set_sensitive (settings_array[WHITE_CLIP_LEVEL_INDEX].combo_box, TRUE);
		else gtk_widget_set_sensitive (settings_array[WHITE_CLIP_LEVEL_INDEX].combo_box, FALSE);
	}

	backup_needed = TRUE;
}

gpointer restart_rcp (rcp_t *rcp)
{
	usleep (4500000);

	send_update_start_cmd (rcp);

	start_rcp (rcp);

	return NULL;
}

void parameter_changed_AW_HE130 (GtkComboBox *combo_box, int *parameter)
{
	int active_item, offset, i;
	cameras_set_t *cameras_set_tmp;
	GList *glist_itr;
	rcp_t *rcp;
	GSList *gslist_itr;

	active_item = gtk_combo_box_get_active (combo_box);

	if (active_item == -1) return;

	offset = parameter - settings_parameters_indexes_array_AW_HE130;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if ((rcp->model == AW_HE130) && (rcp->camera_is_on)) {
			send_cam_control_command_string (rcp, settings_array_AW_HE130[offset].control_cmd, settings_array_AW_HE130[offset].parameters[active_item].value);

			if ((offset == FREQUENCY_INDEX_AW_HE130) || (offset == FORMAT_INDEX_AW_HE130)) {
				rcp_start_working (rcp);
				rcp->thread = g_thread_new (NULL, (GThreadFunc)restart_rcp, rcp);

				for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next)
					rcp_start_working ((rcp_t*)(gslist_itr->data));
			}
		}
	}

	*parameter = active_item;

	if (offset == FREQUENCY_INDEX_AW_HE130) {
		if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) {	//50Hz
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 6;
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_pal_AW_HE130;
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_pal_AW_HE130;
			gtk_widget_set_sensitive (COLOR_BAR_SETUP_LEVEL_AW_HE130_COMBO_BOX, FALSE);
		} else {																			//59,94Hz	//23,98Hz
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 8;
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_ntsc_AW_HE130;
			settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_ntsc_AW_HE130;
			gtk_widget_set_sensitive (COLOR_BAR_SETUP_LEVEL_AW_HE130_COMBO_BOX, TRUE);
		}

		settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] = -1;

		gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].combo_box));

		for (i = 0; i < settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters; i++) {
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].combo_box), settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters[i].label);
		}

		if (physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			send_frequency_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	} else if (offset == FORMAT_INDEX_AW_HE130) {
		if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) {	//50Hz
			if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3)) output_fps_AW_HE130 = fps_50p;
			else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 5)) output_fps_AW_HE130 = fps_25p;
			else output_fps_AW_HE130 = fps_50p;
		} else {																			//59,94Hz	//23,98Hz
			if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 4)) output_fps_AW_HE130 = fps_59_94p;
			else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 6)) output_fps_AW_HE130 = fps_29_97p;
			else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 7)) output_fps_AW_HE130 = fps_23_98p;
			else output_fps_AW_HE130 = fps_59_94i;
		}

		for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
			for (i = 0; i < cameras_set_tmp->number_of_cameras; i++) {
				if (cameras_set_tmp->cameras[i]->active)
					populate_shutter_step_combo_box_AW_HE130 (GTK_COMBO_BOX_TEXT (cameras_set_tmp->cameras[i]->shutter_step_combo_box));
			}
		}

		if (physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			send_format_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}
	}

	backup_needed = TRUE;
}

void parameter_changed_AW_UE150 (GtkComboBox *combo_box, int *parameter)
{
	int active_item, offset, i, response;
	cameras_set_t *cameras_set_tmp;
	GList *glist_itr;
	rcp_t *rcp;
	GSList *gslist_itr;
	rcp_t *other_rcp;

	active_item = gtk_combo_box_get_active (combo_box);

	if (active_item == -1) return;

	offset = parameter - settings_parameters_indexes_array_AW_UE150;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if ((rcp->model == AW_UE150) && (rcp->camera_is_on)) {
			send_cam_control_command_string (rcp, settings_array_AW_UE150[offset].control_cmd, settings_array_AW_UE150[offset].parameters[active_item].value);

			if ((offset == FREQUENCY_INDEX_AW_UE150) || (offset == FORMAT_INDEX_AW_UE150)) {
				rcp_start_working (rcp);
				rcp->thread = g_thread_new (NULL, (GThreadFunc)restart_rcp, rcp);

				for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next)
					rcp_start_working ((rcp_t*)(gslist_itr->data));
			}
		}
	}

	*parameter = active_item;

	switch (offset) {
		case FREQUENCY_INDEX_AW_UE150:
			switch (settings_parameters_indexes_array_AW_UE150[FREQUENCY_INDEX_AW_UE150]) {
				case 0:	//59,94Hz
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 8;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_ntsc_AW_UE150;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_ntsc_AW_UE150;
					break;
				case 1:	//50Hz
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 7;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_pal_AW_UE150;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_pal_AW_UE150;
					break;
				case 2:	//24Hz
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 2;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_24p_AW_UE150;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_24p_AW_UE150;
					break;
				case 3:	//23.98Hz
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters = 3;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters = format_parameters_array_23_98p_AW_UE150;
					settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].answers = format_answers_array_23_98p_AW_UE150;
			}

			settings_parameters_indexes_array_AW_UE150[FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

/*			if (physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				send_frequency_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}*/

			break;
		case FORMAT_INDEX_AW_UE150:
			update_outputs_parameters_AW_UE150 ();

			settings_parameters_indexes_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[SDI_12G_OPTICAL_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[SDI_3G_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[MONI_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			settings_parameters_indexes_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150] = -1;

			gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].combo_box));

			for (i = 0; i < settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].nb_parameters; i++) {
				gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].combo_box), settings_array_AW_UE150[HDMI_FORMAT_INDEX_AW_UE150].parameters[i].label);
			}

			for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
				for (i = 0; i < cameras_set_tmp->number_of_cameras; i++) {
					rcp = cameras_set_tmp->cameras[i];

					if ((rcp->model == AW_UE150) && (rcp->active)) {
						populate_shutter_step_combo_box_AW_UE150 (GTK_COMBO_BOX_TEXT (rcp->shutter_step_combo_box));

						gtk_widget_destroy (rcp->shutter_synchro_window);
						create_shutter_synchro_window_AW_UE150 (rcp);
					}
				}
			}

			for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
				rcp = (rcp_t*)(glist_itr->data);

				if ((rcp->model == AW_UE150) && (rcp->camera_is_on)) {
					send_cam_request_command (rcp, "QSJ:06", &rcp->current_scene.shutter_step);

					update_shutter_step_combo_box_AW_UE150 (rcp);

					for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
						other_rcp = (rcp_t*)(gslist_itr->data);

						other_rcp->current_scene.shutter_step = rcp->current_scene.shutter_step;
						update_shutter_step_combo_box_AW_UE150 (other_rcp);
					}

					send_cam_request_command (rcp, "QSJ:09", &rcp->current_scene.shutter_synchro);

					g_signal_handler_block (rcp->shutter_synchro_scale, rcp->shutter_synchro_handler_id);
					gtk_range_set_value (GTK_RANGE (rcp->shutter_synchro_scale), rcp->current_scene.shutter_synchro);
					g_signal_handler_unblock (rcp->shutter_synchro_scale, rcp->shutter_synchro_handler_id);

					for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
						other_rcp = (rcp_t*)(gslist_itr->data);

						other_rcp->current_scene.shutter_synchro = rcp->current_scene.shutter_synchro;
						g_signal_handler_block (other_rcp->shutter_synchro_scale, other_rcp->shutter_synchro_handler_id);
						gtk_range_set_value (GTK_RANGE (other_rcp->shutter_synchro_scale), other_rcp->current_scene.shutter_synchro);
						g_signal_handler_unblock (other_rcp->shutter_synchro_scale, other_rcp->shutter_synchro_handler_id);
					}
				}
			}

/*			if (physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				send_format_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}*/

			break;
		case SUPER_GAIN_INDEX_AW_UE150:
			for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
				for (i = 0; i < cameras_set_tmp->number_of_cameras; i++) {
					rcp = cameras_set_tmp->cameras[i];

					if ((rcp->model == AW_UE150) && (rcp->active)) {
						populate_gain_combo_box_AW_UE150 (GTK_COMBO_BOX_TEXT (rcp->gain_combo_box));
					}
				}
			}

			for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
				rcp = (rcp_t*)(glist_itr->data);

				if ((rcp->model == AW_UE150) && (rcp->camera_is_on)) {
					send_cam_request_command (rcp, "QGU", &response);

					if (settings_parameters_indexes_array_AW_UE150[SUPER_GAIN_INDEX_AW_UE150] == 1) {
						if (response == 0x80) rcp->current_scene.gain = 43;
						else rcp->current_scene.gain = 0x32 - response;
					} else {
						if (response == 0x80) rcp->current_scene.gain = 37;
						else rcp->current_scene.gain = 0x2C - response;
					}

					g_signal_handler_block (rcp->gain_combo_box, rcp->gain_handler_id);
					gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->gain_combo_box), rcp->current_scene.gain);
					g_signal_handler_unblock (rcp->gain_combo_box, rcp->gain_handler_id);

					for (gslist_itr = rcp->other_rcp; gslist_itr != NULL; gslist_itr = gslist_itr->next) {
						other_rcp = (rcp_t*)(gslist_itr->data);

						other_rcp->current_scene.gain = rcp->current_scene.gain;
						g_signal_handler_block (other_rcp->gain_combo_box, other_rcp->gain_handler_id);
						gtk_combo_box_set_active (GTK_COMBO_BOX (other_rcp->gain_combo_box), other_rcp->current_scene.gain);
						g_signal_handler_unblock (other_rcp->gain_combo_box, other_rcp->gain_handler_id);
					}
				}
			}
			break;
	}

	backup_needed = TRUE;
}

void status_lamp_changed (GtkComboBox *combo_box)
{
	GList *glist_itr;
	rcp_t *rcp;

	status_lamp = gtk_combo_box_get_active (combo_box);

	if (status_lamp == -1) return;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if (rcp->camera_is_on) {
			if (status_lamp == 1) send_ptz_control_command (rcp, "#LMP1");
			else send_ptz_control_command (rcp, "#LMP0");
		}
	}

	backup_needed = TRUE;
}

void tally_input_changed (GtkComboBox *combo_box)
{
	GList *glist_itr;
	rcp_t *rcp;

	tally_input = gtk_combo_box_get_active (combo_box);

	if (tally_input == -1) return;

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if (rcp->camera_is_on) {
			if (tally_input == 1) send_ptz_control_command (rcp, "#TAE1");
			else send_ptz_control_command (rcp, "#TAE0");
		}
	}

	backup_needed = TRUE;
}

void picture_level_value_changed (GtkRange *picture_level_scale)
{
	int picture_level_value;
	GList *glist_itr;
	rcp_t *rcp;
	char label[8];

	picture_level_value = (int)gtk_range_get_value (picture_level_scale);

	if (picture_level != picture_level_value) {
		picture_level = picture_level_value;

		for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
			rcp = (rcp_t*)(glist_itr->data);

			if (rcp->camera_is_on) send_cam_control_command_2_digits (rcp, "OSD:48:", picture_level + 0x32, TRUE);
		}

		if (picture_level == 0) gtk_label_set_text (GTK_LABEL (picture_level_value_label), "0");
		else {
			sprintf (label, "%+d", picture_level);
			gtk_label_set_text (GTK_LABEL (picture_level_value_label), label);
		}

		if (physical_rcp.connected) {
			g_mutex_lock (&physical_rcp.mutex);
			send_picture_level_update_notification ();
			g_mutex_unlock (&physical_rcp.mutex);
		}

		backup_needed = TRUE;
	}
}

void osd_mix_button_toggled_AW_HE130 (GtkToggleButton *toggle_button, gpointer bit_mask)
{
	GList *glist_itr;
	rcp_t *rcp;

	if (gtk_toggle_button_get_active (toggle_button)) {
		osd_mix_AW_HE130 |= GPOINTER_TO_INT (bit_mask);
		gtk_button_set_label (GTK_BUTTON (toggle_button), "On");
	} else {
		osd_mix_AW_HE130 &= ~GPOINTER_TO_INT (bit_mask);
		gtk_button_set_label (GTK_BUTTON (toggle_button), "Off");
	}

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if ((rcp->model == AW_HE130) && (rcp->camera_is_on)) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_HE130, TRUE);
	}

	backup_needed = TRUE;
}

void osd_mix_button_toggled_AW_UE150 (GtkToggleButton *toggle_button, gpointer bit_mask)
{
	GList *glist_itr;
	rcp_t *rcp;

	if (gtk_toggle_button_get_active (toggle_button)) {
		osd_mix_AW_UE150 |= GPOINTER_TO_INT (bit_mask);
		gtk_button_set_label (GTK_BUTTON (toggle_button), "On");
	} else {
		osd_mix_AW_UE150 &= ~GPOINTER_TO_INT (bit_mask);
		gtk_button_set_label (GTK_BUTTON (toggle_button), "Off");
	}

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		rcp = (rcp_t*)(glist_itr->data);

		if ((rcp->model == AW_UE150) && (rcp->camera_is_on)) send_cam_control_command_2_digits (rcp, "OSE:7B:", osd_mix_AW_UE150, TRUE);
	}

	backup_needed = TRUE;
}
#endif

void send_ip_tally_check_button_toggled (GtkToggleButton *togglebutton)
{
	send_ip_tally = gtk_toggle_button_get_active (togglebutton);

	backup_needed = TRUE;
}

void update_notification_tcp_port_entry_activate (GtkEntry *entry, GtkEntryBuffer *entry_buffer)
{
	guint16 port;
	GList *glist_itr;

	stop_update_notification ();

	sscanf (gtk_entry_buffer_get_text (entry_buffer), "%hu", &port);

	if ((port < 1024) || (port > 65535) || (port == ntohs (sw_p_08_address.sin_port)) || (port == ntohs (physical_rcp.address.sin_port))) {
		update_notification_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);
		gtk_entry_buffer_set_text (entry_buffer, "31004", 5);
	} else update_notification_address.sin_port = htons (port);

	start_update_notification ();

	for (glist_itr = rcp_start_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
		if (((rcp_t*)(glist_itr->data))->error_code != CAMERA_IS_UNREACHABLE_ERROR) send_update_start_cmd ((rcp_t*)(glist_itr->data));
	}

	backup_needed = TRUE;
}

void physical_rcp_tcp_port_entry_activate (GtkEntry *entry, GtkEntryBuffer *entry_buffer)
{
	guint16 port;

	stop_physical_rcp ();

	sscanf (gtk_entry_buffer_get_text (entry_buffer), "%hu", &port);

	if ((port < 1024) || (port > 65535) || (port == ntohs (sw_p_08_address.sin_port)) || (port == ntohs (update_notification_address.sin_port))) {
		physical_rcp.address.sin_port = htons (PHYSICAL_RCP_TCP_PORT);
		gtk_entry_buffer_set_text (entry_buffer, "9000", 4);
	} else physical_rcp.address.sin_port = htons (port);

	start_physical_rcp ();

	backup_needed = TRUE;
}

void tsl_umd_v5_udp_port_entry_activate (GtkEntry *entry, GtkEntryBuffer *entry_buffer)
{
	guint16 port;

	stop_tally ();

	sscanf (gtk_entry_buffer_get_text (entry_buffer), "%hu", &port);

	if ((port < 1024) || (port > 65535)) {
		tsl_umd_v5_address.sin_port = htons (TSL_UMD_V5_UDP_PORT);
		gtk_entry_buffer_set_text (entry_buffer, "8900", 5);
	} else tsl_umd_v5_address.sin_port = htons (port);

	start_tally ();

	backup_needed = TRUE;
}

void settings_list_box_row_selected (GtkListBox *list_box, GtkListBoxRow *row)
{
	if (row != NULL) {
		gtk_widget_set_sensitive (settings_configuration_button, TRUE);
		gtk_widget_set_sensitive (settings_delete_button, TRUE);
	} else {
		gtk_widget_set_sensitive (settings_configuration_button, FALSE);
		gtk_widget_set_sensitive (settings_delete_button, FALSE);
	}
}

gboolean delete_confirmation_window_key_press (GtkWidget *confirmation_window, GdkEventKey *event)
{
	if ((event->keyval == GDK_KEY_n) || (event->keyval == GDK_KEY_N) || (event->keyval == GDK_KEY_Escape)) gtk_widget_destroy (confirmation_window);
	else if ((event->keyval == GDK_KEY_o) || (event->keyval == GDK_KEY_O)) delete_cameras_set (NULL, confirmation_window);

	return GDK_EVENT_PROPAGATE;
}

void show_delete_confirmation_window (void)
{
	GtkWidget *window, *box1, *box2, *widget;
	GtkListBoxRow *list_box_row;
	GList *list;
	char message[128];
	char *text = "Etes-vous sûr de vouloir supprimer l'ensemble de caméras \"";

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), warning_txt);
	gtk_window_set_modal (GTK_WINDOW (window), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (delete_confirmation_window_key_press), NULL);

	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box1), WINDOW_MARGIN_VALUE);
		list_box_row = gtk_list_box_get_selected_row (GTK_LIST_BOX (settings_list_box));
		if (list_box_row != NULL) {
			list = gtk_container_get_children (GTK_CONTAINER (list_box_row));
			sprintf (message, "%s%s\"?", text, gtk_label_get_text (GTK_LABEL (list->data)));
			widget = gtk_label_new (message);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		}

		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_halign (box2, GTK_ALIGN_CENTER);
		gtk_widget_set_margin_top (box2, WINDOW_MARGIN_VALUE);
		gtk_box_set_spacing (GTK_BOX (box2), WINDOW_MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box2), TRUE);
			widget = gtk_button_new_with_label ("OUI");
			g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (delete_cameras_set), window);
			gtk_box_pack_start (GTK_BOX (box2), widget, TRUE, TRUE, 0);

			widget = gtk_button_new_with_label ("NON");
			g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (gtk_widget_destroy), window);
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box1);

	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_show_all (window);
}

gboolean escape_key_press (GtkWidget *window, GdkEventKey *event)
{
	if (event->keyval == GDK_KEY_Escape) gtk_widget_destroy (window);

	return GDK_EVENT_PROPAGATE;
}

void show_matrix_window (void)
{
	GtkWidget *window, *box, *grid, *widget;
	int i, j;
	char text[128];

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), "Grille Snell SW-P-08");
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_modal (GTK_WINDOW (window), TRUE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (escape_key_press), NULL);

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_halign (box, GTK_ALIGN_CENTER);

	sprintf (text, "Paramètres de liaison TCP/IP : %s:%s", my_ip_address, gtk_entry_get_text (GTK_ENTRY (tcp_port_entry)));
	widget = gtk_label_new (text);
	gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
	gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

	sprintf (text, "Paramètres de liaison série : 38400 baud, 8 bit data, no parity, one stop bit");
	widget = gtk_label_new (text);
	gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
	gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

	grid = gtk_grid_new ();
	gtk_widget_set_margin_top (grid, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_start (grid, 15);
	gtk_widget_set_margin_end (grid, 15);

	widget = gtk_label_new ("1");
	gtk_grid_attach (GTK_GRID (grid), widget, 0, 0, 1, 1);

	for (j = 1; j < 5; j++) {
#ifdef _WIN32
		if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_1_light);
		else widget = gtk_image_new_from_pixbuf (pixbuf_grille_1_dark);
#elif defined (__linux)
		if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_1_light.png");
		else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_1_dark.png");
#endif
		gtk_grid_attach (GTK_GRID (grid), widget, 0, j, 1, 1);
	}

#ifdef _WIN32
	if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_3_light);
	else widget = gtk_image_new_from_pixbuf (pixbuf_grille_3_dark);
#elif defined (__linux)
	if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_3_light.png");
	else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_3_dark.png");
#endif
	gtk_grid_attach (GTK_GRID (grid), widget, 0, 5, 1, 1);

	for (i = 1; i < MAX_CAMERAS; i++) {
		sprintf (text, "%d", i + 1);
		widget = gtk_label_new (text);
		gtk_grid_attach (GTK_GRID (grid), widget, i, 0, 1, 1);

		for (j = 1; j < 5; j++) {
#ifdef _WIN32
			if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_light);
			else widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_dark);
#elif defined (__linux)
			if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_light.png");
			else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_dark.png");
#endif
			gtk_grid_attach (GTK_GRID (grid), widget, i, j, 1, 1);
		}

#ifdef _WIN32
		if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_light);
		else widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_dark);
#elif defined (__linux)
		if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_light.png");
		else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_dark.png");
#endif
		gtk_grid_attach (GTK_GRID (grid), widget, i, 5, 1, 1);
	}

	widget = gtk_label_new ("Echap");
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS, 0, 1, 1);

	for (j = 1; j < 5; j++) {
#ifdef _WIN32
		if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_light);
		else widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_dark);
#elif defined (__linux)
		if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_light.png");
		else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_dark.png");
#endif
		gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS, j, 1, 1);
	}

#ifdef _WIN32
	if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_light);
	else widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_dark);
#elif defined (__linux)
	if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_light.png");
	else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_dark.png");
#endif
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS, 5, 1, 1);

	widget = gtk_label_new ("Rien");
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 1, 0, 1, 1);

	for (j = 1; j < 5; j++) {
#ifdef _WIN32
		if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_light);
		else widget = gtk_image_new_from_pixbuf (pixbuf_grille_2_dark);
#elif defined (__linux)
		if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_light.png");
		else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_2_dark.png");
#endif
		gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 1, j, 1, 1);
	}

#ifdef _WIN32
	if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_light);
	else widget = gtk_image_new_from_pixbuf (pixbuf_grille_4_dark);
#elif defined (__linux)
	if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_light.png");
	else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/grille_4_dark.png");
#endif
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 1, 5, 1, 1);

	widget = gtk_label_new (" 1: Ensemble de caméras");
	gtk_widget_set_halign (widget, GTK_ALIGN_START);
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 2, 1, 1, 1);

	widget = gtk_label_new (" 2: RCP");
	gtk_widget_set_halign (widget, GTK_ALIGN_START);
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 2, 2, 1, 1);

	widget = gtk_label_new (" 3: Mémoire");
	gtk_widget_set_halign (widget, GTK_ALIGN_START);
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 2, 3, 1, 1);

	widget = gtk_label_new (" 4: PGM");
	gtk_widget_set_halign (widget, GTK_ALIGN_START);
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 2, 4, 1, 1);

	widget = gtk_label_new (" 5: PVW");
	gtk_widget_set_halign (widget, GTK_ALIGN_START);
	gtk_grid_attach (GTK_GRID (grid), widget, MAX_CAMERAS + 2, 5, 1, 1);

	gtk_box_pack_start (GTK_BOX (box), grid, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (window), box);
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_show_all (window);
}

void ip_radio_button_toggled (GtkToggleButton *button)
{
	if (!gtk_toggle_button_get_active (button)) return;

	ip_rs = TRUE;
	gtk_widget_set_sensitive (rs_combo_box_text, FALSE);
	gtk_widget_set_sensitive (tcp_port_entry, TRUE);

	stop_rs_communication ();

	if (start_sw_p_08_tcp_server ()) gtk_widget_show (ip_waiting_label);

	backup_needed = TRUE;
}

void rs_radio_button_toggled (GtkToggleButton *button)
{
	if (!gtk_toggle_button_get_active (button)) return;

	ip_rs = FALSE;
	gtk_widget_set_sensitive (tcp_port_entry, FALSE);
	gtk_widget_set_sensitive (rs_combo_box_text, TRUE);

	stop_sw_p_08_tcp_server ();

	if (start_rs_communication ()) gtk_widget_show (rs_ok_label);

	backup_needed = TRUE;
}

void sw_p_08_tcp_port_entry_activate (GtkEntry *entry, GtkEntryBuffer *entry_buffer)
{
	guint16 port;

	stop_sw_p_08_tcp_server ();

	sscanf (gtk_entry_buffer_get_text (entry_buffer), "%hu", &port);

	if ((port < 1024) || (port > 65535) || (port == ntohs (update_notification_address.sin_port)) || (port == ntohs (physical_rcp.address.sin_port))) {
		sw_p_08_address.sin_port = htons (SW_P_08_TCP_PORT);
		gtk_entry_buffer_set_text (entry_buffer, "8000", 4);
	} else sw_p_08_address.sin_port = htons (port);

	if (start_sw_p_08_tcp_server ()) gtk_widget_show (ip_waiting_label);

	backup_needed = TRUE;
}

void rs_port_changed (GtkComboBoxText *combo_box_text)
{
	char *active_text;

	stop_rs_communication ();

	active_text = gtk_combo_box_text_get_active_text (combo_box_text);

	if (active_text != NULL) {
		strcpy (rs_port_name, active_text);
		g_free (active_text);

		if (start_rs_communication ()) gtk_widget_show (rs_ok_label);
	}

	backup_needed = TRUE;
}

void theme_radio_button_toggled (GtkToggleButton *button)
{
	if (gtk_toggle_button_get_active (button)) {
		theme = TRUE;
		gtk_style_context_remove_provider_for_screen (screen, GTK_STYLE_PROVIDER (dark_css_provider));
		gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (light_css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		gtk_style_context_reset_widgets (screen);
	} else {
		theme = FALSE;
		gtk_style_context_remove_provider_for_screen (screen, GTK_STYLE_PROVIDER (light_css_provider));
		gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (dark_css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		gtk_style_context_reset_widgets (screen);
	}

	backup_needed = TRUE;
}

void show_master_rcp_check_button_toggled (GtkToggleButton *togglebutton)
{
	cameras_set_t *cameras_set_tmp;

	show_master_rcp = gtk_toggle_button_get_active (togglebutton);

	for (cameras_set_tmp = cameras_sets; cameras_set_tmp != NULL; cameras_set_tmp = cameras_set_tmp->next) {
		if (show_master_rcp) gtk_widget_show (cameras_set_tmp->master_rcp.root_widget);
		else gtk_widget_hide (cameras_set_tmp->master_rcp.root_widget);
	}

	backup_needed = TRUE;
}

void show_about_window (void)
{
	GtkWidget *about_window, *box, *widget;
	char gtk_version[64];

	about_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint (GTK_WINDOW (about_window), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title (GTK_WINDOW (about_window), "A propos");
	gtk_window_set_transient_for (GTK_WINDOW (about_window), GTK_WINDOW (main_window));
	gtk_window_set_modal (GTK_WINDOW (about_window), TRUE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (about_window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (about_window), FALSE);
	gtk_window_set_position (GTK_WINDOW (about_window), GTK_WIN_POS_CENTER_ON_PARENT);
	g_signal_connect (G_OBJECT (about_window), "key-press-event", G_CALLBACK (escape_key_press), NULL);

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), WINDOW_MARGIN_VALUE);
		widget = gtk_label_new (NULL);
		gtk_label_set_markup (GTK_LABEL (widget), "<b>RCP virtuels pour caméras Panasonic AW-HE130 et AW-UE150</b>");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("Version " RCP_VIRTUELS_VERSION);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

#ifdef _WIN32
		if (theme) widget = gtk_image_new_from_pixbuf (pixbuf_logo_light);
		else widget = gtk_image_new_from_pixbuf (pixbuf_logo_dark);
#elif defined (__linux)
		if (theme) widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/logo_light.png");
		else widget = gtk_image_new_from_resource ("/org/RCP-Virtuels/images/logo_dark.png");
#endif
		gtk_widget_set_margin_top (widget, WINDOW_MARGIN_VALUE);
		gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("Panasonic Interface Specifications v1.12");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("Router Control Protocols document n°SW-P-88 issue n°4b");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("TSL UMD Protocol V5.0");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		sprintf (gtk_version, "Compiled against GTK+ version %d.%d.%d", GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
		widget = gtk_label_new (gtk_version);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("Interface based on \"Mint-X\" et \"Adwaita-dark\" themes");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("Copyright (c) 2018-2022 Thomas Paillet");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_label_new ("GNU General Public License version 3");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (about_window), box);

	gtk_window_set_resizable (GTK_WINDOW (about_window), FALSE);
	gtk_widget_show_all (about_window);
}


#define SETTINGS_MARGIN_VALUE 4


void create_settings_page (void)
{
	GtkWidget *box1, *box2, *box3, *box4, *box5, *box6, *frame1, *frame2, *widget, *ip_radio_button, *rs_radio_button, *light_radio_button;
	GtkEntryBuffer *entry_buffer;
	int i, j;
	char label[8];
	GSList *rs_port_gslist, *gslist_itr;

	settings_page = gtk_scrolled_window_new (NULL, NULL);
	box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			frame1 = gtk_frame_new ("Réglages communs à toutes les caméras");
			gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
			gtk_container_set_border_width (GTK_CONTAINER (frame1), SETTINGS_MARGIN_VALUE);
			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_container_set_border_width (GTK_CONTAINER (box3), SETTINGS_MARGIN_VALUE);
				for (i = 0; i < NB_SETTINGS; i++) {
					box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
					gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_tooltip_text (box5, settings_array[i].tooltip);
						widget =  gtk_label_new (settings_array[i].name);
						gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

						widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
						gtk_widget_set_sensitive (widget, FALSE);
#else
						for (j = 0; j < settings_array[i].nb_parameters; j++)
							gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array[i].parameters[j].label);

						gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array[i]);
						gtk_widget_set_sensitive (widget, settings_array[i].sensitive);
						g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed), settings_parameters_indexes_array + i);
#endif
						settings_array[i].combo_box = widget;
						gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
					gtk_box_pack_start (GTK_BOX (box3), box5, FALSE, FALSE, 0);
				}

				box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("Status Lamp :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

#ifdef MAIN_SETTINGS_READ_ONLY
					status_lamp_combo_box_text = gtk_combo_box_text_new ();
					gtk_widget_set_sensitive (status_lamp_combo_box_text, FALSE);
					gtk_box_pack_end (GTK_BOX (box5), status_lamp_combo_box_text, FALSE, FALSE, 0);
#else
					widget = gtk_combo_box_text_new ();
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Disable");
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Enable");
					gtk_combo_box_set_active (GTK_COMBO_BOX (widget), status_lamp);
					g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (status_lamp_changed), NULL);
					gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
#endif
				gtk_box_pack_start (GTK_BOX (box3), box5, FALSE, FALSE, 0);

				box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("Tally Input :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

#ifdef MAIN_SETTINGS_READ_ONLY
					tally_input_combo_box_text = gtk_combo_box_text_new ();
					gtk_widget_set_sensitive (tally_input_combo_box_text, FALSE);
					gtk_box_pack_end (GTK_BOX (box5), tally_input_combo_box_text, FALSE, FALSE, 0);
#else
					widget = gtk_combo_box_text_new ();
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Disable");
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Enable");
					gtk_combo_box_set_active (GTK_COMBO_BOX (widget), tally_input);
					g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (tally_input_changed), NULL);
					gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
#endif
				gtk_box_pack_start (GTK_BOX (box3), box5, FALSE, FALSE, 0);

				box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_set_margin_top (box5, SETTINGS_MARGIN_VALUE);
				gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
				gtk_widget_set_tooltip_text (box5, picture_level_tooltip);
					widget =  gtk_label_new ("Picture Level : ");
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);
#ifdef MAIN_SETTINGS_READ_ONLY
					picture_level_value_label =  gtk_label_new ("0");
#else
					if ((int)picture_level == 0) picture_level_value_label =  gtk_label_new ("0");
					else {
						sprintf (label, "%+d", (int)picture_level);
						picture_level_value_label =  gtk_label_new (label);
					}
#endif
					gtk_box_pack_start (GTK_BOX (box5), picture_level_value_label, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box5, FALSE, FALSE, 0);

#ifdef MAIN_SETTINGS_READ_ONLY
				picture_level_scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, -50.0, +50.0, 1.0);
				gtk_range_set_value (GTK_RANGE (picture_level_scale), 0);
				gtk_scale_set_draw_value (GTK_SCALE (picture_level_scale), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (picture_level_scale), FALSE);
				gtk_widget_set_tooltip_text (picture_level_scale, picture_level_tooltip);
				gtk_box_pack_start (GTK_BOX (box3), picture_level_scale, FALSE, FALSE, 0);
#else
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, -50.0, +50.0, 1.0);
				gtk_range_set_value (GTK_RANGE (widget), picture_level);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_widget_set_tooltip_text (widget, picture_level_tooltip);
				g_signal_connect (G_OBJECT (widget), "value-changed", G_CALLBACK (picture_level_value_changed), NULL);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
#endif
				box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_set_margin_top (box5, SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("Update Notification Port TCP/IP :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

					entry_buffer = gtk_entry_buffer_new (label, sprintf (label, "%hu", ntohs (update_notification_address.sin_port)));
					widget = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (entry_buffer));
					gtk_entry_set_input_purpose (GTK_ENTRY (widget), GTK_INPUT_PURPOSE_DIGITS);
					gtk_entry_set_max_length (GTK_ENTRY (widget), 5);
					gtk_entry_set_width_chars (GTK_ENTRY (widget), 5);
					gtk_entry_set_alignment (GTK_ENTRY (widget), 0.5);
					g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (digit_key_press), NULL);
					g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (update_notification_tcp_port_entry_activate), entry_buffer);
					gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
				gtk_box_pack_end (GTK_BOX (box3), box5, FALSE, FALSE, 0);

				box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_set_margin_bottom (box4, SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("Envoyer les tallies aux caméras via IP :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					widget = gtk_check_button_new ();
					gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), send_ip_tally);
					g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (send_ip_tally_check_button_toggled), NULL);
					gtk_box_pack_end (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_box_pack_end (GTK_BOX (box3), box4, FALSE, FALSE, 0);
			gtk_container_add (GTK_CONTAINER (frame1), box3);
			gtk_box_pack_start (GTK_BOX (box2), frame1, FALSE, FALSE, 0);

			frame1 = gtk_frame_new ("Réglages communs aux AW-HE130");
			gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
			gtk_container_set_border_width (GTK_CONTAINER (frame1), SETTINGS_MARGIN_VALUE);
			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_container_set_border_width (GTK_CONTAINER (box3), SETTINGS_MARGIN_VALUE);
				for (i = 0; i < NB_SETTINGS_AW_HE130; i++) {
					box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
					gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_tooltip_text (box5, settings_array_AW_HE130[i].tooltip);
						widget =  gtk_label_new (settings_array_AW_HE130[i].name);
						gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

						widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
						gtk_widget_set_sensitive (widget, FALSE);
#else
						for (j = 0; j < settings_array_AW_HE130[i].nb_parameters; j++)
							gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_HE130[i].parameters[j].label);
						gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_HE130[i]);
						gtk_widget_set_sensitive (widget, settings_array_AW_HE130[i].sensitive);
						g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_HE130), settings_parameters_indexes_array_AW_HE130 + i);
#endif
						settings_array_AW_HE130[i].combo_box = widget;
						gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
					gtk_box_pack_start (GTK_BOX (box3), box5, FALSE, FALSE, 0);
				}

#ifndef MAIN_SETTINGS_READ_ONLY
				if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) gtk_widget_set_sensitive (COLOR_BAR_SETUP_LEVEL_AW_HE130_COMBO_BOX, FALSE);
				else gtk_widget_set_sensitive (COLOR_BAR_SETUP_LEVEL_AW_HE130_COMBO_BOX, TRUE);
#endif

				frame2 = gtk_frame_new ("OSD Mix");
				gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
				box5 = gtk_grid_new ();
				gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
				gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
				gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
				gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
					widget = gtk_label_new ("SDI");
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 0, 0, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
					osd_mix_sdi_toggle_button_AW_HE130 = gtk_toggle_button_new ();
					gtk_widget_set_sensitive (osd_mix_sdi_toggle_button_AW_HE130, FALSE);
					gtk_widget_set_margin_start (osd_mix_sdi_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (osd_mix_sdi_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), osd_mix_sdi_toggle_button_AW_HE130, 1, 0, 1, 1);
#else
					if (osd_mix_AW_HE130 & 0x01) {
						widget = gtk_toggle_button_new_with_label ("On");
						gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
					} else widget = gtk_toggle_button_new_with_label ("Off");
					g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_HE130), GINT_TO_POINTER (0x01));
					gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 1, 0, 1, 1);
#endif
					widget = gtk_label_new ("HDMI");
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 0, 1, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
					osd_mix_hdmi_toggle_button_AW_HE130 = gtk_toggle_button_new ();
					gtk_widget_set_sensitive (osd_mix_hdmi_toggle_button_AW_HE130, FALSE);
					gtk_widget_set_margin_start (osd_mix_hdmi_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (osd_mix_hdmi_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), osd_mix_hdmi_toggle_button_AW_HE130, 1, 1, 1, 1);
#else
					if (osd_mix_AW_HE130 & 0x02) {
						widget = gtk_toggle_button_new_with_label ("On");
						gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
					} else widget = gtk_toggle_button_new_with_label ("Off");
					g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_HE130), GINT_TO_POINTER (0x02));
					gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 1, 1, 1, 1);
#endif
					widget = gtk_label_new ("Video");
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 0, 2, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
					osd_mix_video_toggle_button_AW_HE130 = gtk_toggle_button_new ();
					gtk_widget_set_sensitive (osd_mix_video_toggle_button_AW_HE130, FALSE);
					gtk_widget_set_margin_start (osd_mix_video_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (osd_mix_video_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), osd_mix_video_toggle_button_AW_HE130, 1, 2, 1, 1);
#else
					if (osd_mix_AW_HE130 & 0x08) {
						widget = gtk_toggle_button_new_with_label ("On");
						gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
					} else widget = gtk_toggle_button_new_with_label ("Off");
					g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_HE130), GINT_TO_POINTER (0x08));
					gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 1, 2, 1, 1);
#endif
					widget = gtk_label_new ("IP");
					gtk_grid_attach (GTK_GRID (box5), widget, 0, 3, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
					osd_mix_ip_toggle_button_AW_HE130 = gtk_toggle_button_new ();
					gtk_widget_set_sensitive (osd_mix_ip_toggle_button_AW_HE130, FALSE);
					gtk_widget_set_margin_start (osd_mix_ip_toggle_button_AW_HE130, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), osd_mix_ip_toggle_button_AW_HE130, 1, 3, 1, 1);
#else
					if (osd_mix_AW_HE130 & 0x10) {
						widget = gtk_toggle_button_new_with_label ("On");
						gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
					} else widget = gtk_toggle_button_new_with_label ("Off");
					g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_HE130), GINT_TO_POINTER (0x10));
					gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
					gtk_grid_attach (GTK_GRID (box5), widget, 1, 3, 1, 1);
#endif
				gtk_container_add (GTK_CONTAINER (frame2), box5);
				gtk_box_pack_start (GTK_BOX (box3), frame2, FALSE, FALSE, 0);
			gtk_container_add (GTK_CONTAINER (frame1), box3);
			gtk_box_pack_start (GTK_BOX (box2), frame1, FALSE, FALSE, 0);

			frame1 = gtk_frame_new ("Réglages communs aux AW-UE150");
			gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
			gtk_container_set_border_width (GTK_CONTAINER (frame1), SETTINGS_MARGIN_VALUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
					for (i = 0; i < 8; i++) {
						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box5, settings_array_AW_UE150[i].tooltip);
							widget =  gtk_label_new (settings_array_AW_UE150[i].name);
							gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

							widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
							gtk_widget_set_sensitive (widget, FALSE);
#else
							for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++)
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_UE150[i].parameters[j].label);
							gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_UE150[i]);
							gtk_widget_set_sensitive (widget, settings_array_AW_UE150[i].sensitive);
							g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_UE150), settings_parameters_indexes_array_AW_UE150 + i);
#endif
							settings_array_AW_UE150[i].combo_box = widget;
							gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);
					}

					frame2 = gtk_frame_new ("OSD Mix");
					gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
					box5 = gtk_grid_new ();
					gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
						widget = gtk_label_new ("3G SDI");
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 0, 0, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
						osd_mix_3g_sdi_toggle_button_AW_UE150 = gtk_toggle_button_new ();
						gtk_widget_set_sensitive (osd_mix_3g_sdi_toggle_button_AW_UE150, FALSE);
						gtk_widget_set_margin_start (osd_mix_3g_sdi_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (osd_mix_3g_sdi_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), osd_mix_3g_sdi_toggle_button_AW_UE150, 1, 0, 1, 1);
#else
						if (osd_mix_AW_UE150 & 0x01) {
							widget = gtk_toggle_button_new_with_label ("On");
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						} else widget = gtk_toggle_button_new_with_label ("Off");
						g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_UE150), GINT_TO_POINTER (0x01));
						gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 1, 0, 1, 1);
#endif
						widget = gtk_label_new ("HDMI");
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 0, 1, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
						osd_mix_hdmi_toggle_button_AW_UE150 = gtk_toggle_button_new ();
						gtk_widget_set_sensitive (osd_mix_hdmi_toggle_button_AW_UE150, FALSE);
						gtk_widget_set_margin_start (osd_mix_hdmi_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (osd_mix_hdmi_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), osd_mix_hdmi_toggle_button_AW_UE150, 1, 1, 1, 1);
#else
						if (osd_mix_AW_UE150 & 0x02) {
							widget = gtk_toggle_button_new_with_label ("On");
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						} else widget = gtk_toggle_button_new_with_label ("Off");
						g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_UE150), GINT_TO_POINTER (0x02));
						gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 1, 1, 1, 1);
#endif
						widget = gtk_label_new ("IP");
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 0, 2, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
						osd_mix_ip_toggle_button_AW_UE150 = gtk_toggle_button_new ();
						gtk_widget_set_sensitive (osd_mix_ip_toggle_button_AW_UE150, FALSE);
						gtk_widget_set_margin_start (osd_mix_ip_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (osd_mix_ip_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), osd_mix_ip_toggle_button_AW_UE150, 1, 2, 1, 1);
#else
						if (osd_mix_AW_UE150 & 0x10) {
							widget = gtk_toggle_button_new_with_label ("On");
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						} else widget = gtk_toggle_button_new_with_label ("Off");
						g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_UE150), GINT_TO_POINTER (0x10));
						gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 1, 2, 1, 1);
#endif
						widget = gtk_label_new ("12G SDI/OPTICAL");
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 0, 3, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
						osd_mix_12g_sdi_optical_toggle_button_AW_UE150 = gtk_toggle_button_new ();
						gtk_widget_set_sensitive (osd_mix_12g_sdi_optical_toggle_button_AW_UE150, FALSE);
						gtk_widget_set_margin_start (osd_mix_12g_sdi_optical_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (osd_mix_12g_sdi_optical_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), osd_mix_12g_sdi_optical_toggle_button_AW_UE150, 1, 3, 1, 1);
#else
						if (osd_mix_AW_UE150 & 0x20) {
							widget = gtk_toggle_button_new_with_label ("On");
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						} else widget = gtk_toggle_button_new_with_label ("Off");
						g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_UE150), GINT_TO_POINTER (0x20));
						gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 1, 3, 1, 1);
#endif
						widget = gtk_label_new ("MONI");
						gtk_grid_attach (GTK_GRID (box5), widget, 0, 4, 1, 1);
#ifdef MAIN_SETTINGS_READ_ONLY
						osd_mix_moni_toggle_button_AW_UE150 = gtk_toggle_button_new ();
						gtk_widget_set_sensitive (osd_mix_moni_toggle_button_AW_UE150, FALSE);
						gtk_widget_set_margin_start (osd_mix_moni_toggle_button_AW_UE150, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), osd_mix_moni_toggle_button_AW_UE150, 1, 4, 1, 1);
#else
						if (osd_mix_AW_UE150 & 0x40) {
							widget = gtk_toggle_button_new_with_label ("On");
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						} else widget = gtk_toggle_button_new_with_label ("Off");
						g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (osd_mix_button_toggled_AW_UE150), GINT_TO_POINTER (0x40));
						gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_grid_attach (GTK_GRID (box5), widget, 1, 4, 1, 1);
#endif
					gtk_container_add (GTK_CONTAINER (frame2), box5);
					gtk_box_pack_start (GTK_BOX (box4), frame2, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box4, FALSE, FALSE, 0);

				box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_end (box4, SETTINGS_MARGIN_VALUE);
				gtk_widget_set_margin_bottom (box4, SETTINGS_MARGIN_VALUE);
					frame2 = gtk_frame_new ("12G SDI/OPTICAL");
					gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
					box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
					for (i = 8; i < 12; i++) {
						box6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box6, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box6, settings_array_AW_UE150[i].tooltip);
							widget =  gtk_label_new (settings_array_AW_UE150[i].name);
							gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_start (GTK_BOX (box6), widget, FALSE, FALSE, 0);

							widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
							gtk_widget_set_sensitive (widget, FALSE);
#else
							for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++)
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_UE150[i].parameters[j].label);
							gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_UE150[i]);
							gtk_widget_set_sensitive (widget, settings_array_AW_UE150[i].sensitive);
							g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_UE150), settings_parameters_indexes_array_AW_UE150 + i);
#endif
							settings_array_AW_UE150[i].combo_box = widget;
							gtk_box_pack_end (GTK_BOX (box6), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box5), box6, FALSE, FALSE, 0);
					}
					gtk_container_add (GTK_CONTAINER (frame2), box5);
					gtk_box_pack_start (GTK_BOX (box4), frame2, FALSE, FALSE, 0);

					frame2 = gtk_frame_new ("3G SDI");
					gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
					box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
					for (i = 12; i < 16; i++) {
						box6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box6, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box6, settings_array_AW_UE150[i].tooltip);
							widget =  gtk_label_new (settings_array_AW_UE150[i].name);
							gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_start (GTK_BOX (box6), widget, FALSE, FALSE, 0);

							widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
							gtk_widget_set_sensitive (widget, FALSE);
#else
							for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++)
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_UE150[i].parameters[j].label);
							gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_UE150[i]);
							gtk_widget_set_sensitive (widget, settings_array_AW_UE150[i].sensitive);
							g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_UE150), settings_parameters_indexes_array_AW_UE150 + i);
#endif
							settings_array_AW_UE150[i].combo_box = widget;
							gtk_box_pack_end (GTK_BOX (box6), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box5), box6, FALSE, FALSE, 0);
					}
					gtk_container_add (GTK_CONTAINER (frame2), box5);
					gtk_box_pack_start (GTK_BOX (box4), frame2, FALSE, FALSE, 0);

					frame2 = gtk_frame_new ("MONI");
					gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
					box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
					for (i = 16; i < 19; i++) {
						box6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box6, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box6, settings_array_AW_UE150[i].tooltip);
							widget =  gtk_label_new (settings_array_AW_UE150[i].name);
							gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_start (GTK_BOX (box6), widget, FALSE, FALSE, 0);

							widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
							gtk_widget_set_sensitive (widget, FALSE);
#else
							for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++)
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_UE150[i].parameters[j].label);
							gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_UE150[i]);
							gtk_widget_set_sensitive (widget, settings_array_AW_UE150[i].sensitive);
							g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_UE150), settings_parameters_indexes_array_AW_UE150 + i);
#endif
							settings_array_AW_UE150[i].combo_box = widget;
							gtk_box_pack_end (GTK_BOX (box6), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box5), box6, FALSE, FALSE, 0);
					}
					gtk_container_add (GTK_CONTAINER (frame2), box5);
					gtk_box_pack_start (GTK_BOX (box4), frame2, FALSE, FALSE, 0);

					frame2 = gtk_frame_new ("HDMI");
					gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
					box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_widget_set_halign (box5, GTK_ALIGN_CENTER);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
					gtk_widget_set_margin_end (box5, SETTINGS_MARGIN_VALUE);
					for (i = 19; i < NB_SETTINGS_AW_UE150; i++) {
						box6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box6, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box6, settings_array_AW_UE150[i].tooltip);
							widget =  gtk_label_new (settings_array_AW_UE150[i].name);
							gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_start (GTK_BOX (box6), widget, FALSE, FALSE, 0);

							widget = gtk_combo_box_text_new ();
#ifdef MAIN_SETTINGS_READ_ONLY
							gtk_widget_set_sensitive (widget, FALSE);
#else
							for (j = 0; j < settings_array_AW_UE150[i].nb_parameters; j++)
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), settings_array_AW_UE150[i].parameters[j].label);
							gtk_combo_box_set_active (GTK_COMBO_BOX (widget), settings_parameters_indexes_array_AW_UE150[i]);
							gtk_widget_set_sensitive (widget, settings_array_AW_UE150[i].sensitive);
							g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (parameter_changed_AW_UE150), settings_parameters_indexes_array_AW_UE150 + i);
#endif
							settings_array_AW_UE150[i].combo_box = widget;
							gtk_box_pack_end (GTK_BOX (box6), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box5), box6, FALSE, FALSE, 0);
					}
					gtk_container_add (GTK_CONTAINER (frame2), box5);
					gtk_box_pack_start (GTK_BOX (box4), frame2, FALSE, FALSE, 0);
				gtk_box_pack_start (GTK_BOX (box3), box4, FALSE, FALSE, 0);
			gtk_container_add (GTK_CONTAINER (frame1), box3);
			gtk_box_pack_start (GTK_BOX (box2), frame1, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_container_set_border_width (GTK_CONTAINER (box3), SETTINGS_MARGIN_VALUE);
				frame1 = gtk_frame_new (cameras_set_label);
				gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
				box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
					settings_list_box = gtk_list_box_new ();
					gtk_widget_set_size_request (settings_list_box, 100, 100);
					g_signal_connect (G_OBJECT (settings_list_box), "row-selected", G_CALLBACK (settings_list_box_row_selected), NULL);
					gtk_box_pack_start (GTK_BOX (box4), settings_list_box, TRUE, TRUE, 0);

					box5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_widget_set_margin_start (box5, SETTINGS_MARGIN_VALUE);
						settings_configuration_button = gtk_button_new_with_label ("Configuration");
						gtk_widget_set_margin_bottom (settings_configuration_button, SETTINGS_MARGIN_VALUE);
						g_signal_connect (G_OBJECT (settings_configuration_button), "clicked", G_CALLBACK (show_cameras_set_configuration_window), NULL);
						if (number_of_cameras_sets == 0) gtk_widget_set_sensitive (settings_configuration_button, FALSE);
						gtk_box_pack_start (GTK_BOX (box5), settings_configuration_button, FALSE, FALSE, 0);

						settings_new_button = gtk_button_new_with_label ("Nouveau");
						gtk_widget_set_margin_bottom (settings_new_button, SETTINGS_MARGIN_VALUE);
						g_signal_connect (G_OBJECT (settings_new_button), "clicked", G_CALLBACK (add_cameras_set), NULL);
						if (number_of_cameras_sets == MAX_CAMERAS_SET) gtk_widget_set_sensitive (settings_new_button, FALSE);
						gtk_box_pack_start (GTK_BOX (box5), settings_new_button, FALSE, FALSE, 0);

						settings_delete_button = gtk_button_new_with_label ("Supprimer");
						g_signal_connect (G_OBJECT (settings_delete_button), "clicked", G_CALLBACK (show_delete_confirmation_window), NULL);
						if (number_of_cameras_sets == 0) gtk_widget_set_sensitive (settings_delete_button, FALSE);
						gtk_box_pack_start (GTK_BOX (box5), settings_delete_button, FALSE, FALSE, 0);
					gtk_box_pack_end (GTK_BOX (box4), box5, FALSE, FALSE, 0);
				gtk_container_add (GTK_CONTAINER (frame1), box4);
				gtk_box_pack_start (GTK_BOX (box3), frame1, TRUE, TRUE, 0);

				frame1 = gtk_frame_new ("Connexion avec la régie");
				gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
					box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
							widget =  gtk_button_new_with_label ("Grille Snell SW-P-08");
							g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (show_matrix_window), NULL);
							gtk_box_set_center_widget (GTK_BOX (box5), widget);
						gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);

						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_tooltip_text (box5, my_ip_address);
							ip_radio_button = gtk_radio_button_new_with_label (NULL, "Port TCP/IP :");
							gtk_box_pack_start (GTK_BOX (box5), ip_radio_button, FALSE, FALSE, 0);

							entry_buffer = gtk_entry_buffer_new (label, sprintf (label, "%hu", ntohs (sw_p_08_address.sin_port)));
							tcp_port_entry = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (entry_buffer));
							gtk_entry_set_input_purpose (GTK_ENTRY (tcp_port_entry), GTK_INPUT_PURPOSE_DIGITS);
							gtk_entry_set_max_length (GTK_ENTRY (tcp_port_entry), 5);
							gtk_entry_set_width_chars (GTK_ENTRY (tcp_port_entry), 5);
							gtk_entry_set_alignment (GTK_ENTRY (tcp_port_entry), 0.5);
							gtk_widget_set_margin_start (tcp_port_entry, SETTINGS_MARGIN_VALUE);
							g_signal_connect (G_OBJECT (tcp_port_entry), "key-press-event", G_CALLBACK (digit_key_press), NULL);
							g_signal_connect (G_OBJECT (tcp_port_entry), "activate", G_CALLBACK (sw_p_08_tcp_port_entry_activate), entry_buffer);
							gtk_box_pack_end (GTK_BOX (box5), tcp_port_entry, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);

						ip_waiting_label = gtk_label_new ("En attente de connexion");
						gtk_widget_set_margin_bottom (ip_waiting_label, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box4), ip_waiting_label, FALSE, FALSE, 0);

						remote_devices[0].connected_label = gtk_label_new (NULL);
						gtk_widget_set_margin_bottom (remote_devices[0].connected_label, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box4), remote_devices[0].connected_label, FALSE, FALSE, 0);

						remote_devices[1].connected_label = gtk_label_new (NULL);
						gtk_widget_set_margin_bottom (remote_devices[1].connected_label, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box4), remote_devices[1].connected_label, FALSE, FALSE, 0);

						rs_port_gslist = list_rs_port ();

						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_tooltip_text (box5, "38400 baud, 8 bit data, no parity, one stop bit");
							rs_radio_button = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (ip_radio_button), "Port RS :");
							gtk_box_pack_start (GTK_BOX (box5), rs_radio_button, FALSE, FALSE, 0);

							rs_combo_box_text = gtk_combo_box_text_new ();
							gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rs_combo_box_text), " ");
							for (gslist_itr = rs_port_gslist, j = 1; gslist_itr != NULL; gslist_itr = gslist_itr->next, j++) {
								gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (rs_combo_box_text), gslist_itr->data);
								if (strcmp (gslist_itr->data, rs_port_name) == 0) gtk_combo_box_set_active (GTK_COMBO_BOX (rs_combo_box_text), j);
							}
							g_signal_connect (G_OBJECT (rs_combo_box_text), "changed", G_CALLBACK (rs_port_changed), NULL);
							gtk_widget_set_margin_start (rs_combo_box_text, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_end (GTK_BOX (box5), rs_combo_box_text, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);

						g_slist_free_full (rs_port_gslist, g_free);

						if (ip_rs) gtk_widget_set_sensitive (rs_combo_box_text, FALSE);
						else {
							gtk_widget_set_sensitive (tcp_port_entry, FALSE);
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rs_radio_button), TRUE);
						}
						g_signal_connect (G_OBJECT (ip_radio_button), "toggled", G_CALLBACK (ip_radio_button_toggled), NULL);
						g_signal_connect (G_OBJECT (rs_radio_button), "toggled", G_CALLBACK (rs_radio_button_toggled), NULL);

						rs_ok_label = gtk_label_new ("Liaison OK");
						gtk_widget_set_margin_top (rs_ok_label, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_end (GTK_BOX (box4), rs_ok_label, FALSE, FALSE, 0);
					gtk_container_add (GTK_CONTAINER (frame1), box4);
				gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);

				frame1 = gtk_frame_new ("Tally");
				gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
				box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("TSL UMD V5 Port UDP/IP :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					entry_buffer = gtk_entry_buffer_new (label, sprintf (label, "%hu", ntohs (tsl_umd_v5_address.sin_port)));
					widget = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (entry_buffer));
					gtk_entry_set_input_purpose (GTK_ENTRY (widget), GTK_INPUT_PURPOSE_DIGITS);
					gtk_entry_set_max_length (GTK_ENTRY (widget), 5);
					gtk_entry_set_width_chars (GTK_ENTRY (widget), 5);
					gtk_entry_set_alignment (GTK_ENTRY (widget), 0.5);
					g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (digit_key_press), NULL);
					g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (tsl_umd_v5_udp_port_entry_activate), entry_buffer);
					gtk_box_pack_end (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_container_add (GTK_CONTAINER (frame1), box4);
				gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);

				frame1 = gtk_frame_new ("RCP Panasonic AK-HRP200");
				gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
				box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
					widget =  gtk_label_new ("Port TCP/IP :");
					gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0);

					entry_buffer = gtk_entry_buffer_new (label, sprintf (label, "%hu", ntohs (physical_rcp.address.sin_port)));
					widget = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER (entry_buffer));
					gtk_entry_set_input_purpose (GTK_ENTRY (widget), GTK_INPUT_PURPOSE_DIGITS);
					gtk_entry_set_max_length (GTK_ENTRY (widget), 5);
					gtk_entry_set_width_chars (GTK_ENTRY (widget), 5);
					gtk_entry_set_alignment (GTK_ENTRY (widget), 0.5);
					g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (digit_key_press), NULL);
					g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (physical_rcp_tcp_port_entry_activate), entry_buffer);
					gtk_box_pack_end (GTK_BOX (box4), widget, FALSE, FALSE, 0);
				gtk_container_add (GTK_CONTAINER (frame1), box4);
				gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);

				frame1 = gtk_frame_new ("Interface");
				gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
					box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
					gtk_container_set_border_width (GTK_CONTAINER (box4), SETTINGS_MARGIN_VALUE);
						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
							widget = gtk_label_new ("Thème :");
						gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

							light_radio_button = gtk_radio_button_new_with_label (NULL, "Clair");
							gtk_widget_set_margin_start (light_radio_button, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box5), light_radio_button, FALSE, FALSE, 0);

							widget = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (light_radio_button), "Sombre");
							gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
						gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

						if (!theme) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
						g_signal_connect (G_OBJECT (light_radio_button), "toggled", G_CALLBACK (theme_radio_button_toggled), NULL);
					gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);

						box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
						gtk_widget_set_margin_top (box5, SETTINGS_MARGIN_VALUE);
						gtk_widget_set_margin_bottom (box5, SETTINGS_MARGIN_VALUE);
							widget =  gtk_label_new ("Afficher le RCP maître");
							gtk_box_pack_start (GTK_BOX (box5), widget, FALSE, FALSE, 0);

							widget =  gtk_check_button_new ();
							gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), show_master_rcp);
							g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (show_master_rcp_check_button_toggled), NULL);
							gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
							gtk_box_pack_end (GTK_BOX (box5), widget, FALSE, FALSE, 0);
						gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);
					gtk_container_add (GTK_CONTAINER (frame1), box4);
				gtk_box_pack_start (GTK_BOX (box3), frame1, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_container_set_border_width (GTK_CONTAINER (box3), SETTINGS_MARGIN_VALUE);
				widget = gtk_button_new_with_label ("A propos");
				g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (show_about_window), NULL);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_end (GTK_BOX (box2), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, FALSE, 0);

		widget = gtk_label_new (key_info_2_txt);
		gtk_box_pack_end (GTK_BOX (box1), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (settings_page), box1);

#ifndef MAIN_SETTINGS_READ_ONLY
	if (settings_parameters_indexes_array[WHITE_CLIP_INDEX] == 1) gtk_widget_set_sensitive (settings_array[WHITE_CLIP_LEVEL_INDEX].combo_box, TRUE);
	else gtk_widget_set_sensitive (settings_array[WHITE_CLIP_LEVEL_INDEX].combo_box, FALSE);
#endif

	widget = gtk_label_new ("Paramètres");
	gtk_notebook_append_page (GTK_NOTEBOOK (main_window_notebook), settings_page, widget);
	gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (main_window_notebook), settings_page, FALSE);
}

void load_settings_from_config_file (void)
{
	int i;

	for (i = 0; i < NB_SETTINGS; i++) settings_parameters_indexes_array[i] = settings_array[i].default_value;
	for (i = 0; i < NB_SETTINGS_AW_HE130; i++) settings_parameters_indexes_array_AW_HE130[i] = settings_array_AW_HE130[i].default_value;
	for (i = 0; i < NB_SETTINGS_AW_UE150; i++) settings_parameters_indexes_array_AW_UE150[i] = settings_array_AW_UE150[i].default_value;

	config_file = fopen (config_file_name, "rb");
	if (config_file == NULL) return;

#ifdef MAIN_SETTINGS_READ_ONLY
	fseek (config_file, sizeof (int) * (NB_SETTINGS + 2), SEEK_SET);	//settings_parameters_indexes_array[NB_SETTINGS] + status_lamp + tally_input

	fread (&send_ip_tally, sizeof (gboolean), 1, config_file);

	fseek (config_file, sizeof (int), SEEK_CUR);	//picture_level

	fread (&update_notification_address.sin_port, sizeof (guint16), 1, config_file);
	if (ntohs (update_notification_address.sin_port) < 1024) update_notification_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);

	fseek (config_file, sizeof (int) * (NB_SETTINGS_AW_HE130 + 1 + NB_SETTINGS_AW_UE150 + 1), SEEK_CUR);	//settings_parameters_indexes_array_AW_HE130[NB_SETTINGS_AW_HE130] + osd_mix_AW_HE130 + settings_parameters_indexes_array_AW_UE150[NB_SETTINGS_AW_UE150] + osd_mix_AW_UE150
#else
	fread (settings_parameters_indexes_array, sizeof (int), NB_SETTINGS, config_file);

	for (i = 0; i < NB_SETTINGS; i++) {
		if (!settings_array[i].sensitive) settings_parameters_indexes_array[i] = settings_array[i].default_value;
		else if ((settings_parameters_indexes_array[i] < 0) || (settings_parameters_indexes_array[i] >= settings_array[i].nb_parameters))
			settings_parameters_indexes_array[i] = settings_array[i].default_value;
	}

	fread (&status_lamp, sizeof (int), 1, config_file);
	if ((status_lamp != 0) && (status_lamp != 1)) status_lamp = 1;

	fread (&tally_input, sizeof (int), 1, config_file);
	if ((tally_input != 0) && (tally_input != 1)) tally_input = 1;

	fread (&send_ip_tally, sizeof (gboolean), 1, config_file);

	fread (&picture_level, sizeof (int), 1, config_file);
	if ((picture_level < -50) || (picture_level > +50)) picture_level = 0;

	fread (&update_notification_address.sin_port, sizeof (guint16), 1, config_file);
	if (ntohs (update_notification_address.sin_port) < 1024) update_notification_address.sin_port = htons (UPDATE_NOTIFICATION_TCP_PORT);

//AW_HE130
	fread (settings_parameters_indexes_array_AW_HE130, sizeof (int), NB_SETTINGS_AW_HE130, config_file);

	for (i = 0; i < NB_SETTINGS_AW_HE130; i++) {
		if (!settings_array_AW_HE130[i].sensitive) settings_parameters_indexes_array_AW_HE130[i] = settings_array_AW_HE130[i].default_value;
		else if ((settings_parameters_indexes_array_AW_HE130[i] < 0) || (settings_parameters_indexes_array_AW_HE130[i] >= settings_array_AW_HE130[i].nb_parameters))
			settings_parameters_indexes_array_AW_HE130[i] = settings_array_AW_HE130[i].default_value;
	}

	if (settings_parameters_indexes_array_AW_HE130[FREQUENCY_INDEX_AW_HE130] == 1) {
		if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3)) output_fps_AW_HE130 = fps_50p;
		else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 5)) output_fps_AW_HE130 = fps_25p;
	} else {
		settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].nb_parameters = 8;
		settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].parameters = format_parameters_array_ntsc_AW_HE130;
		settings_array_AW_HE130[FORMAT_INDEX_AW_HE130].answers = format_answers_array_ntsc_AW_HE130;

		if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 0) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 4)) output_fps_AW_HE130 = fps_59_94p;
		else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 2) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 6)) output_fps_AW_HE130 = fps_29_97p;
		else if ((settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 3) || (settings_parameters_indexes_array_AW_HE130[FORMAT_INDEX_AW_HE130] == 7)) output_fps_AW_HE130 = fps_23_98p;
		else output_fps_AW_HE130 = fps_59_94i;
	}

	fread (&osd_mix_AW_HE130, sizeof (int), 1, config_file);
	osd_mix_AW_HE130 &= 0x1B;

//AW_UE150
	fread (settings_parameters_indexes_array_AW_UE150, sizeof (int), NB_SETTINGS_AW_UE150, config_file);

	for (i = 0; i < NB_SETTINGS_AW_UE150; i++) {
		if (!settings_array_AW_UE150[i].sensitive) settings_parameters_indexes_array_AW_UE150[i] = settings_array_AW_UE150[i].default_value;
		else if ((settings_parameters_indexes_array_AW_UE150[i] < 0) || (settings_parameters_indexes_array_AW_UE150[i] >= settings_array_AW_UE150[i].nb_parameters))
			settings_parameters_indexes_array_AW_UE150[i] = settings_array_AW_UE150[i].default_value;
	}

	update_outputs_parameters_AW_UE150 ();

	fread (&osd_mix_AW_UE150, sizeof (int), 1, config_file);
	osd_mix_AW_UE150 &= 0x73;
#endif

	fread (&physical_rcp.address.sin_port, sizeof (guint16), 1, config_file);
	if ((ntohs (physical_rcp.address.sin_port) < 1024) || (physical_rcp.address.sin_port == update_notification_address.sin_port))
		physical_rcp.address.sin_port = htons (PHYSICAL_RCP_TCP_PORT);

	fread (&physical_rcp.iris, sizeof (int), 1, config_file);
	if ((physical_rcp.iris < 0x555) || (physical_rcp.iris > 0xFFF)) physical_rcp.iris = IRIS_DEFAULT;

	fread (&physical_rcp.pedestal, sizeof (int), 1, config_file);
	if ((physical_rcp.pedestal < 0x000) || (physical_rcp.pedestal > 0x12C)) physical_rcp.pedestal = PEDESTAL_DEFAULT_AW_HE130;

	fread (&tsl_umd_v5_address.sin_port, sizeof (guint16), 1, config_file);

	fread (&ip_rs, sizeof (gboolean), 1, config_file);

	fread (&sw_p_08_address.sin_port, sizeof (guint16), 1, config_file);
	if ((ntohs (sw_p_08_address.sin_port) < 1024) || (sw_p_08_address.sin_port == update_notification_address.sin_port) || (sw_p_08_address.sin_port == physical_rcp.address.sin_port))
		sw_p_08_address.sin_port = htons (SW_P_08_TCP_PORT);

	fread (rs_port_name, sizeof (char), 16, config_file);
	rs_port_name[15] = '\0';

	fread (&theme, sizeof (gboolean), 1, config_file);

	fread (&show_master_rcp, sizeof (gboolean), 1, config_file);
}

void load_cameras_sets_from_config_file (void)
{
	int i, j;
	cameras_set_t *cameras_set_tmp;
	rcp_t *rcp;
	GtkWidget *box, *widget;
	GList *glist_itr;

	if (config_file == NULL) return;

	fread (&number_of_cameras_sets, sizeof (int), 1, config_file);

	if (number_of_cameras_sets < 0) number_of_cameras_sets = 0;
	if (number_of_cameras_sets > MAX_CAMERAS_SET) number_of_cameras_sets = 0;

	for (i = 0; i < number_of_cameras_sets; i++)
	{
		cameras_set_tmp = g_malloc (sizeof (cameras_set_t));

		cameras_set_tmp->next = cameras_sets;
		cameras_sets = cameras_set_tmp;

		fread (cameras_set_tmp->name, sizeof (char), CAMERAS_SET_NAME_LENGTH + 1, config_file);
		cameras_set_tmp->name[CAMERAS_SET_NAME_LENGTH] = '\0';

		fread (&cameras_set_tmp->number_of_cameras, sizeof (int), 1, config_file);
		if ((cameras_set_tmp->number_of_cameras < 1) || (cameras_set_tmp->number_of_cameras > MAX_CAMERAS)) cameras_set_tmp->number_of_cameras = 5;

		cameras_set_tmp->rcp_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

		cameras_set_tmp->source_rcp_list_box = gtk_list_box_new ();
		gtk_list_box_set_sort_func (GTK_LIST_BOX (cameras_set_tmp->source_rcp_list_box), (GtkListBoxSortFunc)rcp_list_box_sort, cameras_set_tmp, NULL);
		g_signal_connect (G_OBJECT (cameras_set_tmp->source_rcp_list_box), "row-selected", G_CALLBACK (source_rcp_list_box_row_selected), NULL);
		gtk_box_pack_start (GTK_BOX (source_rcp_list_box_box), cameras_set_tmp->source_rcp_list_box, FALSE, FALSE, 0);

		cameras_set_tmp->destination_rcp_list_box = gtk_list_box_new ();
		gtk_list_box_set_sort_func (GTK_LIST_BOX (cameras_set_tmp->destination_rcp_list_box), (GtkListBoxSortFunc)rcp_list_box_sort, cameras_set_tmp, NULL);
		g_signal_connect (G_OBJECT (cameras_set_tmp->destination_rcp_list_box), "row-selected", G_CALLBACK (destination_rcp_list_box_row_selected), NULL);
		gtk_box_pack_start (GTK_BOX (destination_rcp_list_box_box), cameras_set_tmp->destination_rcp_list_box, FALSE, FALSE, 0);

		for (j = 0; j < cameras_set_tmp->number_of_cameras; j++) {
			rcp = g_malloc (sizeof (rcp_t));
			cameras_set_tmp->cameras[j] = rcp;
			rcp->cameras_set = cameras_set_tmp;

			fread (rcp->name, sizeof (char), 3, config_file);
			rcp->name[2] = '\0';

			init_rcp (rcp);

			rcp->index = j;

			fread (&rcp->active, sizeof (gboolean), 1, config_file);

			fread (&rcp->model, sizeof (int), 1, config_file);

			if (rcp->active) {
				create_rcp_widgets (rcp);

				fread (rcp->ip_address, sizeof (char), 16, config_file);
				rcp->ip_address[15] = '\0';

				fread (rcp->scenes, sizeof (scene_t), NB_SCENES, config_file);

				rcp->address.sin_addr.s_addr = inet_addr (rcp->ip_address);

				if (rcp->address.sin_addr.s_addr == INADDR_NONE) {
					rcp->ip_address_is_valid = FALSE;
					rcp->ip_address[0] = '\0';

					gtk_widget_set_sensitive (rcp->on_standby_switch, FALSE);
					gtk_widget_set_sensitive (rcp->standard_button, FALSE);
					gtk_widget_set_sensitive (rcp->mire_toggle_button, FALSE);
					gtk_widget_set_sensitive (rcp->day_night_toggle_button, FALSE);
					gtk_widget_set_sensitive (rcp->sensitive_widgets, FALSE);

					cameras_set_with_error = cameras_set_tmp;
				} else {
					rcp->ip_address_is_valid = TRUE;

					for (glist_itr = rcp_glist; glist_itr != NULL; glist_itr = glist_itr->next) {
						if (strcmp (((rcp_t*)(glist_itr->data))->ip_address, rcp->ip_address) == 0) {
							rcp->other_rcp = g_slist_prepend (rcp->other_rcp, glist_itr->data);
							((rcp_t*)(glist_itr->data))->other_rcp = g_slist_prepend (((rcp_t*)(glist_itr->data))->other_rcp, rcp);
						}
					}

					if (rcp->other_rcp == NULL) rcp_start_glist = g_list_prepend (rcp_start_glist, rcp);
				}

				rcp_glist = g_list_prepend (rcp_glist, rcp);

				widget = gtk_label_new (rcp->name);
				rcp->source_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->source_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set_tmp->source_rcp_list_box), rcp->source_rcp_list_box_row);

				widget = gtk_label_new (rcp->name);
				rcp->destination_rcp_list_box_row = gtk_list_box_row_new ();
				gtk_container_add (GTK_CONTAINER (rcp->destination_rcp_list_box_row), widget);
				gtk_container_add (GTK_CONTAINER (cameras_set_tmp->destination_rcp_list_box), rcp->destination_rcp_list_box_row);
			} else {
				create_ghost_rcp_widgets (rcp);
				ghost_rcp_glist = g_list_prepend (ghost_rcp_glist, rcp);
			}

			gtk_box_pack_start (GTK_BOX (cameras_set_tmp->rcp_box), rcp->root_widget, FALSE, FALSE, 0);
		}

		init_master_rcp (cameras_set_tmp);

		widget = gtk_label_new (cameras_set_tmp->name);
		gtk_widget_set_margin_start (widget, SETTINGS_MARGIN_VALUE);
		gtk_widget_set_margin_end (widget, SETTINGS_MARGIN_VALUE);
		gtk_container_add (GTK_CONTAINER (settings_list_box), widget);

		cameras_set_tmp->page = gtk_scrolled_window_new (NULL, NULL);
		box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_box_pack_start (GTK_BOX (box), cameras_set_tmp->rcp_box, FALSE, FALSE, 0);
		widget = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		widget = gtk_label_new (key_info_1_txt);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (cameras_set_tmp->page), box);
		gtk_widget_show_all (cameras_set_tmp->page);

		widget = gtk_label_new (cameras_set_tmp->name);
		cameras_set_tmp->page_num = gtk_notebook_insert_page (GTK_NOTEBOOK (main_window_notebook), cameras_set_tmp->page, widget, i);
		gtk_notebook_set_tab_reorderable (GTK_NOTEBOOK (main_window_notebook), cameras_set_tmp->page, TRUE);

		widget = gtk_label_new (cameras_set_tmp->name);
		cameras_set_tmp->source_list_box_row = gtk_list_box_row_new ();
		gtk_container_add (GTK_CONTAINER (cameras_set_tmp->source_list_box_row), widget);
		gtk_container_add (GTK_CONTAINER (source_cameras_set_list_box), cameras_set_tmp->source_list_box_row);
		if (i == 0) {
			gtk_list_box_select_row (GTK_LIST_BOX (source_cameras_set_list_box), GTK_LIST_BOX_ROW (cameras_set_tmp->source_list_box_row));
			source_cameras_set = cameras_set_tmp;
		}

		widget = gtk_label_new (cameras_set_tmp->name);
		cameras_set_tmp->destination_list_box_row = gtk_list_box_row_new ();
		gtk_container_add (GTK_CONTAINER (cameras_set_tmp->destination_list_box_row), widget);
		gtk_container_add (GTK_CONTAINER (destination_cameras_set_list_box), cameras_set_tmp->destination_list_box_row);
		if (i == 0) {
			gtk_list_box_select_row (GTK_LIST_BOX (destination_cameras_set_list_box), GTK_LIST_BOX_ROW (cameras_set_tmp->destination_list_box_row));
			destination_cameras_set = cameras_set_tmp;
		}
	}

	if (number_of_cameras_sets != 0) rcp_start_glist = g_list_reverse (rcp_start_glist);

	fclose (config_file);
}

void save_settings_and_cameras_sets_to_config_file (void)
{
	int i, j;
	cameras_set_t *cameras_set_tmp;
	rcp_t *rcp;

	config_file = fopen (config_file_name, "wb");

	fwrite (settings_parameters_indexes_array, sizeof (int), NB_SETTINGS, config_file);

	fwrite (&status_lamp, sizeof (int), 1, config_file);

	fwrite (&tally_input, sizeof (int), 1, config_file);

	fwrite (&send_ip_tally, sizeof (gboolean), 1, config_file);

	fwrite (&picture_level, sizeof (int), 1, config_file);

	fwrite (&update_notification_address.sin_port, sizeof (guint16), 1, config_file);

	fwrite (settings_parameters_indexes_array_AW_HE130, sizeof (int), NB_SETTINGS_AW_HE130, config_file);

	fwrite (&osd_mix_AW_HE130, sizeof (int), 1, config_file);

	fwrite (settings_parameters_indexes_array_AW_UE150, sizeof (int), NB_SETTINGS_AW_UE150, config_file);

	fwrite (&osd_mix_AW_UE150, sizeof (int), 1, config_file);

	fwrite (&physical_rcp.address.sin_port, sizeof (guint16), 1, config_file);
	fwrite (&physical_rcp.iris, sizeof (int), 1, config_file);
	fwrite (&physical_rcp.pedestal, sizeof (int), 1, config_file);

	fwrite (&tsl_umd_v5_address.sin_port, sizeof (guint16), 1, config_file);

	fwrite (&ip_rs, sizeof (gboolean), 1, config_file);

	fwrite (&sw_p_08_address.sin_port, sizeof (guint16), 1, config_file);

	fwrite (rs_port_name, sizeof (char), 16, config_file);

	fwrite (&theme, sizeof (gboolean), 1, config_file);

	fwrite (&show_master_rcp, sizeof (gboolean), 1, config_file);

	fwrite (&number_of_cameras_sets, sizeof (int), 1, config_file);

	for (i = 0; i < number_of_cameras_sets; i++)
	{
		cameras_set_tmp = cameras_sets;
		while (cameras_set_tmp->page_num != i) cameras_set_tmp = cameras_set_tmp->next;

		fwrite (cameras_set_tmp->name, sizeof (char), CAMERAS_SET_NAME_LENGTH + 1, config_file);
		fwrite (&cameras_set_tmp->number_of_cameras, sizeof (int), 1, config_file);

		for (j = 0; j < cameras_set_tmp->number_of_cameras; j++) {
			rcp = cameras_set_tmp->cameras[j];

			fwrite (rcp->name, sizeof (char), 3, config_file);
			fwrite (&rcp->active, sizeof (gboolean), 1, config_file);

			fwrite (&rcp->model, sizeof (int), 1, config_file);

			if (rcp->active) {
				fwrite (rcp->ip_address, sizeof (char), 16, config_file);
				fwrite (rcp->scenes, sizeof (scene_t), NB_SCENES, config_file);
			}
		}
	}

	fclose (config_file);

	backup_needed = FALSE;
}

