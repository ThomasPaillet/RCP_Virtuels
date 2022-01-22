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


int color_temperature_array_AW_HE130[121] = { \
	2000, 2010, 2020, 2040, 2050, 2070, 2080, 2090, 2110, 2120, 2140, 2150, 2170, 2180, 2200, 2210, 2230, 2240, 2260, \
	2280, 2300, 2310, 2330, 2340, 2360, 2380, 2400, 2420, 2440, 2460, 2480, 2500, 2520, 2540, 2560, 2600, 2620, 2640, 2680, 2700, 2720, 2740, 2780, \
	2800, 2820, 2850, 2870, 2920, 2950, 2970, 3000, 3020, 3070, 3100, 3120, 3150, 3200, 3250, 3270, 3330, 3360, 3420, 3450, 3510, 3570, 3600, 3660, \
	3720, 3780, 3840, 3870, 3930, 3990, 4050, 4110, 4170, 4240, 4320, 4360, 4440, 4520, 4600, 4680, 4760, 4840, 4920, 5000, 5100, 5200, 5300, 5400, \
	5500, 5600, 5750, 5850, 6000, 6150, 6300, 6450, 6650, 6800, 7000, 7150, 7400, 7600, 7800, 8100, 8300, 8600, 8900, 9200, 9600, 10000, 10500, 11000, \
	11500, 12000, 12500, 13000, 14000, 15000 \
	};

int color_temperature_array_AW_UE150[401] = { \
	2000, 2005, 2010, 2015, 2020, 2025, 2030, 2035, 2040, 2045, 2050, 2055, 2060, 2065, 2070, 2075, 2080, 2085, 2090, 2095, 2100, 2105, 2110, 2115, 2120, 2125, 2130, 2135, 2140, 2145, 2150, 2155, 2160, 2165, 2170, 2175, 2180, 2185, 2190, 2195, 2200, 2205, 2210, 2215, 2220, 2225, 2230, 2235, 2240, 2245, 2250, 2255, 2260, 2265, 2270, 2275, 2280, 2285, 2290, 2295, 2300, 2305, 2310, 2315, 2320, 2325, 2330, 2335, 2340, 2345, 2350, 2355, 2360, 2365, 2370, 2375, 2380, 2385, 2390, 2395, 2400, 2405, 2410, 2415, 2420, 2425, 2430, 2435, 2440, 2445, 2450, 2455, 2460, 2465, 2470, 2475, 2480, 2485, 2490, 2495, 2500, 2505, 2510, 2515, 2520, 2525, 2530, 2535, 2540, 2545, 2550, 2555, 2560, 2565, 2570, 2575, 2580, 2585, 2590, 2595, 2600, 2605, 2610, 2615, 2620, 2625, 2630, 2635, 2640, 2645, 2650, 2655, 2660, 2665, 2670, 2675, 2680, 2685, 2690, 2695, 2700, 2705, 2710, 2715, 2720, 2725, 2730, 2735, 2740, 2745, 2750, 2755, 2760, 2765, 2770, 2775, 2780, 2785, 2790, 2795, 2800, 2805, 2810, 2815, 2820, 2825, 2830, 2835, 2840, 2845, 2850, 2855, 2860, 2865, 2870, 2875, 2880, 2885, 2890, 2895, \
	2900, 2910, 2920, 2930, 2940, 2950, 2960, 2970, 2980, 2990, \
	3000, 3020, 3040, 3060, 3080, 3100, 3120, 3140, 3160, 3180, 3200, 3220, 3240, 3260, 3280, 3300, 3320, 3340, 3360, 3380, 3400, 3420, 3440, 3460, 3480, 3500, 3520, 3540, 3560, 3580, 3600, 3620, 3640, 3660, 3680, 3700, 3720, 3740, 3760, 3780, 3800, 3820, 3840, 3860, 3880, 3900, 3920, 3940, 3960, 3980, 4000, 4020, 4040, 4060, 4080, 4100, 4120, 4140, 4160, 4180, 4200, 4220, 4240, 4260, 4280, 4300, 4320, 4340, 4360, 4380, 4400, 4420, 4440, 4460, 4480, 4500, 4520, 4540, 4560, 4580, 4600, 4620, 4640, 4660, 4680, 4700, 4720, 4740, 4760, 4780, 4800, 4820, 4840, 4860, 4880, 4900, 4920, 4940, 4960, 4980, 5000, 5020, 5040, 5060, 5080, 5100, 5120, 5140, 5160, 5180, 5200, 5220, 5240, 5260, 5280, 5300, 5320, 5340, 5360, 5380, 5400, 5420, 5440, 5460, 5480, 5500, 5520, 5540, 5560, 5580, 5600, 5620, 5640, 5660, 5680, 5700, 5720, 5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, 5900, 5920, 5940, 5960, 5980, \
	6000, 6100, 6200, 6300, 6400, 6500, 6600, 6700, 6800, 6900, 7000, 7100, 7200, 7300, 7400, 7500, 7600, 7700, 7800, 7900, 8000, 8100, 8200, 8300, 8400, 8500, 8600, 8700, 8800, 8900, 9000, 9100, 9200, 9300, 9400, 9500, 9600, 9700, 9800, 9900, \
	10000, 10250, 10500, 10750, 11000, 11250, 11500, 11750, 12000, 12250, 12500, 12750, 13000, 13250, 13500, 13750, 14000, 14250, 14500, 14750, 15000 \
	};


void set_color_temperature_AW_HE130 (rcp_t *rcp)
{
	send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, TRUE);
}

gboolean set_color_temperature_delayed_AW_HE130 (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, FALSE);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
}

void color_temperature_changed_AW_HE130 (GtkComboBox *color_temperature_combo_box, rcp_t *rcp)
{
	struct timeval current_time, elapsed_time;

	rcp->current_scene.color_temperature = gtk_combo_box_get_active (color_temperature_combo_box);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);

	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
		if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_color_temperature_delayed_AW_HE130, rcp);
	} else {
		if (rcp->timeout_id != 0) {
			g_source_remove (rcp->timeout_id);
			rcp->timeout_id = 0;
		}

		rcp->last_time = current_time;

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, FALSE);
	}
}

gboolean color_temperature_button_held_AW_HE130 (rcp_t *rcp)
{
	int index;

	index = rcp->current_scene.color_temperature + rcp->timeout_value;
	if (index > 120) index = 120;
	else if (index < 0) index = 0;

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		rcp->last_time.tv_usec += 130000;
		if (rcp->last_time.tv_usec >= 1000000) {
			rcp->last_time.tv_sec++;
			rcp->last_time.tv_usec -= 1000000;
		}

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, FALSE);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}

gboolean color_temperature_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature + 10;
		if (index > 120) index = 120;
		rcp->timeout_value = 10;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_HE130, rcp);
	}

	return FALSE;
}

gboolean color_temperature_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature + 1;
		if (index > 120) index = 120;
		rcp->timeout_value = 1;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_HE130, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature + 1;
		if (index > 120) index = 120;
		rcp->timeout_value = 1;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_HE130, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature + 10;
		if (index > 120) index = 120;
		rcp->timeout_value = 10;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		send_cam_control_command_3_digits (rcp, "OSD:B1:", 120 - rcp->current_scene.color_temperature, TRUE);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_HE130, rcp);
	}

	return FALSE;
}

GtkWidget *create_color_temperature_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;
	int i;
	char label[8];

	frame = gtk_frame_new ("Température de couleur");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.04, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		for (i = 120; i >= 0; i--) {
			sprintf (label, "%dK", color_temperature_array_AW_HE130[i]);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		rcp->color_temperature_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (color_temperature_changed_AW_HE130), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->color_temperature_combo_box = widget;

		widget = gtk_button_new_with_label ("--");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_10_button_pressed_AW_HE130), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("-");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_1_button_pressed_AW_HE130), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("+");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_1_button_pressed_AW_HE130), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("++");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_10_button_pressed_AW_HE130), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

int find_color_temperature_index (int color_temperature)
{
	int index = 0;

	while (color_temperature_array_AW_UE150[index] < color_temperature) index++;

	return 400 - index;
}

void set_color_temperature_AW_UE150 (rcp_t *rcp)
{
	int color_temperature_index;
	char color_temperature_cmd[] = "OSI:20:00000:0";

	color_temperature_index = 400 - rcp->current_scene.color_temperature;

	color_temperature_cmd[7] = '0' + (color_temperature_array_AW_UE150[color_temperature_index] >> 16);
	if (color_temperature_cmd[7] > '9') color_temperature_cmd[7] += 7;
	color_temperature_cmd[8] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x0FFFF) >> 12);
	if (color_temperature_cmd[8] > '9') color_temperature_cmd[8] += 7;
	color_temperature_cmd[9] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x00FFF) >> 8);
	if (color_temperature_cmd[9] > '9') color_temperature_cmd[9] += 7;
	color_temperature_cmd[10] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x000FF) >> 4);
	if (color_temperature_cmd[10] > '9') color_temperature_cmd[10] += 7;
	color_temperature_cmd[11] = '0' + (color_temperature_array_AW_UE150[color_temperature_index] & 0x0000F);
	if (color_temperature_cmd[11] > '9') color_temperature_cmd[11] += 7;

	send_cam_control_command (rcp, color_temperature_cmd);
}

void set_color_temperature_AW_UE150_now (rcp_t *rcp)
{
	int color_temperature_index;
	char color_temperature_cmd[] = "OSI:20:00000:0";

	color_temperature_index = 400 - rcp->current_scene.color_temperature;

	color_temperature_cmd[7] = '0' + (color_temperature_array_AW_UE150[color_temperature_index] >> 16);
	if (color_temperature_cmd[7] > '9') color_temperature_cmd[7] += 7;
	color_temperature_cmd[8] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x0FFFF) >> 12);
	if (color_temperature_cmd[8] > '9') color_temperature_cmd[8] += 7;
	color_temperature_cmd[9] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x00FFF) >> 8);
	if (color_temperature_cmd[9] > '9') color_temperature_cmd[9] += 7;
	color_temperature_cmd[10] = '0' + ((color_temperature_array_AW_UE150[color_temperature_index] & 0x000FF) >> 4);
	if (color_temperature_cmd[10] > '9') color_temperature_cmd[10] += 7;
	color_temperature_cmd[11] = '0' + (color_temperature_array_AW_UE150[color_temperature_index] & 0x0000F);
	if (color_temperature_cmd[11] > '9') color_temperature_cmd[11] += 7;

	send_cam_control_command_now (rcp, color_temperature_cmd);
}

gboolean set_color_temperature_delayed_AW_UE150 (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	set_color_temperature_AW_UE150_now (rcp);

	rcp->timeout_id = 0;
	return G_SOURCE_REMOVE;
}

void color_temperature_changed_AW_UE150 (GtkComboBox *color_temperature_combo_box, rcp_t *rcp)
{
	struct timeval current_time, elapsed_time;

	rcp->current_scene.color_temperature = gtk_combo_box_get_active (color_temperature_combo_box);

	gettimeofday (&current_time, NULL);
	timersub (&current_time, &rcp->last_time, &elapsed_time);

	if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
		if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_color_temperature_delayed_AW_UE150, rcp);
	} else {
		if (rcp->timeout_id != 0) {
			g_source_remove (rcp->timeout_id);
			rcp->timeout_id = 0;
		}

		rcp->last_time = current_time;

		set_color_temperature_AW_UE150_now (rcp);
	}
}

gboolean color_temperature_button_held_AW_UE150 (rcp_t *rcp)
{
	int index;

	index = rcp->current_scene.color_temperature + rcp->timeout_value;
	if (index > 400) index = 400;
	else if (index < 0) index = 0;

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		rcp->last_time.tv_usec += 130000;
		if (rcp->last_time.tv_usec >= 1000000) {
			rcp->last_time.tv_sec++;
			rcp->last_time.tv_usec -= 1000000;
		}

		set_color_temperature_AW_UE150_now (rcp);

		return G_SOURCE_CONTINUE;
	} else {
		rcp->timeout_id = 0;
		return G_SOURCE_REMOVE;
	}
}

gboolean color_temperature_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature + 10;
		if (index > 400) index = 400;
		rcp->timeout_value = 10;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		set_color_temperature_AW_UE150 (rcp);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_UE150, rcp);
	}

	return FALSE;
}

gboolean color_temperature_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature + 1;
		if (index > 400) index = 400;
		rcp->timeout_value = 1;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		set_color_temperature_AW_UE150 (rcp);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_UE150, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature - 1;
		if (index < 0) index = 0;
		rcp->timeout_value = -1;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature + 1;
		if (index > 400) index = 400;
		rcp->timeout_value = 1;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		set_color_temperature_AW_UE150 (rcp);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_UE150, rcp);
	}

	return FALSE;
}

gboolean color_temperature_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int index;

	if (event->button == GDK_BUTTON_PRIMARY) {
		index = rcp->current_scene.color_temperature - 10;
		if (index < 0) index = 0;
		rcp->timeout_value = -10;
	} else if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		index = rcp->current_scene.color_temperature + 10;
		if (index > 400) index = 400;
		rcp->timeout_value = 10;
	} else return FALSE;

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->current_scene.color_temperature != index) {
		rcp->current_scene.color_temperature = index;

		g_signal_handler_block (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);
		gtk_combo_box_set_active (GTK_COMBO_BOX (rcp->color_temperature_combo_box), index);
		g_signal_handler_unblock (rcp->color_temperature_combo_box, rcp->color_temperature_handler_id);

		set_color_temperature_AW_UE150 (rcp);

		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)color_temperature_button_held_AW_UE150, rcp);
	}

	return FALSE;
}

GtkWidget *create_color_temperature_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;
	int i;
	char label[8];

	frame = gtk_frame_new ("Température de couleur");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.04, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		for (i = 400; i >= 0; i--) {
			sprintf (label, "%dK", color_temperature_array_AW_UE150[i]);
			gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), label);
		}
		rcp->color_temperature_handler_id = g_signal_connect (widget, "changed", G_CALLBACK (color_temperature_changed_AW_UE150), rcp);

		gtk_box_pack_start (GTK_BOX (box), widget, TRUE, TRUE, 0);
		rcp->color_temperature_combo_box = widget;

		widget = gtk_button_new_with_label ("--");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_10_button_pressed_AW_UE150), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("-");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_minus_1_button_pressed_AW_UE150), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_widget_set_margin_end (widget, MARGIN_VALUE);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("+");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_1_button_pressed_AW_UE150), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_button_new_with_label ("++");
		g_signal_connect (widget, "button_press_event", G_CALLBACK (color_temperature_plus_10_button_pressed_AW_UE150), rcp);
		g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

