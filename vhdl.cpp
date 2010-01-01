/* vim: ts=2:sw=2:expandtab
 */

class Position
{
  int x;
  int y;
};

class Size
{
  int width;
  int height;
};

class LayoutPort
{
private:
  Glib::ustring       m_portName;
  LayoutBlock::Edge   m_edge;
  int                 position;
}

class LayoutSignal
{
private:
  Glib::ustring         m_signalName;
  std::list<Position>   m_corners;

  LayoutBlock          *m_pStartComponent;
  int                   m_startPortIndex;

  LayoutBlock          *m_pEndComponent;
  int                   m_endPortIndex;
};

class LayoutBlock
{
public:
  enum {
    EDGE_LEFT,
    EDGE_TOP,
    EDGE_RIGHT,
    EDGE_BOTTOM
  } Edge;
private:
  Glib::ustring           m_instanceName;
  Position                m_position;
  Size                    m_size;
  std::vector<LayoutPort> m_ports;
};



class VHDLWire
{
public:
  enum {
    TYPE_SIGNAL,
    TYPE_PORT
  };
private:
  Glib::ustring           m_name;
  VHDLType                m_type;
}

class VHDLInterface
{
public:
  enum {
    TYPE_ENTITY,
    TYPE_COMPONENT
  };
private:
  Glib::ustring           m_name;
  std::list<VHDLGeneric>  m_generics;
  std::list<VHDLWire>     m_ports;
};

class VHDLInstance
{
private:
  Glib::ustring   m_name;
  VHDLInterface  *m_pComponent;
  VHDLGenericMap  m_genericMap;
  VHDLPortMap     m_portMap;
};

class VHDLArchitecture
{
private:
  Glib::ustring           m_name;
  VHDLInterface          *m_pEntity;
  std::list<VHDLWire>     m_signals;
  std::list<VHDLInstance> m_instances;
};


