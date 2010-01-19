/* vim: ts=2:sw=2:expandtab
 */

Ideas:
- Allow changing of a block's color by using a fill tool like in GIMP
- Make sure that there's a facility to highlight components throughout the hierarchy.
  It should be possible to easily find all places where a component is used.
  To find them in a big diagram, allow a inverted ripple effect while zoomed out
  (circles centered on the components, shrinking to a point and then starting at large size again)
- Use highlights for two purposes (use different colors):
  1 - marking items that need an update before compilation
  2 - as the results for a component search

Question: should an instance always have the exact same layout as its component or
          should an instance be allowed to have instance-specific layout?
Answer  : instance-specific layout

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
