/* vim: ts=2:sw=2:expandtab
 *
 * Copyright 2009 Maurice van der Pot <griffon26@kfk4ever.com>
 *
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "vhdl_architecture.h"

bool VHDLArchitecture::write(FILE *pFile, int indent)
{
  std::list<VHDLWire>::iterator wit;
  std::list<VHDLInstance>::iterator iit;

  m_pEntity->write(pFile, indent);

  fprintf(pFile, "%*sarchitecture %s of %s is\n\n", indent, "", m_name, m_pEntity->getName());

  for(iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    iit->getComponent()->write(pFile, indent);
  }

  for(wit = m_signals.begin(); wit != m_signals.end(); wit++)
  {
    wit->write(pFile, indent);
  }

  fprintf(pFile, "%*sbegin\n", indent, "");
  for(iit = m_instances.begin(); iit != m_instances.end(); iit++)
  {
    iit->write(pFile, indent + 2);
  }
  fprintf(pFile, "%*send;\n", indent, "");

  return true;
}

