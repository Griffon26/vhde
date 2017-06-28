# Focus handling

This document describes how focus handling is implemented in VHDE.

## Why it is needed

Both the elements in a diagram and the treeview must react to for instance the
arrow keys. This means that VHDE cannot do without the concept of input focus.
The input focus must be switchable between the treeview and the diagram.

## How to switch input focus

The focus is switched to one widget or the other by clicking on it.

## How to tell which widget has focus

If the treeview has focus, it will draw its selection. The user can select 0
rows and then it will not be visible if the treeview has focus, but this is
acceptable because it is not something a user is likely to do.

If the drawing has focus, it currently will not show it. In the future the plan
is to always have one selected component in the drawing (shown by a different
border or color) and restore this selection when the diagram is given focus.

## Workarounds

There are currently a few work-arounds in place:

### IStageUpdater::onKeyPressEvent

Because Clutter key handling does not integrate well with Gtk's, the key press
handler must be registered on a Gtk widget and one that contains the Clutter
stage. This means that IStageUpdater implementations cannot subscribe to key
events on the stage itself. For this reason IStageUpdaters are required to
implement an onKeyPressEvent function that will be connected to the
signal_key_press_event of the ClutterEmbedBox by VHDEWindow before VHDEWindow
calls setStage.

### VHDEWindow::on_stage_captured_event

Because focus must be given to the ClutterEmbedBox whenever the stage is
clicked, the window registers for the appropriate event on the stage and calls
grab_focus on the ClutterEmbedBox.

The signal_captured_event is used, because the signal_button_press_event is
emitted for the actual actor that is clicked, which is not always the stage
itself.

