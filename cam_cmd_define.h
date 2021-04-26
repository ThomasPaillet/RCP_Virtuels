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

#ifndef __CAM_CMD_DEFINE_H
#define __CAM_CMD_DEFINE_H


#define SEND_CMD_POST_ACTION

#define BUTTON_PRESSED_PLUS_FUNC(l,c,d,v) \
gboolean l##_plus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else { \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} \
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
		send_cam_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		set_##l##_label (rcp); \
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
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else { \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_cam_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		set_##l##_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)l##_button_held, rcp); \
	} \
 \
	return FALSE; \
}


#define CAM_CMD_FUNCS(l,c,d) \
void set_##l (rcp_t *rcp) \
{ \
	send_cam_control_command_##d##_digits (rcp, c, rcp->current_scene.l, TRUE); \
 \
	SEND_CMD_POST_ACTION \
} \
 \
gboolean set_##l##_delayed (rcp_t *rcp) \
{ \
	send_cam_control_command_##d##_digits (rcp, c, rcp->current_scene.l, FALSE); \
 \
	SEND_CMD_POST_ACTION \
 \
	rcp->timeout_id = 0; \
	return G_SOURCE_REMOVE; \
} \
 \
void l##_value_changed (GtkRange *l##_scale, rcp_t *rcp) \
{ \
	int value; \
	struct timeval current_time, elapsed_time; \
 \
	value = (int)gtk_range_get_value (l##_scale); \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
 \
		gettimeofday (&current_time, NULL); \
		timersub (&current_time, &rcp->last_time, &elapsed_time); \
 \
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) { \
			if (rcp->timeout_id == 0) \
				rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_##l##_delayed, rcp); \
		} else { \
			if (rcp->timeout_id != 0) { \
				g_source_remove (rcp->timeout_id); \
				rcp->timeout_id = 0; \
			} \
			send_cam_control_command_##d##_digits (rcp, c, value, FALSE); \
			SEND_CMD_POST_ACTION \
		} \
		set_##l##_label (rcp); \
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
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_cam_control_command_##d##_digits (rcp, c, value, FALSE); \
 \
		set_##l##_label (rcp); \
 \
		return G_SOURCE_CONTINUE; \
	} else { \
		rcp->timeout_id = 0; \
		return G_SOURCE_REMOVE; \
	} \
} \
 \
BUTTON_PRESSED_MINUS_FUNC(l,c,d,10) \
BUTTON_PRESSED_MINUS_FUNC(l,c,d,1) \
BUTTON_PRESSED_PLUS_FUNC(l,c,d,1) \
BUTTON_PRESSED_PLUS_FUNC(l,c,d,10)


#define BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,v) \
gboolean l##_plus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} else { \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} \
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
		send_cam_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		if ((rcp == rcp_vision) && physical_rcp.connected) { \
			g_mutex_lock (&physical_rcp.mutex); \
			physical_rcp.l = rcp->current_scene.l; \
			send_##l##_update_notification (); \
			g_mutex_unlock (&physical_rcp.mutex); \
		} \
 \
		set_##l##_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)l##_button_held, rcp); \
	} \
 \
	return FALSE; \
}

#define BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,v) \
gboolean l##_minus_##v##_button_pressed (GtkButton *button, GdkEventButton *event, rcp_t *rcp) \
{ \
	int value; \
 \
	if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
		value = rcp->current_scene.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		rcp->timeout_value = v; \
	} else { \
		value = rcp->current_scene.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		rcp->timeout_value = -v; \
	} \
 \
	if (rcp->timeout_id != 0) { \
		g_source_remove (rcp->timeout_id); \
		rcp->timeout_id = 0; \
	} \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_cam_control_command_##d##_digits (rcp, c, value, TRUE); \
 \
		if ((rcp == rcp_vision) && physical_rcp.connected) { \
			g_mutex_lock (&physical_rcp.mutex); \
			physical_rcp.l = rcp->current_scene.l; \
			send_##l##_update_notification (); \
			g_mutex_unlock (&physical_rcp.mutex); \
		} \
 \
		set_##l##_label (rcp); \
 \
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)l##_button_held, rcp); \
	} \
 \
	return FALSE; \
}


#define CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(l,c,d) \
void set_##l (rcp_t *rcp) \
{ \
	send_cam_control_command_##d##_digits (rcp, c, rcp->current_scene.l, TRUE); \
 \
	if ((rcp == rcp_vision) && physical_rcp.connected) { \
		g_mutex_lock (&physical_rcp.mutex); \
		physical_rcp.l = rcp->current_scene.l; \
		send_##l##_update_notification (); \
		g_mutex_unlock (&physical_rcp.mutex); \
	} \
 \
	SEND_CMD_POST_ACTION \
} \
 \
gboolean set_##l##_delayed (rcp_t *rcp) \
{ \
	send_cam_control_command_##d##_digits (rcp, c, rcp->current_scene.l, FALSE); \
 \
	if ((rcp == rcp_vision) && physical_rcp.connected) { \
		g_mutex_lock (&physical_rcp.mutex); \
		physical_rcp.l = rcp->current_scene.l; \
		send_##l##_update_notification (); \
		g_mutex_unlock (&physical_rcp.mutex); \
	} \
 \
	SEND_CMD_POST_ACTION \
 \
	rcp->timeout_id = 0; \
	return G_SOURCE_REMOVE; \
} \
 \
void l##_value_changed (GtkRange *l##_scale, rcp_t *rcp) \
{ \
	int value; \
	struct timeval current_time, elapsed_time; \
 \
	value = (int)gtk_range_get_value (l##_scale); \
 \
	if (rcp->current_scene.l != value) { \
		rcp->current_scene.l = value; \
 \
		gettimeofday (&current_time, NULL); \
		timersub (&current_time, &rcp->last_time, &elapsed_time); \
 \
		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) { \
			if (rcp->timeout_id == 0) \
				rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_##l##_delayed, rcp); \
		} else { \
			if (rcp->timeout_id != 0) { \
				g_source_remove (rcp->timeout_id); \
				rcp->timeout_id = 0; \
			} \
			send_cam_control_command_##d##_digits (rcp, c, value, FALSE); \
 \
			if ((rcp == rcp_vision) && physical_rcp.connected) { \
				g_mutex_lock (&physical_rcp.mutex); \
				physical_rcp.l = rcp->current_scene.l; \
				send_##l##_update_notification (); \
				g_mutex_unlock (&physical_rcp.mutex); \
			} \
 \
			SEND_CMD_POST_ACTION \
		} \
		set_##l##_label (rcp); \
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
		g_signal_handler_block (rcp->l##_scale, rcp->l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), value); \
		g_signal_handler_unblock (rcp->l##_scale, rcp->l##_handler_id); \
 \
		send_cam_control_command_##d##_digits (rcp, c, value, FALSE); \
 \
		if ((rcp == rcp_vision) && physical_rcp.connected) { \
			g_mutex_lock (&physical_rcp.mutex); \
			physical_rcp.l = rcp->current_scene.l; \
			send_##l##_update_notification (); \
			g_mutex_unlock (&physical_rcp.mutex); \
		} \
 \
		set_##l##_label (rcp); \
 \
		return G_SOURCE_CONTINUE; \
	} else { \
		rcp->timeout_id = 0; \
		return G_SOURCE_REMOVE; \
	} \
} \
 \
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,10) \
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,1) \
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,1) \
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(l,c,d,10)


#endif

