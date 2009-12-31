/*
 * Copyright 2008 Openismus GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef CLUTTER_TUTORIAL_TRIANGLE_ACTOR_H
#define CLUTTER_TUTORIAL_TRIANGLE_ACTOR_H

#include <cluttermm.h>

namespace Tutorial
{

class Triangle : public Clutter::Actor
{
public:
  static Glib::RefPtr<Triangle> create();
  static Glib::RefPtr<Triangle> create(const Clutter::Color& color);

  virtual ~Triangle();

  void set_color(const Clutter::Color& color);
  Clutter::Color get_color() const;

protected:
  Triangle();
  explicit Triangle(const Clutter::Color& color);

  virtual void on_paint();
  virtual void pick_vfunc(const Clutter::Color& color);

private:
  Clutter::Color color_;

  void do_triangle_paint(const CoglColor* color);
};

} // namespace Tutorial

#endif /* !CLUTTER_TUTORIAL_TRIANGLE_ACTOR */
