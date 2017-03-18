/* vim: ts=2:sw=2:expandtab
 *
 * VHDE - The VHDL Diagram Editor
 * Copyright (C) 2009-2010  Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "clutter_scaling_text.h"

/*
 * Public methods
 */

Glib::RefPtr<ClutterScalingText> ClutterScalingText::create(const Glib::ustring& text)
{
  return Glib::RefPtr<ClutterScalingText>(new ClutterScalingText("", text));
}

/*
 * Private methods
 */

ClutterScalingText::ClutterScalingText(const Glib::ustring& font_name, const Glib::ustring& text):
  Clutter::Text(font_name, text)
{
}

void ClutterScalingText::paint_vfunc()
{
  double fontSizeSmall = 12, fontSizeMed = 27, fontSizeLarge = 39;
  double fontScale;
  int fontSize;

  char fontName[] = "monospace NNpx";
  Clutter::ActorBox alloc = get_allocation_box();

  /*
  printf("actor allocation box is %f,%f - %f,%f\n", alloc.get_x1(), alloc.get_y1(),
                                                    alloc.get_x2(), alloc.get_y2());
  printf("actor allocation box is %f x %f\n", alloc.get_x2() - alloc.get_x1(),
                                              alloc.get_y2() - alloc.get_y1());
                                              */

  double sx, sy;
  get_stage()->get_scale(sx, sy);

  /*
   * 1.01 is large enough to make sure floating point inaccuracies do not
   * influence the outcome of the conditions, yet small enough to be less than
   * the difference between two zoom levels.
   */
  if(sx > 1.01 * fontSizeMed/fontSizeSmall)
  {
    fontSize = fontSizeLarge;
  }
  else if(sx > 1.01 * fontSizeSmall/fontSizeSmall)
  {
    fontSize = fontSizeMed;
  }
  else
  {
    fontSize = fontSizeSmall;
  }
  fontScale = fontSizeSmall / fontSize;
  cogl_scale(fontScale, fontScale, 1);
  sprintf(fontName, "monospace %dpx", fontSize);
  //printf("Setting font to %s (%f would have been ideal)\n", fontName, sx * fontSizeSmall);
  set_font_name(fontName);

  Clutter::Text::paint_vfunc();
}

#ifdef BROKEN
void ClutterScalingText::pick_vfunc(const Clutter::Color &color)
{
  set_font_name("monospace 12px");
  Clutter::Text::pick_vfunc(color);
}
#endif
