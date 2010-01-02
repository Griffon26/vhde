/* vim: ts=2:sw=2:expandtab
 */

Ideas:
- Make sure that there's a facility to highlight components throughout the hierarchy.
  It should be possible to easily find all places where a component is used.
  To find them in a big diagram, allow a inverted ripple effect while zoomed out 
  (circles centered on the components, shrinking to a point and then starting at large size again)
- Use highlights for two purposes (use different colors): 
  1 - marking items that need an update before compilation
  2 - as the results for a component search

Question: should an instance always have the exact same layout as its component or
          should an instance be allowed to have instance-specific layout?
Answer  : Yes.

Use cases:
- port deleted on component
  - remove on all instances and mark connectors as "needing update"
- port added on component
  - simply add to all instances
- ports rearranged on component
  - never update instance layout automatically
  - allow manual action to update all instances
- port renamed on component
  - just rename on instances
- component deleted
  - only allow after confirmation that component is in use
  - delete component and mark connectors as "needing update"
- component renamed
  - update both layout and vhdl model
- component resized
  - see rearranging of ports


/* 
 * GUI
 */


/*
 * Layout
 */

block
  component name
  position
  size
  connections -> port position

connector
  list of corners
  start connected to component port X
  end connected to component port X
  start need update?
  end need update?


/*
 * VHDL
 */

architecture

component
  component name
  generic list
  port list

signal

instance
  instance name
  component name
  generic map
  port map




  list of connections { name, type }


portmap:
begin
  G1: INV port map (SEL, SELB);
  G2: AOI port map (SEL, A, SELB, B, F);
end; 

signals:
signal SELB: STD_LOGIC;

ports:
port (SEL, A, B: in STD_LOGIC;
      F : out STD_LOGIC);




