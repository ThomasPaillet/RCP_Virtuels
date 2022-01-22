/*
 * copyright (c) 2021-2022 Thomas Paillet <thomas.paillet@net-c.fr>

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

#ifndef __MASTER_CMD_DEFINE_H
#define __MASTER_CMD_DEFINE_H


#define MASTER_RCP_CMD_FUNCS(l) \
void l##_master_value_changed (GtkRange *l##_scale, cameras_set_t *cameras_set) \
{ \
	int i, l; \
	rcp_t *rcp; \
 \
	l = (int)gtk_range_get_value (l##_scale); \
 \
	for (i = 0; i < cameras_set->number_of_cameras; i++) { \
		rcp = cameras_set->rcp_ptr_array[i]; \
 \
		if (!rcp->camera_is_on) continue; \
		if (rcp->camera_is_working) continue; \
 \
		gtk_range_set_value (GTK_RANGE (rcp->l##_scale), rcp->current_scene.l + l - cameras_set->master_rcp.l); \
	} \
 \
	cameras_set->master_rcp.l = l; \
} \
 \
gboolean l##_master_button_held (cameras_set_t *cameras_set) \
{ \
	int value; \
 \
	value = cameras_set->master_rcp.l + cameras_set->master_rcp.timeout_value; \
	if (value > MAX_VALUE) value = MAX_VALUE; \
	else if (value < MIN_VALUE) value = MIN_VALUE; \
 \
	if (cameras_set->master_rcp.l != value) { \
		cameras_set->master_rcp.l = value; \
 \
		g_signal_handler_block (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.l##_scale), value); \
		g_signal_handler_unblock (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
 \
		return G_SOURCE_CONTINUE; \
	} else { \
		cameras_set->master_rcp.timeout_id = 0; \
		return G_SOURCE_REMOVE; \
	} \
} \


#define MASTER_BUTTON_PLUS_FUNCS(l,v) \
gboolean l##_plus_##v##_master_button_pressed (GtkButton *button, GdkEventButton *event, cameras_set_t *cameras_set) \
{ \
	int value, i; \
	rcp_t *rcp; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		value = cameras_set->master_rcp.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		cameras_set->master_rcp.timeout_value = v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		value = cameras_set->master_rcp.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		cameras_set->master_rcp.timeout_value = -v; \
	} else return FALSE; \
 \
	if (cameras_set->master_rcp.timeout_id != 0) { \
		g_source_remove (cameras_set->master_rcp.timeout_id); \
		cameras_set->master_rcp.timeout_id = 0; \
	} \
 \
	if (cameras_set->master_rcp.l != value) { \
		for (i = 0; i < cameras_set->number_of_cameras; i++) { \
			rcp = cameras_set->rcp_ptr_array[i]; \
 \
			if (!rcp->camera_is_on) continue; \
			if (rcp->camera_is_working) continue; \
 \
			if (event->button == GDK_BUTTON_PRIMARY) gtk_widget_set_state_flags (rcp->l##_plus_##v##_button, GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
			if (rcp->model == AW_UE150) l##_plus_##v##_button_pressed_AW_UE150 (GTK_BUTTON (rcp->l##_plus_##v##_button), event, rcp); \
			else l##_plus_##v##_button_pressed_AW_HE130 (GTK_BUTTON (rcp->l##_plus_##v##_button), event, rcp); \
		} \
 \
		cameras_set->master_rcp.l = value; \
 \
		g_signal_handler_block (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.l##_scale), value); \
		g_signal_handler_unblock (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
 \
		cameras_set->master_rcp.timeout_id = g_timeout_add (130, (GSourceFunc)l##_master_button_held, cameras_set); \
	} \
 \
	return FALSE; \
} \
 \
gboolean l##_plus_##v##_master_button_released (GtkButton *button, GdkEventButton *event, cameras_set_t *cameras_set) \
{ \
	int i; \
	rcp_t *rcp; \
 \
	gtk_widget_unset_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE); \
 \
	for (i = 0; i < cameras_set->number_of_cameras; i++) { \
		rcp = cameras_set->rcp_ptr_array[i]; \
 \
		if (!rcp->camera_is_on) continue; \
		if (rcp->camera_is_working) continue; \
 \
		remove_timeout (rcp->l##_plus_##v##_button, NULL, rcp); \
	} \
 \
	if (cameras_set->master_rcp.timeout_id != 0) { \
		g_source_remove (cameras_set->master_rcp.timeout_id); \
		cameras_set->master_rcp.timeout_id = 0; \
	} \
 \
	return FALSE; \
}


#define MASTER_BUTTON_MINUS_FUNCS(l,v) \
gboolean l##_minus_##v##_master_button_pressed (GtkButton *button, GdkEventButton *event, cameras_set_t *cameras_set) \
{ \
	int value, i; \
	rcp_t *rcp; \
 \
	if (event->button == GDK_BUTTON_PRIMARY) { \
		value = cameras_set->master_rcp.l - v; \
		if (value < MIN_VALUE) value = MIN_VALUE; \
		cameras_set->master_rcp.timeout_value = -v; \
	} else if (event->button == GDK_BUTTON_SECONDARY) { \
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
		value = cameras_set->master_rcp.l + v; \
		if (value > MAX_VALUE) value = MAX_VALUE; \
		cameras_set->master_rcp.timeout_value = v; \
	} else return FALSE; \
 \
	if (cameras_set->master_rcp.timeout_id != 0) { \
		g_source_remove (cameras_set->master_rcp.timeout_id); \
		cameras_set->master_rcp.timeout_id = 0; \
	} \
 \
	if (cameras_set->master_rcp.l != value) { \
		for (i = 0; i < cameras_set->number_of_cameras; i++) { \
			rcp = cameras_set->rcp_ptr_array[i]; \
 \
			if (!rcp->camera_is_on) continue; \
			if (rcp->camera_is_working) continue; \
 \
			if (event->button == GDK_BUTTON_PRIMARY) gtk_widget_set_state_flags (rcp->l##_minus_##v##_button, GTK_STATE_FLAG_ACTIVE, FALSE); \
 \
			if (rcp->model == AW_UE150) l##_minus_##v##_button_pressed_AW_UE150 (GTK_BUTTON (rcp->l##_minus_##v##_button), event, rcp); \
			else l##_minus_##v##_button_pressed_AW_HE130 (GTK_BUTTON (rcp->l##_minus_##v##_button), event, rcp); \
		} \
 \
		cameras_set->master_rcp.l = value; \
 \
		g_signal_handler_block (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
		gtk_range_set_value (GTK_RANGE (cameras_set->master_rcp.l##_scale), value); \
		g_signal_handler_unblock (cameras_set->master_rcp.l##_scale, cameras_set->master_rcp.l##_handler_id); \
 \
		cameras_set->master_rcp.timeout_id = g_timeout_add (130, (GSourceFunc)l##_master_button_held, cameras_set); \
	} \
 \
	return FALSE; \
} \
 \
gboolean l##_minus_##v##_master_button_released (GtkButton *button, GdkEventButton *event, cameras_set_t *cameras_set) \
{ \
	int i; \
	rcp_t *rcp; \
 \
	gtk_widget_unset_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE); \
 \
	for (i = 0; i < cameras_set->number_of_cameras; i++) { \
		rcp = cameras_set->rcp_ptr_array[i]; \
 \
		if (!rcp->camera_is_on) continue; \
		if (rcp->camera_is_working) continue; \
 \
		remove_timeout (rcp->l##_minus_##v##_button, NULL, rcp); \
	} \
 \
	if (cameras_set->master_rcp.timeout_id != 0) { \
		g_source_remove (cameras_set->master_rcp.timeout_id); \
		cameras_set->master_rcp.timeout_id = 0; \
	} \
 \
	return FALSE; \
}


#endif

