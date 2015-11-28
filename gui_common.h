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

#ifndef _GUI_COMMON_H
#define _GUI_COMMON_H

#define BORDER_WIDTH  1

#define STAGE_COLOR       Clutter::Color(0xF0, 0xF0, 0xF0, 0xFF)
#define COMPONENT_COLOR   Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF)
#define PORT_COLOR        Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF)

#define UNHANDLED false
#define HANDLED   true

#define ALL_MODIFIERS_MASK  (CLUTTER_SHIFT_MASK | CLUTTER_CONTROL_MASK)

#endif /* _GUI_COMMON_H */
