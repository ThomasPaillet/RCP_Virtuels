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

#include <gdk/gdk.h>

#include "icon.h"

#include "grille_1_light.h"
#include "grille_2_light.h"
#include "grille_3_light.h"
#include "grille_4_light.h"

#include "grille_1_dark.h"
#include "grille_2_dark.h"
#include "grille_3_dark.h"
#include "grille_4_dark.h"

#include "logo_light.h"
#include "logo_dark.h"


GdkPixbuf *pixbuf_icon;

GdkPixbuf *pixbuf_grille_1_light, *pixbuf_grille_2_light, *pixbuf_grille_3_light, *pixbuf_grille_4_light;
GdkPixbuf *pixbuf_grille_1_dark, *pixbuf_grille_2_dark, *pixbuf_grille_3_dark, *pixbuf_grille_4_dark;

GdkPixbuf *pixbuf_logo_light;
GdkPixbuf *pixbuf_logo_dark;


void load_pixbufs (void)
{
	pixbuf_icon = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_icon), pixbuf_inline_icon, FALSE, NULL);

	pixbuf_grille_1_light = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_1_light), pixbuf_inline_grille_1_light, FALSE, NULL);
	pixbuf_grille_2_light = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_2_light), pixbuf_inline_grille_2_light, FALSE, NULL);
	pixbuf_grille_3_light = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_3_light), pixbuf_inline_grille_3_light, FALSE, NULL);
	pixbuf_grille_4_light = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_4_light), pixbuf_inline_grille_4_light, FALSE, NULL);

	pixbuf_grille_1_dark = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_1_dark), pixbuf_inline_grille_1_dark, FALSE, NULL);
	pixbuf_grille_2_dark = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_2_dark), pixbuf_inline_grille_2_dark, FALSE, NULL);
	pixbuf_grille_3_dark = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_3_dark), pixbuf_inline_grille_3_dark, FALSE, NULL);
	pixbuf_grille_4_dark = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_grille_4_dark), pixbuf_inline_grille_4_dark, FALSE, NULL);

	pixbuf_logo_light = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_logo_light), pixbuf_inline_logo_light, FALSE, NULL);
	pixbuf_logo_dark = gdk_pixbuf_new_from_inline (sizeof (pixbuf_inline_logo_dark), pixbuf_inline_logo_dark, FALSE, NULL);
}

