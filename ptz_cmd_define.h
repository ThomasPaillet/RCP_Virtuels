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

#ifndef __PTZ_CMD_DEFINE_H
#define __PTZ_CMD_DEFINE_H


#define PTZ_CMD_FUNCS(l,c,d) \
void set_##l (rcp_t *rcp) \
{ \
	send_ptz_control_command_##d##_digits (rcp, c, rcp->current_scene.l, TRUE); \
} \
 \
gboolean set_##l##_delayed (rcp_t *rcp) \
{ \
	rcp->last_time += 130000; \
 \
	send_ptz_control_command_##d##_digits (rcp, c, rcp->current_scene.l, FALSE); \
 \
	rcp->timeout_id = 0; \
	return G_SOURCE_REMOVE; \
} \
 \
void l##_value_changed (GtkRange *l##_scale, rcp_t *rcp) \
{ \
	int l; \
	gint64 current_time, elapsed_time; \
 \
	l = (int)gtk_range_get_value (l##_scale); \
 \
	if (rcp->current_scene.l != l) { \
		rcp->current_scene.l = l; \
 \
		current_time = g_get_monotonic_time (); \
		elapsed_time = current_time - rcp->last_time; \
 \
		if (elapsed_time < 130000) { \
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time) / 1000, (GSourceFunc)set_##l##_delayed, rcp); \
		} else { \
			if (rcp->timeout_id != 0) { \
				g_source_remove (rcp->timeout_id); \
				rcp->timeout_id = 0; \
			} \
 \
			rcp->last_time = current_time; \
 \
			send_ptz_control_command_##d##_digits (rcp, c, rcp->current_scene.l, FALSE); \
		} \
	} \
} \
 \
gboolean l##_button_held (rcp_t *rcp) \
{ \
	int value; \
 \
	value = rcp->current_scene.l + rcp->timeout_value; \
	if (value > MAX_VALUE) value = MAX_VALUE; \
	else if (value < MIN_VALUE) value = MIN_VALUE; \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
 \
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		rcp->last_time += 130000; \
 \
		send_ptz_control_command_##d##_digits (rcp, c, value, FALSE); \
 \
		return G_SOURCE_CONTINUE; \
	} else { \
		rcp->timeout_id = 0; \
		return G_SOURCE_REMOVE; \
	} \
}


#define BUTTON_PRESSED_PLUS_FUNC(l,c,d,v) \
gboolean l##_plus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else return FALSE; \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
 \
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_ptz_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)l##_button_held, rcp); \
	} \
 \
	return FALSE; \
}

#define BUTTON_PRESSED_MINUS_FUNC(l,c,d,v) \
gboolean l##_minus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else return FALSE; \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
 \
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_ptz_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)l##_button_held, rcp); \
	} \
 \
	return FALSE; \
}


#endif

