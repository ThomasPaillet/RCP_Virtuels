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


#define MIN_VALUE 0x000
#define MAX_VALUE 0x12C

#define set_r_gain_label(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->r_gain_need_update = FALSE;

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(r_gain,"ORI:",3)

BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_gain,"ORI:",3,10)
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_gain,"ORI:",3,1)

BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_gain,"ORI:",3,1)
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(r_gain,"ORI:",3,10)

gboolean set_g_gain_delayed_AW_HE130 (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	if (rcp->r_b) {
		if (rcp->r_gain_need_update) {
			send_cam_control_command_3_digits (rcp, "ORI:", rcp->current_scene.r_gain, FALSE);
			rcp->r_gain_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->b_gain_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->b_gain_need_update) {
				send_cam_control_command_3_digits (rcp, "OBI:", rcp->current_scene.b_gain, FALSE);
				rcp->b_gain_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	} else {
		if (rcp->b_gain_need_update) {
			send_cam_control_command_3_digits (rcp, "OBI:", rcp->current_scene.b_gain, FALSE);
			rcp->b_gain_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->r_gain_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->r_gain_need_update) {
				send_cam_control_command_3_digits (rcp, "ORI:", rcp->current_scene.r_gain, FALSE);
				rcp->r_gain_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	}

	rcp->r_b = !rcp->r_b;

	if (rcp->need_last_call) {
		rcp->need_last_call = FALSE;
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_HE130, rcp);
	} else rcp->timeout_id = 0;

	return G_SOURCE_REMOVE;
}

void g_gain_value_changed_AW_HE130 (GtkRange *g_gain_scale, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;
	struct timeval current_time, elapsed_time;

	g_gain = (int)gtk_range_get_value (g_gain_scale);

	if (rcp->current_scene.g_gain != g_gain) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			rcp->r_gain_need_update = TRUE;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);
		}

		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			rcp->b_gain_need_update = TRUE;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);
		}

		rcp->current_scene.g_gain = g_gain;

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			rcp->need_last_call = TRUE;
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_g_gain_delayed_AW_HE130, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}

			if (rcp->r_b) {
				if (rcp->r_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "ORI:", rcp->current_scene.r_gain, FALSE);
					rcp->r_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_gain = rcp->current_scene.r_gain;
						send_r_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->b_gain_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_HE130, rcp);
					}
				} else if (rcp->b_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OBI:", rcp->current_scene.b_gain, FALSE);
					rcp->b_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_gain = rcp->current_scene.b_gain;
						send_b_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			} else {
				if (rcp->b_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OBI:", rcp->current_scene.b_gain, FALSE);
					rcp->b_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_gain = rcp->current_scene.b_gain;
						send_b_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->r_gain_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_HE130, rcp);
					}
				} else if (rcp->r_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "ORI:", rcp->current_scene.r_gain, FALSE);
					rcp->r_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_gain = rcp->current_scene.r_gain;
						send_r_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			}

			rcp->r_b = !rcp->r_b;
		}
	}
}

gboolean g_gain_button_held_AW_HE130 (rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	g_gain = rcp->current_scene.g_gain + rcp->timeout_value;
	if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
	else if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "ORI:", r_gain, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "OBI:", b_gain, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			} else {
				rcp->timeout_id = 0;
				return G_SOURCE_REMOVE;
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "OBI:", b_gain, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "ORI:", r_gain, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
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
		if (rcp->current_scene.g_gain != g_gain) {
			rcp->current_scene.g_gain = g_gain;

			g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), g_gain);
			g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
		}
		rcp->green_is_ahead = TRUE;
	}

	if (rcp->green_timeout_return_value == G_SOURCE_REMOVE) rcp->timeout_id = 0;

	return rcp->green_timeout_return_value;
}

gboolean g_gain_plus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_gain = rcp->current_scene.g_gain - 10;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -10;
	} else {
		g_gain = rcp->current_scene.g_gain + 10;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_plus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_gain = rcp->current_scene.g_gain - 1;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -1;
	} else {
		g_gain = rcp->current_scene.g_gain + 1;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_minus_1_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_gain = rcp->current_scene.g_gain + 1;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 1;
	} else {
		g_gain = rcp->current_scene.g_gain - 1;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_minus_10_button_pressed_AW_HE130 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);

		g_gain = rcp->current_scene.g_gain + 10;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 10;
	} else {
		g_gain = rcp->current_scene.g_gain - 10;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;

			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = rcp->current_scene.r_gain;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;

				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = rcp->current_scene.b_gain;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;

			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OBI:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = rcp->current_scene.b_gain;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;

				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "ORI:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = rcp->current_scene.r_gain;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_HE130, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

#define set_b_gain_label(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->b_gain_need_update = FALSE;

CAM_CMD_FUNCS_PLUS_UPDATE_NOTIFICATION(b_gain,"OBI:",3)

BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_gain,"OBI:",3,10)
BUTTON_PRESSED_PLUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_gain,"OBI:",3,1)

BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_gain,"OBI:",3,1)
BUTTON_PRESSED_MINUS_FUNC_PLUS_UPDATE_NOTIFICATION(b_gain,"OBI:",3,10)

void white_raz_button_clicked_AW_HE130 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE(r_gain,R_GAIN_DEFAULT_AW_HE130)

	if (rcp->current_scene.g_gain != G_GAIN_DEFAULT_AW_HE130) {
		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_HE130;

		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_HE130);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
	}

	RAZ_IHM_UPDATE_SCALE(b_gain,B_GAIN_DEFAULT_AW_HE130)
}

GtkWidget *create_white_frame_AW_HE130 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box3, *box2, *box4, *widget;

	frame = gtk_frame_new ("Blanc");
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
				gtk_range_set_value (GTK_RANGE (widget), R_GAIN_DEFAULT_AW_HE130);
				rcp->r_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_gain_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->r_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_GAIN_DEFAULT_AW_HE130);
				rcp->g_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_gain_value_changed_AW_HE130), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->g_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_10_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_1_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_1_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_10_button_pressed_AW_HE130), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_GAIN_DEFAULT_AW_HE130);
				rcp->b_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_gain_value_changed), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->b_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_1_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_10_button_pressed), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (white_raz_button_clicked_AW_HE130), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

#define MIN_VALUE 0x738
#define MAX_VALUE 0x8C8

#define set_r_gain_label_AW_UE150(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->r_gain_need_update = FALSE;

CAM_CMD_FUNCS_S(r_gain,"OSG:39:",3,AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(r_gain,"OSG:39:",3,10,AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(r_gain,"OSG:39:",3,1,AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(r_gain,"OSG:39:",3,1,AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(r_gain,"OSG:39:",3,10,AW_UE150)

gboolean set_g_gain_delayed_AW_UE150 (rcp_t *rcp)
{
	rcp->last_time.tv_usec += 130000;
	if (rcp->last_time.tv_usec >= 1000000) {
		rcp->last_time.tv_sec++;
		rcp->last_time.tv_usec -= 1000000;
	}

	if (rcp->r_b) {
		if (rcp->r_gain_need_update) {
			send_cam_control_command_3_digits (rcp, "OSG:39:", rcp->current_scene.r_gain, FALSE);
			rcp->r_gain_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->b_gain_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->b_gain_need_update) {
				send_cam_control_command_3_digits (rcp, "OSG:3A:", rcp->current_scene.b_gain, FALSE);
				rcp->b_gain_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	} else {
		if (rcp->b_gain_need_update) {
			send_cam_control_command_3_digits (rcp, "OSG:3A:", rcp->current_scene.b_gain, FALSE);
			rcp->b_gain_need_update = FALSE;

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			if (!rcp->r_gain_need_update) rcp->need_last_call = FALSE;
		} else {
			if (rcp->r_gain_need_update) {
				send_cam_control_command_3_digits (rcp, "OSG:39:", rcp->current_scene.r_gain, FALSE);
				rcp->r_gain_need_update = FALSE;

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			}
			rcp->need_last_call = FALSE;
		}
	}

	rcp->r_b = !rcp->r_b;

	if (rcp->need_last_call) {
		rcp->need_last_call = FALSE;
		rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_UE150, rcp);
	} else rcp->timeout_id = 0;

	return G_SOURCE_REMOVE;
}

void g_gain_value_changed_AW_UE150 (GtkRange *g_gain_scale, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;
	struct timeval current_time, elapsed_time;

	g_gain = (int)gtk_range_get_value (g_gain_scale);

	if (rcp->current_scene.g_gain != g_gain) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			rcp->r_gain_need_update = TRUE;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);
		}

		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			rcp->b_gain_need_update = TRUE;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);
		}

		rcp->current_scene.g_gain = g_gain;

		gettimeofday (&current_time, NULL);
		timersub (&current_time, &rcp->last_time, &elapsed_time);

		if ((elapsed_time.tv_sec == 0) && (elapsed_time.tv_usec < 130000)) {
			rcp->need_last_call = TRUE;
			if (rcp->timeout_id == 0) rcp->timeout_id = g_timeout_add ((130000 - elapsed_time.tv_usec) / 1000, (GSourceFunc)set_g_gain_delayed_AW_UE150, rcp);
		} else {
			if (rcp->timeout_id != 0) {
				g_source_remove (rcp->timeout_id);
				rcp->timeout_id = 0;
			}

			if (rcp->r_b) {
				if (rcp->r_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OSG:39:", rcp->current_scene.r_gain, FALSE);
					rcp->r_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
						send_r_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->b_gain_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_UE150, rcp);
					}
				} else if (rcp->b_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OSG:3A:", rcp->current_scene.b_gain, FALSE);
					rcp->b_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
						send_b_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			} else {
				if (rcp->b_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OSG:3A:", rcp->current_scene.b_gain, FALSE);
					rcp->b_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
						send_b_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}

					if (rcp->r_gain_need_update) {
						rcp->need_last_call = FALSE;
						rcp->timeout_id = g_timeout_add (130, (GSourceFunc)set_g_gain_delayed_AW_UE150, rcp);
					}
				} else if (rcp->r_gain_need_update) {
					rcp->last_time = current_time;
					send_cam_control_command_3_digits (rcp, "OSG:39:", rcp->current_scene.r_gain, FALSE);
					rcp->r_gain_need_update = FALSE;

					if ((rcp == rcp_vision) && physical_rcp.connected) {
						g_mutex_lock (&physical_rcp.mutex);
						physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
						send_r_gain_update_notification ();
						g_mutex_unlock (&physical_rcp.mutex);
					}
				}
			}

			rcp->r_b = !rcp->r_b;
		}
	}
}

gboolean g_gain_button_held_AW_UE150 (rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	g_gain = rcp->current_scene.g_gain + rcp->timeout_value;
	if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
	else if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;
				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}
			} else {
				rcp->timeout_id = 0;
				return G_SOURCE_REMOVE;
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			rcp->last_time.tv_usec += 130000;
			if (rcp->last_time.tv_usec >= 1000000) {
				rcp->last_time.tv_sec++;
				rcp->last_time.tv_usec -= 1000000;
			}

			send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, FALSE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;
				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				rcp->need_last_call = FALSE;

				rcp->last_time.tv_usec += 130000;
				if (rcp->last_time.tv_usec >= 1000000) {
					rcp->last_time.tv_sec++;
					rcp->last_time.tv_usec -= 1000000;
				}

				send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, FALSE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
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
		if (rcp->current_scene.g_gain != g_gain) {
			rcp->current_scene.g_gain = g_gain;
			g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), g_gain);
			g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
		}
		rcp->green_is_ahead = TRUE;
	}

	if (rcp->green_timeout_return_value == G_SOURCE_REMOVE) rcp->timeout_id = 0;

	return rcp->green_timeout_return_value;
}

gboolean g_gain_plus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		g_gain = rcp->current_scene.g_gain - 10;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -10;
	} else {
		g_gain = rcp->current_scene.g_gain + 10;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;
				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;
				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_plus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		g_gain = rcp->current_scene.g_gain - 1;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -1;
	} else {
		g_gain = rcp->current_scene.g_gain + 1;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;
				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;
				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_minus_1_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		g_gain = rcp->current_scene.g_gain + 1;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 1;
	} else {
		g_gain = rcp->current_scene.g_gain - 1;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -1;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;
				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;
				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

gboolean g_gain_minus_10_button_pressed_AW_UE150 (GtkButton *button, GdkEventButton *event, rcp_t *rcp)
{
	int r_gain, g_gain, b_gain;

	if (event->button == GDK_BUTTON_SECONDARY) {
		gtk_widget_set_state_flags (GTK_WIDGET (button), GTK_STATE_FLAG_ACTIVE, FALSE);
		g_gain = rcp->current_scene.g_gain + 10;
		if (g_gain > MAX_VALUE) g_gain = MAX_VALUE;
		rcp->timeout_value = 10;
	} else {
		g_gain = rcp->current_scene.g_gain - 10;
		if (g_gain < MIN_VALUE) g_gain = MIN_VALUE;
		rcp->timeout_value = -10;
	}

	if (rcp->timeout_id != 0) {
		g_source_remove (rcp->timeout_id);
		rcp->timeout_id = 0;
	}

	if (rcp->r_b) {
		r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
		if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
		else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
		if (rcp->current_scene.r_gain != r_gain) {
			rcp->current_scene.r_gain = r_gain;
			g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
			g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
				send_r_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
			if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
			else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
			if (rcp->current_scene.b_gain != b_gain) {
				rcp->current_scene.b_gain = b_gain;
				g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
				g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
					send_b_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = FALSE;
	} else {
		b_gain = rcp->current_scene.b_gain - (g_gain - rcp->current_scene.g_gain);
		if (b_gain < MIN_VALUE) b_gain = MIN_VALUE;
		else if (b_gain > MAX_VALUE) b_gain = MAX_VALUE;
		if (rcp->current_scene.b_gain != b_gain) {
			rcp->current_scene.b_gain = b_gain;
			g_signal_handler_block (rcp->b_gain_scale, rcp->b_gain_handler_id);
			gtk_range_set_value (GTK_RANGE (rcp->b_gain_scale), b_gain);
			g_signal_handler_unblock (rcp->b_gain_scale, rcp->b_gain_handler_id);

			send_cam_control_command_3_digits (rcp, "OSG:3A:", b_gain, TRUE);

			if ((rcp == rcp_vision) && physical_rcp.connected) {
				g_mutex_lock (&physical_rcp.mutex);
				physical_rcp.b_gain = (rcp->current_scene.b_gain - 0x738) * 0.75;
				send_b_gain_update_notification ();
				g_mutex_unlock (&physical_rcp.mutex);
			}

			rcp->need_last_call = TRUE;
			rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
		} else {
			r_gain = rcp->current_scene.r_gain - (g_gain - rcp->current_scene.g_gain);
			if (r_gain < MIN_VALUE) r_gain = MIN_VALUE;
			else if (r_gain > MAX_VALUE) r_gain = MAX_VALUE;
			if (rcp->current_scene.r_gain != r_gain) {
				rcp->current_scene.r_gain = r_gain;
				g_signal_handler_block (rcp->r_gain_scale, rcp->r_gain_handler_id);
				gtk_range_set_value (GTK_RANGE (rcp->r_gain_scale), r_gain);
				g_signal_handler_unblock (rcp->r_gain_scale, rcp->r_gain_handler_id);

				send_cam_control_command_3_digits (rcp, "OSG:39:", r_gain, TRUE);

				if ((rcp == rcp_vision) && physical_rcp.connected) {
					g_mutex_lock (&physical_rcp.mutex);
					physical_rcp.r_gain = (rcp->current_scene.r_gain - 0x738) * 0.75;
					send_r_gain_update_notification ();
					g_mutex_unlock (&physical_rcp.mutex);
				}

				rcp->need_last_call = FALSE;
				rcp->timeout_id = g_timeout_add (130, (GSourceFunc)g_gain_button_held_AW_UE150, rcp);
			}
		}
		rcp->r_b = TRUE;
	}

	rcp->green_is_ahead = FALSE;
	rcp->green_timeout_return_value = G_SOURCE_CONTINUE;

	return FALSE;
}

#define set_b_gain_label_AW_UE150(r)

#undef SEND_CMD_POST_ACTION
#define SEND_CMD_POST_ACTION rcp->b_gain_need_update = FALSE;

CAM_CMD_FUNCS_S(b_gain,"OSG:3A:",3,AW_UE150)

BUTTON_PRESSED_PLUS_FUNC_S(b_gain,"OSG:3A:",3,10,AW_UE150)
BUTTON_PRESSED_PLUS_FUNC_S(b_gain,"OSG:3A:",3,1,AW_UE150)

BUTTON_PRESSED_MINUS_FUNC_S(b_gain,"OSG:3A:",3,1,AW_UE150)
BUTTON_PRESSED_MINUS_FUNC_S(b_gain,"OSG:3A:",3,10,AW_UE150)

void white_raz_button_clicked_AW_UE150 (GtkButton *button, rcp_t *rcp)
{
	RAZ_IHM_UPDATE_SCALE_S(r_gain,R_GAIN_DEFAULT_AW_UE150,AW_UE150)

	if (rcp->current_scene.g_gain != G_GAIN_DEFAULT_AW_UE150) {
		rcp->current_scene.g_gain = G_GAIN_DEFAULT_AW_UE150;

		g_signal_handler_block (rcp->g_gain_scale, rcp->g_gain_handler_id);
		gtk_range_set_value (GTK_RANGE (rcp->g_gain_scale), G_GAIN_DEFAULT_AW_UE150);
		g_signal_handler_unblock (rcp->g_gain_scale, rcp->g_gain_handler_id);
	}

	RAZ_IHM_UPDATE_SCALE_S(b_gain,B_GAIN_DEFAULT_AW_UE150,AW_UE150)
}

GtkWidget *create_white_frame_AW_UE150 (rcp_t *rcp)
{
	GtkWidget *frame, *box1, *box3, *box2, *box4, *widget;

	frame = gtk_frame_new ("Blanc");
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
				gtk_range_set_value (GTK_RANGE (widget), R_GAIN_DEFAULT_AW_UE150);
				rcp->r_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (r_gain_value_changed_AW_UE150), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->r_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Rouge");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_plus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (r_gain_minus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_red), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->r_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), G_GAIN_DEFAULT_AW_UE150);
				rcp->g_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (g_gain_value_changed_AW_UE150), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->g_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Vert");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_plus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (g_gain_minus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_green_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_green), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->g_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);

			box3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
				widget = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, MIN_VALUE, MAX_VALUE, 1.0);
				gtk_widget_set_margin_bottom (widget, WINDOW_MARGIN_VALUE);
				gtk_range_set_inverted (GTK_RANGE (widget), TRUE);
				gtk_scale_set_draw_value (GTK_SCALE (widget), FALSE);
				gtk_scale_set_has_origin (GTK_SCALE (widget), FALSE);
				gtk_range_set_value (GTK_RANGE (widget), B_GAIN_DEFAULT_AW_UE150);
				rcp->b_gain_handler_id = g_signal_connect (widget, "value-changed", G_CALLBACK (b_gain_value_changed_AW_UE150), rcp);
				gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
				gtk_box_pack_start (GTK_BOX (box3), widget, FALSE, FALSE, 0);
				rcp->b_gain_scale = widget;

				box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
				gtk_widget_set_margin_start (box2, MARGIN_VALUE);
					widget = gtk_label_new ("Bleu");
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);

					widget = gtk_button_new_with_label ("+10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_plus_10_button = widget;

					widget = gtk_button_new_with_label ("+1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_plus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_plus_1_button = widget;

					widget = gtk_button_new_with_label ("-1");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_1_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_minus_1_button = widget;

					widget = gtk_button_new_with_label ("-10");
					g_signal_connect (widget, "button_press_event", G_CALLBACK (b_gain_minus_10_button_pressed_AW_UE150), rcp);
					g_signal_connect (widget, "button_release_event", G_CALLBACK (remove_timeout), rcp);
					gtk_widget_set_margin_bottom (widget, MARGIN_VALUE);
					gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_gain_blue), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
					gtk_box_pack_start (GTK_BOX (box2), widget, FALSE, FALSE, 0);
					rcp->b_gain_minus_10_button = widget;
				gtk_box_pack_start (GTK_BOX (box3), box2, FALSE, FALSE, 0);
			gtk_box_pack_start (GTK_BOX (box1), box3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box4), box1, TRUE, TRUE, 0);

		box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			widget = gtk_button_new_with_label ("raz");
			g_signal_connect (widget, "clicked", G_CALLBACK (white_raz_button_clicked_AW_UE150), rcp);
			gtk_style_context_add_provider (gtk_widget_get_style_context (widget), GTK_STYLE_PROVIDER (css_provider_raz), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
			gtk_box_pack_start (GTK_BOX (box1), widget, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (box4), box1, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (frame), box4);

	return frame;
}

#undef MIN_VALUE
#undef MAX_VALUE

