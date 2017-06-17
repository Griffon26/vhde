# Backlog

This page lists work that needs to be done in order of priority. It is a mix of
functionality and code improvements. Sub-bullets are used for further
explanation or subdivision.

The identifiers (`VHDE-nn`) are only used to refer to other items on the list
and do not indicate priority.

1. `VHDE-04`: Create a usable GUI (based on Clutter for now)
1. `VHDE-02`: Complete support for the currently implemented VHDL items (remove shortcuts)
1. `VHDE-01`: Get TravisCI builds to work
1. `VHDE-11`: Add coverity static analysis to Travis build (deps on `VHDE-01`)
1. `VHDE-13`: Resolve conflicts between .layout and .vhd files
1. `VHDE-12`: Remove restriction of 1 architecture per file
1. `VHDE-05`: Add a comment for each class describing its reponsibility
1. `VHDE-16`: Add support for configuration declaration
1. `VHDE-17`: Add support for package declaration and body
1. `VHDE-09`: Figure out how to do user-assisted routing of signals in a diagram
1. `VHDE-10`: Add tests
1. `VHDE-18`: Write user manual
1. `VHDE-06`: Port from clutter to GTK+ Scene Graph Kit (GSK), once broadly available
1. ...
1. `VHDE-07`: Verilog support (don't hold your breath)

The above backlog focuses on getting VHDE into a state that is usable and
useful for regular users. To achieve this goal as soon as possible often a
simple implementation of a feature is chosen first, which can be replaced by a
more sophisticated one at a later time. The list below is meant to record some
ideas for improvement that are not important enough to make it to the current
backlog.

1. When loading a VHDL file that does not have a corresponding layout file,
   instead of putting half of the ports on the left side of a block and half of
   them on the right, put inputs on the left and outputs on the right.
1. When adding a VHDL file that was not created by VHDE to a project, detect if
   the content of the file would change if VHDE had written it. If so, warn the
   user about this and give him the option to open the file read-only or to
   accept that VHDE will change the file.
1. When loading VHDL files with comments, maintain them in the model and
   include them when the VHDL is written. Use some heuristics to determine
   where to put the comments as the file is modified (associate a comment
   directly above or at the end of a line with the VHDL object on that line,
   associate a comment followed by an empty line with the current section, e.g.
   the list of signals or the header of the file)
1. Implement graceful degradation when reading layout files that do not match
   the VHDL file. 
1. Fall back to generating a default layout when the layout file has syntactic
   errors
