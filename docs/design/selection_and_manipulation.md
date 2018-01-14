# Selection

Any selectable Gui object implements the ISelectable interface.
This interface contains signals clicked.

Any movable or resizable Gui object implements the IDraggable interface.
IDraggable inherits from ISelectable. This interface contains signal
dragstarted.

The MoveAndResizeHandler is created by the stage updater and passed a reference
to the GuiSelection.  Every Gui object that is IDraggable is registered at the
MoveAndResizeHandler (and removed when destroyed, or if the MoveAndResizeHandler
only needs to connect signals, then it will do that and removing is not
required because the registration is at the Gui object side).  The
MoveAndResizeHandler registers for the dragStarted event of IDraggables and
passes the IDraggable pointer so it can call functions on it without
remembering which object belongs to which connection.

When a dragstarted is received, it decides if it's a move or a resize based on
modifiers in the event. It also registers for stage events to be able to process
motion events or the button up that signals the end of a drag operation.
It checks if the whole selection should move or only the object itself.
If the selection is not empty and the drag is done on an object not part of the
selection, then the selection is first changed to be only that object and then
dragging is performed as normal.
For all objects that need to move, it calls startMove/startResize,
finishMove/finishResize and updateMove/updateResize (also part of the
IDraggable interface).

Currently explicitly not supported behaviour:
* selecting multiple ports to drag them as a group

