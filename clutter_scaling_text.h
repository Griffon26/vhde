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

#ifndef _CLUTTER_SCALING_TEXT_H
#define _CLUTTER_SCALING_TEXT_H

#include <cluttermm.h>

#include "common.h"

class ClutterScalingText: public Clutter::Text
{
public:
  static Glib::RefPtr<ClutterScalingText> create(const Glib::ustring& text);

private:
  explicit ClutterScalingText(const Glib::ustring& font_name, const Glib::ustring& text);

  //void drawText(const Clutter::Color &color);

#ifdef BROKEN
  void paint_vfunc() override;
  void pick_vfunc(const Clutter::Color &color);
#endif
};

#endif /* _CLUTTER_SCALING_TEXT_H */
