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
#include "black.h"
#include "cam_cmd_define.h"

#include "protocol.h"
#include "misc.h"

#include "sw_p_08.h"
#include "physical_rcp.h"

#include "main_window.h"


#define MIN_VALUE 0x032
#define MAX_VALUE 0x0FA

#define set_r_pedestal_label(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->r_pedestal_need_update = FALSE;

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(r_pedestal,"ORP:",3)

BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_pedestal,"ORP:",3,10)
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_pedestal,"ORP:",3,1)

BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_pedestal,"ORP:",3,1)
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_pedestal,"ORP:",3,10)

gboolean set_g_pedestal_delayed_AW_HE130 (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	if (rcp->r_b) {
		if (rcp->r_pedestal_need_update) {
			send_cam_control_command_3_digits (rcp, "ORP:", rcp->current_scene.r_pedestal, FALSE);
			rcp->r_pedestal_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->b_pedestal_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->b_pedestal_need_update) {
				send_cam_control_command_3_digits (rcp, "OBP:", rcp->current_scene.b_pedestal, FALSE);
				rcp->b_pedestal_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	} else {
		if (rcp->b_pedestal_need_update) {
			send_cam_control_command_3_digits (rcp, "OBP:", rcp->current_scene.b_pedestal, FALSE);
			rcp->b_pedestal_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->r_pedestal_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->r_pedestal_need_update) {
				send_cam_control_command_3_digits (rcp, "ORP:", rcp->current_scene.r_pedestal, FALSE);
				rcp->r_pedestal_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	}

	rcp->r_b = !rcp->r_b;

	if (rcp->need_last_call) {
		rcp->need_last_call = FALSE;
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_pedestal_delayed_AW_HE130, rcp);
	} else rcp->timeout_id = 0;

	return G_SOURCE_REMOVE;
}

void g_pedestal_value_changed_AW_HE130 (GtkRange *g_pedestal_scale, rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;
	struct timeval current_time, elapsed_time;

	g_pedestal = (int)gtk_range_get_value (g_pedestal_scale);

	if (rcp->current_scene.g_pedestal != g_pedestal) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;
			rcp->r_pedestal_need_update = TRUE;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
		}

		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;
			rcp->b_pedestal_need_update = TRUE;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
		}

		rcp->current_scene.g_pedestal = g_pedestal;

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			rcp->need_last_call = TRUE;
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_g_pedestal_delayed_AW_HE130, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}

			if (rcp->r_b) {
				if (rcp->r_pedestal_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "ORP:", rcp->current_scene.r_pedestal, FALSE);
					rcp->r_pedestal_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
						send_r_pedestal_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->b_pedestal_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_pedestal_delayed_AW_HE130, rcp);
					}
				} else if (rcp->b_pedestal_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OBP:", rcp->current_scene.b_pedestal, FALSE);
					rcp->b_pedestal_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
						send_b_pedestal_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			} else {
				if (rcp->b_pedestal_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OBP:", rcp->current_scene.b_pedestal, FALSE);
					rcp->b_pedestal_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
						send_b_pedestal_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->r_pedestal_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_pedestal_delayed_AW_HE130, rcp);
					}
				} else if (rcp->r_pedestal_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "ORP:", rcp->current_scene.r_pedestal, FALSE);
					rcp->r_pedestal_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
						send_r_pedestal_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			}

			rcp->r_b = !rcp->r_b;
		}
	}
}

gboolean g_pedestal_button_held_AW_HE130 (rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;

	g_pedestal = rcp->current_scene.g_pedestal + rcp->timeout_value;
	if (g_pedestal > MAX_VALUE) g_pedestal = MAX_VALUE;
	else if (g_pedestal < MIN_VALUE) g_pedestal = MIN_VALUE;

	if (rcp->r_b) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
			else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
			if (rcp->current_scene.b_pedestal != b_pedestal) {
				rcp->current_scene.b_pedestal = b_pedestal;

				g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
				g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			} else {
				rcp->timeout_id = 0;
				return G_SOURCE_REMOVE;
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
			else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
			if (rcp->current_scene.r_pedestal != r_pedestal) {
				rcp->current_scene.r_pedestal = r_pedestal;

				g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
				g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			} else {
				rcp->timeout_id = 0;
				return G_SOURCE_REMOVE;
			}
		}
		rcp->r_b = TRUE;
	}

	if (rcp->green_is_ahead) rcp->green_is_ahead = FALSE;
	else {
		if (rcp->current_scene.g_pedestal != g_pedestal) {
			rcp->current_scene.g_pedestal = g_pedestal;

			g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), g_pedestal);
			g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
		}
		rcp->green_is_ahead = TRUE;
	}

	if (rcp->green_timeout_return_value == G_SOURCE_REMOVE) rcp->timeout_id = 0;

	return rcp->green_timeout_return_value;
}

gboolean g_pedestal_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_pedestal = rcp->current_scene.g_pedestal - 10;
		if (g_pedestal < MIN_VALUE) g_pedestal = MIN_VALUE;
		rcp->timeout_value = -10;
	} else {
		g_pedestal = rcp->current_scene.g_pedestal + 10;
		if (g_pedestal > MAX_VALUE) g_pedestal = MAX_VALUE;
		rcp->timeout_value = 10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
			else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
			if (rcp->current_scene.b_pedestal != b_pedestal) {
				rcp->current_scene.b_pedestal = b_pedestal;

				g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
				g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
			else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
			if (rcp->current_scene.r_pedestal != r_pedestal) {
				rcp->current_scene.r_pedestal = r_pedestal;

				g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
				g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_pedestal_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_pedestal = rcp->current_scene.g_pedestal - 1;
		if (g_pedestal < MIN_VALUE) g_pedestal = MIN_VALUE;
		rcp->timeout_value = -1;
	} else {
		g_pedestal = rcp->current_scene.g_pedestal + 1;
		if (g_pedestal > MAX_VALUE) g_pedestal = MAX_VALUE;
		rcp->timeout_value = 1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
			else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
			if (rcp->current_scene.b_pedestal != b_pedestal) {
				rcp->current_scene.b_pedestal = b_pedestal;

				g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
				g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
			else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
			if (rcp->current_scene.r_pedestal != r_pedestal) {
				rcp->current_scene.r_pedestal = r_pedestal;

				g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
				g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_pedestal_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_pedestal = rcp->current_scene.g_pedestal + 1;
		if (g_pedestal > MAX_VALUE) g_pedestal = MAX_VALUE;
		rcp->timeout_value = 1;
	} else {
		g_pedestal = rcp->current_scene.g_pedestal - 1;
		if (g_pedestal < MIN_VALUE) g_pedestal = MIN_VALUE;
		rcp->timeout_value = -1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
			else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
			if (rcp->current_scene.b_pedestal != b_pedestal) {
				rcp->current_scene.b_pedestal = b_pedestal;

				g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
				g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
			else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
			if (rcp->current_scene.r_pedestal != r_pedestal) {
				rcp->current_scene.r_pedestal = r_pedestal;

				g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
				g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_pedestal_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_pedestal, g_pedestal, b_pedestal;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_pedestal = rcp->current_scene.g_pedestal + 10;
		if (g_pedestal > MAX_VALUE) g_pedestal = MAX_VALUE;
		rcp->timeout_value = 10;
	} else {
		g_pedestal = rcp->current_scene.g_pedestal - 10;
		if (g_pedestal < MIN_VALUE) g_pedestal = MIN_VALUE;
		rcp->timeout_value = -10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
		else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
		if (rcp->current_scene.r_pedestal != r_pedestal) {
			rcp->current_scene.r_pedestal = r_pedestal;

			g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
			g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
				send_r_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
			else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
			if (rcp->current_scene.b_pedestal != b_pedestal) {
				rcp->current_scene.b_pedestal = b_pedestal;

				g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
				g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
					send_b_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_pedestal = rcp->current_scene.b_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
		if (b_pedestal < MIN_VALUE) b_pedestal = MIN_VALUE;
		else if (b_pedestal > MAX_VALUE) b_pedestal = MAX_VALUE;
		if (rcp->current_scene.b_pedestal != b_pedestal) {
			rcp->current_scene.b_pedestal = b_pedestal;

			g_signal_handler_block (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_pedestal_scale), b_pedestal);
			g_signal_handler_unblock (rcp->b_pedestal_scale, rcp->b_pedestal_handler_id);

			send_cam_control_command_3_digits (rcp, "OBP:", b_pedestal, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_pedestal = rcp->current_scene.b_pedestal;
				send_b_pedestal_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
		} else {
			r_pedestal = rcp->current_scene.r_pedestal - (g_pedestal - rcp->current_scene.g_pedestal);
			if (r_pedestal < MIN_VALUE) r_pedestal = MIN_VALUE;
			else if (r_pedestal > MAX_VALUE) r_pedestal = MAX_VALUE;
			if (rcp->current_scene.r_pedestal != r_pedestal) {
				rcp->current_scene.r_pedestal = r_pedestal;

				g_signal_handler_block (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_pedestal_scale), r_pedestal);
				g_signal_handler_unblock (rcp->r_pedestal_scale, rcp->r_pedestal_handler_id);

				send_cam_control_command_3_digits (rcp, "ORP:", r_pedestal, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_pedestal = rcp->current_scene.r_pedestal;
					send_r_pedestal_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_pedestal_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

#define set_b_pedestal_label(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->b_pedestal_need_update = FALSE;

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(b_pedestal,"OBP:",3)

BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_pedestal,"OBP:",3,10)
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_pedestal,"OBP:",3,1)

BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_pedestal,"OBP:",3,1)
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_pedestal,"OBP:",3,10)

void black_raz_button_clicked_AW_HE_130 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(r_pedestal,R_PEDESTAL_DEFAULT)

	if (rcp->current_scene.g_pedestal != G_PEDESTAL_DEFAULT) {
		rcp->current_scene.g_pedestal = G_PEDESTAL_DEFAULT;

		g_signal_handler_block (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_pedestal_scale), G_PEDESTAL_DEFAULT);
		g_signal_handler_unblock (rcp->g_pedestal_scale, rcp->g_pedestal_handler_id);
	}

	RAZ_IHM_UPDATE_SCALE(b_pedestal,B_PEDESTAL_DEFAULT)
}

GtkWidget *create_black_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box3, *box2, *box4, *widget;

	frame = gtk_frame_new ("Noir");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);

	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_end (box4, MARGIN_VALUE);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box1), TRUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), R_PEDESTAL_DEFAULT);
				rcp->r_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_pedestal_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->r_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_PEDESTAL_DEFAULT);
				rcp->g_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_pedestal_value_changed_AW_HE130), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->g_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_10_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_1_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_1_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_10_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_PEDESTAL_DEFAULT);
				rcp->b_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_pedestal_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->b_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (black_raz_button_clicked_AW_HE_130), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);

	return frame;
}

#define set_g_pedestal_label_AW_UE150(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION

CAM_CMD_FUNCS_S(g_pedestal,"OSJ:10:",3,AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(g_pedestal,"OSJ:10:",3,10,AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(g_pedestal,"OSJ:10:",3,1,AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(g_pedestal,"OSJ:10:",3,1,AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(g_pedestal,"OSJ:10:",3,10,AW_UE150)

void black_raz_button_clicked_AW_UE150 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(r_pedestal,R_PEDESTAL_DEFAULT)
	RAZ_IHM_UPDATE_SCALE_S(g_pedestal,G_PEDESTAL_DEFAULT,AW_UE150)
	RAZ_IHM_UPDATE_SCALE(b_pedestal,B_PEDESTAL_DEFAULT)
}

GtkWidget *create_black_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box3, *box2, *box4, *widget;

	frame = gtk_frame_new ("Noir");
	gtk_frame_set_label_align (GTK_FRAME (frame), 0.02, 0.5);
	gtk_widget_set_margin_bottom (frame, MARGIN_VALUE);

	box4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_end (box4, MARGIN_VALUE);
		box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_margin_start (box1, MARGIN_VALUE);
		gtk_widget_set_margin_end (box1, MARGIN_VALUE);
		gtk_box_set_homogeneous (GTK_BOX (box1), TRUE);
			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), R_PEDESTAL_DEFAULT);
				rcp->r_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_pedestal_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->r_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_pedestal_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_PEDESTAL_DEFAULT);
				rcp->g_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_pedestal_value_changed_AW_UE150), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->g_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_plus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_pedestal_minus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_PEDESTAL_DEFAULT);
				rcp->b_pedestal_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_pedestal_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->b_pedestal_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_pedestal_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_pedestal_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_pedestal_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (black_raz_button_clicked_AW_UE150), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

