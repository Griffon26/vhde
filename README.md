# VHDE - a VHDL Diagram Editor that doesn't suck

[![Build Status](https://travis-ci.org/spoorcc/vhde.svg?branch=master)](https://travis-ci.org/spoorcc/vhde)

## Introduction

With the VHDL Diagram Editor you can create hierarchical VHDL where 
only the lowest level is written as text and the others are presented
in a diagram as graphical components that can be connected to each other.
Each diagram defines its inputs and outputs so it can be used as a
component in a higher level diagram.

## Unique "selling" points

It is similar to [Mentor Graphics' HDL designer][1], but with the following
key improvements:
* it is Free Software
* all project files are text files
* the functional structure of the diagrams is stored in one set of files,
  while the layout information is stored in others.
* the file content is sorted/organized in such a way that it is easy to
  compare two versions of a file and see what was changed
* it does not integrate version control, but because it stores things in
  a version control friendly format, you can use any version control
  system you want
* it does not have an integrated editor for VHDL code, because existing
  editors are better than a custom one would ever be
  
[1]: https://www.mentor.com/products/fpga/hdl_design/hdl_designer_series/

## Current status

I haven't worked on this project for a while, but if you think this program
is interesting send me a mail at griffon26@kfk4ever.com and I'll see what I
can do about picking up development again.
