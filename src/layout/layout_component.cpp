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

#include <stdio.h>

#include "i_named_item.h"
#include "layout_component.h"
#include "layout_port.h"

/*
 * Public methods
 */

LayoutComponent::LayoutComponent()
{
  m_position.x = 800;
  m_position.y = 50;
}

void LayoutComponent::associateEntity(INamedItem *pVHDLEntity)
{
  g_assert(pVHDLEntity);
  printf("LayoutComponent(%p)::associateEntity(%p)\n", this, pVHDLEntity);
  m_pVHDLEntity = pVHDLEntity;
}

INamedItem *LayoutComponent::getAssociatedVHDLEntity()
{
  return m_pVHDLEntity;
}

LayoutPort *LayoutComponent::createPort(Edge edge, int position, INamedItem *pVHDLPort)
{
  printf("LayoutComponent::createPort\n");
  LayoutPort *pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pVHDLPort);
  addPort(edge, position, pLayoutPort);
  return pLayoutPort;
}

void LayoutComponent::destroyPort(Edge edge, int position)
{
  LayoutPort *pLayoutPort = getPort(edge, position);
  printf("LayoutComponent(%p)::destroyPort(%p)\n", this, pLayoutPort);
  g_assert(pLayoutPort != NULL);
  removePort(pLayoutPort);
  delete pLayoutPort;
}

/*
component {
  size 200 300
  ports {
    LEFT 0 "dinges"
    LEFT 1 "blaat"
  }
}

*/
void LayoutComponent::write(std::ostream &stream, int indent)
{
  int edge;
  std::map<int, LayoutPort *>::iterator it;
  std::string indentString(indent, ' ');

  stream << indentString << "component {\n"
         << indentString << "  size " << m_size.width << " " << m_size.height << "\n"
         << indentString << "  ports {\n";

  for(edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
    {
      stream << indentString << "    " << EDGE_TO_NAME(edge) << " " << it->first << " \"" << it->second->getAssociatedVHDLPort()->getName() << "\"\n";
    }
  }

  stream << indentString << "  }\n"
         << indentString << "}\n"
         << "\n";
}

