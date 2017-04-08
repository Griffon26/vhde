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

#ifndef _LAYOUT_TYPES_H
#define _LAYOUT_TYPES_H

class LayoutPosition
{
public:
  LayoutPosition() {}

  LayoutPosition(int x, int y):
    x(x),
    y(y)
  {
  }

  bool operator==(const LayoutPosition &other) const
  {
    return (other.x == x) && (other.y == y);
  }
  bool operator!=(const LayoutPosition &other) const
  {
    return !(other == *this);
  }

  int x;
  int y;
};

class LayoutSize
{
public:
  LayoutSize() {}

  LayoutSize(int width, int height):
    width(width),
    height(height)
  {
  }

  int width;
  int height;
};

typedef enum {
  EDGE_LEFT,
  EDGE_TOP,
  EDGE_RIGHT,
  EDGE_BOTTOM,
  NR_OF_EDGES
} Edge;

#define EDGE_TO_NAME(e) ((e == EDGE_LEFT) ? "LEFT" : \
                         (e == EDGE_TOP) ? "TOP" : \
                         (e == EDGE_RIGHT) ? "RIGHT" : \
                         (e == EDGE_BOTTOM) ? "BOTTOM" : "INVALID")

#define NAME_TO_EDGE(n) ((n == "LEFT") ? EDGE_LEFT : \
                         (n == "TOP") ? EDGE_TOP : \
                         (n == "RIGHT") ? EDGE_RIGHT : \
                         (n == "BOTTOM") ? EDGE_BOTTOM : NR_OF_EDGES)


#endif /* _LAYOUT_TYPES_H */
