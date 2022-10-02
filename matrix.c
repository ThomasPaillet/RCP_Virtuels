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
#include "matrix.h"

#include "protocol.h"
#include "misc.h"

#include "cameras_set.h"
#include "sw_p_08.h"

#include "main_window.h"

#include <math.h>


char *linear_matrix_R_G_label = "R_G: ";
char *linear_matrix_R_B_label = "R_B: ";
char *linear_matrix_G_R_label = "G_R: ";
char *linear_matrix_G_B_label = "G_B: ";
char *linear_matrix_B_R_label = "B_R: ";
char *linear_matrix_B_G_label = "B_G: ";


#define LINEAR_MATRIX_BUTTON_PRESSED_PLUS_FUNC(l,c,v) \
gboolean linear_matrix_##l##_plus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.linear_matrix.l - v; \
		if (value < 0x41) value = 0x41; \
		rcp->timeout_value = -v; \
	} else { \
		value = rcp->current_scene.linear_matrix.l + v; \
		if (value > 0xBF) value = 0xBF; \
		rcp->timeout_value = v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.linear_matrix.l != value) { \
		rcp->current_scene.linear_matrix.l = value; \
 \
		g_signal_handler_block (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), value); \
		g_signal_handler_unblock (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
 \
		send_cam_control_command_2_digits (rcp, c, value, TRUE); \
 \
		set_linear_matrix_##l##_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)linear_matrix_##l##_button_held, rcp); \
	} \
 \
	return GDK_EVENT_PROPAGATE; \
}

#define LINEAR_MATRIX_BUTTON_PRESSED_MINUS_FUNC(l,c,v) \
gboolean linear_matrix_##l##_minus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.linear_matrix.l + v; \
		if (value > 0xBF) value = 0xBF; \
		rcp->timeout_value = v; \
	} else { \
		value = rcp->current_scene.linear_matrix.l - v; \
		if (value < 0x41) value = 0x41; \
		rcp->timeout_value = -v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.linear_matrix.l != value) { \
		rcp->current_scene.linear_matrix.l = value; \
 \
		g_signal_handler_block (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), value); \
		g_signal_handler_unblock (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
 \
		send_cam_control_command_2_digits (rcp, c, value, TRUE); \
 \
		set_linear_matrix_##l##_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)linear_matrix_##l##_button_held, rcp); \
	} \
 \
	return GDK_EVENT_PROPAGATE; \
}

#define LINEAR_MATRIX_FUNCS(l,c) \
void set_linear_matrix_##l##_label (rcp_t *rcp) \
{ \
	char label[16]; \
 \
	if (rcp->current_scene.linear_matrix.l == 0x80) sprintf (label, "%s0", linear_matrix_##l##_label); \
	else sprintf (label, "%s%+d", linear_matrix_##l##_label, rcp->current_scene.linear_matrix.l - 0x80); \
 \
	gtk_frame_set_label (GTK_FRAME (rcp->linear_matrix_##l##_frame), label); \
} \
 \
void set_linear_matrix_##l (rcp_t *rcp) \
{ \
	send_cam_control_command_2_digits (rcp, c, rcp->current_scene.linear_matrix.l, TRUE); \
} \
 \
gboolean set_linear_matrix_##l##_delayed (rcp_t *rcp) \
{ \
	rcp->last_time.tv_usec += 130000; \
	if (rcp->last_time.tv_usec >= 1000000) { \
		rcp->last_time.tv_sec++; \
		rcp->last_time.tv_usec -= 1000000; \
	} \
 \
	send_cam_control_command_2_digits (rcp, c, rcp->current_scene.linear_matrix.l, FALSE); \
 \
	rcp->timeout_id = 0; \
	return G_SOURCE_REMOVE; \
} \
 \
void linear_matrix_##l##_value_changed (GtkRange *linear_matrix_##l##_scale, rcp_t *rcp) \
{ \
	int l; \
	struct timeval current_time, elapsed_time; \
 \
	l = (int)gtk_range_get_value (linear_matrix_##l##_scale); \
 \
	if (rcp->current_scene.linear_matrix.l != l) { \
		rcp->current_scene.linear_matrix.l = l; \
 \
		gettimeofday (&current_time, NULL); \
		timersub (&current_time, &rcp->last_time, &elapsed_time); \
	 \
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) { \
			if (rcp->timeout_id == 0) \
				rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_linear_matrix_##l##_delayed, rcp); \
		} else { \
			if (rcp->timeout_id != 0) { \
				g_source_remove (rcp->timeout_id); \
				rcp->timeout_id = 0; \
			} \
 \
			rcp->last_time = current_time; \
			send_cam_control_command_2_digits (rcp, c, rcp->current_scene.linear_matrix.l, FALSE); \
		} \
 \
		set_linear_matrix_##l##_label (rcp); \
	} \
} \
 \
gboolean linear_matrix_##l##_button_held (rcp_t *rcp) \
{ \
	int value; \
 \
	value = rcp->current_scene.linear_matrix.l + rcp->timeout_value; \
	if (value > 0xBF) value = 0xBF; \
	else if (value < 0x41) value = 0x41; \
 \
	if (rcp->current_scene.linear_matrix.l != value) { \
		rcp->current_scene.linear_matrix.l = value; \
 \
		g_signal_handler_block (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), value); \
		g_signal_handler_unblock (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
 \
		rcp->last_time.tv_usec += 130000; \
		if (rcp->last_time.tv_usec >= 1000000) { \
			rcp->last_time.tv_sec++; \
			rcp->last_time.tv_usec -= 1000000; \
		} \
	 \
		send_cam_control_command_2_digits (rcp, c, value, FALSE); \
 \
		set_linear_matrix_##l##_label (rcp); \
 \
		return G_SOURCE_CONTINUE; \
	} else { \
		rcp->timeout_id = 0; \
		return G_SOURCE_REMOVE; \
	} \
} \
 \
LINEAR_MATRIX_BUTTON_PRESSED_MINUS_FUNC(l,c,10) \
LINEAR_MATRIX_BUTTON_PRESSED_MINUS_FUNC(l,c,1) \
 \
void linear_matrix_##l##_0_button_clicked (GtkButton *button, rcp_t *rcp) \
{ \
	gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), LINEAR_MATRIX_DEFAULT); \
} \
 \
LINEAR_MATRIX_BUTTON_PRESSED_PLUS_FUNC(l,c,1) \
LINEAR_MATRIX_BUTTON_PRESSED_PLUS_FUNC(l,c,10)

LINEAR_MATRIX_FUNCS(R_G,"OSD:A4:")
LINEAR_MATRIX_FUNCS(R_B,"OSD:A5:")
LINEAR_MATRIX_FUNCS(G_R,"OSD:A6:")
LINEAR_MATRIX_FUNCS(G_B,"OSD:A7:")
LINEAR_MATRIX_FUNCS(B_R,"OSD:A8:")
LINEAR_MATRIX_FUNCS(B_G,"OSD:A9:")

#define RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(l,v) \
	if (rcp->current_scene.linear_matrix.l != v) { \
		rcp->current_scene.linear_matrix.l = v; \
		g_signal_handler_block (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->linear_matrix_##l##_scale), v); \
		g_signal_handler_unblock (rcp->linear_matrix_##l##_scale, rcp->linear_matrix_##l##_handler_id); \
 \
		set_linear_matrix_##l (rcp); \
		set_linear_matrix_##l##_label (rcp); \
	}


void linear_matrix_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(R_G,LINEAR_MATRIX_DEFAULT);
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(R_B,LINEAR_MATRIX_DEFAULT);
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(G_R,LINEAR_MATRIX_DEFAULT);
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(G_B,LINEAR_MATRIX_DEFAULT);
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(B_R,LINEAR_MATRIX_DEFAULT);
	RAZ_IHM_UPDATE_LINEAR_MATRIX_SCALE(B_G,LINEAR_MATRIX_DEFAULT);

	if (rcp->selected_color != nothing) {
		rcp->selected_color = nothing;
		gtk_widget_queue_draw (rcp->color_correction_drawing_area);
	}
}


GdkRGBA R_rgba = {1, 0, 0, 1};
GdkRGBA R_R_Yl_rgba = {1, 0.25, 0, 1};
GdkRGBA R_Yl_rgba = {1, 0.5, 0, 1};
GdkRGBA R_Yl_Yl_rgba = {1, 0.75, 0, 1};
GdkRGBA Yl_rgba = {1, 1, 0, 1};
GdkRGBA Yl_G_rgba = {0.5, 1, 0, 1};
GdkRGBA G_rgba = {0, 1, 0, 1};
GdkRGBA G_Cy_rgba = {0, 1, 0.5, 1};
GdkRGBA Cy_rgba = {0, 1, 1, 1};
GdkRGBA Cy_B_rgba = {0, 0.5, 1, 1};
GdkRGBA B_rgba = {0, 0, 1, 1};
GdkRGBA B_Mg_rgba = {0.5, 0, 1, 1};
GdkRGBA Mg_rgba = {1, 0, 1, 1};
GdkRGBA Mg_R_rgba = {1, 0, 0.66, 1};
GdkRGBA Mg_R_R_rgba = {1, 0, 0.33, 1};

GdkRGBA Black_0_5_rgba = {0, 0, 0, 0.5};
GdkRGBA Black_0_25_rgba = {0, 0, 0, 0.25};

gboolean cc_move = FALSE;
gboolean s_p = TRUE;
gboolean cc_saturation_need_update = FALSE;
gboolean cc_phase_need_update = FALSE;
gboolean cc_saturation_is_updatable = TRUE;
gboolean cc_phase_is_updatable = TRUE;


#define CC_FUNCS(l,c,d) \
void set_cc_saturation_##l##_label (rcp_t *rcp) \
{ \
	char label[4]; \
 \
	if (rcp->current_scene.cc_saturation[l] == 0) gtk_label_set_text (GTK_LABEL (rcp->cc_saturation_##l##_value), "0"); \
	else { \
		sprintf (label, "%+d", rcp->current_scene.cc_saturation[l]); \
		gtk_label_set_text (GTK_LABEL (rcp->cc_saturation_##l##_value), label); \
	} \
} \
 \
void set_cc_saturation_##l (rcp_t *rcp, gboolean wait) \
{ \
	send_cam_control_command_2_digits (rcp, c, rcp->current_scene.cc_saturation[l] + 0x80, wait); \
	cc_saturation_need_update = FALSE; \
} \
 \
void set_cc_phase_##l##_label (rcp_t *rcp) \
{ \
	char label[4]; \
 \
	if (rcp->current_scene.cc_phase[l] == 0) gtk_label_set_text (GTK_LABEL (rcp->cc_phase_##l##_value), "0"); \
	else { \
		sprintf (label, "%+d", rcp->current_scene.cc_phase[l]); \
		gtk_label_set_text (GTK_LABEL (rcp->cc_phase_##l##_value), label); \
	} \
} \
 \
void set_cc_phase_##l (rcp_t *rcp, gboolean wait) \
{ \
	send_cam_control_command_2_digits (rcp, d, rcp->current_scene.cc_phase[l] + 0x80, wait); \
	cc_phase_need_update = FALSE; \
}

CC_FUNCS(R,"OSD:86:","OSD:87:")
CC_FUNCS(R_R_Yl,"OSD:9C:","OSD:9D:")
CC_FUNCS(R_Yl,"OSD:88:","OSD:89:")
CC_FUNCS(R_Yl_Yl,"OSD:9E:","OSD:9F:")
CC_FUNCS(Yl,"OSD:8A:","OSD:8B:")
CC_FUNCS(Yl_G,"OSD:8C:","OSD:8D:")
CC_FUNCS(G,"OSD:8E:","OSD:8F:")
CC_FUNCS(G_Cy,"OSD:90:","OSD:91:")
CC_FUNCS(Cy,"OSD:92:","OSD:93:")
CC_FUNCS(Cy_B,"OSD:94:","OSD:95:")
CC_FUNCS(B,"OSD:96:","OSD:97:")
CC_FUNCS(B_Mg,"OSD:80:","OSD:81:")
CC_FUNCS(Mg,"OSD:82:","OSD:83:")
CC_FUNCS(Mg_R,"OSD:84:","OSD:85:")
CC_FUNCS(Mg_R_R,"OSD:9A:","OSD:9B:")

void (*set_cc_saturation_funcs[15]) (rcp_t *rcp, gboolean wait) = {set_cc_saturation_R, set_cc_saturation_R_R_Yl, set_cc_saturation_R_Yl, set_cc_saturation_R_Yl_Yl, set_cc_saturation_Yl, set_cc_saturation_Yl_G, set_cc_saturation_G, set_cc_saturation_G_Cy, set_cc_saturation_Cy, set_cc_saturation_Cy_B, set_cc_saturation_B, set_cc_saturation_B_Mg, set_cc_saturation_Mg, set_cc_saturation_Mg_R, set_cc_saturation_Mg_R_R};
void (*set_cc_saturation_label_funcs[15]) (rcp_t *rcp) = {set_cc_saturation_R_label, set_cc_saturation_R_R_Yl_label, set_cc_saturation_R_Yl_label, set_cc_saturation_R_Yl_Yl_label, set_cc_saturation_Yl_label, set_cc_saturation_Yl_G_label, set_cc_saturation_G_label, set_cc_saturation_G_Cy_label, set_cc_saturation_Cy_label, set_cc_saturation_Cy_B_label, set_cc_saturation_B_label, set_cc_saturation_B_Mg_label, set_cc_saturation_Mg_label, set_cc_saturation_Mg_R_label, set_cc_saturation_Mg_R_R_label};

void (*set_cc_phase_funcs[15]) (rcp_t *rcp, gboolean wait) = {set_cc_phase_R, set_cc_phase_R_R_Yl, set_cc_phase_R_Yl, set_cc_phase_R_Yl_Yl, set_cc_phase_Yl, set_cc_phase_Yl_G, set_cc_phase_G, set_cc_phase_G_Cy, set_cc_phase_Cy, set_cc_phase_Cy_B, set_cc_phase_B, set_cc_phase_B_Mg, set_cc_phase_Mg, set_cc_phase_Mg_R, set_cc_phase_Mg_R_R};
void (*set_cc_phase_label_funcs[15]) (rcp_t *rcp) = {set_cc_phase_R_label, set_cc_phase_R_R_Yl_label, set_cc_phase_R_Yl_label, set_cc_phase_R_Yl_Yl_label, set_cc_phase_Yl_label, set_cc_phase_Yl_G_label, set_cc_phase_G_label, set_cc_phase_G_Cy_label, set_cc_phase_Cy_label, set_cc_phase_Cy_B_label, set_cc_phase_B_label, set_cc_phase_B_Mg_label, set_cc_phase_Mg_label, set_cc_phase_Mg_R_label, set_cc_phase_Mg_R_R_label};

void color_correction_raz_button_clicked (GtkButton *button, rcp_t *rcp)
{
	int i;

	for (i = 0; i < 15; i++) {
		if (rcp->current_scene.cc_saturation[i] != COLOR_CORRECTION_DEFAULT) {
			rcp->current_scene.cc_saturation[i] = COLOR_CORRECTION_DEFAULT;
			set_cc_saturation_funcs[i] (rcp, TRUE);
			set_cc_saturation_label_funcs[i] (rcp);
		}
		if (rcp->current_scene.cc_phase[i] != COLOR_CORRECTION_DEFAULT) {
			rcp->current_scene.cc_phase[i] = COLOR_CORRECTION_DEFAULT;
			set_cc_phase_funcs[i] (rcp, TRUE);
			set_cc_phase_label_funcs[i] (rcp);
		}
	}

	rcp->selected_color = nothing;

	gtk_widget_queue_draw (rcp->color_correction_drawing_area);
}

#define CC_ARC(c) \
	gdk_cairo_set_source_rgba (cr, &c##_rgba); \
	cairo_arc (cr, 300, 300, 295, angle1, angle2); \
	cairo_arc_negative (cr, 300, 300, 285, angle2, angle1); \
	cairo_fill (cr); \
	phase = rcp->current_scene.cc_phase[c] * 0.003324437; \
	saturation = (63 - rcp->current_scene.cc_saturation[c]) * 2; \
	cairo_arc (cr, 300, 300, 285 - saturation, angle1 - phase, angle2 - phase); \
	cairo_arc_negative (cr, 300, 300, 265 - saturation, angle2 - phase, angle1 - phase); \
	cairo_fill (cr); \
	angle1 += 0.41887902; \
	angle2 += 0.41887902;

gboolean color_correction_draw (GtkWidget *widget, cairo_t *cr, rcp_t *rcp)
{
	double angle1, angle2, angle3, angle4, saturation, phase;

	if (rcp->selected_color != nothing) {
		angle1 = 0.41887902 * rcp->selected_color;
		angle2 = angle1 + 0.41887902;
		angle3 = angle2 + 0.20943951;
		angle4 = angle1 - 0.20943951;
		gdk_cairo_set_source_rgba (cr, &Black_0_25_rgba);
		cairo_arc (cr, 300, 300, 265 - 4 * 63, angle4, angle1);
		cairo_arc_negative (cr, 300, 300, 285, angle1, angle4);
		cairo_fill (cr);
		cairo_arc (cr, 300, 300, 265 - 4 * 63, angle2, angle3);
		cairo_arc_negative (cr, 300, 300, 285, angle3, angle2);
		cairo_fill (cr);
		gdk_cairo_set_source_rgba (cr, &Black_0_5_rgba);
		cairo_arc (cr, 300, 300, 265 - 4 * 63, angle1, angle2);
		cairo_arc_negative (cr, 300, 300, 285, angle2, angle1);
		cairo_fill (cr);
	}

	angle1 = 0;
	angle2 = 0.41887902;

	CC_ARC(R)
	CC_ARC(R_R_Yl)
	CC_ARC(R_Yl)
	CC_ARC(R_Yl_Yl)
	CC_ARC(Yl)
	CC_ARC(Yl_G)
	CC_ARC(G)
	CC_ARC(G_Cy)
	CC_ARC(Cy)
	CC_ARC(Cy_B)
	CC_ARC(B)
	CC_ARC(B_Mg)
	CC_ARC(Mg)
	CC_ARC(Mg_R)
	CC_ARC(Mg_R_R)

	return GDK_EVENT_PROPAGATE;
}

gboolean color_correction_drawing_area_button_press (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp)
{
	double x, y, r, t;

	x = event->x - 300;
	y = event->y - 300;
	r = (x * x) + (y * y);

	if ((r < 169) || ( r > 87025)) {
		if (rcp->selected_color != nothing) {
			rcp->selected_color = nothing;
			gtk_widget_queue_draw (widget);
		}
	} else {
		if (x > 0) {
			if (y > 0 ) {
				if (y < 0.445228685 * x) rcp->selected_color = R;
				else if (y < 1.110612515 * x) rcp->selected_color = R_R_Yl;
				else if (y < 3.077683537 * x) rcp->selected_color = R_Yl;
				else rcp->selected_color = R_Yl_Yl;
			} else  {
				if (y > -0.445228685 * x) rcp->selected_color = Mg_R_R;
				else if (y > -1.110612515 * x) rcp->selected_color = Mg_R;
				else if (y > -3.077683537 * x) rcp->selected_color = Mg;
				else rcp->selected_color = B_Mg;
			}
		} else {
			if (y > 0 ) {
				if (y < -0.212556562 * x) rcp->selected_color = G_Cy;
				else if (y < -0.726542528 * x) rcp->selected_color = G;
				else if (y < -1.732050808 * x) rcp->selected_color = Yl_G;
				else if (y < -9.514364454 * x) rcp->selected_color = Yl;
				else rcp->selected_color = R_Yl_Yl;
			} else {
				if (y > 0.212556562 * x) rcp->selected_color = G_Cy;
				else if (y > 0.726542528 * x) rcp->selected_color = Cy;
				else if (y > 1.732050808 * x) rcp->selected_color = Cy_B;
				else if (y > 9.514364454 * x) rcp->selected_color = B;
				else rcp->selected_color = B_Mg;
			}
		}

		if (event->button == GDK_BUTTON_SECONDARY) {
			if (rcp->current_scene.cc_saturation[rcp->selected_color] != COLOR_CORRECTION_DEFAULT) {
				rcp->current_scene.cc_saturation[rcp->selected_color] = COLOR_CORRECTION_DEFAULT;
				set_cc_saturation_funcs[rcp->selected_color] (rcp, TRUE);
				set_cc_saturation_label_funcs[rcp->selected_color] (rcp);
			}
			if (rcp->current_scene.cc_phase[rcp->selected_color] != COLOR_CORRECTION_DEFAULT) {
				rcp->current_scene.cc_phase[rcp->selected_color] = COLOR_CORRECTION_DEFAULT;
				set_cc_phase_funcs[rcp->selected_color] (rcp, TRUE);
				set_cc_phase_label_funcs[rcp->selected_color] (rcp);
			}
		} else {
			r = sqrt(r);
			t = atan2 (y, x);

			if (y > 0) {
				if ((r > 265 - (63 - rcp->current_scene.cc_saturation[rcp->selected_color]) * 2) && \
					(r < 285 - (63 - rcp->current_scene.cc_saturation[rcp->selected_color]) * 2) && \
					(t > 0.41887902 * rcp->selected_color - rcp->current_scene.cc_phase[rcp->selected_color] * 0.003324437) && \
					(t < 0.41887902 * (rcp->selected_color + 1) - rcp->current_scene.cc_phase[rcp->selected_color] * 0.003324437)) cc_move = TRUE;
			} else {
				if ((r > 265 - (63 - rcp->current_scene.cc_saturation[rcp->selected_color]) * 2) && \
					(r < 285 - (63 - rcp->current_scene.cc_saturation[rcp->selected_color]) * 2) && \
					(t > -0.41887902 * (15 - rcp->selected_color) - rcp->current_scene.cc_phase[rcp->selected_color] * 0.003324437) && \
					(t < -0.41887902 * (14 - rcp->selected_color) - rcp->current_scene.cc_phase[rcp->selected_color] * 0.003324437)) cc_move = TRUE;
			}
		}

		gtk_widget_queue_draw (widget);
	}

	return GDK_EVENT_STOP;
}

gboolean set_color_corrections_delayed (rcp_t *rcp)
{
	if (s_p) {
		if (cc_saturation_need_update) {
			set_cc_saturation_funcs[rcp->selected_color] (rcp, FALSE);
			if (!cc_phase_need_update) rcp->need_last_call = FALSE;
		} else {
			if (cc_phase_need_update) set_cc_phase_funcs[rcp->selected_color] (rcp, FALSE);
			rcp->need_last_call = FALSE;
		}
	} else {
		if (cc_phase_need_update) {
			set_cc_phase_funcs[rcp->selected_color] (rcp, FALSE);
			if (!cc_saturation_need_update) rcp->need_last_call = FALSE;
		} else {
			if (cc_saturation_need_update) set_cc_saturation_funcs[rcp->selected_color] (rcp, FALSE);
			rcp->need_last_call = FALSE;
		}
	}

	s_p = !s_p;

	if (rcp->need_last_call) {
		rcp->need_last_call = FALSE;
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_color_corrections_delayed, rcp);
	} else rcp->timeout_id = 0;

	return G_SOURCE_REMOVE;
}

#define CC_MOVE(l,a) \
	case l: if (cc_saturation_is_updatable) { \
			saturation = 63 - ((275 - r) / 2.0); \
			if (saturation > 63) saturation = 63; \
			else if (saturation < -63) saturation = -63; \
			if (rcp->current_scene.cc_saturation[l] != saturation) { \
				rcp->current_scene.cc_saturation[l] = saturation; \
				cc_saturation_need_update = TRUE; \
				set_cc_saturation_##l##_label (rcp); \
			} \
		} \
		if (cc_phase_is_updatable) { \
			phase = (a - t) / 0.003324437; \
			if (phase > 63) phase = 63; \
			else if (phase < -63) phase = -63; \
			if (rcp->current_scene.cc_phase[l] != phase) { \
				rcp->current_scene.cc_phase[l] = phase; \
				cc_phase_need_update = TRUE; \
				set_cc_phase_##l##_label (rcp); \
			} \
		} \
		break;

gboolean color_correction_drawing_area_motion_notify (GtkWidget *widget, GdkEventMotion *event, rcp_t *rcp)
{
	double x, y, r, t;
	int saturation, phase;
	struct timeval current_time, elapsed_time;

	x = event->x - 300;
	y = event->y - 300;
	r = sqrt((x * x) + (y * y));
	t = atan2 (y, x);

	if ((event->state & GDK_BUTTON1_MASK) && cc_move) {
		switch (rcp->selected_color) {
			CC_MOVE(R,0.20943951)
			CC_MOVE(R_R_Yl,0.62831853)
			CC_MOVE(R_Yl,1.04719755)
			CC_MOVE(R_Yl_Yl,1.46607657)
			CC_MOVE(Yl,1.88495559)
			CC_MOVE(Yl_G,2.30383461)
			CC_MOVE(G,2.72271363)
			case G_Cy: if (cc_saturation_is_updatable) {
						saturation = 63 - ((275 - r) / 2.0);
						if (saturation > 63) saturation = 63;
						else if (saturation < -63) saturation = -63;
						if (rcp->current_scene.cc_saturation[G_Cy] != saturation) {
							rcp->current_scene.cc_saturation[G_Cy] = saturation;
							cc_saturation_need_update = TRUE;
							set_cc_saturation_G_Cy_label (rcp);
						}
					}
					if (cc_phase_is_updatable) {
						if (y > 0) phase = (3.14159265 - t) / 0.003324437;
						else phase = (-3.14159265 - t) / 0.003324437;
						if (phase > 63) phase = 63;
						else if (phase < -63) phase = -63;
						if (rcp->current_scene.cc_phase[G_Cy] != phase) {
							rcp->current_scene.cc_phase[G_Cy] = phase;
							cc_phase_need_update = TRUE;
							set_cc_phase_G_Cy_label (rcp);
						}
					}
					break;
			CC_MOVE(Cy,-2.72271363)
			CC_MOVE(Cy_B,-2.30383461)
			CC_MOVE(B,-1.88495559)
			CC_MOVE(B_Mg,-1.46607657)
			CC_MOVE(Mg,-1.04719755)
			CC_MOVE(Mg_R,-0.62831853)
			CC_MOVE(Mg_R_R,-0.20943951)
			case nothing:
					break;
		}

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			rcp->need_last_call = TRUE;
			if (rcp->timeout_id == 0)
				rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_color_corrections_delayed, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}
			if (s_p) {
				if (cc_saturation_need_update) {
					set_cc_saturation_funcs[rcp->selected_color] (rcp, FALSE);
					if (cc_phase_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_color_corrections_delayed, rcp);
					}
				} else if (cc_phase_need_update) set_cc_phase_funcs[rcp->selected_color] (rcp, FALSE);
			} else {
				if (cc_phase_need_update) {
					set_cc_phase_funcs[rcp->selected_color] (rcp, FALSE);
					if (cc_saturation_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_color_corrections_delayed, rcp);
					}
				} else if (cc_saturation_need_update) set_cc_saturation_funcs[rcp->selected_color] (rcp, FALSE);
			}

			s_p = !s_p;
		}

		gtk_widget_queue_draw (widget);
	}

	return GDK_EVENT_STOP;
}

gboolean color_correction_drawing_area_button_release (GtkWidget *widget, GdkEventButton *event, rcp_t *rcp)
{
	cc_move = FALSE;

	return GDK_EVENT_STOP;
}

gboolean color_correction_drawing_area_key_press (GtkWidget *window, GdkEventKey *event, rcp_t *rcp)
{
	if ((event->keyval == GDK_KEY_s) || (event->keyval == GDK_KEY_S)) cc_phase_is_updatable = FALSE;
	else if ((event->keyval == GDK_KEY_p) || (event->keyval == GDK_KEY_P)) cc_saturation_is_updatable = FALSE;
	else if ((event->keyval == GDK_KEY_i) || (event->keyval == GDK_KEY_I)) send_jpeg_image_request_cmd (rcp);
	else if (event->keyval == GDK_KEY_Escape) {
		gtk_widget_hide (window);
		popup_window = NULL;

		gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp_vision->event_box), FALSE);
		gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp_vision->cameras_set))->master_rcp.root_widget), FALSE);
	}

	return GDK_EVENT_STOP;
}

gboolean color_correction_drawing_area_key_release (GtkWidget *widget, GdkEventKey *event)
{
	if ((event->keyval == GDK_KEY_s) || (event->keyval == GDK_KEY_S)) cc_phase_is_updatable = TRUE;
	else if ((event->keyval == GDK_KEY_p) || (event->keyval == GDK_KEY_P)) cc_saturation_is_updatable = TRUE;

	return GDK_EVENT_STOP;
}

#define LINEAR_MATRIX_WIDGETS(l) \
	sprintf (label, "%s%d", linear_matrix_##l##_label, LINEAR_MATRIX_DEFAULT - 0x80); \
 \
	frame3 = gtk_frame_new (label); \
	gtk_frame_set_label_align (GTK_FRAME (frame3), 0.02, 0.5); \
	gtk_container_set_border_width (GTK_CONTAINER (frame3), WINDOW_MARGIN_VALUE); \
	box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0); \
		gtk_widget_set_margin_start (box3, WINDOW_MARGIN_VALUE); \
		gtk_widget_set_margin_end (box3, WINDOW_MARGIN_VALUE); \
		gtk_widget_set_margin_bottom (box3, WINDOW_MARGIN_VALUE); \
		widget = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0x41, 0xBF, 1.0); \
		gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE); \
		gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE); \
		rcp->linear_matrix_##l##_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (linear_matrix_##l##_value_changed), rcp); \
		gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0); \
		rcp->linear_matrix_##l##_scale = widget; \
 \
		box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0); \
		gtk_widget_set_margin_top (box4, MARGIN_VALUE); \
			widget = gtk_button_new_with_label ("-10"); \
			g_signal_connect (widget, "button_press_event", G_CALLBACK (linear_matrix_##l##_minus_10_button_pressed), rcp); \
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp); \
			gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0); \
 \
			widget = gtk_button_new_with_label ("-1"); \
			g_signal_connect (widget, "button_press_event", G_CALLBACK (linear_matrix_##l##_minus_1_button_pressed), rcp); \
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp); \
			gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0); \
 \
			widget = gtk_button_new_with_label ("0"); \
			gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE); \
			gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE); \
			g_signal_connect (widget, "clicked", G_CALLBACK (linear_matrix_##l##_0_button_clicked), rcp); \
			gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0); \
 \
			widget = gtk_button_new_with_label ("+1"); \
			g_signal_connect (widget, "button_press_event", G_CALLBACK (linear_matrix_##l##_plus_1_button_pressed), rcp); \
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp); \
			gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0); \
 \
			widget = gtk_button_new_with_label ("+10"); \
			g_signal_connect (widget, "button_press_event", G_CALLBACK (linear_matrix_##l##_plus_10_button_pressed), rcp); \
			g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp); \
			gtk_box_pack_start (GTK_BOX (box4), widget, FALSE, FALSE, 0); \
		gtk_box_pack_start (GTK_BOX (box3), box4, FALSE, FALSE, 0); \
	gtk_container_add (GTK_CONTAINER (frame3), box3); \
	gtk_box_pack_start (GTK_BOX (box2), frame3, FALSE, FALSE, 0); \
	rcp->linear_matrix_##l##_frame = frame3;

#define CC_LABEL_WIDGETS(l) \
	widget = gtk_label_new (#l); \
	gtk_grid_attach (GTK_GRID (grid), widget, 0, i, 1, 1); \
	rcp->cc_saturation_##l##_value = gtk_label_new (NULL); \
	gtk_grid_attach (GTK_GRID (grid), rcp->cc_saturation_##l##_value, 1, i, 1, 1); \
	rcp->cc_phase_##l##_value = gtk_label_new (NULL); \
	gtk_grid_attach (GTK_GRID (grid), rcp->cc_phase_##l##_value, 2, i, 1, 1); \
	i++;

gboolean hide_matrix_window (GtkWidget *window, GdkEvent *event, rcp_t *rcp)
{
	hide_popup_window (window);

	rcp->selected_color = nothing;

	return GDK_EVENT_STOP;
}

void set_adaptive_matrix (rcp_t *rcp)
{
	if (rcp->current_scene.adaptive_matrix) send_cam_control_command (rcp, "OSJ:4F:1");
	else send_cam_control_command (rcp, "OSJ:4F:0");
}

void adaptive_matrix_toggle_button_clicked_1 (GtkToggleButton *adaptive_matrix_toggle_button_1, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (adaptive_matrix_toggle_button_1)) {
		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_2), TRUE);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);

		gtk_button_set_label (GTK_BUTTON (adaptive_matrix_toggle_button_1), "On");
		gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_2), "On");
		rcp->current_scene.adaptive_matrix = TRUE;
	} else {
		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_2), FALSE);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_2, rcp->adaptive_matrix_handler_id_2);

		rcp->current_scene.adaptive_matrix = FALSE;
		gtk_button_set_label (GTK_BUTTON (adaptive_matrix_toggle_button_1), "Off");
		gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_2), "Off");
	}

	set_adaptive_matrix (rcp);
}

void adaptive_matrix_toggle_button_clicked_2 (GtkToggleButton *adaptive_matrix_toggle_button_2, rcp_t *rcp)
{
	if (gtk_toggle_button_get_active (adaptive_matrix_toggle_button_2)) {
		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_1), TRUE);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);

		gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_1), "On");
		gtk_button_set_label (GTK_BUTTON (adaptive_matrix_toggle_button_2), "On");
		rcp->current_scene.adaptive_matrix = TRUE;
	} else {
		g_signal_handler_block (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rcp->adaptive_matrix_toggle_button_1), FALSE);
		g_signal_handler_unblock (rcp->adaptive_matrix_toggle_button_1, rcp->adaptive_matrix_handler_id_1);

		rcp->current_scene.adaptive_matrix = FALSE;
		gtk_button_set_label (GTK_BUTTON (rcp->adaptive_matrix_toggle_button_1), "Off");
		gtk_button_set_label (GTK_BUTTON (adaptive_matrix_toggle_button_2), "Off");
	}

	set_adaptive_matrix (rcp);
}

void create_adaptive_matrix_window (rcp_t *rcp)
{
	GtkWidget *window, *frame, *box, *widget;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 100, 20);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_widget_set_events (window, gtk_widget_get_events (window) | GDK_FOCUS_CHANGE_MASK);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (popup_window_key_press), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_popup_window), NULL);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_popup_window), NULL);

	frame = gtk_frame_new (NULL);
	gtk_container_set_border_width (GTK_CONTAINER (frame), WINDOW_MARGIN_VALUE);
	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box), WINDOW_MARGIN_VALUE);
		widget = gtk_label_new ("Adaptive Matrix");
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

		widget = gtk_toggle_button_new_with_label ("Off");
		gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
		rcp->adaptive_matrix_handler_id_1 = g_signal_connect (widget, "toggled", G_CALLBACK (adaptive_matrix_toggle_button_clicked_1), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->adaptive_matrix_toggle_button_1 = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);
	gtk_container_add (GTK_CONTAINER (window), frame);

	rcp->adaptive_matrix_window = window;
}

void create_matrix_window (rcp_t *rcp)
{
	GtkWidget *window, *frame1, *frame2, *frame3, *box1, *box2, *box3, *box4, *box5, *box6, *grid, *widget;
	char label[32];
	int i;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (main_window));
	gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	g_signal_connect (G_OBJECT (window), "key-press-event", G_CALLBACK (color_correction_drawing_area_key_press), rcp);
	g_signal_connect (G_OBJECT (window), "key-release-event", G_CALLBACK (color_correction_drawing_area_key_release), NULL);
	g_signal_connect (G_OBJECT (window), "focus-out-event", G_CALLBACK (hide_matrix_window), rcp);
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (hide_matrix_window), rcp);

	frame1 = gtk_frame_new ("Matriçage");
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
	gtk_widget_set_margin_start (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (frame1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (frame1, WINDOW_MARGIN_VALUE);
	box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_end (box1, WINDOW_MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box1, WINDOW_MARGIN_VALUE);
		box6 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		gtk_widget_set_margin_end (box6, WINDOW_MARGIN_VALUE);

		frame2 = gtk_frame_new ("Linear Matrix");
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);

		box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				if (rcp->model == AW_UE150) {
					box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
					gtk_widget_set_margin_top (box3, WINDOW_MARGIN_VALUE);
					gtk_widget_set_halign (box3, GTK_ALIGN_CENTER);
						widget = gtk_label_new ("Adaptive Matrix");
						gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);

						widget = gtk_toggle_button_new_with_label ("Off");
						gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
						rcp->adaptive_matrix_handler_id_2 = g_signal_connect (widget, "toggled", G_CALLBACK (adaptive_matrix_toggle_button_clicked_2), rcp);
						gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
						rcp->adaptive_matrix_toggle_button_2 = widget;

					gtk_box_pack_start (GTK_BOX (box2), box3, FALSE, FALSE, 0);
				}
				LINEAR_MATRIX_WIDGETS(R_G)
				LINEAR_MATRIX_WIDGETS(R_B)
				LINEAR_MATRIX_WIDGETS(G_R)
				LINEAR_MATRIX_WIDGETS(G_B)
				LINEAR_MATRIX_WIDGETS(B_R)
				LINEAR_MATRIX_WIDGETS(B_G)
			gtk_box_pack_start (GTK_BOX (box5), box2, FALSE, FALSE, 0);

			box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				widget = gtk_button_new_with_label ("raz");
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (linear_matrix_raz_button_clicked), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
			gtk_box_pack_end (GTK_BOX (box5), box2, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box5);
		gtk_box_pack_start (GTK_BOX (box6), frame2, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box1), box6, FALSE, FALSE, 0);

		frame2 = gtk_frame_new ("Color Correction");
		gtk_frame_set_label_align (GTK_FRAME (frame2), 0.5, 0.5);
		box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			widget = gtk_drawing_area_new ();
			gtk_widget_set_size_request (widget, 600, 600);
			gtk_widget_set_events (widget, gtk_widget_get_events (widget) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
			g_signal_connect (G_OBJECT (widget), "draw", G_CALLBACK (color_correction_draw), rcp);
			g_signal_connect (G_OBJECT (widget), "button-press-event", G_CALLBACK (color_correction_drawing_area_button_press), rcp);
			g_signal_connect (G_OBJECT (widget), "motion-notify-event", G_CALLBACK (color_correction_drawing_area_motion_notify), rcp);
			g_signal_connect (G_OBJECT (widget), "button-release-event", G_CALLBACK (color_correction_drawing_area_button_release), rcp);
			rcp->color_correction_drawing_area = widget;
			gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

			box4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			box5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			frame3 = gtk_frame_new (NULL);
			gtk_frame_set_label_align (GTK_FRAME (frame3), 0.5, 0.5);
			grid = gtk_grid_new ();
			gtk_widget_set_margin_start (grid, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_end (grid, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_top (grid, WINDOW_MARGIN_VALUE);
			gtk_widget_set_margin_bottom (grid, WINDOW_MARGIN_VALUE);
				widget = gtk_label_new (" Saturation ");
				gtk_grid_attach (GTK_GRID (grid), widget, 1, 0, 1, 1);
				widget = gtk_label_new (" Phase ");
				gtk_grid_attach (GTK_GRID (grid), widget, 2, 0, 1, 1);
				i = 1;
				CC_LABEL_WIDGETS(B_Mg)
				CC_LABEL_WIDGETS(Mg)
				CC_LABEL_WIDGETS(Mg_R)
				CC_LABEL_WIDGETS(Mg_R_R)
				CC_LABEL_WIDGETS(R)
				CC_LABEL_WIDGETS(R_R_Yl)
				CC_LABEL_WIDGETS(R_Yl)
				CC_LABEL_WIDGETS(R_Yl_Yl)
				CC_LABEL_WIDGETS(Yl)
				CC_LABEL_WIDGETS(Yl_G)
				CC_LABEL_WIDGETS(G)
				CC_LABEL_WIDGETS(G_Cy)
				CC_LABEL_WIDGETS(Cy)
				CC_LABEL_WIDGETS(Cy_B)
				CC_LABEL_WIDGETS(B)
			gtk_container_add (GTK_CONTAINER (frame3), grid);
			gtk_box_pack_start (GTK_BOX (box5), frame3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				widget = gtk_button_new_with_label ("raz");
				gtk_widget_set_margin_start (widget, WINDOW_MARGIN_VALUE);
				gtk_widget_set_margin_end (widget, WINDOW_MARGIN_VALUE);
				g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (color_correction_raz_button_clicked), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box5), box3, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box4), box5, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box2), box4, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame2), box2);
		gtk_box_pack_start (GTK_BOX (box1), frame2, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame1), box1);
	gtk_container_add (GTK_CONTAINER (window), frame1);

	rcp->matrix_window = window;
}

void show_rcp_matrix_window (rcp_t *rcp)
{
	if (rcp->current_scene.matrix_type != 3) {
		gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
		gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

		popup_window = rcp->adaptive_matrix_window;
		gtk_widget_show_all (rcp->adaptive_matrix_window);
	} else {
		cc_move = FALSE;
		s_p = TRUE;
		cc_saturation_need_update = FALSE;
		cc_phase_need_update = FALSE;
		cc_saturation_is_updatable = TRUE;
		cc_phase_is_updatable = TRUE;

		gtk_event_box_set_above_child (GTK_EVENT_BOX (rcp->event_box), TRUE);
		gtk_event_box_set_above_child (GTK_EVENT_BOX (((cameras_set_t*)(rcp->cameras_set))->master_rcp.root_widget), TRUE);

		popup_window = rcp->matrix_window;
		gtk_widget_show_all (rcp->matrix_window);
	}
}

void set_matrix_type (rcp_t *rcp)
{
	send_cam_control_command_1_digit (rcp, "OSE:31:", rcp->current_scene.matrix_type, TRUE);
}

void matrix_type_changed (GtkComboBox *matrix_type_combo_box, rcp_t *rcp)
{
	rcp->current_scene.matrix_type = gtk_combo_box_get_active (matrix_type_combo_box);

	set_matrix_type (rcp);

	if (rcp->model == AW_HE130) {
		if (rcp->current_scene.matrix_type == 3) gtk_widget_set_sensitive (rcp->matrix_settings_button, TRUE);
		else gtk_widget_set_sensitive (rcp->matrix_settings_button, FALSE);
	}
}

GtkWidget *create_matrix_frame (rcp_t *rcp)
{
	GtkWidget *frame, *box, *widget;

	frame = gtk_frame_new ("Matriçage");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.1, 0.5);
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_start (box, MARGIN_VALUE);
	gtk_widget_set_margin_end (box, MARGIN_VALUE);
	gtk_widget_set_margin_bottom (box, MARGIN_VALUE);
		widget = gtk_combo_box_text_new ();
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "Normal");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "EBU");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "NTSC");
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (widget), "User");
		rcp->matrix_type_handler_id = g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (matrix_type_changed), rcp);
		gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->matrix_type_combo_box = widget;

		if (rcp->model == AW_UE150) create_adaptive_matrix_window (rcp);

		create_matrix_window (rcp);

		widget = gtk_button_new_with_label ("Réglages");
		g_signal_connect_swapped (G_OBJECT (widget), "clicked", G_CALLBACK (show_rcp_matrix_window), rcp);
		gtk_box_pack_end (GTK_BOX (box), widget, FALSE, FALSE, 0);
		rcp->matrix_settings_button = widget;
	gtk_container_add (GTK_CONTAINER (frame), box);

	return frame;
}

