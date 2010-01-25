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
  m_pVHDLEntity = pVHDLEntity;
}

INamedItem *LayoutComponent::getAssociatedVHDLEntity()
{
  return m_pVHDLEntity;
}

LayoutPort *LayoutComponent::createPort(int actionId, Edge edge, int position, INamedItem *pVHDLPort)
{
  printf("LayoutComponent::createPort\n");
  LayoutPort *pLayoutPort = new LayoutPort();
  pLayoutPort->associateVHDLPort(pVHDLPort);
  addPort(actionId, edge, position, pLayoutPort);
  return pLayoutPort;
}

void LayoutComponent::destroyPort(int actionId, Edge edge, int position)
{
  LayoutPort *pLayoutPort = getPort(edge, position);
  printf("LayoutComponent(%p)::destroyPort(%p)\n", this, pLayoutPort);
  g_assert(pLayoutPort != NULL);
  removePort(actionId, pLayoutPort);
  delete pLayoutPort;
}

/*
instance "blaat" {
  position 100 200
  size 200 300
  ports {
    LEFT 0 "dinges"
    LEFT 1 "blaat"
  }
}

*/
void LayoutComponent::write(FILE *pFile)
{
  int edge;
  std::map<int, LayoutPort *>::iterator it;

  fprintf(pFile, "component \"%s\" {\n", m_pVHDLEntity->getName().c_str());
  fprintf(pFile, "  size %d %d\n", m_size.width, m_size.height);
  fprintf(pFile, "  ports {\n");

  for(edge = 0; edge < NR_OF_EDGES; edge++)
  {
    for(it = m_ports[edge].begin(); it != m_ports[edge].end(); it++)
    {
      fprintf(pFile, "    %s %d \"%s\"\n", EDGE_TO_NAME(edge), it->first, it->second->getAssociatedVHDLPort()->getName().c_str());
    }
  }

  fprintf(pFile, "  }\n"
                 "}\n"
                 "\n");
}
