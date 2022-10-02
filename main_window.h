/*
 * copyright (c) 2022 Thomas Paillet <thomas.paillet@net-c.fr>

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

#ifndef __MAIN_WINDOW_H
#define __MAIN_WINDOW_H


#define WINDOW_MARGIN_VALUE 5

extern GdkScreen *screen;
extern GtkCssProvider *light_css_provider;
extern GtkCssProvider *dark_css_provider;

extern GtkCssProvider *css_provider_gain_red, *css_provider_gain_green, *css_provider_gain_blue;
extern GtkCssProvider *css_provider_pedestal_red, *css_provider_pedestal_green, *css_provider_pedestal_blue;
extern GtkCssProvider *css_provider_white, *css_provider_black, *css_provider_raz, *css_provider_store, *css_provider_scene_selected;

extern GtkWidget *main_window;
extern GtkWidget *main_window_notebook;
extern GtkWidget *popup_window;

extern const char *application_name_txt;
extern const char *key_info_1_txt;
extern const char *key_info_2_txt;
extern const char *warning_txt;


gboolean digit_key_press (GtkEntry *entry, GdkEventKey *event);

gboolean hide_popup_window (GtkWidget *window);

gboolean popup_window_key_press (GtkWidget *window, GdkEventKey *event);

void show_rs_connection_error_window (void);


#endif

