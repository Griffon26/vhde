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

#ifndef _COMMON_H
#define _COMMON_H

#include <vector>
#include <map>

#define DIR_TO_NAME(d) ((d == DIR_IN) ? "IN" : \
                        (d == DIR_OUT) ? "OUT" : \
                        (d == DIR_INOUT) ? "INOUT" : \
                                           "INVALID")

typedef enum {
  DIR_IN,
  DIR_OUT,
  DIR_INOUT,
  DIR_INVALID
} Direction;

template<typename T, typename U> std::vector<T> getKeysFromMap(std::map<T,U> map)
{
  std::vector<T> keys;
  
  for(auto &kv: map)
  {
    keys.push_back(kv.first);
  }
  return keys;
}

#endif /* _COMMON_H */
